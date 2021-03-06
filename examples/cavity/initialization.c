#include "math.h"
#include "fileio.h"
#include "suntans.h"
#include "initialization.h"

#define sech 1/cosh
/*
 * Function: GetDZ
 * Usage: GetDZ(dz,depth,Nkmax,myproc);
 * ------------------------------------
 * Returns the vertical grid spacing in the array dz.
 *
 */
int GetDZ(REAL *dz, REAL depth, REAL localdepth, int Nkmax, int myproc) {
  int k;
  REAL zk, zkp1, *z = (REAL *)SunMalloc(Nkmax*sizeof(REAL),"GetDZ");
  
  if(Nkmax==1) {
    dz[0]=depth;
  } else {
    for(k=0;k<Nkmax;k++) {
      dz[k]=1.0/Nkmax;
    }
  }
  return Nkmax;
}
  
/*
 * Function: ReturnDepth
 * Usage: grid->dv[n]=ReturnDepth(grid->xv[n],grid->yv[n]);
 * --------------------------------------------------------
 * Helper function to create a bottom bathymetry.  Used in
 * grid.c in the GetDepth function when IntDepth is 0.
 *
 */
REAL ReturnDepth(REAL x, REAL y) {
  return 1.0;//e6;
}

 /*
  * Function: ReturnFreeSurface
  * Usage: grid->h[n]=ReturnFreeSurface(grid->xv[n],grid->yv[n]);
  * -------------------------------------------------------------
  * Helper function to create an initial free-surface. Used
  * in phys.c in the InitializePhysicalVariables function.
  *
  */
REAL ReturnFreeSurface(REAL x, REAL y, REAL d) {
  return 0;
}

/*
 * Function: ReturnSalinity
 * Usage: grid->s[n]=ReturnSalinity(grid->xv[n],grid->yv[n],z);
 * ------------------------------------------------------------
 * Helper function to create an initial salinity field.  Used
 * in phys.c in the InitializePhysicalVariables function.
 *
 */
REAL ReturnSalinity(REAL x, REAL y, REAL z) {
  return 0;
}

/*
 * Function: ReturnTemperature
 * Usage: grid->T[n]=ReturnTemperaturegrid->xv[n],grid->yv[n],z);
 * ------------------------------------------------------------
 * Helper function to create an initial temperature field.  Used
 * in phys.c in the InitializePhysicalVariables function.
 *
 */
REAL ReturnTemperature(REAL x, REAL y, REAL z, REAL depth) {
  return 0;
}

/*
 * Function: ReturnHorizontalVelocity
 * Usage: grid->u[n]=ReturnHorizontalVelocity(grid->xv[n],grid->yv[n],
 *                                            grid->n1[n],grid->n2[n],z);
 * ------------------------------------------------------------
 * Helper function to create an initial velocity field.  Used
 * in phys.c in the InitializePhysicalVariables function.
 *
 */
REAL ReturnHorizontalVelocity(REAL x, REAL y, REAL n1, REAL n2, REAL z) {
  return 0;
}
REAL ReturnSediment(REAL x, REAL y, REAL z, int sizeno) {}
REAL ReturnBedSedimentRatio(REAL x, REAL y, int layer, int sizeno,int nsize) {}
REAL IsoReturnSalinity(REAL x, REAL y, REAL z,int i,int k){}
REAL IsoReturnTemperature(REAL x, REAL y, REAL z, REAL depth,int i,int k){}
REAL ReturnCulvertTop(REAL x, REAL y, REAL d){}
REAL ReturnFluxHeight(REAL x1,REAL y1, REAL x2, REAL y2, REAL h){}   
REAL ReturnMarshDragCoefficient(REAL x, REAL y){}
REAL ReturnMarshHeight(REAL x, REAL y){}
REAL ReturnSubCellArea(REAL x1, REAL y1, REAL x2, REAL y2, REAL x3, REAL y3, REAL h){}
REAL ReturnSubgridPointDepth(REAL x, REAL y, REAL xv, REAL yv){}
REAL ReturnSubgridPointeDepth(REAL x, REAL y){}
REAL ReturnWindSpeed(REAL x, REAL y){}
REAL ReturnWindDirection(REAL x, REAL y){}
