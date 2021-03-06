#!/bin/sh
########################################################################
#
# Shell script to run a suntans test case.
#
########################################################################

SUNTANSHOME=../../main
SUN=$SUNTANSHOME/sun
SUNPLOT=$SUNTANSHOME/sunplot

. $SUNTANSHOME/Makefile.in

maindatadir=rundata
datadir=data

NUMPROCS=$1

if [ -z "$MPIHOME" ] ; then
    EXEC=$SUN
else
    EXEC="$MPIHOME/bin/mpirun -np $NUMPROCS $SUN"
fi

if [ -z "$TRIANGLEHOME" ] ; then
    echo Error: This example will not run without the triangle libraries.
    echo Make sure TRIANGLEHOME is set in $SUNTANSHOME/Makefile.in
    exit 1
fi

#dirs="L1000Nx128dt0.5z L1000Nx128dt0.25z L1000Nx128dt0.125z L1000Nx128dt0.0625z L1000Nx128dt0.03125z"
#dirs="L1000Nx128dt0.05 L1000Nx128dt0.025 L1000Nx128dt0.0125 L1000Nx128dt0.00625 L1000Nx128dt0.003125"
#dirs="L100Nx4dt0.01 L100Nx8dt0.01 L100Nx16dt0.01 L100Nx32dt0.01 L100Nx64dt0.01 L100Nx128dt0.01"
dirs="L100Nx512dt0.01Nk4 L100Nx512dt0.01Nk8 L100Nx512dt0.01Nk16 L100Nx512dt0.01Nk32 L100Nx512dt0.01Nk64"


for dir in `echo $dirs` ; do
    echo On $dir...

if [ ! -d $dir ] ; then
    cp -r $maindatadir/$dir $dir
    #cp $maindatadir/suntans.dat-$dir $dir/suntans.dat
    #cp $maindatadir/dataxy.dat $dir/.
    echo Creating grid...
    $EXEC -g --datadir=$dir
else
    rm -rf $dir
    cp -r $maindatadir/$dir $dir
    #cp $maindatadir/suntans.dat-$dir $dir/suntans.dat
    #cp $maindatadir/dataxy.dat $dir/.
    echo Creating grid...
    $EXEC -g --datadir=$dir    
fi

echo Running suntans...
$EXEC -s --datadir=$dir >& make-$dir.out

done


