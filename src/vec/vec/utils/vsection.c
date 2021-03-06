/*
   This file contains routines for section object operations on Vecs
*/
#include <petsc-private/isimpl.h>   /*I  "petscvec.h"   I*/
#include <petsc-private/vecimpl.h>   /*I  "petscvec.h"   I*/

#undef __FUNCT__
#define __FUNCT__ "PetscSectionVecView_ASCII"
PetscErrorCode PetscSectionVecView_ASCII(PetscSection s, Vec v, PetscViewer viewer)
{
  PetscScalar    *array;
  PetscInt       p, i;
  PetscMPIInt    rank;
  PetscErrorCode ierr;

  PetscFunctionBegin;
  if (s->atlasLayout.numDof != 1) SETERRQ1(PETSC_COMM_SELF, PETSC_ERR_SUP, "Cannot handle %d dof in a uniform section", s->atlasLayout.numDof);
  ierr = MPI_Comm_rank(PetscObjectComm((PetscObject)viewer), &rank);CHKERRQ(ierr);
  ierr = VecGetArray(v, &array);CHKERRQ(ierr);
  ierr = PetscViewerASCIISynchronizedAllow(viewer, PETSC_TRUE);CHKERRQ(ierr);
  ierr = PetscViewerASCIISynchronizedPrintf(viewer, "Process %d:\n", rank);CHKERRQ(ierr);
  for (p = 0; p < s->atlasLayout.pEnd - s->atlasLayout.pStart; ++p) {
    if ((s->bc) && (s->bc->atlasDof[p] > 0)) {
      PetscInt b;

      ierr = PetscViewerASCIISynchronizedPrintf(viewer, "  (%4d) dim %2d offset %3d", p+s->atlasLayout.pStart, s->atlasDof[p], s->atlasOff[p]);CHKERRQ(ierr);
      for (i = s->atlasOff[p]; i < s->atlasOff[p]+s->atlasDof[p]; ++i) {
        PetscScalar v = array[i];
#if defined(PETSC_USE_COMPLEX)
        if (PetscImaginaryPart(v) > 0.0) {
          ierr = PetscViewerASCIISynchronizedPrintf(viewer," %G + %G i", PetscRealPart(v), PetscImaginaryPart(v));CHKERRQ(ierr);
        } else if (PetscImaginaryPart(v) < 0.0) {
          ierr = PetscViewerASCIISynchronizedPrintf(viewer," %G - %G i", PetscRealPart(v),-PetscImaginaryPart(v));CHKERRQ(ierr);
        } else {
          ierr = PetscViewerASCIISynchronizedPrintf(viewer, " %G", PetscRealPart(v));CHKERRQ(ierr);
        }
#else
        ierr = PetscViewerASCIISynchronizedPrintf(viewer, " %G", v);CHKERRQ(ierr);
#endif
      }
      ierr = PetscViewerASCIISynchronizedPrintf(viewer, " constrained");CHKERRQ(ierr);
      for (b = 0; b < s->bc->atlasDof[p]; ++b) {
        ierr = PetscViewerASCIISynchronizedPrintf(viewer, " %d", s->bcIndices[s->bc->atlasOff[p]+b]);CHKERRQ(ierr);
      }
      ierr = PetscViewerASCIISynchronizedPrintf(viewer, "\n");CHKERRQ(ierr);
    } else {
      ierr = PetscViewerASCIISynchronizedPrintf(viewer, "  (%4d) dim %2d offset %3d", p+s->atlasLayout.pStart, s->atlasDof[p], s->atlasOff[p]);CHKERRQ(ierr);
      for (i = s->atlasOff[p]; i < s->atlasOff[p]+s->atlasDof[p]; ++i) {
        PetscScalar v = array[i];
#if defined(PETSC_USE_COMPLEX)
        if (PetscImaginaryPart(v) > 0.0) {
          ierr = PetscViewerASCIISynchronizedPrintf(viewer," %G + %G i", PetscRealPart(v), PetscImaginaryPart(v));CHKERRQ(ierr);
        } else if (PetscImaginaryPart(v) < 0.0) {
          ierr = PetscViewerASCIISynchronizedPrintf(viewer," %G - %G i", PetscRealPart(v),-PetscImaginaryPart(v));CHKERRQ(ierr);
        } else {
          ierr = PetscViewerASCIISynchronizedPrintf(viewer, " %G", PetscRealPart(v));CHKERRQ(ierr);
        }
#else
        ierr = PetscViewerASCIISynchronizedPrintf(viewer, " %G", v);CHKERRQ(ierr);
#endif
      }
      ierr = PetscViewerASCIISynchronizedPrintf(viewer, "\n");CHKERRQ(ierr);
    }
  }
  ierr = PetscViewerFlush(viewer);CHKERRQ(ierr);
  ierr = VecRestoreArray(v, &array);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "PetscSectionVecView"
PetscErrorCode PetscSectionVecView(PetscSection s, Vec v, PetscViewer viewer)
{
  PetscBool      isascii;
  PetscInt       f;
  PetscErrorCode ierr;

  PetscFunctionBegin;
  if (!viewer) {ierr = PetscViewerASCIIGetStdout(PetscObjectComm((PetscObject)v), &viewer);CHKERRQ(ierr);}
  PetscValidHeaderSpecific(v, VEC_CLASSID, 2);
  PetscValidHeaderSpecific(viewer, PETSC_VIEWER_CLASSID, 3);
  ierr = PetscObjectTypeCompare((PetscObject) viewer, PETSCVIEWERASCII, &isascii);CHKERRQ(ierr);
  if (isascii) {
    const char *name;

    ierr = PetscObjectGetName((PetscObject) v, &name);CHKERRQ(ierr);
    if (s->numFields) {
      ierr = PetscViewerASCIIPrintf(viewer, "%s with %d fields\n", name, s->numFields);CHKERRQ(ierr);
      for (f = 0; f < s->numFields; ++f) {
        ierr = PetscViewerASCIIPrintf(viewer, "  field %d with %d components\n", f, s->numFieldComponents[f]);CHKERRQ(ierr);
        ierr = PetscSectionVecView_ASCII(s->field[f], v, viewer);CHKERRQ(ierr);
      }
    } else {
      ierr = PetscViewerASCIIPrintf(viewer, "%s\n", name);CHKERRQ(ierr);
      ierr = PetscSectionVecView_ASCII(s, v, viewer);CHKERRQ(ierr);
    }
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "VecGetValuesSection"
PetscErrorCode VecGetValuesSection(Vec v, PetscSection s, PetscInt point, PetscScalar **values)
{
  PetscScalar    *baseArray;
  const PetscInt p = point - s->atlasLayout.pStart;
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = VecGetArray(v, &baseArray);CHKERRQ(ierr);
  *values = &baseArray[s->atlasOff[p]];
  ierr = VecRestoreArray(v, &baseArray);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "VecSetValuesSection"
/*@C
  VecSetValuesSection - Sets all the values associated with a given point, accoridng to the section, in the given Vec

  Not collective

  Input Parameters:
+ v - the Vec
. s - the organizing PetscSection
. point - the point
. values - the array of input values
- mode - the insertion mode, either ADD_VALUES or INSERT_VALUES

  Level: developer

  Note: This is similar to MatSetValuesStencil()

.seealso: PetscSection, PetscSectionCreate()
@*/
PetscErrorCode VecSetValuesSection(Vec v, PetscSection s, PetscInt point, PetscScalar values[], InsertMode mode)
{
  PetscScalar     *baseArray, *array;
  const PetscBool doInsert    = mode == INSERT_VALUES     || mode == INSERT_ALL_VALUES || mode == INSERT_BC_VALUES                          ? PETSC_TRUE : PETSC_FALSE;
  const PetscBool doInterior  = mode == INSERT_ALL_VALUES || mode == ADD_ALL_VALUES    || mode == INSERT_VALUES    || mode == ADD_VALUES    ? PETSC_TRUE : PETSC_FALSE;
  const PetscBool doBC        = mode == INSERT_ALL_VALUES || mode == ADD_ALL_VALUES    || mode == INSERT_BC_VALUES || mode == ADD_BC_VALUES ? PETSC_TRUE : PETSC_FALSE;
  const PetscInt  p           = point - s->atlasLayout.pStart;
  const PetscInt  orientation = 0; /* Needs to be included for use in closure operations */
  PetscInt        cDim        = 0;
  PetscErrorCode  ierr;

  PetscFunctionBegin;
  ierr  = PetscSectionGetConstraintDof(s, p, &cDim);CHKERRQ(ierr);
  ierr  = VecGetArray(v, &baseArray);CHKERRQ(ierr);
  array = &baseArray[s->atlasOff[p]];
  if (!cDim && doInterior) {
    if (orientation >= 0) {
      const PetscInt dim = s->atlasDof[p];
      PetscInt       i;

      if (doInsert) {
        for (i = 0; i < dim; ++i) array[i] = values[i];
      } else {
        for (i = 0; i < dim; ++i) array[i] += values[i];
      }
    } else {
      PetscInt offset = 0;
      PetscInt j      = -1, field, i;

      for (field = 0; field < s->numFields; ++field) {
        const PetscInt dim = s->field[field]->atlasDof[p]; /* PetscSectionGetFieldDof() */

        for (i = dim-1; i >= 0; --i) array[++j] = values[i+offset];
        offset += dim;
      }
    }
  } else if (cDim) {
    if (orientation >= 0) {
      const PetscInt dim  = s->atlasDof[p];
      PetscInt       cInd = 0, i;
      const PetscInt *cDof;

      ierr = PetscSectionGetConstraintIndices(s, point, &cDof);CHKERRQ(ierr);
      if (doInsert) {
        for (i = 0; i < dim; ++i) {
          if ((cInd < cDim) && (i == cDof[cInd])) {
            if (doBC) array[i] = values[i]; /* Constrained update */
            ++cInd;
            continue;
          }
          if (doInterior) array[i] = values[i]; /* Unconstrained update */
        }
      } else {
        for (i = 0; i < dim; ++i) {
          if ((cInd < cDim) && (i == cDof[cInd])) {
            if (doBC) array[i] += values[i]; /* Constrained update */
            ++cInd;
            continue;
          }
          if (doInterior) array[i] += values[i]; /* Unconstrained update */
        }
      }
    } else {
      /* TODO This is broken for add and constrained update */
      const PetscInt *cDof;
      PetscInt       offset  = 0;
      PetscInt       cOffset = 0;
      PetscInt       j       = 0, field;

      ierr = PetscSectionGetConstraintIndices(s, point, &cDof);CHKERRQ(ierr);
      for (field = 0; field < s->numFields; ++field) {
        const PetscInt dim  = s->field[field]->atlasDof[p];     /* PetscSectionGetFieldDof() */
        const PetscInt tDim = s->field[field]->bc->atlasDof[p]; /* PetscSectionGetFieldConstraintDof() */
        const PetscInt sDim = dim - tDim;
        PetscInt       cInd = 0, i ,k;

        for (i = 0, k = dim+offset-1; i < dim; ++i, ++j, --k) {
          if ((cInd < sDim) && (j == cDof[cInd+cOffset])) {++cInd; continue;}
          if (doInterior) array[j] = values[k];   /* Unconstrained update */
        }
        offset  += dim;
        cOffset += dim - tDim;
      }
    }
  }
  ierr = VecRestoreArray(v, &baseArray);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}
