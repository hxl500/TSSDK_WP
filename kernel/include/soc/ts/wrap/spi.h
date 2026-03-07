/***************************************************************************
 *                      COPYRIGHT NOTICE
 *               Copyright 2022 Tsingmicro, Inc.
 *                     All rights reserved.
 ***************************************************************************/

#ifndef __TS_WRAP_SPI_H__
#define __TS_WRAP_SPI_H__

#include <linux/spi/spi.h>

struct device_node *ts_of_get_dev_node_spi(struct spi_device *pspi_dev);

void *ts_spi_get_drvdata(struct spi_device *spi);
void ts_spi_set_drvdata(struct spi_device *spi, void *data);
unsigned int ts_spi_get_max_speed(struct spi_device *spi);
void ts_spi_message_init(struct spi_message *m);
void ts_spi_message_add_tail(struct spi_transfer *t, struct spi_message *m);

#endif

