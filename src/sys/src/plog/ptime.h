/* $Id: ptime.h,v 1.15 1995/12/07 19:35:20 bsmith Exp balay $ */
/*
     Low cost access to system time. This, in general, should not
  be included in user programs.
*/

#if !defined(__PTIME_PACKAGE)
#define __PTIME_PACKAGE

#if defined(PARCH_IRIX) && defined(__cplusplus)
struct timeval {
        long    tv_sec;         /* seconds */
        long    tv_usec;        /* and microseconds */
};

struct timezone {
        int     tz_minuteswest; /* minutes west of Greenwich */
        int     tz_dsttime;     /* type of dst correction */
};
extern "C" {
extern int gettimeofday(struct timeval *, struct timezone *);
}
#else
#include <sys/types.h>
#include <sys/time.h>
#endif
#if defined(PARCH_sun4) && !defined(__cplusplus)
extern int gettimeofday(struct timeval *, struct timezone *);
#endif
#if (defined(PARCH_solaris) || defined(PARCH_sun4)) && defined(__cplusplus)
extern "C" {
extern int gettimeofday(struct timeval *, struct timezone *);
}
#endif

/*
    Macros for timing. In the future some of these may be 
    machine dependent versions. The are not intended for PETSc users!
*/

/*
   PetscTime - Returns the current time of day in seconds.  

   Output Parameter:
.  v - time counter

   Synopsis:
   PetscTime(double v)

   Usage: 
     double v;
     PetscTime(v);
     .... perform some calculation ...
     printf("Time for operation %g\n",v);

   Notes:
   Since the PETSc libraries incorporate timing of phases and operations, 
   PetscTime() is intended only for timing of application codes.  
   The options database commands -log, -log_summary, and -log_all activate
   PETSc library timing.  See the users manual for further details.

.seealso:  PetscTimeSubtract(), PetscTimeAdd()

.keywords:  Petsc, time
*/


/*
   PetscTimeSubtract - Subtracts the current time of day (in seconds) from
   the value v.  

   Input Parameter:
.  v - time counter

   Output Parameter:
.  v - time counter (v = v - current time)

   Synopsis:
   PetscTimeSubtract(double v)

   Notes:
   Since the PETSc libraries incorporate timing of phases and operations, 
   PetscTimeSubtract() is intended only for timing of application codes.  
   The options database commands -log, -log_summary, and -log_all activate
   PETSc library timing.  See the users manual for further details.

.seealso:  PetscTime(), PetscTimeAdd()

.keywords:  Petsc, time, subtract
*/


/*
   PetscTimeAdd - Adds the current time of day (in seconds) to the value v.  

   Input Parameter:
.  v - time counter

   Output Parameter:
.  v - time counter (v = v + current time)

   Synopsis:
   PetscTimeAdd(double v)

   Notes:
   Since the PETSc libraries incorporate timing of phases and operations, 
   PetscTimeAdd() is intended only for timing of application codes.  
   The options database commands -log, -log_summary, and -log_all activate
   PETSc library timing.  See the users manual for further details.

.seealso:  PetscTime(), PetscTimeSubtract()

.keywords:  Petsc, time, add
*/
#if defined(PARCH_rs6000)
struct timestruc_t {
  unsigned long tv_sec;   /* seconds              */
  long          tv_nsec;  /* and nanoseconds      */
};
extern UTP_readTime(struct timestruc_t *);

#define PetscTime(v)         {static struct  timestruc_t _tp; \
                             UTP_readTime(&_tp); \
                             (v)=((double)_tp.tv_sec)+(1.0e-9)*(_tp.tv_nsec);}

#define PetscTimeSubtract(v) {static struct timestruc_t  _tp; \
                             UTP_readTime(&_tp); \
                             (v)-=((double)_tp.tv_sec)+(1.0e-9)*(_tp.tv_nsec);}

#define PetscTimeAdd(v)      {static struct timestruc_t  _tp; \
                             UTP_readTime(&_tp); \
                             (v)+=((double)_tp.tv_sec)+(1.0e-9)*(_tp.tv_nsec);}

#else

#define PetscTime(v)         {static struct timeval _tp; \
                             gettimeofday(&_tp,(struct timezone *)0);\
                             (v)=((double)_tp.tv_sec)+(1.0e-6)*(_tp.tv_usec);}

#define PetscTimeSubtract(v) {static struct timeval _tp; \
                             gettimeofday(&_tp,(struct timezone *)0);\
                             (v)-=((double)_tp.tv_sec)+(1.0e-6)*(_tp.tv_usec);}

#define PetscTimeAdd(v)      {static struct timeval _tp; \
                             gettimeofday(&_tp,(struct timezone *)0);\
                             (v)+=((double)_tp.tv_sec)+(1.0e-6)*(_tp.tv_usec);}
#endif
#endif


