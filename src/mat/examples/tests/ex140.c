
static char help[] = "Tests MatLoad(), MatZeroRowsColumns(), MatView() for MPIBAIJ.\n\n";

#include "petscmat.h"

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **args)
{
  Mat            A;
  PetscErrorCode ierr;
  char           file[PETSC_MAX_PATH_LEN];
  PetscBool      aij,sbaij,flg;
  PetscViewer    fd;
  const MatType  type = MATBAIJ;
  PetscInt       n = 7, idx[] = {1,5,6,8,9,12,15};
  Vec            b,x;
  IS             is;
  PetscInt       i;

  PetscInitialize(&argc,&args,(char *)0,help);
  ierr = PetscOptionsHasName(PETSC_NULL,"-aij",&aij);CHKERRQ(ierr);
  if (aij) type = MATAIJ;
  ierr = PetscOptionsHasName(PETSC_NULL,"-sbaij",&sbaij);CHKERRQ(ierr);
  if (sbaij) type = MATSBAIJ;

  ierr = PetscOptionsGetString(PETSC_NULL,"-f",file,PETSC_MAX_PATH_LEN,&flg);CHKERRQ(ierr);
  if (!flg) SETERRQ(PETSC_COMM_WORLD,1,"Must indicate binary file with the -f option");

  /* 
     Open binary file.  Note that we use FILE_MODE_READ to indicate
     reading from this file.
  */
  ierr = PetscViewerBinaryOpen(PETSC_COMM_WORLD,file,FILE_MODE_READ,&fd);CHKERRQ(ierr);

  /*
     Load the matrix; then destroy the viewer.
  */
  ierr = MatCreate(PETSC_COMM_WORLD,&A);CHKERRQ(ierr);
  ierr = MatSetType(A,type);CHKERRQ(ierr);
  ierr = MatLoad(A,fd);CHKERRQ(ierr);
  ierr = VecCreate(PETSC_COMM_WORLD,&b);CHKERRQ(ierr);
  ierr = VecLoad(b,fd);CHKERRQ(ierr);
  ierr = PetscViewerDestroy(fd);CHKERRQ(ierr);

  /* save original matrix and vector for testing with Matlab */
  ierr = VecView(b,PETSC_VIEWER_BINARY_WORLD);CHKERRQ(ierr);
  ierr = MatView(A,PETSC_VIEWER_BINARY_WORLD);CHKERRQ(ierr);

  ierr = ISCreateGeneral(PETSC_COMM_WORLD,n,idx,PETSC_COPY_VALUES,&is);CHKERRQ(ierr);
  ierr = VecDuplicate(b,&x);CHKERRQ(ierr);
  ierr = VecSet(x,0.0);CHKERRQ(ierr);
  for (i=0; i<n; i++) {
    ierr = VecSetValue(x,idx[i],1.0,INSERT_VALUES);CHKERRQ(ierr);
  }
  ierr = VecAssemblyBegin(x);CHKERRQ(ierr);
  ierr = VecAssemblyEnd(x);CHKERRQ(ierr);
  ierr = VecView(x,PETSC_VIEWER_BINARY_WORLD);CHKERRQ(ierr);

  ierr = MatZeroRowsColumnsIS(A,is,2.0,x,b);CHKERRQ(ierr);
  /*
     Save the matrix and vector; then destroy the viewer.
  */
  ierr = ISView(is,PETSC_VIEWER_BINARY_WORLD);CHKERRQ(ierr);
  ierr = VecView(b,PETSC_VIEWER_BINARY_WORLD);CHKERRQ(ierr);
  ierr = MatView(A,PETSC_VIEWER_BINARY_WORLD);CHKERRQ(ierr);
  ierr = VecDestroy(x);CHKERRQ(ierr);
  ierr = VecDestroy(b);CHKERRQ(ierr);
  ierr = ISDestroy(is);CHKERRQ(ierr);
  ierr = MatDestroy(A);CHKERRQ(ierr);
  ierr = PetscFinalize();
  return 0;
}