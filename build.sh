#!/bin/bash

# build: Project build script

source export.sh
ctags -R .
mkdir -p build && cd build

function init()
{
  local log_level
  log_level=$1

  local install_dir
  install_dir=$2

  if [ -z $log_level ]; then
    log_level=notice
  fi

  if [ -z $install_dir ]; then
    install_dir="${PWD}"
  fi
  
  mkdir -p $install_dir
  cd $install_dir

  cmake --log-level=$log_level -DCMAKE_INSTALL_PREFIX:PATH="${install_dir}" .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-linux-dynamic

}


function build()
{
  local target
  target=$1

  cmake --build . --target $target

  if [ $? -ne 0 ]; then
    exit $?
  fi
}

case $1 in
  init) 
    init $2 $3
    exit
    ;;
  build) 
    build ""
    exit
    ;;
  test)
    build "test"
    exit
    ;;
  gcov)
    build "gcov"
    exit
    ;;
  install)
    build "install"
    exit
    ;;  
  *) 
    init $1 $2
    build ""
    build "test"
    build "gcov"
    build "install"
    exit
    ;;
    
esac
