#include "../src/snes/impls/fas/fasimpls.h"

#undef __FUNCT__
#define __FUNCT__ "SNESFASGalerkinDefaultFunction"
/*
SNESFASGalerkinDefaultFunction

 */
PetscErrorCode SNESFASGalerkinDefaultFunction(SNES snes, Vec X, Vec F, void * ctx) {
  /* the Galerkin FAS function evalutation is defined as
   F^l(x^l) = I^l_0F^0(P^0_lx^l)
   */
  SNES       fassnes;
  SNES_FAS * fas;
  SNES_FAS * prevfas;
  SNES       prevsnes;
  Vec b_temp;
  PetscErrorCode ierr;
  PetscFunctionBegin;
  /* prolong to the fine level and evaluate there. */
  fassnes = (SNES)ctx;
  fas     = (SNES_FAS *)fassnes->data;
  prevsnes = fas->previous;
  prevfas = (SNES_FAS *)prevsnes->data;
  /* interpolate down the solution */
  ierr = MatInterpolate(prevfas->interpolate, X, prevfas->Xg);CHKERRQ(ierr);
  /* the RHS we care about is at the coarsest level */
  b_temp = prevsnes->vec_rhs;
  prevsnes->vec_rhs = PETSC_NULL;
  ierr = SNESComputeFunction(prevsnes, prevfas->Xg, prevfas->Fg);CHKERRQ(ierr);
  prevsnes->vec_rhs = b_temp;
  /* restrict up the function */
  ierr = MatRestrict(prevfas->restrct, prevfas->Fg, F);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}