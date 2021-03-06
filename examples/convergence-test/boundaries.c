/*
 * Boundaries test file.
 *
 */
#include "boundaries.h"
#include "sediments.h"
#include "wave.h"
#include "subgrid.h"
#include "vertcoordinate.h"
static void SetUVWH(gridT *grid, physT *phys, propT *prop, int ib, int j, int boundary_index, REAL boundary_flag);

/*
 * Function: OpenBoundaryFluxes
 * Usage: OpenBoundaryFluxes(q,ubnew,ubn,grid,phys,prop);
 * ----------------------------------------------------
 * This will update the boundary flux at the edgedist[2] to edgedist[3] edges.
 * 
 * Note that phys->uold,vold contain the velocity at time step n-1 and 
 * phys->uc,vc contain it at time step n.
 *
 * The radiative open boundary condition does not work yet!!!  For this reason c[k] is
 * set to 0
 *
 */
void OpenBoundaryFluxes(REAL **q, REAL **ub, REAL **ubn, gridT *grid, physT *phys, propT *prop) {
  int j, jptr, ib, k, forced;
  REAL *uboundary = phys->a, **u = phys->uc, **v = phys->vc, **uold = phys->uold, **vold = phys->vold;
  REAL z, c0, c1, C0, C1, dt=prop->dt, u0, u0new, uc0, vc0, uc0old, vc0old, ub0;

  for(jptr=grid->edgedist[2];jptr<grid->edgedist[3];jptr++) {
    j = grid->edgep[jptr];

    ib = grid->grad[2*j];

    for(k=grid->etop[j];k<grid->Nke[j];k++) 
      ub[j][k]=phys->boundary_u[jptr-grid->edgedist[2]][k]*grid->n1[j]+
	phys->boundary_v[jptr-grid->edgedist[2]][k]*grid->n2[j];
  }
}

/*
 * Function: BoundaryScalars
 * Usage: BoundaryScalars(boundary_s,boundary_T,grid,phys,prop);
 * -------------------------------------------------------------
 * This will set the values of the scalars at the open boundaries.
 * 
 */
void BoundaryScalars(gridT *grid, physT *phys, propT *prop, int myproc, MPI_Comm comm) {
  int jptr, j, iptr, i, ib, k;
  REAL z;

  // At the upstream boundary
  for(jptr=grid->edgedist[2];jptr<grid->edgedist[3];jptr++) {
      j=grid->edgep[jptr];
      ib=grid->grad[2*j];

      for(k=grid->ctop[ib];k<grid->Nk[ib];k++) {
	phys->boundary_T[jptr-grid->edgedist[2]][k]=0;
	phys->boundary_s[jptr-grid->edgedist[2]][k]=0;
      }
  }

  // At the ocean boundary
  for(iptr=grid->celldist[1];iptr<grid->celldist[2];iptr++) {
    i = grid->cellp[iptr];
    
    for(k=0;k<grid->ctop[i];k++) {
      phys->s[i][k]=0;
      phys->T[i][k]=0;
    } 
    for(k=grid->ctop[i];k<grid->Nk[i];k++) {
      phys->s[i][k]=0;
      phys->T[i][k]=0;
    }
  }
}

/*
 * Function: BoundaryVelocities
 * Usage: BoundaryVelocities(grid,phys,prop,myproc);
 * -------------------------------------------------
 * This will set the values of u,v,w, and h at the boundaries.
 * 
 */
void BoundaryVelocities(gridT *grid, physT *phys, propT *prop, int myproc, MPI_Comm comm) {
  int jptr, j, ib, k,iptr,i;
  REAL z;

  for(jptr=grid->edgedist[2];jptr<grid->edgedist[3];jptr++) {
    j = grid->edgep[jptr];

    ib = grid->grad[2*j];

    if(grid->xv[ib]>5)
      for(k=grid->etop[j];k<grid->Nke[j];k++) {
        // Maintain a constant free-surface height at the right boundary.
        phys->boundary_u[jptr-grid->edgedist[2]][k]=phys->uc[ib][k];
        phys->boundary_v[jptr-grid->edgedist[2]][k]=phys->vc[ib][k];
        phys->boundary_w[jptr-grid->edgedist[2]][k]=phys->w[ib][k];
      }
    else
      for(k=grid->etop[j];k<grid->Nke[j];k++) {
        // Maintain a constant inflow volume flux at the left boundary.
        phys->boundary_u[jptr-grid->edgedist[2]][k]=0.0;  
        phys->boundary_v[jptr-grid->edgedist[2]][k]=0.0;
        phys->boundary_w[jptr-grid->edgedist[2]][k]=0;
      }
  }

  // At the ocean boundary
  for(iptr=grid->celldist[1];iptr<grid->celldist[2];iptr++) {
    i = grid->cellp[iptr];
    phys->h[i]=0;//-1+1*sin(prop->n*prop->dt*2*3.1415926/3600/12);
  }
}

static void SetUVWH(gridT *grid, physT *phys, propT *prop, int ib, int j, int boundary_index, REAL boundary_flag) {
  int k;

  if(boundary_flag==open) {
    phys->boundary_h[boundary_index]=phys->h[ib];
    for(k=grid->ctop[ib];k<grid->Nk[ib];k++) {
      phys->boundary_u[boundary_index][k]=phys->uc[ib][k];
      phys->boundary_v[boundary_index][k]=phys->vc[ib][k];
      phys->boundary_w[boundary_index][k]=0.5*(phys->w[ib][k]+phys->w[ib][k+1]);
    }
  } else {
    phys->boundary_h[boundary_index]=prop->amp*fabs(cos(prop->omega*prop->rtime));
    for(k=grid->ctop[ib];k<grid->Nk[ib];k++) {
      phys->boundary_u[boundary_index][k]=phys->u[j][k]*grid->n1[j];
      phys->boundary_v[boundary_index][k]=phys->u[j][k]*grid->n2[j];
      phys->boundary_w[boundary_index][k]=0.5*(phys->w[ib][k]+phys->w[ib][k+1]);
    }
  }
}
	
/*
 * Function: WindStress
 * Usage: WindStress(grid,phys,prop,myproc);
 * -----------------------------------------
 * Set the wind stress.
 *
 */
void WindStress(gridT *grid, physT *phys, propT *prop, metT *met, int myproc) {
  int j, jptr;

  for(jptr=grid->edgedist[0];jptr<grid->edgedist[5];jptr++) {
    j = grid->edgep[jptr];
    
    phys->tau_T[j]=grid->n2[j]*prop->tau_T;
    phys->tau_B[j]=0;
  }
}

//added part
/*
 * Function: BoundarySediment
 * Usage: BoundarySediment(boundary_s,boundary_T,grid,phys,prop);
 * -------------------------------------------------------------
 * This will set the values of the suspended sediment concentration
 * at the open boundaries.
 * 
 */
void BoundarySediment(gridT *grid, physT *phys, propT *prop) {
  int jptr, j, ib, k,nosize,i,iptr;
  REAL z;

  // At the upstream boundary
  for(jptr=grid->edgedist[2];jptr<grid->edgedist[3];jptr++) {
    j=grid->edgep[jptr];
    ib=grid->grad[2*j];
    for(nosize=0;nosize<sediments->Nsize;nosize++){
      for(k=grid->ctop[ib];k<grid->Nk[ib];k++) {
        sediments->boundary_sediC[nosize][jptr-grid->edgedist[2]][k]=0;
      }
    }
  }

  // At the ocean boundary
  for(iptr=grid->celldist[1];iptr<grid->celldist[2];iptr++) {
    i = grid->cellp[iptr];
    for(nosize=0;nosize<sediments->Nsize;nosize++){
      for(k=0;k<grid->ctop[i];k++) {
        sediments->SediC[nosize][i][k]=0;
      } 
      for(k=grid->ctop[i];k<grid->Nk[i];k++) {
        sediments->SediC[nosize][i][k]=0;
      }
    }
  }
}
/*
 * Function: WindSpeedandDirection
 * usage: calculate Wind field when Wind is not constant
 * -------------------------------------------------
 * calculate Uwind and Winddir
 *
 */
void FetchWindSpeedandDirection(gridT *grid, propT *prop, int myproc){
   int i;
   for(i=0;i<grid->Nc;i++){
     wave->Uwind[i]=0;
     wave->Winddir[i]=0;
   }
}
void InitBoundaryData(propT *prop, gridT *grid, int myproc, MPI_Comm comm){}
void AllocateBoundaryData(propT *prop, gridT *grid, boundT **bound, int myproc, MPI_Comm comm){}
void UserDefinedFunction(gridT *grid, physT *phys, propT *prop,int myproc)
{
  int i,k,j,jptr,nc1,nc2,ne,nf,iptr,index,index2;
  REAL sum=0,normal,dz;
  REAL fac1=prop->imfac1,fac2=prop->imfac2,fac3=prop->imfac3;
  if(prop->n==prop->nsteps-1)
    for(i=0;i<grid->Nc;i++)
      for(k=0;k<grid->Nk[i];k++)
        phys->user_def_nc_nk[i][k]=phys->q[i][k];

  if(prop->n==prop->nsteps)
    for(i=0;i<grid->Nc;i++)
    {
      for(k=0;k<grid->Nk[i];k++)
        phys->user_def_nc_nk[i][k]=1.5*phys->q[i][k]-0.5*phys->user_def_nc_nk[i][k];
      //printf("%e %e %12.12e\n",phys->user_def_nc[i][0],phys->user_def_nc[i][1],-50+grid->dzz[i][1]);
    }

  for(j=0;j<grid->Ne;j++){
    if(grid->xe[j]==25){
      index=j;
      nc1=grid->grad[2*j];
      nc2=grid->grad[2*j+1];
    }
  }

  index2=grid->Nk[0]/2;
  dz=0;
  for(k=grid->Nk[0]-1;k>grid->Nk[0]/2-1;k--)
    dz+=grid->dzz[0][k];
  if(grid->Nk[0]>2)
    printf("%12.12e %12.12e %12.12e %12.12e %12.12e\n",phys->h[0],-50+dz,1.5*phys->q[0][index2]-0.5*phys->q[0][index2+1],
      1.5*phys->u[index][index2]-0.5*phys->u[index][index2+1],phys->w[0][index2]);  
  else
    printf("%12.12e %12.12e %12.12e %12.12e %12.12e\n",phys->h[0],-50+dz,0.5*phys->q[0][index2]+0.5*phys->q[0][index2-1],
      0.5*phys->u[index][index2]+0.5*phys->u[index][index2-1],phys->w[0][index2]); 
  if(prop->n==prop->nsteps)
  {
    for(iptr=grid->celldist[0];iptr<grid->celldist[1];iptr++) {
      i = grid->cellp[iptr];
      sum = 0;
      for(nf=0;nf<grid->nfaces[i];nf++) {
        ne = grid->face[i*grid->maxfaces+nf];
        normal = grid->normal[i*grid->maxfaces+nf];
        nc1=grid->grad[2*ne];
        nc2=grid->grad[2*ne+1];
        if(nc1==-1)
          nc1=nc2;
        if(nc2==-1)
          nc2=nc1;
        for(k=grid->etop[ne];k<grid->Nke[ne];k++) 
        {  
          sum+=(fac2*phys->u_old[ne][k]+fac1*phys->u[ne][k]+fac3*phys->u_old2[ne][k])*
            grid->df[ne]*normal*grid->dzf[ne][k]*prop->dt;
        }
      }
      //printf("h %e hold %e diff %e flux %e diff %e fac1 %e fac2 %e fac3 %e\n",phys->h[i],phys->h_old[i],grid->Ac[i]*(phys->h[i]-phys->h_old[i]),sum,
        //grid->Ac[i]*(phys->h[i]-phys->h_old[i])+sum,fac1,fac2,fac3);
    }
  }

  if(prop->n==prop->nsteps)
  {
    for(jptr=grid->edgedist[0];jptr<grid->edgedist[1];jptr++) {
      j = grid->edgep[jptr];
      nc1 = grid->grad[2*j];
      nc2 = grid->grad[2*j+1];
      for(k=grid->etop[j];k<grid->Nke[j];k++) {
        //printf("%e\n",phys->u[j][k]);
      }
    }
  }
}
