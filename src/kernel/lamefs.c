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
#define MAX_ENTRIES_PER_DIR 40

uint32_t lamefs_bitmap[2048];
uint16_t sector[256];

bool bitmap_intialized = false;
bool dir_initialized = false;

bool copy_data(void* dataPtr, size_t size, uint16_t* nextAddressPtr);
bool initialize_lameFS();
uint16_t find_empty_sector();
uint16_t write_file(void* data, size_t size, char name[10], uint8_t type, uint8_t ownerId, uint8_t groupId);
void* read_file(uint16_t startAddress);
void add_dir_entry(uint16_t dirStartAddress, char fileName[10], uint16_t fileStartAddress);
uint16_t update_file_data(uint16_t startAddress, void* data, size_t size);

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

static inline void clear_sector(uint16_t address)
{
    lamefs_bitmap[address / 32] &= ~(1 << (address % 32));
}

static inline void set_sector(uint16_t address)
{
    lamefs_bitmap[address / 32] |= 1 << address % 32;
}

bool copy_data(void* dataPtr, size_t size, uint16_t* nextAddressPtr)
{
    if (size <= FILE_DATA_SIZE) 
    {
        *nextAddressPtr = 0x0000;
        memset(START_FILE_DATA, '\0', FILE_DATA_SIZE);
        memcpy(START_FILE_DATA, dataPtr, size);
    }
    else 
    {
        *nextAddressPtr = find_empty_sector();
        if (*nextAddressPtr == 0x0000) return false;
        memcpy(START_FILE_DATA, dataPtr, FILE_DATA_SIZE);
    }

    return true;
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
                set_sector(i * 32 + j);
                write_sector(BITMAP_START + i / 128, (uint16_t*)&lamefs_bitmap[i / 128]);
                return i * 32 + j;
            }
        }
        
    }

    return 0x0000;
}

/// @brief Formats a lameFS header
/// @returns Allocated lameFS header. It must be freed when not used.
struct lameFSHeader* format_header(char name[10], uint8_t type, uint8_t ownerId, uint8_t groupId, uint16_t size, uint16_t startAddress, uint16_t nextAddress)
{
    struct lameFSHeader* header = malloc(sizeof(struct lameFSHeader));

    memcpy(header->fileName, name, 10);
    header->type_permisions = create_type_permisions(type, 7, 7, 7);
    header->ownerId = ownerId;
    header->groupId = groupId;

    header->creationTime = 0;   // TODO: Make use of real time
    header->creationDate = 0;
    header->modificationTime = 0;
    header->modificationDate = 0;

    header->size = size;
    header->startAddress = startAddress;
    header->nextAddress = nextAddress;

    return header;
}

/// @brief Writes a file to disk
/// @param data Data of the file. The pointer 
/// @param size Size of data in bytes
/// @param name Name of the file
/// @param type Dir/Link/File/Special
/// @param ownerId Owner ID
/// @param groupId Group ID
/// @return Returns start address of the file. If writing failed, it returns 0x0000
uint16_t write_file(void* data, size_t size, char name[10], uint8_t type, uint8_t ownerId, uint8_t groupId)
{
    size_t sectorSize = size / FILE_DATA_SIZE;
    if (size % FILE_DATA_SIZE > 0) sectorSize++;

    uint16_t address = find_empty_sector();

    if (address == 0x0000) return 0x0000;
    uint16_t nextAddress;

    struct lameFSHeader* formated_header = format_header(name, type, ownerId, groupId, sectorSize, address, 0x0000);

    memcpy(sector, formated_header, sizeof(struct lameFSHeader));

    free(formated_header, sizeof(struct lameFSHeader));

    uint8_t* dataPtr = data;

    // Format data
    do
    {
        if(!copy_data(dataPtr, size, &nextAddress)) return false;
        header->nextAddress = nextAddress;
        write_sector(address, sector);

        dataPtr += FILE_DATA_SIZE;
        size -= FILE_DATA_SIZE;  
        address = nextAddress;
    } while (nextAddress != 0);

    for (size_t i = 0; i < 16; i++)
    {
        write_sector(BITMAP_START + i, (uint16_t*)(lamefs_bitmap + i * 128));
    } 

    return header->startAddress;
}

/// @brief Reads file from disk and deletes it. At start of data is a 28 byte header
/// @return Allocated pointer to data. When the file is not used anymore, you MUST free it!!!
void* read_file(uint16_t startAddress)
{
    read_sector(startAddress, sector);

    size_t size = header->size * FILE_DATA_SIZE;
    
    void* data = malloc(size);

    memcpy(data, header, sizeof(struct lameFSHeader));

    lamefs_bitmap[startAddress / 32] &= ~(1 << (startAddress % 32));

    for (size_t i = 0; i < header->size; i++)
    {
        memcpy(data + i * FILE_DATA_SIZE + sizeof(struct lameFSHeader), START_FILE_DATA, FILE_DATA_SIZE);
        if (header->nextAddress != 0x0000)
        {
            read_sector(header->nextAddress, sector);
            lamefs_bitmap[header->nextAddress / 32] &= ~(1 << (header->nextAddress % 32));
        }
    }

    return data;
}

uint16_t update_file_data(uint16_t startAddress, void* data, size_t size)
{
    size_t sectorSize = size / FILE_DATA_SIZE;
    if (size % FILE_DATA_SIZE > 0) sectorSize++;

    uint16_t address = startAddress;
    uint8_t* dataPtr = data;

    bool updating = true;
    bool EOFClearBitmap = false;

    
    uint16_t nextAddress;
    uint16_t nextAddressToUnset;
    
    // Format data
    do
    {
        if (updating)
        {
            read_sector(address, sector);
            nextAddress = header->nextAddress;
            if (nextAddress == 0x0000)
            {
                updating = false;
                nextAddress = find_empty_sector();
                if (nextAddress == 0x0000) return 0x0000;
            }
        }
        else // if new size is bigger that the old one
        {
            nextAddress = find_empty_sector();
            if (nextAddress == 0x0000) return 0x0000;
        }

        if (size <= FILE_DATA_SIZE) 
        {
            nextAddress = 0x0000;
            memset(START_FILE_DATA, '\0', FILE_DATA_SIZE);
            memcpy(START_FILE_DATA, dataPtr, size);

            EOFClearBitmap = true;
            nextAddressToUnset = header->nextAddress;
        }
        else 
        {
            memcpy(START_FILE_DATA, dataPtr, FILE_DATA_SIZE);
        }

        header->nextAddress = nextAddress;
        header->size = sectorSize;
        write_sector(address, sector);

        if (EOFClearBitmap && nextAddressToUnset != 0x000)
        {
            // unset bitmap when updating with smaller file
            while (nextAddressToUnset > 0x0000)
            {
                clear_sector(nextAddressToUnset);
                read_sector(nextAddressToUnset, sector);
                nextAddressToUnset = header->nextAddress;
            }
        }

        dataPtr += FILE_DATA_SIZE;
        size -= FILE_DATA_SIZE;  
        address = nextAddress;
    } while (nextAddress != 0);

    for (size_t i = 0; i < 16; i++)
    {
        write_sector(BITMAP_START + i, (uint16_t*)(lamefs_bitmap + i * 128));
    } 

    return startAddress;
}

struct dir_entry
{
    char fileName[10];
    uint16_t startAddress;
}__attribute__((packed));


void add_dir_entry(uint16_t dirStartAddress, char fileName[10], uint16_t fileStartAddress)
{
    struct lameFSHeader* header = read_file(dirStartAddress);
    struct dir_entry* dir_sector = (struct dir_entry*)((uint8_t*)(header) + sizeof(struct lameFSHeader));

    size_t entryCount = 0;
    while (dir_sector[entryCount].startAddress != 0x0000) entryCount++;

    if (entryCount < MAX_ENTRIES_PER_DIR)
    {
        memcpy(dir_sector[entryCount].fileName, fileName, 10);
        dir_sector[entryCount].startAddress = fileStartAddress;
    }

    entryCount++;
    
    struct dir_entry* dir = malloc(sizeof(struct dir_entry) * entryCount);
    memcpy(dir, dir_sector,  sizeof(struct dir_entry) * entryCount);

    write_file(dir, sizeof(struct dir_entry) * entryCount, header->fileName, header->type_permisions & 0b011, header->ownerId, header->groupId);
    free(header, sizeof(struct lameFSHeader) + FILE_DATA_SIZE);
    free(dir, sizeof(struct dir_entry) * entryCount);
}