#!/bin/sh

WORKDIR=out/source

if [ -e ${WORKDIR} ] ; then
  rm -rf ${WORKDIR}
fi

VERSION=`date -I`
PACKAGE="OffscreenGecko-source-${VERSION}"
TARNAME="${PACKAGE}.zip"
TOP="`pwd`"

"${TOP}/bin/ignorantcopy.sh" . ${WORKDIR}/${PACKAGE}/

cd ${WORKDIR}
rm -f "${TOP}/out/${TARNAME}"
zip -9 -r "${TOP}/out/${TARNAME}" ${PACKAGE}

cd "${TOP}"
