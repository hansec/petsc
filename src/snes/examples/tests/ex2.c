#ifndef lint
static char vcid[] = "$Id: ex2.c,v 1.43 1996/04/15 20:44:02 bsmith Exp curfman $";
#endif

static char *help="Uses Newton's method to solve a two-variable system.\n";

#include "snes.h"

int  FormJacobian(SNES,Vec,Mat*,Mat*,MatStructure*,void*),
     FormFunction(SNES,Vec,Vec,void*),
     Monitor(SNES,int,double,void*);

int main( int argc, char **argv )
{
  SNES         snes;               /* SNES context */
  Vec          x,r;                /* solution, residual vectors */
  Mat          J;                  /* Jacobian matrix */
  int          ierr, its;
  Scalar       pfive = .5;

  PetscInitialize( &argc, &argv,(char *)0,help );

  /* Set up data structures */
  ierr = VecCreateSeq(MPI_COMM_SELF,2,&x); CHKERRA(ierr);
  ierr = VecDuplicate(x,&r); CHKERRA(ierr);
  ierr = MatCreateSeqDense(MPI_COMM_SELF,2,2,PETSC_NULL,&J); CHKERRA(ierr);

  /* Create nonlinear solver */
  ierr = SNESCreate(MPI_COMM_WORLD,SNES_NONLINEAR_EQUATIONS,&snes); CHKERRA(ierr);

  /* Set various routines and options */
  ierr = SNESSetFunction(snes,r,FormFunction,0);CHKERRA(ierr);
  ierr = SNESSetJacobian(snes,J,J,FormJacobian,0); CHKERRA(ierr);
  ierr = SNESSetMonitor(snes,Monitor,0); CHKERRA(ierr);
  ierr = SNESSetFromOptions(snes); CHKERRA(ierr);

  /* set an initial guess of .5 */
  ierr = VecSet(&pfive,x); CHKERRA(ierr);

  /* Solve nonlinear system */
  ierr = SNESSolve(snes,x,&its); CHKERRA(ierr);
  PetscPrintf(MPI_COMM_SELF,"number of Newton iterations = %d\n\n", its);

  /* Free data structures */
  ierr = VecDestroy(x); CHKERRA(ierr);  ierr = VecDestroy(r); CHKERRA(ierr);
  ierr = MatDestroy(J); CHKERRA(ierr);  ierr = SNESDestroy(snes); CHKERRA(ierr);
  PetscFinalize();
  return 0;
}/* --------------------  Evaluate Function F(x) --------------------- */
int FormFunction(SNES snes,Vec x,Vec f,void *dummy )
{
  int    ierr;
  Scalar *xx, *ff;

  ierr = VecGetArray(x,&xx); CHKERRQ(ierr);
  ierr = VecGetArray(f,&ff); CHKERRQ(ierr);
  ff[0] = xx[0]*xx[0] + xx[0]*xx[1] - 3.0;
  ff[1] = xx[0]*xx[1] + xx[1]*xx[1] - 6.0;
  ierr = VecRestoreArray(x,&xx); CHKERRQ(ierr);
  ierr = VecRestoreArray(f,&ff); CHKERRQ(ierr); 
  return 0;
}/* --------------------  Evaluate Jacobian F'(x) -------------------- */
int FormJacobian(SNES snes,Vec x,Mat *jac,Mat *B,MatStructure *flag,void *dummy)
{
  Scalar *xx, A[4];
  int    ierr, idx[2] = {0,1};

  ierr = VecGetArray(x,&xx); CHKERRQ(ierr);
  A[0] = 2.0*xx[0] + xx[1]; A[1] = xx[0];
  A[2] = xx[1]; A[3] = xx[0] + 2.0*xx[1];
  ierr = MatSetValues(*jac,2,idx,2,idx,A,INSERT_VALUES); CHKERRQ(ierr);
  *flag = DIFFERENT_NONZERO_PATTERN;
  ierr = VecRestoreArray(x,&xx); CHKERRQ(ierr);
  ierr = MatAssemblyBegin(*jac,FINAL_ASSEMBLY); CHKERRQ(ierr);
  ierr = MatAssemblyEnd(*jac,FINAL_ASSEMBLY); CHKERRQ(ierr);
  return 0;
}/* --------------------  User-defined monitor ----------------------- */
int Monitor(SNES snes,int its,double fnorm,void *dummy)
{
  int      ierr;
  Vec      x;
  MPI_Comm comm;

  PetscObjectGetComm((PetscObject)snes,&comm);
  if (fnorm > 1.e-9 || fnorm == 0.0) {
    PetscPrintf(comm, "iter = %d, SNES Function norm %g \n",its,fnorm);
  }
  else if (fnorm > 1.e-11){
    PetscPrintf(comm, "iter = %d, SNES Function norm %5.3e \n",its,fnorm);
  }
  else {
    PetscPrintf(comm, "iter = %d, SNES Function norm < 1.e-11\n",its);
  }
  ierr = SNESGetSolution(snes,&x); CHKERRQ(ierr);
  ierr = VecView(x,STDOUT_VIEWER_SELF); CHKERRQ(ierr);
  return 0;
}
