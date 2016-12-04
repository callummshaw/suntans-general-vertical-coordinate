/*
 * File: vertcoordinate.h
 * Author : Yun Zhang
 * Institution: Stanford University
 * --------------------------------
 * Header file for vertcoordinate.c
 * including all the variables for the general vertical coordinate
 *
 */
#ifndef _uservertcoordinate_h
#define __uservertcoordinate_h

#include "suntans.h"
#include "grid.h"
#include "phys.h"

void UserDefinedVerticalCoordinate(gridT *grid, propT *prop, physT *phys, int myproc);
void UserDefinedSigmaCoordinate(gridT *grid, int myproc);
#endif