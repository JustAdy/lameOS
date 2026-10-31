#include <stdint.h>
#include <stddef.h>
#include <kernel/system.h>
#include <kernel/string.h>
#include <io/disk_driver.h>
#include <bool.h>
#include <kernel/lamefs.h>

#include <io/vga_terminal.h>

#define BITMAP_START 32
#define DATA_SECTION_START 48

uint32_t lamefs_bitmap[2048];
uint16_t sector[512];

bool bitmap_intialized = false;
bool dir_initialized = false;

struct lameFSHeader
{
    char fileName[10];
    uint16_t type_permisions;
    uint8_t ownerId;
    uint8_t groupId;
    uint16_t creationTime;
    uint16_t creationDate;
    uint16_t modificationTime;
    uint16_t modificationDate;
    uint16_t size;
    uint16_t startAddress;
    uint16_t nextAddress;
} __attribute__((packed));

struct lameFSHeader* header = (struct lameFSHeader*)sector;

static inline uint16_t create_type_permisions(uint8_t type, uint8_t user, uint8_t group, uint8_t other)
{
    return other << 8 | group << 5 | user << 2  | type;
}

/// @brief Checks if the file system is initialized. If not, initialize it. Initialized system has first 49 bits in the bitmap set and the root directory is initialized.
/// @return True if the file system was already initialized. False if it was not initialized.
bool initialize_lameFS()
{
    // Read bitmap from disk - sectors 32-48
    for (size_t i = 0; i < 16; i++)
    {
        read_sector(BITMAP_START + i, sector);
        memcpy(lamefs_bitmap + i * 512, sector, 512);
    }

    // Check if bitmap is initialized correctly
    if (lamefs_bitmap[0] == 0xffffffff && lamefs_bitmap[1] == 0x0001ffff) 
    {
        bitmap_intialized = true;
    }
    else
    {
        // Initialize bitmap
        lamefs_bitmap[0] = 0xffffffff;
        lamefs_bitmap[1] = 0x0001ffff;

        write_sector(BITMAP_START, (uint16_t*)lamefs_bitmap);
    }

    // Check if root directory exists
    read_sector(DATA_SECTION_START, sector);

    if (memcmp(header->fileName, "\0\0\0\0\0\0\0\0\0\0", 10) == 0 &&
        (header->type_permisions & 0b011) == 0 &&
        header->size > 0 &&
        header->startAddress == DATA_SECTION_START
        )
    {
        dir_initialized = true;
    }
    else
    {
        // Initialize root directory
        memcpy(header->fileName, "\0\0\0\0\0\0\0\0\0\0", 10);
        header->type_permisions = create_type_permisions(DIRECTORY, 7, 7, 7);
        header->creationTime = 0;   // TODO: Make use of real time
        header->creationDate = 0;
        header->modificationTime = 0;
        header->modificationDate = 0;
        header->size = 1;
        header->startAddress = DATA_SECTION_START;
        header->nextAddress = 0x0000;

        write_sector(DATA_SECTION_START, sector);
    }
    
    return bitmap_intialized & dir_initialized;
}

