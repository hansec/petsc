#define PETSCKSP_DLL

/*
  This file defines a "solve the problem redistributely on each subgroup of processor" preconditioner.
*/
#include "private/pcimpl.h"     /*I "petscpc.h" I*/
#include "petscksp.h"

typedef struct {
  KSP          ksp;
  Vec          x,b;
  Mat          mat;   
  VecScatter   scatter;
} PC_Redistribute;

#undef __FUNCT__  
#define __FUNCT__ "PCView_Redistribute"
static PetscErrorCode PCView_Redistribute(PC pc,PetscViewer viewer)
{
  PC_Redistribute *red = (PC_Redistribute*)pc->data;
  PetscErrorCode  ierr;
  PetscTruth      iascii,isstring;

  PetscFunctionBegin;
  ierr = PetscTypeCompare((PetscObject)viewer,PETSC_VIEWER_ASCII,&iascii);CHKERRQ(ierr);
  ierr = PetscTypeCompare((PetscObject)viewer,PETSC_VIEWER_STRING,&isstring);CHKERRQ(ierr);
  if (iascii) {
    ierr = PetscViewerASCIIPrintf(viewer,"  Redistribute preconditioner: \n");CHKERRQ(ierr);
    ierr = KSPView(red->ksp,viewer);CHKERRQ(ierr);
  } else if (isstring) {
    ierr = PetscViewerStringSPrintf(viewer," Redistribute preconditioner");CHKERRQ(ierr);
    ierr = KSPView(red->ksp,viewer);CHKERRQ(ierr);
  } else {
    SETERRQ1(PETSC_ERR_SUP,"Viewer type %s not supported for PC redistribute",((PetscObject)viewer)->type_name);
  }
  PetscFunctionReturn(0);
}

#include "private/matimpl.h"        /*I "petscmat.h" I*/
#undef __FUNCT__  
#define __FUNCT__ "PCSetUp_Redistribute"
static PetscErrorCode PCSetUp_Redistribute(PC pc)
{
  PC_Redistribute   *red = (PC_Redistribute*)pc->data;
  PetscErrorCode    ierr;
  const char        *prefix;
  MPI_Comm          comm;
  PetscInt          rstart,rend,i,nz,cnt,*rows,ncnt;
  PetscMap          *map,*nmap;
  PetscMPIInt       size,rank,imdex,tag,n;
  PetscInt          *source = PETSC_NULL;
  PetscMPIInt       *nprocs = PETSC_NULL,nrecvs;
  PetscInt          j,nsends;
  PetscInt          *owner = PETSC_NULL,*starts = PETSC_NULL,count,slen;
  PetscInt          *rvalues,*svalues,recvtotal;
  PetscMPIInt       *onodes1,*olengths1;
  MPI_Request       *send_waits = PETSC_NULL,*recv_waits = PETSC_NULL;
  MPI_Status        recv_status,*send_status;

  PetscFunctionBegin;
  ierr = PetscObjectGetComm((PetscObject)pc,&comm);CHKERRQ(ierr);
  ierr = MPI_Comm_size(comm,&size);CHKERRQ(ierr);
  ierr = MPI_Comm_rank(comm,&rank);CHKERRQ(ierr);

  if (!pc->setupcalled) {
    if (!red->ksp) {
      ierr = KSPCreate(comm,&red->ksp);CHKERRQ(ierr);
      ierr = PetscObjectIncrementTabLevel((PetscObject)red->ksp,(PetscObject)pc,1);CHKERRQ(ierr);
      ierr = PetscLogObjectParent(pc,red->ksp);CHKERRQ(ierr);
      ierr = PCGetOptionsPrefix(pc,&prefix);CHKERRQ(ierr);
      ierr = KSPSetOptionsPrefix(red->ksp,prefix);CHKERRQ(ierr); 
      ierr = KSPAppendOptionsPrefix(red->ksp,"redistribute_");CHKERRQ(ierr); 
    }
  } else SETERRQ(PETSC_ERR_SUP,"Cannot yet re-setup a redistribute KSP");

  ierr = MatGetOwnershipRange(pc->mat,&rstart,&rend);CHKERRQ(ierr);
  for (i=rstart; i<rend; i++) {
    ierr = MatGetRow(pc->mat,i,&nz,PETSC_NULL,PETSC_NULL);CHKERRQ(ierr);
    if (nz > 1) cnt++;
  }
  ierr = PetscMalloc(cnt*sizeof(PetscInt),&rows);CHKERRQ(ierr);
  cnt  = 0;  
  for (i=rstart; i<rend; i++) {
    ierr = MatGetRow(pc->mat,i,&nz,PETSC_NULL,PETSC_NULL);CHKERRQ(ierr);
    if (nz > 1) rows[cnt++] = i;
  }
  ierr = PetscMalloc(sizeof(PetscMap),&map);CHKERRQ(ierr);
  ierr = PetscMapInitialize(comm,map);CHKERRQ(ierr);
  ierr = PetscMapSetLocalSize(map,cnt);CHKERRQ(ierr);
  ierr = PetscMapSetBlockSize(map,1);CHKERRQ(ierr);
  ierr = PetscMapSetUp(map);CHKERRQ(ierr);
  rstart = map->rstart;
  rend   = map->rend;

  ierr = PetscMalloc(sizeof(PetscMap),&nmap);CHKERRQ(ierr);
  ierr = PetscMapInitialize(comm,nmap);CHKERRQ(ierr);
  ierr = MPI_Allreduce(&cnt,&ncnt,1,MPIU_INT,MPI_SUM,comm);CHKERRQ(ierr);
  ierr = PetscMapSetSize(nmap,ncnt);CHKERRQ(ierr);
  ierr = PetscMapSetBlockSize(nmap,1);CHKERRQ(ierr);
  ierr = PetscMapSetUp(nmap);CHKERRQ(ierr);

  /*  this code is taken from VecScatterCreate_PtoS() */
  /*  count number of contributors to each processor */
  ierr = PetscMalloc2(size,PetscMPIInt,&nprocs,cnt,PetscInt,&owner);CHKERRQ(ierr);
  ierr = PetscMemzero(nprocs,size*sizeof(PetscMPIInt));CHKERRQ(ierr);
  j      = 0;
  nsends = 0;
  for (i=rstart; i<rend; i++) {
    if (i < nmap->range[j]) j = 0;
    for (; j<size; j++) {
      if (i < nmap->range[j+1]) {
        if (!nprocs[j]++) nsends++;
        owner[i] = j; 
        break;
      }
    }
  }
  /* inform other processors of number of messages and max length*/
  ierr = PetscGatherNumberOfMessages(comm,PETSC_NULL,nprocs,&nrecvs);CHKERRQ(ierr);
  ierr = PetscGatherMessageLengths(comm,nsends,nrecvs,nprocs,&onodes1,&olengths1);CHKERRQ(ierr);
  ierr = PetscSortMPIIntWithArray(nrecvs,onodes1,olengths1);CHKERRQ(ierr);
  recvtotal = 0; for (i=0; i<nrecvs; i++) recvtotal += olengths1[i];

  /* post receives:  rvalues - rows I will own; count - nu */
  ierr = PetscMalloc3(recvtotal,PetscInt,&rvalues,nrecvs,PetscInt,&source,nrecvs,MPI_Request,&recv_waits);CHKERRQ(ierr);
  count  = 0;
  for (i=0; i<nrecvs; i++) {
    ierr  = MPI_Irecv((rvalues+count),olengths1[i],MPIU_INT,onodes1[i],tag,comm,recv_waits+i);CHKERRQ(ierr);
    count += olengths1[i];
  }

  /* do sends:
     1) starts[i] gives the starting index in svalues for stuff going to 
     the ith processor
  */
  ierr = PetscMalloc3(cnt,PetscInt,&svalues,nsends,MPI_Request,&send_waits,size+1,PetscInt,&starts);CHKERRQ(ierr);
  starts[0]  = 0; 
  for (i=1; i<size; i++) { starts[i] = starts[i-1] + nprocs[i-1];} 
  for (i=0; i<cnt; i++) {
    if (owner[i] != rank) {
      svalues[starts[owner[i]]++] = rows[i];
    }
  }

  starts[0] = 0;
  for (i=1; i<size+1; i++) { starts[i] = starts[i-1] + nprocs[i-1];} 
  count = 0;
  for (i=0; i<size; i++) {
    if (nprocs[i]) {
      ierr = MPI_Isend(svalues+starts[i],nprocs[i],MPIU_INT,i,tag,comm,send_waits+count++);CHKERRQ(ierr);
    }
  }

  /*  wait on receives */
  count  = nrecvs; 
  slen   = 0;
  while (count) {
    ierr = MPI_Waitany(nrecvs,recv_waits,&imdex,&recv_status);CHKERRQ(ierr);
    /* unpack receives into our local space */
    ierr = MPI_Get_count(&recv_status,MPIU_INT,&n);CHKERRQ(ierr);
    slen += n;
    count--;
  }
  if (slen != recvtotal) SETERRQ2(PETSC_ERR_PLIB,"Total message lengths %D not expected %D",slen,recvtotal);

  ierr = PetscFree(olengths1);CHKERRQ(ierr);
  ierr = PetscFree(onodes1);CHKERRQ(ierr);
  ierr = PetscFree3(rvalues,source,recv_waits);CHKERRQ(ierr);

  /* wait on sends */
  if (nsends) {
    ierr = PetscMalloc(nsends*sizeof(MPI_Status),&send_status);CHKERRQ(ierr);
    ierr = MPI_Waitall(nsends,send_waits,send_status);CHKERRQ(ierr);
    ierr = PetscFree(send_status);CHKERRQ(ierr);
  }
  ierr = PetscFree3(svalues,send_waits,starts);CHKERRQ(ierr);


  ierr = PetscFree(map);CHKERRQ(ierr);
  ierr = PetscFree(nmap);CHKERRQ(ierr);

  ierr = KSPSetUp(red->ksp);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "PCApply_Redistribute"
static PetscErrorCode PCApply_Redistribute(PC pc,Vec b,Vec x)
{
  PC_Redistribute   *red = (PC_Redistribute*)pc->data;
  PetscErrorCode    ierr;

  PetscFunctionBegin;
  ierr = VecScatterBegin(red->scatter,b,red->b,INSERT_VALUES,SCATTER_FORWARD);CHKERRQ(ierr);
  ierr = VecScatterEnd(red->scatter,b,red->b,INSERT_VALUES,SCATTER_FORWARD);CHKERRQ(ierr);
  ierr = KSPSolve(red->ksp,red->b,red->x);CHKERRQ(ierr);
  ierr = VecScatterBegin(red->scatter,red->x,x,INSERT_VALUES,SCATTER_REVERSE);CHKERRQ(ierr);
  ierr = VecScatterEnd(red->scatter,red->x,x,INSERT_VALUES,SCATTER_REVERSE);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "PCDestroy_Redistribute"
static PetscErrorCode PCDestroy_Redistribute(PC pc)
{
  PC_Redistribute *red = (PC_Redistribute*)pc->data;
  PetscErrorCode  ierr;

  PetscFunctionBegin;
  if (red->scatter)  {ierr = VecScatterDestroy(red->scatter);CHKERRQ(ierr);}
  if (red->b)        {ierr = VecDestroy(red->b);CHKERRQ(ierr);}
  if (red->x)        {ierr = VecDestroy(red->x);CHKERRQ(ierr);}
  if (red->mat)      {ierr = MatDestroy(red->mat);CHKERRQ(ierr);}
  if (red->ksp)      {ierr = KSPDestroy(red->ksp);CHKERRQ(ierr);}
  ierr = PetscFree(red);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNCT__  
#define __FUNCT__ "PCSetFromOptions_Redistribute"
static PetscErrorCode PCSetFromOptions_Redistribute(PC pc)
{
  PetscErrorCode  ierr;
  PC_Redistribute *red = (PC_Redistribute*)pc->data;

  PetscFunctionBegin;
  ierr = KSPSetFromOptions(red->ksp);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

/* -------------------------------------------------------------------------------------*/
/*MC
     PCREDISTRIBUTE - Redistributes a matrix for load balancing and then applys a KSP to that new matrix

     Options for the redistribute preconditioners can be set with -redistribute_ksp_xxx <values> and -redistribute_pc_xxx <values>

   Level: intermediate

.seealso:  PCCreate(), PCSetType(), PCType (for list of available types)
M*/

EXTERN_C_BEGIN
#undef __FUNCT__  
#define __FUNCT__ "PCCreate_Redistribute"
PetscErrorCode PETSCKSP_DLLEXPORT PCCreate_Redistribute(PC pc)
{
  PetscErrorCode  ierr;
  PC_Redistribute *red;
  
  PetscFunctionBegin;
  ierr = PetscNewLog(pc,PC_Redistribute,&red);CHKERRQ(ierr);
  pc->data            = (void*)red; 

  pc->ops->apply           = PCApply_Redistribute;
  pc->ops->applytranspose  = 0;
  pc->ops->setup           = PCSetUp_Redistribute;
  pc->ops->destroy         = PCDestroy_Redistribute;
  pc->ops->setfromoptions  = PCSetFromOptions_Redistribute;
  pc->ops->view            = PCView_Redistribute;    
  PetscFunctionReturn(0);
}
EXTERN_C_END