/*
 * File: sources.c
 * Author: Oliver B. Fringer
 * Institution: Stanford University
 * --------------------------------
 * Right-hand sides for momentum and scalars, in the form rhs = dt*SOURCE at time step n.
 *
 * Copyright (C) 2005-2006 The Board of Trustees of the Leland Stanford Junior 
 * University. All Rights Reserved.
 *
 */
#include "phys.h"
#include "sources.h"
#include "memory.h"
#include "marsh.h"
void MomentumSource(REAL **usource, gridT *grid, physT *phys, propT *prop) {
}

/*
 * Function: HeatSource
 * Usage: HeatSource(grid,phys,prop,A,B);
 * --------------------------------------
 * Source terms for heat equation of the form
 *
 * dT/dt + u dot grad T = d/dz ( kappaT dT/dz) + A + B*T
 *
 * Assuming adiabatic top and bottom.  Horizontal advection is
 * explicit while all other terms use the theta method.
 *
 * Note that they must be set to zero if there is no heat
 * flux since these are temporary variables with values that may
 * be assigned elsewhere.
 *
 */
void HeatSource(REAL **A, REAL **B, gridT *grid, physT *phys, propT *prop, metT *met, int myproc, MPI_Comm comm) {
  int i, k;
  for(i=0;i<grid->Nc;i++)
    for(k=0;k<grid->Nk[i];k++)
      A[i][k]=B[i][k]=0;
}

/*
* Funtion: SaltSoutce
*/

void SaltSource(REAL **A, REAL **B, gridT *grid, physT *phys, propT *prop, metT *met) {
  int i, iptr, k, ktop, gc;
  REAL alpha;
  REAL marshsalinity=36;
  for(i=0;i<grid->Nc;i++)
  {

    for(k=0;k<grid->Nk[i];k++)
      A[i][k]=B[i][k]=0;

    if(phys->s[i][grid->Nk[i]-1]<marshsalinity)
    {
     // B[i][grid->Nk[i]-1]=alpha*(prop->kappa_s+phys->kappa_tv[i][grid->Nk[i]-1])/grid->dzz[i][grid->Nk[i]-1]/grid->dzz[i][grid->Nk[i]-1];
      //A[i][grid->Nk[i]-1]=alpha*(prop->kappa_s+phys->kappa_tv[i][grid->Nk[i]-1])/grid->dzzold[i][grid->Nk[i]-1]/grid->dzzold[i][grid->Nk[i]-1]*marshsalinity;
    }

    if(prop->marshmodel && phys->s[i][grid->Nk[i]-1]<marshsalinity)
    {
       if(marsh->hmarshcenter[i]>0)
       {
         //B[i][grid->Nk[i]-1]=alpha*(prop->kappa_s+phys->kappa_tv[i][grid->Nk[i]-1])/grid->dzz[i][grid->Nk[i]-1]/grid->dzz[i][grid->Nk[i]-1];
         //A[i][grid->Nk[i]-1]=alpha*(prop->kappa_s+phys->kappa_tv[i][grid->Nk[i]-1])/grid->dzzold[i][grid->Nk[i]-1]/grid->dzzold[i][grid->Nk[i]-1]*marshsalinity;
       }
    }
  }
}

/*
 * Function: InitSponge
 * Usage: InitSponge(grid,myproc);
 * -------------------------------
 * Apply a sponge layer to all type 2 boundaries.
 *
 */
void InitSponge(gridT *grid, int myproc) {
  int Nb, p1, p2, mark, g1, g2;
  int j, n, NeAll, NpAll;
  REAL *xb, *yb, *xp, *yp, r2;
  char str[BUFFERLENGTH];
  FILE *ifile;

  NeAll = MPI_GetSize(EDGEFILE,"InitSponge",myproc);
  NpAll = MPI_GetSize(POINTSFILE,"InitSponge",myproc);

  xp = (REAL *)SunMalloc(NpAll*sizeof(REAL),"InitSponge");
  yp = (REAL *)SunMalloc(NpAll*sizeof(REAL),"InitSponge");
  rSponge = (REAL *)SunMalloc(grid->Ne*sizeof(REAL),"InitSponge");

  // Read in points on entire grid
  ifile = MPI_FOpen(POINTSFILE,"r","InitSponge",myproc);
  for(j=0;j<NpAll;j++) {
    xp[j]=getfield(ifile,str);
    yp[j]=getfield(ifile,str);
    getfield(ifile,str);
  }
  fclose(ifile);

  // Count number of nonzero boundary markers on entire grid
  ifile = MPI_FOpen(EDGEFILE,"r","InitSponge",myproc);
  Nb = 0;
  for(j=0;j<NeAll;j++) {
    fscanf(ifile, "%d %d %d %d %d",&p1,&p2,&mark,&g1,&g2);
    if(mark==2 || mark==3)
      Nb++;
  }
  fclose(ifile);

  xb = (REAL *)SunMalloc(Nb*sizeof(REAL),"InitSponge");
  yb = (REAL *)SunMalloc(Nb*sizeof(REAL),"InitSponge");

  n=0;
  ifile = MPI_FOpen(EDGEFILE,"r","InitSponge",myproc);
  for(j=0;j<NeAll;j++) {
    fscanf(ifile, "%d %d %d %d %d",&p1,&p2,&mark,&g1,&g2);
    if(mark==2 || mark==3) {
      xb[n]=0.5*(xp[p1]+xp[p2]);
      yb[n]=0.5*(yp[p1]+yp[p2]);
      n++;
    }
  }
  fclose(ifile);  

  // Now compute the minimum distance between the edge on the
  // local processor and the boundary and place this in rSponge.
  for(j=0;j<grid->Ne;j++) {
    rSponge[j]=INFTY;

    for(n=0;n<Nb;n++) {
      r2=pow(xb[n]-grid->xe[j],2)+pow(yb[n]-grid->ye[j],2);
      if(r2<rSponge[j])
	rSponge[j]=r2;
    }
    rSponge[j]=sqrt(rSponge[j]);
    //    printf("Processor %d: rSponge[%d]=%f\n",myproc,j,rSponge[j]);
  }
}
  
