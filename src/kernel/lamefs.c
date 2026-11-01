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
#define FILE_DATA_SIZE (512 - sizeof(struct lameFSHeader))
#define START_FILE_DATA ((uint8_t*)sector + sizeof(struct lameFSHeader))

uint32_t lamefs_bitmap[2048];
uint16_t sector[256];

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
    if (lamefs_bitmap[0] == 0xffffffff && lamefs_bitmap[1] >= 0x0001ffff) 
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

uint16_t find_empty_sector()
{
    for (size_t i = 0; i < 2048; i++)
    {
        if (lamefs_bitmap[i] == 0xffffffff) continue;

        for (size_t j = 0; j < 32; j++)
        {
            if ((lamefs_bitmap[i] & (1 << j)) == 0)
            {
                lamefs_bitmap[i] |= 1 << j;
                write_sector(BITMAP_START + i / 128, (uint16_t*)&lamefs_bitmap[i / 128]);
                return i * 32 + j;
            }
        }
        
    }

    return 0x0000;
}

/// @brief Writes a file to disk
/// @param data Data of the file. The pointer 
/// @param size Size of data im bytes
/// @param name Name of the file
/// @param type Dir/Link/File/Special
/// @param ownerId Owner ID
/// @param groupId Group ID
/// @return True if file was writen successfully. False if an error occured.
bool write_file(void* data, size_t size, char name[10], uint8_t type, uint8_t ownerId, uint8_t groupId)
{
    // Format header
    memcpy(header->fileName, name, 10);
    header->type_permisions = create_type_permisions(type, 7, 7, 7);
    header->ownerId = ownerId;
    header->groupId = groupId;

    header->creationTime = 0;   // TODO: Make use of real time
    header->creationDate = 0;
    header->modificationTime = 0;
    header->modificationDate = 0;

    size_t sectorSize = size / FILE_DATA_SIZE;
    if (size % FILE_DATA_SIZE > 0) sectorSize++;
    header->size = (uint16_t)sectorSize;

    header->startAddress = find_empty_sector();

    if (header->startAddress == 0x0000) return false;

    uint16_t address = header->startAddress;
    uint16_t nextAddress;

    // Format data
    do
    {
        if (size <= FILE_DATA_SIZE) 
        {
            nextAddress = 0x0000;
            memset(START_FILE_DATA, '\0', FILE_DATA_SIZE);
            memcpy(START_FILE_DATA, data, size);
        }
        else 
        {
            nextAddress = find_empty_sector();
            if (nextAddress == 0x0000) return false;
            memcpy(START_FILE_DATA, data, FILE_DATA_SIZE);
        }

        header->nextAddress = nextAddress;
        write_sector(address, sector);

        size -= FILE_DATA_SIZE;
        address = nextAddress;
    } while (nextAddress != 0);

    return true;
}

/// @brief Reads file from disk
/// @return Allocated pointer to data. When the file is not used anymore, you MUST free it!!!
void* read_file(uint16_t startAddress)
{
    read_sector(startAddress, sector);

    size_t size = header->size * FILE_DATA_SIZE;
    
    void* data = malloc(size);

    for (size_t i = 0; i < header->size; i++)
    {
        memcpy(data + i * FILE_DATA_SIZE, START_FILE_DATA, FILE_DATA_SIZE);
        if (header->nextAddress != 0x0000) read_sector(header->nextAddress, sector);
    }

    return data;
}

struct dir_entry
{
    char fileName[10];
    uint16_t startAdress;
}__attribute__((packed));


/*
void add_dir_entry(uint16_t dirStartAddress, char fileName[10], uint16_t fileStartAddress)
{
    read_sector(dirStartAddress)
}
*/