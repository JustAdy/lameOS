# lameOS
This is a 32b operating system for learning purposes.

# lameFS
Custom file system for lameOS

Max 32 MiB per disk

1 Sector - Bootloader

31 Sectors - Kernel

16 Sectors - Bit array

65488 Sectors - Data (First sector is the start of the root directory)

## FILE

### File header

**28 bytes**

#### Name
10 bytes - name of the file

No file extension

#### Type

2 bits

d - directory - 0

l - link      - 1

f - file      - 2

s - special   - 3

#### Permisions

3 bits - rwx - user

3 bits - rwx - group

3 bits - rwx - other

5 bits - reserved

#### Owner

1 byte - user

1 byte - group


#### Creation time

2 bytes - The time when the file was created. Multiply seconds by 2.

|       |        |
|-------|--------|
| Hours | 5 bits |
|Minutes| 6 bits |
|Seconds| 5 bits |

#### Creation Date

2 bytes - Date when the file was created. Years since 1970.

|       |        |
|-------|--------|
| Years | 7 bits |
|Months | 4 bits |
| Days  | 5 bits | 

#### Modification time

2 bytes - Last modification time. Same as creation time.

#### Modification date

2 bytes - Last modification date. Same as creation date.

#### Size

2 bytes - Size in clusters

#### File start address

2 bytes - Address of the first sector

#### File next address

2 bytes - Address of the next sector

0x0000 - End of file. No more sectors. Address of the first root directory sector e.g. not a valid next address

___

### File data

484 bytes - File data
