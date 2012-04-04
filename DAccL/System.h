//
//  System.h
//  DAccL
//
//  Created by Mike Barriault on 12-02-28.
//  Copyright (c) 2012 Memorial University. All rights reserved.
//

#ifndef DAccL_System_h
#define DAccL_System_h

#include "Array.h"
#include <iostream>

extern "C" void ddassl_(
                        void (*funcptr)(const real& time, const real y[], const real yPrime[], real residue[], int& iRes, const real rPar[], const int iPar[]),
                        const int& noOfEquations, 
                        const real& currentTime,
                        const real initialY[],
                        const real initialYPrime[],
                        const real& finalTime,
                        const int info[15],
                        const real& relativeTolerance,
                        const real& absoluteTolerance,
                        int& outputStatusFlag,
                        const real rWorkArray[], 
                        const int& lengthOfrWork,
                        const int iWorkArray[], 
                        const int& lengthOfIWork,
                        const real rParArray[],
                        const int iParArray[],
                        void (*jacobian)(const real& time, const real y[], const real yPrime[], real PD[], real& CJ, real rPar[], int iPar[])
                        );

namespace DACCL {

    typedef Array<real> Function;
    typedef Array<real> Matrix;
    typedef std::vector<real> realp;
    typedef std::vector<int> intp;

    class System {
    private:
        intp info;
        realp rWork;
        intp iWork;
        bool hasStarted;
        bool jacobianSet;
        bool stepSet;
        
        static void Residue_Wrapper(const real& time, const real y[], const real z[], real residue[], int& iRes, const real rPar[], const int iPar[]) {
            System* system = (System*)iPar;
            Tuple& N = system->N;
            Function res_ptr(N,residue);
            // Do something with iRes here
            Function res = system->Residue(time, Function(N,y), Function(N,z));
            res_ptr &= res;
        }
        
        static void Jacobian_Wrapper(const real& time, const real y[], const real z[], real PD[], real& CJ, real rPar[], int iPar[]) {
            System* system = (System*)iPar;
            Tuple& N = system->N;
            Tuple M = N.Double();
            Matrix Jac_ptr(M,PD);
            Matrix Jac = system->Jacobian(time, Function(N,y), Function(N,z), CJ);
            Jac_ptr &= Jac;
        }
        
        void rework() {
            int MAXORD = maxOrd();
            int NEQ = N.Pr();
            int ML = lowerBand();
            int MU = upperBand();
            
            if ( banded() and needJacobian() ) // LRW .GE. 40+(MAXORD+4)*NEQ+(2*ML+MU+1)*NEQ
                rWork.resize( 40 + (MAXORD+4)*NEQ + (2*ML+MU+1)*NEQ);
            else if ( banded() ) // LRW .GE. 40+(MAXORD+4)*NEQ+(2*ML+MU+1)*NEQ+2*(NEQ/(ML+MU+1)+1)
                rWork.resize( 40 + (MAXORD+4)*NEQ + (2*ML+MU+1)*NEQ + 2*(NEQ/(ML+MU+1)+1));
            else // LRW .GE. 40+(MAXORD+4)*NEQ+NEQ**2
                rWork.resize( 40 + (MAXORD+4)*NEQ + NEQ*NEQ);
        }
        
    protected:
        Tuple N;
        System(Tuple N = Tuple(1,1)) : N(N), Y(Function(N)), Z(Function(N)), YPrime(Z) {
            jacobianSet = true;
            stepSet = true;
            hasStarted = false;
            info.assign(15,0);
            currentTime = 0;
            finalTime = 0;
            relativeTolerance = 1e-6;
            absoluteTolerance = 1e-6;
            outputStatusFlag = 0;
            iWork.assign(20 + N.Pr(), 0);
            rework();
        }
        
        Function NewFunction() {
            return Function(N);
        }
        
        Matrix NewMatrix() {
            Tuple M;
            if ( banded() ) {
                M = N;
                M.insert(M.begin(), 1+upperBand()+2*lowerBand());
            }
            else
                M = N.Double();
            return Matrix(M);
        }
        
    public:
        Function Y;
        Function Z; // Z = D(Y)(t)
        Function& YPrime;
        real currentTime;
        real finalTime;
        real relativeTolerance;
        real absoluteTolerance;
        int outputStatusFlag;
        virtual Function Residue(const real& time, const Function& y, const Function& yPrime) = 0;
#define BLANK_STEP void Step() { stepSet=false;}
        virtual BLANK_STEP;
#define BLANK_JACOBIAN Matrix Jacobian(const real&, const Function&, const Function&, real&) { jacobianSet=false; return NewMatrix();}
        virtual BLANK_JACOBIAN;
        
        void DASSL() {
            Step();
            Jacobian(currentTime, Y, Z, finalTime);
            if ( jacobianSet ) specifyJacobian();
            if ( stepSet ) iterateSlowly();
            do {
                DASSLtick();
                if ( not iteratingSlowly() )
                    break;
                Step();
            } while ( currentTime < finalTime );
        }
        
        void operator()() {
            DASSL();
        }
        
        void DASSLtick() {
            ddassl_(System::Residue_Wrapper, Y.N.Pr(), currentTime, Y.pointer(), Z.pointer(), finalTime, &info.front(), relativeTolerance, absoluteTolerance, outputStatusFlag, &rWork.front(), rWork.size(), &iWork.front(), iWork.size(), 0, (int*)this, System::Jacobian_Wrapper);
        }
        
        void notFirstCall() {
            // Huh?
        }
        
        void tolerencesAreVectors() {
            info[1] = 1;
        }
        
        bool areTolerencesVectors() {
            return info[1];
        }
        
        void iterateSlowly() {
            info[2] = 1;
        }
        
        bool iteratingSlowly() {
            return info[2];
        }
        
        void setStopping(real TSTOP) {
            info[3] = 1;
            rWork[0] = TSTOP;
        }
        
        real stoppingAt() {
            if ( info[3] )
                return rWork[0];
            else
                return 0.;
        }
        
        void specifyJacobian() {
            info[4] = 1;
            rework();
        }
        
        bool needJacobian() {
            return info[4];
        }
        
        void setBanded(int ML, int MU) {
            info[5] = 1;
            iWork[0] = ML;
            iWork[1] = MU;
            rework();
        }
        
        void setBanded(std::pair<int,int> M) {
            setBanded(M.first, M.second);
        }
        
        bool banded() {
            return info[5];
        }
        
        std::pair<int, int> getBands() {
            if ( info[5] )
                return std::pair<int,int>(iWork[0], iWork[1]);
            else
                return std::pair<int,int>(-1, -1);
        }
        
        int lowerBand() {
            if ( info[5] )
                return iWork[0];
            else
                return -1;
        }
        
        int upperBand() {
            if ( info[5] )
                return iWork[1];
            else
                return -1;
        }
        
        int bandWidth() {
            if ( info[5] )
                return iWork[0]+iWork[1];
            else
                return -1;
        }
        
        int maxBand() {
            if ( info[5] ) {
                if ( iWork[0] > iWork[1] )
                    return iWork[0];
                else
                    return iWork[1];
            }
            else
                return -1;
        }
        
        void setMaxStep(real hMax) {
            info[6] = 1;
            rWork[1] = hMax;
        }
        
        void setH(real hMax) {
            setMaxStep(hMax);
        }
        
        void seth(real hMax) {
            setMaxStep(hMax);
        }
        
        real maxStep() {
            if ( info[6] )
                return rWork[1];
            else
                return 0.;
        }
        
        real timeStep() {
            return rWork[6];
        }
        
        void setInitialStep(real h0) {
            info[7] = 1;
            rWork[2] = h0;
        }
        
        bool initialStep() {
            if ( info[7] )
                return rWork[2];
            else
                return 0.;
        }
        
        void setMaxOrd(int MAXORD) {
            info[8] = 1;
            iWork[2] = MAXORD;
            rework();
        }
        
        long maxOrd() {
            if ( info[8] )
                return iWork[2];
            else
                return 5;
        }
        
        void nonNegative() {
            info[9] = 1;
        }
        
        bool isNonNegative() {
            return info[9];
        }
        
        void calcYprime() {
            info[10] = 1;
        }
        
        bool calcingYprime() {
            return info[10];
        }
        
        long resCalls() {
            return iWork[11];
        }
        
        long jacCalls() {
            return iWork[12];
        }
    };

}

#endif

// DASSL documentation on info, with indices rescaled for C
//C  INFO(*) -- Use the INFO array to give the code more details about
//C         how you want your problem solved.  This array should be
//C         dimensioned of length 15, though DDASSL uses only the first
//C         eleven entries.  You must respond to all of the following
//C         items, which are arranged as questions.  The simplest use
//C         of the code corresponds to answering all questions as yes,
//C         i.e. setting all entries of INFO to 0.
//C
//C       INFO(0) - This parameter enables the code to initialize
//C              itself. You must set it to indicate the start of every
//C              new problem.
//C
//C          **** Is this the first call for this problem ...
//C                Yes - Set INFO(0) = 0
//C                 No - Not applicable here.
//C                      See below for continuation calls.  ****
//C
//C       INFO(1) - How much accuracy you want of your solution
//C              is specified by the error tolerances RTOL and ATOL.
//C              The simplest use is to take them both to be scalars.
//C              To obtain more flexibility, they can both be vectors.
//C              The code must be told your choice.
//C
//C          **** Are both error tolerances RTOL, ATOL scalars ...
//C                Yes - Set INFO(1) = 0
//C                      and input scalars for both RTOL and ATOL
//C                 No - Set INFO(1) = 1
//C                      and input arrays for both RTOL and ATOL ****
//C
//C       INFO(2) - The code integrates from T in the direction
//C              of TOUT by steps. If you wish, it will return the
//C              computed solution and derivative at the next
//C              intermediate step (the intermediate-output mode) or
//C              TOUT, whichever comes first. This is a good way to
//C              proceed if you want to see the behavior of the solution.
//C              If you must have solutions at a great many specific
//C              TOUT points, this code will compute them efficiently.
//C
//C          **** Do you want the solution only at
//C                TOUT (and not at the next intermediate step) ...
//C                 Yes - Set INFO(2) = 0
//C                  No - Set INFO(2) = 1 ****
//C
//C       INFO(3) - To handle solutions at a great many specific
//C              values TOUT efficiently, this code may integrate past
//C              TOUT and interpolate to obtain the result at TOUT.
//C              Sometimes it is not possible to integrate beyond some
//C              point TSTOP because the equation changes there or it is
//C              not defined past TSTOP. Then you must tell the code
//C              not to go past.
//C
//C           **** Can the integration be carried out without any
//C                restrictions on the independent variable T ...
//C                 Yes - Set INFO(3)=0
//C                  No - Set INFO(3)=1
//C                       and define the stopping point TSTOP by
//C                       setting RWORK(0)=TSTOP ****
//C
//C       INFO(4) - To solve differential/algebraic problems it is
//C              necessary to use a matrix of partial derivatives of the
//C              system of differential equations. If you do not
//C              provide a subroutine to evaluate it analytically (see
//C              description of the item JAC in the call list), it will
//C              be approximated by numerical differencing in this code.
//C              although it is less trouble for you to have the code
//C              compute partial derivatives by numerical differencing,
//C              the solution will be more reliable if you provide the
//C              derivatives via JAC. Sometimes numerical differencing
//C              is cheaper than evaluating derivatives in JAC and
//C              sometimes it is not - this depends on your problem.
//C
//C           **** Do you want the code to evaluate the partial
//C                derivatives automatically by numerical differences ...
//C                   Yes - Set INFO(4)=0
//C                    No - Set INFO(4)=1
//C                  and provide subroutine JAC for evaluating the
//C                  matrix of partial derivatives ****
//C
//C       INFO(5) - DDASSL will perform much better if the matrix of
//C              partial derivatives, DG/DY + CJ*DG/DYPRIME,
//C              (here CJ is a scalar determined by DDASSL)
//C              is banded and the code is told this. In this
//C              case, the storage needed will be greatly reduced,
//C              numerical differencing will be performed much cheaper,
//C              and a number of important algorithms will execute much
//C              faster. The differential equation is said to have
//C              half-bandwidths ML (lower) and MU (upper) if equation i
//C              involves only unknowns Y(J) with
//C                             I-ML .LE. J .LE. I+MU
//C              for all I=0,1,2,...,NEQ-1. Thus, ML and MU are the widths
//C              of the lower and upper parts of the band, respectively,
//C              with the main diagonal being excluded. If you do not
//C              indicate that the equation has a banded matrix of partial
//C              derivatives, the code works with a full matrix of NEQ**2
//C              elements (stored in the conventional way). Computations
//C              with banded matrices cost less time and storage than with
//C              full matrices if 2*ML+MU .LT. NEQ. If you tell the
//C              code that the matrix of partial derivatives has a banded
//C              structure and you want to provide subroutine JAC to
//C              compute the partial derivatives, then you must be careful
//C              to store the elements of the matrix in the special form
//C              indicated in the description of JAC.
//C
//C          **** Do you want to solve the problem using a full
//C               (dense) matrix (and not a special banded
//C               structure) ...
//C                Yes - Set INFO(5)=0
//C                 No - Set INFO(5)=1
//C                       and provide the lower (ML) and upper (MU)
//C                       bandwidths by setting
//C                       IWORK(0)=ML
//C                       IWORK(1)=MU ****
//C
//C
//C        INFO(6) -- You can specify a maximum (absolute value of)
//C              stepsize, so that the code
//C              will avoid passing over very
//C              large regions.
//C
//C          ****  Do you want the code to decide
//C                on its own maximum stepsize?
//C                Yes - Set INFO(6)=0
//C                 No - Set INFO(6)=1
//C                      and define HMAX by setting
//C                      RWORK(1)=HMAX ****
//C
//C        INFO(7) -- Differential/algebraic problems
//C              may occaisionally suffer from
//C              severe scaling difficulties on the
//C              first step. If you know a great deal
//C              about the scaling of your problem, you can
//C              help to alleviate this problem by
//C              specifying an initial stepsize HO.
//C
//C          ****  Do you want the code to define
//C                its own initial stepsize?
//C                Yes - Set INFO(7)=0
//C                 No - Set INFO(7)=1
//C                      and define HO by setting
//C                      RWORK(2)=HO ****
//C
//C        INFO(8) -- If storage is a severe problem,
//C              you can save some locations by
//C              restricting the maximum order MAXORD.
//C              the default value is 5. for each
//C              order decrease below 5, the code
//C              requires NEQ fewer locations, however
//C              it is likely to be slower. In any
//C              case, you must have 1 .LE. MAXORD .LE. 5
//C          ****  Do you want the maximum order to
//C                default to 5?
//C                Yes - Set INFO(8)=0
//C                 No - Set INFO(8)=1
//C                      and define MAXORD by setting
//C                      IWORK(2)=MAXORD ****
//C
//C        INFO(9) --If you know that the solutions to your equations
//C               will always be nonnegative, it may help to set this
//C               parameter. However, it is probably best to
//C               try the code without using this option first,
//C               and only to use this option if that doesn't
//C               work very well.
//C           ****  Do you want the code to solve the problem without
//C                 invoking any special nonnegativity constraints?
//C                  Yes - Set INFO(9)=0
//C                   No - Set INFO(9)=1
//C
//C        INFO(10) --DDASSL normally requires the initial T,
//C               Y, and YPRIME to be consistent. That is,
//C               you must have G(T,Y,YPRIME) = 0 at the initial
//C               time. If you do not know the initial
//C               derivative precisely, you can let DDASSL try
//C               to compute it.
//C          ****   Are the initialHE INITIAL T, Y, YPRIME consistent?
//C                 Yes - Set INFO(10) = 0
//C                  No - Set INFO(10) = 1,
//C                       and set YPRIME to an initial approximation
//C                       to YPRIME.  (If you have no idea what
//C                       YPRIME should be, set it to zero. Note
//C                       that the initial Y should be such
//C                       that there must exist a YPRIME so that
//C                       G(T,Y,YPRIME) = 0.)
//C  LRW -- Set it to the declared length of the RWORK array.
//C               You must have
//C                    LRW .GE. 40+(MAXORD+4)*NEQ+NEQ**2
//C               for the full (dense) JACOBIAN case (when INFO(5)=0), or
//C                    LRW .GE. 40+(MAXORD+4)*NEQ+(2*ML+MU+1)*NEQ
//C               for the banded user-defined JACOBIAN case
//C               (when INFO(4)=1 and INFO(5)=1), or
//C                     LRW .GE. 40+(MAXORD+4)*NEQ+(2*ML+MU+1)*NEQ
//C                           +2*(NEQ/(ML+MU+1)+1)
//C               for the banded finite-difference-generated JACOBIAN case
//C               (when INFO(4)=0 and INFO(5)=1)
//C               *** INFO(7)=0 -- Full (dense) matrix ***
//C                   Give PD a first dimension of NEQ.
//C                   When you evaluate the (non-zero) partial derivative
//C                   of equation I with respect to variable J, you must
//C                   store it in PD according to
//C                   PD(I,J) = "DG(I)/DY(J)+CJ*DG(I)/DYPRIME(J)"
//C               *** INFO(7)=1 -- Banded JACOBIAN with ML lower and MU
//C                   upper diagonal bands (refer to INFO(7) description
//                                          C                   of ML and MU) ***
//C                   Give PD a first dimension of 2*ML+MU+1.
//C                   when you evaluate the (non-zero) partial derivative
//C                   of equation I with respect to variable J, you must
//C                   store it in PD according to
//C                   IROW = I - J + ML + MU + 1
//C                   PD(IROW,J) = "DG(I)/DY(J)+CJ*DG(I)/DYPRIME(J)"
