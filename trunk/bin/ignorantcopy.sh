#!/bin/sh

# Copies a directory with subdirectory, ignoring entries in SVN ignore property
# as well as CVS and .svn dirs

SRC=$1
DST=$2
IFS=$'\n\r'
  
# Generate ignored file list
IGNORED=`svn pg svn:ignore ${SRC}`
LSOPT="-I CVS -I .svn"
for i in ${IGNORED} ; do
  LSOPT="${LSOPT} -I '${i}'"
done
  
# Generate list of files to copy
FILELIST=`sh -c "ls -A -1 ${LSOPT} ${SRC}/"`
SRCLIST=
# Construct sources list/recursively copy dirs
for f in ${FILELIST} ; do
  if [ -d ${SRC}/${f} ] ; then
    $0 ${SRC}/${f} ${DST}/${f}
  else
    SRCLIST="${SRCLIST} \"${SRC}/${f}\""
  fi
done
# The actual copy
mkdir -p ${DST}
if [ -n "${SRCLIST}" ] ; then
  sh -c "cp ${SRCLIST} ${DST}/"
fi
