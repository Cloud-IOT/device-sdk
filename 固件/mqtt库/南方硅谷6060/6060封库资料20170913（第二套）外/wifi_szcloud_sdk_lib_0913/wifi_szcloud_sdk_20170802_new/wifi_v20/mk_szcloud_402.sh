#!/bin/sh

export RCVer=SV_$(svn info | grep Revision | cut -f2 -d' ')
echo "#define  D_szRCVer  \"$RCVer\"" > ./include/uRCVer.h

export BuildTime=$(date +'%Y%m%d_%H%M%S')
echo "#define  D_szBuildTime  \"$BuildTime\"" >> ./include/uRCVer.h

export BuildTimeSec=$(date +%s)
echo "#define  D_dwBuildTimeSec  $BuildTimeSec" >> ./include/uRCVer.h

#rm -f ./icomapps/objs/UAVersion.o
#make clean
make MYCUSTOMER=SZ MYPROJECT=UA402_SZCLOUD | tee build.txt
