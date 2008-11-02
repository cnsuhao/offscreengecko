#!/bin/sh

export TOP=`pwd`
mkdir -p out/api/
doxygen docs/api.dox
