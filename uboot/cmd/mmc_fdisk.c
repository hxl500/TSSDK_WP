/*
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *              http://www.samsung.com/
 *
 * fdisk command for U-boot
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <common.h>
#include <command.h>
#include <mmc.h>
// #include <movi.h> //mj
#include <stdlib.h>

#define RAW_AREA_SIZE (38 * 1024 * 1024) /*start 45M change by lyy 2017-3-20*/
#define BLOCK_SIZE 512
#define BLOCK_END 0xFFFFFFFF
#define _10MB (10 * 1024 * 1024)
#define _100MB (101 * 1024 * 1024)
#define _200MB (200 * 1024 * 1024)
#define _300MB (300 * 1024 * 1024)
#define _512MB (512 * 1024 * 1024)
// #define		_8_4GB				(1023*254*63)
#define _8_4GB (1023 * 254 * 63 * 8) // 32G
#define _1GB (1024 * 1024 * 1024)
#define DISK_START RAW_AREA_SIZE // mj (234*1024*1024) //same as raw area size

#define SYSTEM_PART_SIZE _512MB	   // rootfs1
#define USER_DATA_PART_SIZE _512MB // roofs2
#define CACHE_PART_SIZE _1GB	   // update

#define CHS_MODE 0
#define LBA_MODE !(CHS_MODE)
#define emmcdbg(fmt, args...) printf(fmt, ##args) // for emmc dbg
// #define     emmcdbg(fmt,args)
uint32_t g_raw_area_size;

typedef struct
{
	int C_start;
	int H_start;
	int S_start;

	int C_end;
	int H_end;
	int S_end;

	int available_block;
	int unit;
	int total_block_count;
	int addr_mode; // LBA_MODE or CHS_MODE
} SDInfo;

typedef struct
{
	unsigned char bootable;
	unsigned char partitionId;

	int C_start;
	int H_start;
	int S_start;

	int C_end;
	int H_end;
	int S_end;

	int block_start;
	int block_count;
	int block_end;
} PartitionInfo;

/////////////////////////////////////////////////////////////////
int calc_unit(u64 length, SDInfo sdInfo)
{
	// printf("sdInfo.unit : %d\n",sdInfo.unit);
	// printf("%lld:%lld\n", length, length / BLOCK_SIZE);

	if (sdInfo.addr_mode == CHS_MODE)
	{
		// return ( (length / BLOCK_SIZE / sdInfo.unit + 1 ) * sdInfo.unit);
		return (length / BLOCK_SIZE);
	}
	else
	{
		return (length / BLOCK_SIZE);
	}
}

/////////////////////////////////////////////////////////////////
void encode_chs(int C, int H, int S, unsigned char *result)
{
	*result++ = (unsigned char)H;
	*result++ = (unsigned char)(S + ((C & 0x00000300) >> 2));
	*result = (unsigned char)(C & 0x000000FF);
}

/////////////////////////////////////////////////////////////////
void encode_partitionInfo(PartitionInfo partInfo, unsigned char *result)
{
	*result++ = partInfo.bootable;

	encode_chs(partInfo.C_start, partInfo.H_start, partInfo.S_start, result);
	result += 3;
	*result++ = partInfo.partitionId;

	encode_chs(partInfo.C_end, partInfo.H_end, partInfo.S_end, result);
	result += 3;

	memcpy(result, (unsigned char *)&(partInfo.block_start), 4);
	result += 4;

	memcpy(result, (unsigned char *)&(partInfo.block_count), 4);
}

/////////////////////////////////////////////////////////////////
void decode_partitionInfo(unsigned char *in, PartitionInfo *partInfo)
{
	partInfo->bootable = *in;
	partInfo->partitionId = *(in + 4);

	memcpy((unsigned char *)&(partInfo->block_start), (in + 8), 4);
	memcpy((unsigned char *)&(partInfo->block_count), (in + 12), 4);
}

/////////////////////////////////////////////////////////////////
void get_SDInfo(u64 block_count, SDInfo *sdInfo)
{
	int C, H, S;

	int C_max = 1023, H_max = 255, S_max = 63 * 8; // S_max = 63;
	int H_start = 1, S_start = 1;
	int diff_min = 0, diff = 0;

	if (block_count >= _8_4GB)
	{
		sdInfo->addr_mode = LBA_MODE;
	}
	else
	{
		sdInfo->addr_mode = CHS_MODE;
	}

	//-----------------------------------------------------
	if (sdInfo->addr_mode == CHS_MODE)
	{
		diff_min = C_max;

		for (H = H_start; H <= H_max; H++)
		{
			for (S = S_start; S <= S_max; S++)
			{
				C = block_count / (H * S);

				if ((C <= C_max))
				{
					diff = C_max - C;
					if (diff <= diff_min)
					{
						diff_min = diff;
						sdInfo->C_end = C;
						sdInfo->H_end = H;
						sdInfo->S_end = S;
					}
				}
			}
		}
	}
	//-----------------------------------------------------
	else
	{
		sdInfo->C_end = 1023;
		sdInfo->H_end = 254;
		// sdInfo->S_end = 63;
		sdInfo->S_end = 63 * 8;
	}

	//-----------------------------------------------------
	sdInfo->C_start = 0;
	sdInfo->H_start = 1;
	sdInfo->S_start = 1;

	sdInfo->total_block_count = block_count;
	sdInfo->available_block = sdInfo->C_end * sdInfo->H_end * sdInfo->S_end;
	sdInfo->unit = sdInfo->H_end * sdInfo->S_end;
}

/////////////////////////////////////////////////////////////////
void make_partitionInfo(int LBA_start, int count, SDInfo sdInfo, PartitionInfo *partInfo)
{
	int temp = 0;
	int _10MB_unit;

	partInfo->block_start = LBA_start;

	//-----------------------------------------------------
	if (sdInfo.addr_mode == CHS_MODE)
	{
		partInfo->C_start = partInfo->block_start / (sdInfo.H_end * sdInfo.S_end);
		temp = partInfo->block_start % (sdInfo.H_end * sdInfo.S_end);
		partInfo->H_start = temp / sdInfo.S_end;
		partInfo->S_start = temp % sdInfo.S_end + 1;

		if (count == BLOCK_END)
		{
			_10MB_unit = calc_unit(_10MB, sdInfo);
			partInfo->block_end = sdInfo.C_end * sdInfo.H_end * sdInfo.S_end - _10MB_unit - 1;
			partInfo->block_count = partInfo->block_end - partInfo->block_start + 1;

			partInfo->C_end = partInfo->block_end / sdInfo.unit;
			partInfo->H_end = sdInfo.H_end - 1;
			partInfo->S_end = sdInfo.S_end;
		}
		else
		{
			partInfo->block_count = count;

			partInfo->block_end = partInfo->block_start + count - 1;
			partInfo->C_end = partInfo->block_end / sdInfo.unit;

			temp = partInfo->block_end % sdInfo.unit;
			partInfo->H_end = temp / sdInfo.S_end;
			partInfo->S_end = temp % sdInfo.S_end + 1;
		}
	}
	//-----------------------------------------------------
	else
	{
		partInfo->C_start = 0;
		partInfo->H_start = 1;
		partInfo->S_start = 1;

		partInfo->C_end = 1023;
		partInfo->H_end = 254;
		// partInfo->S_end         = 63;
		partInfo->S_end = 63 * 8;

		if (count == BLOCK_END)
		{
			_10MB_unit = calc_unit(_10MB, sdInfo);
			partInfo->block_end = sdInfo.total_block_count - _10MB_unit - 1;
			partInfo->block_count = partInfo->block_end - partInfo->block_start + 1;
		}
		else
		{
			partInfo->block_count = count;
			partInfo->block_end = partInfo->block_start + count - 1;
		}
	}
}

/////////////////////////////////////////////////////////////////
int make_mmc_partition(int total_block_count, unsigned char *mbr, int flag, char *const argv[])
{
	int block_start = 0, block_offset = 0;
	u64 size = 0, block_mean = 0;

	SDInfo sdInfo;
	PartitionInfo partInfo[4];

	///////////////////////////////////////////////////////////
	memset((unsigned char *)&sdInfo, 0x00, sizeof(SDInfo));

	///////////////////////////////////////////////////////////
	get_SDInfo(total_block_count, &sdInfo);

	///////////////////////////////////////////////////////////
	block_mean = total_block_count - calc_unit((g_raw_area_size + SYSTEM_PART_SIZE), sdInfo);
	block_mean /= 3;

	///////////////////////////////////////////////////////////
	// �ݵ�� Unit������ ���� ����Ѵ�.
	size = simple_strtol(argv[3], NULL, 0);
	g_raw_area_size = size * 1024 * 1024;
	block_start = calc_unit(g_raw_area_size /*DISK_START*/, sdInfo);
	if (flag == 0)
	{
		block_offset = calc_unit(SYSTEM_PART_SIZE, sdInfo);
	}
	else if (flag == 1)
	{
		size = simple_strtol(argv[4], NULL, 0);
		block_offset = calc_unit((size * 1024 * 1024), sdInfo);
	}

	partInfo[0].bootable = 0x00;
	// partInfo[0].partitionId	= 0x0C;
	partInfo[0].partitionId = 0x83; // change by lyy 2017-3-20

	make_partitionInfo(block_start, block_offset, sdInfo, &partInfo[0]);

	///////////////////////////////////////////////////////////
	block_start += block_offset;

	if (flag == 0)
	{
		if (strcmp(argv[2], "1") == 0)
		{ // TF card
			block_offset = block_mean;
		}
		else
		{
			block_offset = block_mean;
		}
	}
	else if (flag == 1)
	{
		size = simple_strtol(argv[5], NULL, 0);
		block_offset = calc_unit((size * 1024 * 1024), sdInfo);
	}

	partInfo[1].bootable = 0x00;
	partInfo[1].partitionId = 0x83;

	make_partitionInfo(block_start, block_offset, sdInfo, &partInfo[1]);

	///////////////////////////////////////////////////////////
	block_start += block_offset;
	if (flag == 0)
	{
		block_offset = block_mean;
	}
	else if (flag == 1)
	{
		size = simple_strtol(argv[6], NULL, 0);
		block_offset = calc_unit((size * 1024 * 1024), sdInfo);
	}

	partInfo[2].bootable = 0x00;
	partInfo[2].partitionId	= 0x83;
	//partInfo[2].partitionId = 0x0C; // modify by lyy 2022.11.30

	make_partitionInfo(block_start, block_offset, sdInfo, &partInfo[2]);

	///////////////////////////////////////////////////////////
	block_start += block_offset;
	block_offset = BLOCK_END;

	partInfo[3].bootable = 0x00;
	partInfo[3].partitionId = 0x83;

	make_partitionInfo(block_start, block_offset, sdInfo, &partInfo[3]);

	///////////////////////////////////////////////////////////
	memset(mbr, 0x00, sizeof(*mbr) * 512); // liang, clean the mem again
	mbr[510] = 0x55;
	mbr[511] = 0xAA;

	encode_partitionInfo(partInfo[0], &mbr[0x1BE]);
	encode_partitionInfo(partInfo[1], &mbr[0x1CE]);
	encode_partitionInfo(partInfo[2], &mbr[0x1DE]);
	encode_partitionInfo(partInfo[3], &mbr[0x1EE]);

	return 0;
}

/////////////////////////////////////////////////////////////////
int get_mmc_block_count(char *device_name)
{
	// int rv;
	struct mmc *mmc;
	int block_count = 0;
	int dev_num;

	dev_num = simple_strtoul(device_name, NULL, 0);

	mmc = find_mmc_device(dev_num);
	if (!mmc)
	{
		printf("mmc/sd device is NOT founded.\n");
		return -1;
	}

	/*modify by lyy 20170724*/
	// block_count = mmc->block_dev.lba * (mmc->read_bl_len / BLOCK_SIZE);   //uboot2015
	block_count = mmc->capacity / BLOCK_SIZE; // uboot2017

	/*
	   printf("mmc->capacity: %llu\n",mmc->capacity);
	   printf("mmc->capacity_user: %llu\n",mmc->capacity_user);
	   printf("mmc->capacity_boot: %llu\n",mmc->capacity_boot);
	   printf("mmc->capacity_rpmb: %llu\n",mmc->capacity_rpmb);


	   printf("block_count = %d\n", block_count);
	   printf("mmc->block_dev.dev: %d\n",mmc->block_dev.dev);
	   printf("mmc->block_dev.part_type: %d\n",mmc->block_dev.part_type);
	   printf("mmc->block_dev.target: %d\n",mmc->block_dev.target);
	   printf("mmc->block_dev.lun: %d\n",mmc->block_dev.lun);
	   printf("mmc->block_dev.type: %d\n",mmc->block_dev.type);
	   printf("mmc->block_dev.lba: %llu\n",mmc->block_dev.lba);
	   printf("mmc->block_dev.blksz: %lu\n",mmc->block_dev.blksz);
	   printf("mmc->block_dev.log2blksz: %d\n",mmc->block_dev.log2blksz);*/
	return block_count;
}

int print_mmc_debug_info(int argc, char *device_name)
{
	struct mmc *mmc;
	int block_count = 0;
	int dev_num;

	dev_num = simple_strtoul(device_name, NULL, 0);

	mmc = find_mmc_device(dev_num);
	if (!mmc)
	{
		printf("mmc/sd device is NOT founded.\n");
		return -1;
	}

	/*modify by lyy 20170724*/
	// block_count = mmc->block_dev.lba * (mmc->read_bl_len / BLOCK_SIZE);   //uboot2015
	block_count = mmc->capacity / BLOCK_SIZE; // uboot2017

	printf("mmc->capacity: %llu\n", mmc->capacity);
	printf("mmc->capacity_user: %llu\n", mmc->capacity_user);
	printf("mmc->capacity_boot: %llu\n", mmc->capacity_boot);
	printf("mmc->capacity_rpmb: %llu\n", mmc->capacity_rpmb);

	printf("block_count = %d\n", block_count);
	printf("mmc->read_bl_len = %u\n", mmc->read_bl_len);
	// printf("mmc->block_dev.dev: %d\n",mmc->block_dev.dev);
	// printf("mmc->block_dev.part_type: %d\n",mmc->block_dev.part_type);
	// printf("mmc->block_dev.target: %d\n",mmc->block_dev.target);
	// printf("mmc->block_dev.lun: %d\n",mmc->block_dev.lun);
	// printf("mmc->block_dev.type: %d\n",mmc->block_dev.type);
	// printf("mmc->block_dev.lba: %llu\n",mmc->block_dev.lba);
	// printf("mmc->block_dev.blksz: %lu\n",mmc->block_dev.blksz);
	// printf("mmc->block_dev.log2blksz: %d\n",mmc->block_dev.log2blksz);

	return 0;
}

/////////////////////////////////////////////////////////////////
int get_mmc_mbr(char *device_name, unsigned char *mbr)
{
	int rv;
	struct mmc *mmc;
	int dev_num;

	dev_num = simple_strtoul(device_name, NULL, 0);

	mmc = find_mmc_device(dev_num);
	if (!mmc)
	{
		printf("mmc/sd device is NOT founded.\n");
		return -1;
	}

	// rv = mmc->block_dev.block_read(dev_num, 0, 1, mbr);//uboot 2015-modify by lyy
	rv = blk_dread(mmc_get_blk_desc(mmc), 0, 1, mbr); // uboot 2017-modify by lyy

	if (rv == 1)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

/////////////////////////////////////////////////////////////////
int put_mmc_mbr(unsigned char *mbr, char *device_name)
{
	int rv;
	struct mmc *mmc;
	int dev_num;
	unsigned char mbr_r[512];

	dev_num = simple_strtoul(device_name, NULL, 0);
	mmc = find_mmc_device(dev_num);
	emmcdbg("find the dev-num %d,%s", dev_num, mmc->cfg->name);
	if (!mmc)
	{
		printf("mmc/sd device is NOT founded.\n");
		return -1;
	}

	blk_dread(mmc_get_blk_desc(mmc), 0, 1, mbr_r);
	if (memcmp(mbr_r, mbr, sizeof(mbr_r)) == 0)
	{
		printf("skip write mbr.\n");
		return 0; // mbr same, skip write. add by lyy 2022.11.30
	}

	printf("mbr write to mmc(%d).\n", dev_num);
	// rv = mmc->block_dev.block_write(dev_num, 0, 1, mbr);//uboot 2015-modify by lyy
	rv = blk_dwrite(mmc_get_blk_desc(mmc), 0, 1, mbr); // uboot 2015-modify by lyy
	if (rv == 1)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

/////////////////////////////////////////////////////////////////
int get_mmc_part_info(char *device_name, int part_num, int *block_start, int *block_count, unsigned char *part_Id)
{
	int rv;
	PartitionInfo partInfo;
	unsigned char mbr[512];

	rv = get_mmc_mbr(device_name, mbr);
	if (rv != 0)
	{
		return -1;
	}

	switch (part_num)
	{
	case 1:
		decode_partitionInfo(&mbr[0x1BE], &partInfo);
		*block_start = partInfo.block_start;
		*block_count = partInfo.block_count;
		*part_Id = partInfo.partitionId;
		break;

	case 2:
		decode_partitionInfo(&mbr[0x1CE], &partInfo);
		*block_start = partInfo.block_start;
		*block_count = partInfo.block_count;
		*part_Id = partInfo.partitionId;
		break;

	case 3:
		decode_partitionInfo(&mbr[0x1DE], &partInfo);
		*block_start = partInfo.block_start;
		*block_count = partInfo.block_count;
		*part_Id = partInfo.partitionId;
		break;

	case 4:
		decode_partitionInfo(&mbr[0x1EE], &partInfo);
		*block_start = partInfo.block_start;
		*block_count = partInfo.block_count;
		*part_Id = partInfo.partitionId;
		break;

	default:
		return -1;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////
int print_mmc_part_info(int argc, char *const argv[])
{
	int rv;

	PartitionInfo partInfo[4];

	rv = get_mmc_part_info(argv[2], 1, &(partInfo[0].block_start), &(partInfo[0].block_count),
						   &(partInfo[0].partitionId));

	rv = get_mmc_part_info(argv[2], 2, &(partInfo[1].block_start), &(partInfo[1].block_count),
						   &(partInfo[1].partitionId));

	rv = get_mmc_part_info(argv[2], 3, &(partInfo[2].block_start), &(partInfo[2].block_count),
						   &(partInfo[2].partitionId));

	rv = get_mmc_part_info(argv[2], 4, &(partInfo[3].block_start), &(partInfo[3].block_count),
						   &(partInfo[3].partitionId));

	printf("\n");
	printf("partion #    size(MB)     block start #    block count    partition_Id \n");

	if ((partInfo[0].block_start != 0) && (partInfo[0].block_count != 0))
	{
		printf("   1        %6d         %8d        %8d          0x%.2X \n",
			   (partInfo[0].block_count / 2048), partInfo[0].block_start,
			   partInfo[0].block_count, partInfo[0].partitionId);
	}

	if ((partInfo[1].block_start != 0) && (partInfo[1].block_count != 0))
	{
		printf("   2        %6d         %8d        %8d          0x%.2X \n",
			   (partInfo[1].block_count / 2048), partInfo[1].block_start,
			   partInfo[1].block_count, partInfo[1].partitionId);
	}

	if ((partInfo[2].block_start != 0) && (partInfo[2].block_count != 0))
	{
		printf("   3        %6d         %8d        %8d          0x%.2X \n",
			   (partInfo[2].block_count / 2048), partInfo[2].block_start,
			   partInfo[2].block_count, partInfo[2].partitionId);
	}

	if ((partInfo[3].block_start != 0) && (partInfo[3].block_count != 0))
	{
		printf("   4        %6d         %8d        %8d          0x%.2X \n",
			   (partInfo[3].block_count / 2048), partInfo[3].block_start,
			   partInfo[3].block_count, partInfo[3].partitionId);
	}
	rv = rv;
	return 1;
}

/////////////////////////////////////////////////////////////////
int create_mmc_fdisk(int argc, char *const argv[])
{
	int rv;
	int total_block_count;
	unsigned char mbr[512];

	memset(mbr, 0x00, 512);

	total_block_count = get_mmc_block_count(argv[2]);
	if (total_block_count < 0)
	{
		return -1;
	}

	make_mmc_partition(total_block_count, mbr, (argc == 7 ? 1 : 0), argv);

	rv = put_mmc_mbr(mbr, argv[2]);
	if (rv != 0)
	{
		return -1;
	}

	printf("fdisk is completed\n");

	argv[1][1] = 'p';
	print_mmc_part_info(argc, argv);
	return 0;
}

/////////////////////////////////////////////////////////////////
int do_fdisk(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	if ((argc == 3) || (argc >= 6))
	{
		if (strcmp(argv[1], "-c") == 0)
		{
			return create_mmc_fdisk(argc, argv);
		}
		else if (strcmp(argv[1], "-p") == 0)
		{
			return print_mmc_part_info(argc, argv);
		}
		else if (strcmp(argv[1], "-d") == 0)
		{
			return print_mmc_debug_info(argc, argv[2]);
		}
	}
	else
	{
		// printf("song-debug: sizeof(int):%2d  sizeof(long):%2d sizeof(64):%2d\n",sizeof(int),sizeof(long),sizeof(u64));
		// printf("Usage:\nfdisk <-p> <device_num>\n");
		// printf("fdisk <-c> <device_num> [<sys. part size(MB)> <user part size> <cache part size>]\n");
		printf("fdisk\t- fdisk for sd/mmc.\n");
		printf("fdisk -c <device_num> [<part start(MB)> <sys. part size(MB)> <user part size> <cache part size>]\t- create partition.\n");
		printf("example:fdisk -c 0 38 512 512 1024 \n");
		printf("fdisk -p <device_num>\t- print partition information\n");
		printf("fdisk -d <device_num>\t- print test and debug info\n");
	}
	return 0;
}

U_BOOT_CMD(
	fdisk, 7, 0, do_fdisk,
	"fdisk\t- fdisk for sd/mmc.\n",
	"-c <device_num> [<part start(MB)> <sys. part size(MB)> <user part size> <cache part size>]\t- create partition.\n"
	"example:fdisk -c mmc:0 38 512 512 1024 \n"
	"fdisk -p <device_num>\t- print partition information\n"
	"fdisk -d <device_num>\t- print test and debug info\n");
