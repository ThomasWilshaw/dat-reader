# .dat Reader

A small utility script to convert Flanders Scientific Inc. (FSI) LUTs in the
`.dat` format to the `.cube` format and vice versa. Also includes the ability to
inspect `.dat` header information.

Currently only supports 10 bit `.dat` files and 17x17x17 cube files. However 
from here you can use OpenColorIO's (OCIO's) `ocioconvert` tool to convert to and 
from most common LUT formats.

Currently only tested on Windows and Windows Subsystem for Linux (WSL).

## Usage
```
Usage:
        -dtc input.dat output.cube
                Converts input.dat to output.cube

        -ctd input.cube input.dat
                Converts input.cube to output.dat (only works with cubes with a resolution of 17)

        -inspect input.dat
                Prints the dat header and checks the header checksum is correct
```
