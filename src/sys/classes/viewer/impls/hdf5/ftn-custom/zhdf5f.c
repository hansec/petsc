#include <petsc-private/fortranimpl.h>
#include <petscviewerhdf5.h>

#if defined(PETSC_HAVE_FORTRAN_CAPS)
#define petscviewerhdf5open_     PETSCVIEWERHDF5OPEN
#elif !defined(PETSC_HAVE_FORTRAN_UNDERSCORE)
#define petscviewerhdf5open_     petscviewerhdf5open
#endif

PETSC_EXTERN void PETSC_STDCALL petscviewerhdf5open_(MPI_Comm *comm,CHAR name PETSC_MIXED_LEN(len),PetscFileMode *type,
                           PetscViewer *binv,PetscErrorCode *ierr PETSC_END_LEN(len))
{
  char *c1;
  FIXCHAR(name,len,c1);
  *ierr = PetscViewerHDF5Open(MPI_Comm_f2c(*(MPI_Fint*)&*comm),c1,*type,binv);
  FREECHAR(name,c1);
}

