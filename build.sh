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

  cmake -DCMAKE_INSTALL_PREFIX:PATH="${install_dir}" .. --log-level=$log_level
}

function build()
{
  local target
  target=$1

  cmake --build . --target $target
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
