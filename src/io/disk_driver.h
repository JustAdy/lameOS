#ifndef DISK_DRIVER_H
#define DISK_DRIVER_H
#include <stdint.h>

void read_sector(uint32_t lba, uint16_t *buffer);

void write_sector(uint32_t lba, const uint16_t *buffer);
#endif