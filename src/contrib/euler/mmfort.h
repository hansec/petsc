C
C  Include file for Fortran use of the MM (multi-model) package
C
#define MM           integer
#define MMType       integer
C
C  Various multimodels
C
      integer MMEULER, MMFP, MMHYBRID_EF1, MMHYBRID_E, 
     *        MMHYBRID_F, MMNEW 

      parameter (MMEULER = 0, MMFP = 1, MMHYBRID_EF1 = 2, 
     *           MMHYBRID_E = 3, MMHYBRID_F = 4, MMNEW = 5) 

