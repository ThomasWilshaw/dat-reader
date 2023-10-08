#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

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

typedef struct CubeHeader {
    char title[256];
    int lut_size;
} CubeHeader;

typedef struct IntRGB {
    uint16_t r;
    uint16_t g;
    uint16_t b;
} IntRGB;

typedef struct FloatRGB {
    float r;
    float g;
    float b;
} FloatRGB;

// Returns the RGB triplet from a 32bit chunk
/*
    32bit RGB data is stored as follows:
    --BBBBBB BBBBGGGG GGGGGGRR RRRRRRRR
*/
IntRGB get_10_bit_RGB_from_32_bit_chunk(unsigned char* value)
{
    IntRGB rgb;
    // Initilise values
    rgb.r = 0;
    rgb.g = 0;
    rgb.b = 0;

   rgb.r = ((value[2] & 0x03) << 8) + value[3];

   rgb.g = ((value[1] & 0x0f) << 6) + ((value[2] & 0xfc) >> 2);

   rgb.b = ((value[0] << 4)) + ((value[1] & 0xf0)  >> 4);

    if (rgb.r > 1023 || rgb.g > 1023 || rgb.b > 1023)
    {
        printf("ERROR:\n");
        printf("%d\t%d\t%d\n", rgb.r, rgb.g, rgb.b);
        printf("%.2x, %.2x, %.2x, %.2x\n", value[0], value[1], value[2], value[3]);
        exit(0);
    }

    return rgb;
}

/* Converts an IntRGB into a 32bit chunk.
   Assumes output is an array of length 4
   Clears output variable
*/
void get_32_bit_chunk_from_10_bit_IntRGB(IntRGB rgb, unsigned char* output)
{
    output[0] = 0x00;
    output[1] = 0x00;
    output[2] = 0x00;
    output[3] = 0x00;

    output[0] = rgb.b >> 4;
    output[1] = ((rgb.b & 0x000f) << 4) + ((rgb.g & 0x03C0) >> 6);
    output[2] = ((rgb.g & 0x003f) << 2) + ((rgb.r & 0x0300) >> 8);
    output[3] = (rgb.r & 0x00ff);
}

// Convert IntRGB to FloatRGB. Supports 10 and 12 bit
FloatRGB convert_IntRGB_to_FloatRGB(IntRGB rgb, int bit_depth)
{
    FloatRGB output;
    
    if (bit_depth == 10)
    {
        output.r = rgb.r / 1023.0;
        output.g = rgb.g / 1023.0;
        output.b = rgb.b / 1023.0;

        return output;
    }

    if (bit_depth == 12){
        output.r = rgb.r / 4095.0;
        output.g = rgb.g / 4095.0;
        output.b = rgb.b / 4095.0;

        return output;
    }
    
    printf("ERROR: Invalid bit dpeth used\n");
    exit(1);
}

// Convert FloatRGB to int IntRGB. Supports 10 and 12 bit
IntRGB convert_FloatRGB_to_IntRGB(FloatRGB rgb, int bit_depth)
{
    IntRGB output;
    if (bit_depth == 10)
    {
        output.r = (int)(rgb.r * 1023.0 + 0.5);
        output.g = (int)(rgb.g * 1023.0 + 0.5);
        output.b = (int)(rgb.b * 1023.0 + 0.5);

        return output;
    }
    if (bit_depth == 12)
    {
        output.r = (uint16_t)floor(rgb.r * 4095.0 + 0.5);
        output.g = (uint16_t)floor(rgb.g * 4095.0 + 0.5);
        output.b = (uint16_t)floor(rgb.b * 4095.0 + 0.5);

        return output;
    }

    printf("ERROR: Invalid bit dpeth used\n");
    exit(1);
}


// Returns the cube size based on the data length
int get_cube_size(unsigned long length)
{
    if (length == 17*17*17*4)
    {
        return 17;
    }
    if (length == 17*17*17*8)
    {
        return 17;
    }
    if (length == 33*33*33*4)
    {
        return 33;
    }
    if (length == 33*33*33*8)
    {
        return 33;
    }

    return 0;
}

// Return the bytes per chunk based on the data length
int get_bytes_per_chunk(unsigned long length)
{
    if (length == 17*17*17*4)
    {
        return 4;
    }
    if (length == 17*17*17*8)
    {
        return 8;
    }
    if (length == 33*33*33*4)
    {
        return 4;
    }
    if (length == 33*33*33*8)
    {
        return 8;
    }

    return 0;

}

// Print IntRGB
void print_IntRGB(IntRGB rgb)
{
    printf("%d, %d, %d\n", rgb.r, rgb.g, rgb.b);
}

// Prints the header information from a dat file
void print_header(FileHeader file_header)
{
    printf("---HEADER---\n");
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
}

// Calculate the checksum of the .dat file's header. Assumes header is 128 bytes long
unsigned char calculate_header_sum(unsigned char* buf)
{   
    unsigned char header_sum = 0;
    for (int i = 0; i < 127; i++) // 127 not 128 to avoid counting the checksum value itself
    {
	    header_sum += buf[i];
    }

    return header_sum;
}

// Calculate the checksum of the .dat file's body.
unsigned int calculate_body_sum(unsigned char* data_buf, uint16_t data_size)
{
    unsigned int data_sum = 0;
	for (int i = 0; i < data_size; i++)
    {
	    data_sum += (unsigned int)data_buf[i];
    }

    return data_sum;
}

// Creates and saves a cube file from the header information and file data from the dat file
int save_cube_file(FileHeader header, IntRGB* data, int bit_depth, int lut_size, char* output_name)
{
    FILE* cube;
    cube = fopen(output_name, "w");
    if(cube == NULL)
    {
        printf("Failed to read file (%s)\n", output_name);
        exit(0);
    }
    fprintf(cube, "TITLE %s\n", header.name);
    fprintf(cube, "LUT_3D_SIZE %d\n", lut_size);

    for(int i = 0; i < lut_size*lut_size*lut_size; i++)
    {
        FloatRGB float_rgb = convert_IntRGB_to_FloatRGB(data[i], bit_depth);
        fprintf(cube, "%.6f %.6f %.6f\n", float_rgb.r, float_rgb.g, float_rgb.b);
    }

    fclose(cube);

    return 1;
}

// Converts a dat file (fp) to a cube file (output)
int dat_to_cube(FILE* fp, char* output)
{
    // read header into FileHEader struct and print
    FileHeader file_header;
    fread(&file_header, sizeof(file_header), 1, fp);
    print_header(file_header);

    // calculate header checksum and compare
    rewind(fp);
	unsigned char buf[128];
    fread(&buf, sizeof(buf), 1, fp);
    unsigned char header_sum = calculate_header_sum(buf);

    if (header_sum == file_header.header_checksum)
    {
        printf("Header checksum matches (%d)\n", header_sum);
    } else
    {
        printf("ERROR: Header checksum does not match\n");
    }

    // figure out lut size
    int cube_size = get_cube_size(file_header.length);
    int bytes_per_chunk = get_bytes_per_chunk(file_header.length);
    printf("\n");
    printf("Cube size: %d\n", cube_size);
    printf("Bytes per chunk: %d\n", bytes_per_chunk);

    printf("\n---DATA---\n");

    uint16_t lut_size = cube_size*cube_size*cube_size; // number of rgb data points in the lut
    uint16_t data_size = lut_size*bytes_per_chunk; // size of entire data source

    // read the data into an array
    unsigned char* data_buf = malloc(data_size * sizeof(unsigned char));
    fread(data_buf, sizeof(unsigned char), data_size, fp);

    // calculate data checksum and compare
    unsigned int data_sum = calculate_body_sum(data_buf, data_size);

    if (data_sum == file_header.data_checksum)
    {
        printf("Data checksum matches (%d)\n", data_sum);
    } else {
        printf("ERROR: Data checksum does not match (%d)\n", data_sum);
    }

    // check we're at the end of the file
    long end_of_data = ftell(fp);
    fseek(fp, 0, SEEK_END);


    if(end_of_data == ftell(fp))
    {
        printf("Reached EOF, file is expected length\n");
        fclose(fp);
    } else{
        printf("ERROR: Not reached EOF, file is malformed\n");
        fclose(fp);
        exit(1);
    }

    // copy data to buffer and convert to RGB struct
    IntRGB* rgb_data = malloc(lut_size * sizeof(IntRGB));
    unsigned char* value = data_buf;
    IntRGB test_rgb;
    for(int i = 0; i < lut_size; i++)
    {
       rgb_data[i] = get_10_bit_RGB_from_32_bit_chunk(value + (i*4));
    }
    free(data_buf);

    save_cube_file(file_header, rgb_data, 10, cube_size, output);

    free(rgb_data);

    return 1;
}

// Print cmd usage
void print_usage()
{
    printf("Usage:\n");
    printf("\t-dtc input.dat output.cube\n");
    printf("\t\tConverts input.dat to output.cube\n\n");

    printf("\t-ctd input.cube input.dat\n");
    printf("\t\tConverts input.cube to output.dat (NOT IMPLEMENTED YET)\n");
}

// Reads the header data from a .cube into a CubeHeader struct
int read_cube_header(FILE* fp, CubeHeader* header)
{
    char buf[256];
    int line_count = 0;

    while(fgets(buf, 256, fp))
    {
        if(strncmp(buf, "TITLE", 5) == 0)
        {
            sscanf(buf, "TITLE \"%[^\"]\"", header->title);
            line_count++;
        }
        else if(strncmp(buf, "LUT_3D_SIZE", 11) == 0)
        {
            sscanf(buf, "LUT_3D_SIZE %d", &header->lut_size);
            line_count++;
        }
        else if(strncmp(buf, "#", 1) == 0 ||
                  strncmp(buf, "DOMAIN_MIN", 10) == 0 ||
                  strncmp(buf, "DOMAIN_MAX", 10) == 0)
        {
            line_count++;
            continue;
        } else {
            break;
        }
    }

    // the above code takes the file pointer one line to far so here we undo that.
    // move to the start of the file and then seek through the correct number of lines
    fseek(fp, 0, SEEK_SET);
    for(int i = 0; i < line_count; i++)
    {
        fgets(buf, 256, fp);
    }

    return 0;
}

// Print CubeHeader struct
void printf_cube_header(CubeHeader header)
{
    printf("---CUBE HEADER---\n");
    printf("Title: %s\n", header.title);
    printf("Lut size: %d\n", header.lut_size);
}

int cube_to_dat(FILE* fp, char* output)
{
    printf("converting cube to .dat: %s\n", output);

    CubeHeader header;

    read_cube_header(fp, &header);

    printf_cube_header(header);
    int lut_size = header.lut_size*header.lut_size*header.lut_size;

    // Read the float data into an array of FloatRGB structs
    char buf[256];
    //FloatRGB* rgb = malloc(lut_size * sizeof(FloatRGB));
    FloatRGB rgb;
    unsigned char chunk[4];
    unsigned char* data = malloc(lut_size * 4 * sizeof(unsigned char));
    int i = 0;

    while(fgets(buf, 256, fp) != NULL){
        sscanf(buf, "%f %f %f", &rgb.r, &rgb.g, &rgb.b);
        IntRGB int_rgb = convert_FloatRGB_to_IntRGB(rgb, 10);
        print_IntRGB(int_rgb);
        get_32_bit_chunk_from_10_bit_IntRGB(int_rgb, chunk);
        memcpy(&data[i*4], chunk, 4);
        i++;
    }

    // generate data check sum
    int body_sum = calculate_body_sum(data, lut_size*4);

    // Create header
    FileHeader dat_header;
    dat_header.magic = 0x42340299;
    dat_header.ver =  0x02000001;
    dat_header.data_checksum = body_sum;
    dat_header.length = lut_size*4;
    strcpy(dat_header.description, "Made by Tom");
    dat_header.size = lut_size;
    dat_header.header_checksum = calculate_header_sum((unsigned char *)&dat_header);

    // write file
    FILE* out_file = fopen(output, "wb");
    if(out_file == NULL){
        printf("Cannot open file\n");
        exit(0);
    }

    fwrite(&dat_header, sizeof(dat_header), 1, out_file);
    fwrite(data, sizeof(unsigned char), lut_size * 4, out_file);

    fclose(out_file);


    return 0;
}


int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        print_usage();
        return 0;
    }

    if (strcmp(argv[1], "-dtc") == 0 && (argc == 3 || argc == 4)){
        FILE *fp;
        // "fsi_sample_luts\\dit04.dat"
        fp = fopen(argv[2], "rb");
        if(fp == NULL)
        {
            printf("Failed to read file, %s\n", argv[2]);
            exit(0);
        }
        if (argc == 4)
        {
            dat_to_cube(fp, argv[3]);
        } else
        {
            dat_to_cube(fp, "output.cube");
        }
    } else if(strcmp(argv[1], "-ctd") == 0 && (argc == 3 || argc == 4))
    {
        FILE *fp;
        // "fsi_sample_luts\\dit04.dat"
        fp = fopen(argv[2], "rb");
        if(fp == NULL)
        {
            printf("Failed to read file, %s\n", argv[2]);
            exit(0);
        }
        if (argc == 4)
        {
            cube_to_dat(fp, argv[3]);
        } else
        {
            cube_to_dat(fp, "output.dat");
        }
    } else
    {
        print_usage();
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