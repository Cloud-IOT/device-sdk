#!/bin/bash


#-------	Function
function F_Usage () {
    ####传递的参数为: $1, $2, $3, ..., $9
	echo "Usage:"
	echo ".exe        prefix	project_name	cp_dir_path	org_bin_path	org_bin_name	org_bin_postfix_name	[Debug]"
#	echo "            "
	echo "    e.g.    UA  UA402_WifiMoudle  .  .  ssv6060-main  bin  [Debug]"
	echo "            copy ./bin to ./UA_UA402_WifiMoudle/UA_UA402_WifiMoudle_ssv6060-main__time.bin"
	echo ""
	#exit 1
}


if [ "$5" = "" ]; then
	F_Usage;
	exit 1
fi

R_Prefix=$1
R_PrjName=$2
R_ImgPath=$3
R_OrgPath=$4
R_BinName=$5
R_BinPostfix=$6
R_Debug=$7



#-------------------------
#	Env
USER=XXXX
USER=AdOS
Cur_Path=$PWD




#ECHO="echo    : "
ECHO=""
R_Cmd_CP="cp -f"

CompMode=""

#--------------------------------------------------
#	new or remake

	#-------------------------
	#	Make Img Dir
	ImgDirName=${R_Prefix}_${R_PrjName}
	if [ ! -d "${R_ImgPath}/${ImgDirName}" ] ; then
		$ECHO    mkdir -p ${R_ImgPath}/${ImgDirName}
	fi
	
	if [ "${R_Debug}" = "Debug" ]; then
		ImgName=${R_PrjName}_${R_BinName}__$(date +'%Y%m%d_%H%M%S')_D.${R_BinPostfix}
	else
		ImgName=${R_PrjName}_${R_BinName}__$(date +'%Y%m%d_%H%M%S').${R_BinPostfix}
	fi
		

	#-------------------------
	#	Copy DataBase
	$ECHO    $R_Cmd_CP	${R_OrgPath}/${R_BinName}.${R_BinPostfix}		${R_ImgPath}/${ImgDirName}/${ImgName}
	$ECHO    $R_Cmd_CP	${R_OrgPath}/${R_BinName}.${R_BinPostfix}		${R_ImgPath}/${ImgDirName}/
	
	


echo -----------------------------------
#date
echo Done!
exit 0;



