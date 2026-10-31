#include <stdint.h>
#include <stddef.h>
#include <kernel/system.h>
#include <kernel/string.h>
#include <io/disk_driver.h>
#include <bool.h>

#include <io/vga_terminal.h>

uint32_t lamefs_bitmap[2048];
uint16_t sector[256];

bool bitmap_intialized = false;

/// @brief Checks if the file system is initialized. If not, initialize it. Initialized system has first 49 bits in the bitmap set and the root directory is initialized.
/// @return True if the file system was already initialized. False if it was not initialized.
bool initialize_lameFS()
{
    // Read bitmap from disk - sectors 32-48
    for (size_t i = 0; i < 16; i++)
    {
        read_sector(32 + i, sector);
        memcpy(lamefs_bitmap + i * 512, sector, 512);
    }

    // Check if bitmap is initialized correctly
    if (lamefs_bitmap[0] == 0xffffffff && lamefs_bitmap[1] == 0x0001ffff) bitmap_intialized = true;
    else
    {
        // Initialize bitmap
        lamefs_bitmap[0] = 0xffffffff;
        lamefs_bitmap[1] = 0x0001ffff;

        write_sector(33, (uint16_t*)lamefs_bitmap);
    }

    // Check if root directory exists
    


    return true;
    
}

struct lamefs
{
    

};

