#!/bin/bash

scriptdir=`dirname ${0}`
scriptdir=`(cd ${scriptdir}; pwd)`
scriptname=`basename ${0}`

set -e

function errorexit()
{
  errorcode=${1}
  shift
  echo $@
  exit ${errorcode}
}

function usage()
{
  echo "USAGE ${scriptname} <tostrip>"
}

tostripdir=`dirname "$1"`
tostripfile=`basename "$1"`


if [ -z ${tostripfile} ] ; then
  usage
  errorexit 0 "tostrip must be specified"
fi

cd "${tostripdir}"

debugdir=.debug
debugfile="${tostripfile}.debug"

if [ ! -d "${debugdir}" ] ; then
  echo "creating dir ${tostripdir}/${debugdir}"
  mkdir -p "${debugdir}"
fi
echo "stripping ${tostripfile}, putting debug info into ${debugfile}"
/home/anton/Repositories/noise/buildroot/output/host/bin/arm-none-linux-gnueabihf-objcopy --only-keep-debug "${tostripfile}" "${debugdir}/${debugfile}"
/home/anton/Repositories/noise/buildroot/output/host/bin/arm-none-linux-gnueabihf-strip --strip-debug --strip-unneeded "${tostripfile}"
/home/anton/Repositories/noise/buildroot/output/host/bin/arm-none-linux-gnueabihf-objcopy --add-gnu-debuglink="${debugdir}/${debugfile}" "${tostripfile}"
chmod -x "${debugdir}/${debugfile}"
