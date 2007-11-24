#!/bin/sh

WORKDIR=out/sdk

if [ -e ${WORKDIR} ] ; then
  rm -rf ${WORKDIR}
fi

VERSION=`date -I`
PACKAGE="OffscreenGecko-SDK-${VERSION}"
TARNAME="${PACKAGE}.zip"
TOP="`pwd`"

mkdir -p ${WORKDIR}/${PACKAGE}/bin
cp out/release/OffscreenGecko.dll ${WORKDIR}/${PACKAGE}/bin
cp out/release/OffscreenGecko.pdb ${WORKDIR}/${PACKAGE}/bin
mkdir -p ${WORKDIR}/${PACKAGE}/lib
cp out/release/OffscreenGecko.lib ${WORKDIR}/${PACKAGE}/lib
${TOP}/bin/ignorantcopy.sh include/OffscreenGecko ${WORKDIR}/${PACKAGE}/include/OffscreenGecko

cp ${TOP}/sdk/win32/*.txt ${WORKDIR}/${PACKAGE}/
cp -R ${TOP}/out/api/html ${WORKDIR}/${PACKAGE}/apidocs

cd ${WORKDIR}
rm -f ${TOP}/out/${TARNAME}
zip -9 -r ${TOP}/out/${TARNAME} ${PACKAGE}

cd ${TOP}
