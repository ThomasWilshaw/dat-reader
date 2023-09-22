#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdint.h>

typedef struct FileHeader {
	unsigned long magic;            // 4Bytes, must be 0x42340299(little endian)
	unsigned long ver;              // 4Bytes, 0x02000001(little endian)
	char model[16];                 // 16Bytes, monitor model, e.g. "LM-2461W", "CM170". NOTE: model string must match target monitor's model
	char version[16];               // 16Bytes, data version, "x.y.zz". eg. "1.0.11"
	unsigned long data_checksum;    // 4Bytes, FileData sum(little endian)
	unsigned long length;           // 4Bytes, data length = 1048576 (little endian)
	char description[16];           // 16Bytes, 3dlut description info, eg. "CalSoftware"
	unsigned long reserved2;        // 4Bytes, reserved
	char name[16];                  // 16Bytes, information of lut
	char reserved[42];              // 43 Bytes, reserved
	uint8_t size;                   // 17 for 17x17x17
	unsigned char header_checksum;  // file header sum
} FileHeader;

typedef struct RGB {
    uint16_t r;
    uint16_t g;
    uint16_t b;
} RGB;

// Returns the RGB triplet from a 32bit chunk
RGB get_10_bit_RGB_from_32_bit_chunk(unsigned char* value)
{
    RGB rgb;
    rgb.r = (value[0] << 2) + (value[1] >> 6);

    uint8_t mask = 0x3f;
    rgb.g = ((value[1] & mask) << 4) + (value[2] >> 4);

    mask = 0x0f;
    rgb.b = ((value[2] & mask) << 6)+ (value[3] >> 2);

    if (rgb.r > 1023 || rgb.g > 1023 || rgb.b > 1023)
    {
        printf("ERROR:\n");
        printf("%d\t%d\t%d\n", rgb.r, rgb.g, rgb.b);
        printf("%.2x, %.2x, %.2x, %.2x\n", value[0], value[1], value[2], value[3]);
        exit(0);
    }

    return rgb;
}

int get_cube_size(unsigned long length)
{

}


int main()
{
    FILE *fp;
    fp = fopen("dit01.dat", "rb");
    if(fp == NULL)
    {
        printf("Failed to read file\n");
        exit(0);
    }

    FileHeader file_header;
    fread(&file_header, sizeof(file_header), 1, fp);

    printf("magic: %#010x\n", file_header.magic);
    printf("ver: %#010x\n", file_header.ver);
    printf("model: %s\n", file_header.model);
    printf("version: %s\n", file_header.version);
    printf("data_checksum: %#010x, %lu\n", file_header.data_checksum, file_header.data_checksum);
    printf("length: %lu\n", file_header.length);
    printf("description: %s\n", file_header.description);
    printf("reserved2: %lu\n", file_header.reserved2);
    printf("name: %s\n", file_header.name);
    printf("reserved: %s\n", file_header.reserved);
    printf("size: %hhu\n", file_header.size);
    printf("header_checksum: %u\n", file_header.header_checksum);

    rewind(fp);
    unsigned char header_sum = 0;
	unsigned char buf[128];
    fread(&buf, sizeof(buf), 1, fp);
    for (int i = 0; i < 127; i++) // 127 not 128 to avoid counting the checksum value itself
    {
	    header_sum += buf[i];
    }
    
    if (header_sum == file_header.header_checksum)
    {
        printf("Header checksum matches\n");
    } else
    {
        printf("ERROR: Header checksum does not match\n");
    }

    unsigned int data_sum = 0;
	unsigned char data_buf[19652];
	fread(&data_buf, sizeof(data_buf), 1, fp);
	for (int i = 0; i < 19652; i++)
    {
	    data_sum += (unsigned int)data_buf[i];
    }

    if (data_sum == file_header.data_checksum){
        printf("Data checksum matches\n");
    } else {
        printf("ERROR: Data checksum does not match\n");
    }

    unsigned char* value = data_buf;
    RGB test_rgb;
    
    for(int i = 0; i < 17*17*17; i++)
    {
        test_rgb = get_10_bit_RGB_from_32_bit_chunk(value + (i*4));
    }

    long end_of_data = ftell(fp);
    fseek(fp, 0, SEEK_END);

    if(end_of_data == ftell(fp))
    {
        printf("Reached EOF\n");
    }

    return 0;
}

// https://www.dremendo.com/c-programming-tutorial/c-binary-file-handling
// https://cryptii.com/pipes/integer-encoder
// https://flandersscientific.com/lut/


/*
>>> i= int('0000C000', 16)
>>> format(i, '0>32b')

https://stackoverflow.com/questions/12240299/convert-bytes-to-int-uint-in-c

https://stackoverflow.com/questions/10493411/what-is-bit-masking

https://www.tutorialspoint.com/c_standard_library/c_function_fseek.htm

*/