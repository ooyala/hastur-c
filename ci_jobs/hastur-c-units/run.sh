#!/bin/bash -l

# No errors allowed!
set -e

if [[ -z $WORKSPACE ]] ; then
  # Support execution from the shell
  export PROJECT_DIR=$(pwd);
else
  export PROJECT_DIR=$WORKSPACE/hastur-c;
fi

cd $PROJECT_DIR/hastur-c

make clean
make

echo START TASK: tests
./tester && ./threaded_tester
echo END TASK
