#!/bin/bash

source ${TARGET_BUILD_DIR}/scripts/common_set.sh

OLDKEY=tsm
NEWKEY=ts
DIR=0

function usage()
{
        echo -e "usage: $0 <-o xxx> <-n yyy> <-d libs>"

        echo ""
        echo -e "-o tsm          old key need be replaced, upper case also be replaced"
        echo -e "-n ts           new key was set"
        echo -e "-d libs         search dir"
        exit 1
}

function git_pull_and_clean() {
  echo "git_pull_and_clean_begin"
  cd ./${DIR}
  local temp=$(pwd)
  echo "111 ${temp}"
  git reset --hard HEAD
  git pull origin dev
  cd -
  echo "git_pull_and_end"
}

function replace() {
  echo "replace_begin"
  echo "oldkey=${OLDKEY} newkey=${NEWKEY} search_dir=${DIR}"
  sed -i "s/${OLDKEY}/${NEWKEY}/g" `grep ${OLDKEY} -rl ./${DIR}`
  #typeset -u UPPER_OLDKEY
  #typeset -u UPPER_NEWKEY
  #UPPER_OLDKEY=${OLDKEY}
  #UPPER_NEWKEY=${NEWKEY}
  #echo "upper_oldkey=${UPPER_OLDKEY} upper_newkey=${UPPER_NEWKEY}"
  #sed -i "s/${UPPER_OLDKEY}/${UPPER_NEWKEY}/g" `grep "TSM" -rl ./${DIR}`
  echo "replace_end"
}

while getopts "o:n:d:h" arg
do
        case $arg in
        o)
                OLDKEY=$OPTARG
                ;;
        n)
                NEWKEY=$OPTARG
                ;;
        d)
                DIR=$OPTARG
                ;;
        h)
                usage
                exit
                ;;
        ?)
                usage
                exit
                ;;
        esac
done
if [ ${DIR} == 0 ];then
  echo "dir is empty, just exit"
  exit
fi
#git_pull_and_clean
replace

