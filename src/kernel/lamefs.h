#ifndef LAMEFS_H
#define LAMEFS_H

#define DIRECTORY 0
#define LINK 1
#define FILE 2
#define SPECIAL 3

bool initialize_lameFS();

uint16_t write_file(void* data, size_t size, char name[10], uint8_t type, uint8_t ownerId, uint8_t groupId);

void* read_file(uint16_t startAddress);

uint16_t update_file_data(uint16_t startAddress, void* data, size_t size);

void delete_file(uint16_t startAddress);

void add_dir_entry(uint16_t dirStartAddress, char fileName[10], uint16_t fileStartAddress);

#endif