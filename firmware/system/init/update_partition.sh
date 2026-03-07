#!/bin/sh

UPDATE_FILE=$1
# boot kernel root config appfs
PARTITION=$2

trap  "killall flashcp" SIGINT

# 0 is same,1 is not same
check_file_is_same()
{
	FILE_ONE=$1
	FILE_TWO=$2
	#ls ${FILE_ONE}
	#[ $? -ne 0 ] && return 0
	#ls ${FILE_TWO}
	#[ $? -ne 0 ] && return 0
	dd if=${FILE_ONE} of=/tmp/file1.tmp bs=200K count=1
	dd if=${FILE_TWO} of=/tmp/file2.tmp bs=200K count=1
	diff /tmp/file1.tmp  /tmp/file2.tmp
	RET=$?
	rm /tmp/file1.tmp  /tmp/file2.tmp 
	return ${RET}
}


flashcp_partiton()
{
	UPDATE_FILE_NAME=$1
	PARTITION_NAME=$2
	[ -z ${PARTITION_NAME} ] && return 1
	[ -z ${UPDATE_FILE_NAME} ] && return 2

	MTD_NAME=`cat /proc/mtd |grep ${PARTITION_NAME} | awk -F'[: ]+' '{print $1}'`
	[ -z ${MTD_NAME} ] && return 1
	ls /dev/${MTD_NAME}
	[ $? != 0 ] && return 1

	ls ${UPDATE_FILE_NAME}
	[ $? != 0 ] && return 2

	#can check sth or stop process
	check_file_is_same ${UPDATE_FILE_NAME} /dev/${MTD_NAME}
	[ $? -eq 0 ] && return 4

	#flashcp
	echo "run flashcp ${UPDATE_FILE_NAME} /dev/${MTD_NAME}"
	flashcp ${UPDATE_FILE_NAME} /dev/${MTD_NAME} -v
	if [ $? != 0 ];then
		#echo "update fail!"
		return 3
	fi
	#echo "update success!"
	return 0
}

flashcp_partiton ${UPDATE_FILE} ${PARTITION}

RETVAL=$?

case $RETVAL in
	0)
		echo "flashcp success!"
	;;
	1)
		echo "${PARTITION} not found"
	;;
	2)
		echo "${UPDATE_FILE} not found"
	;;
	3)
		echo "flashcp fail!"
	;;
	4)
		echo "file is same, no update!"
	;;
	esac

exit $RETVAL
