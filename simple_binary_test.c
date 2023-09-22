#include <stdio.h>
#include <conio.h>
#include <string.h>

typedef struct record
{
    int roll;
    char name[20];
} record;

int main()
{
    FILE *fp;

    // primitive data types
    int i = 56;
    float f = 45.256;
    double d = 12.54863;
    char c = 'd';

    // array
    int arr[3] = {15, 74, 29};


    // structure
    record x;
    x.roll = 1;
    strcpy(x.name, "Peter");

    fp = fopen("D:\\src\\dat_reader\\test.txt", "wb");
    if(fp == NULL){
        printf("Cannot open file\n");
        exit(0);
    }

    // writing primitive data types to binary file
    fwrite(&i, sizeof(i), 1, fp);
    fwrite(&f, sizeof(f), 1, fp);
    fwrite(&d, sizeof(d), 1, fp);
    fwrite(&c, sizeof(c), 1, fp);

    // writing an array to binary file
    fwrite(&arr, sizeof(arr), 1, fp);

    // writing a structure to binary file
    fwrite(&x, sizeof(x), 1, fp);

    fflush(fp);

    fclose(fp);

    /////////////////////////

    // primitives
    int read_i;
    float read_f;
    double read_d;
    char read_c;

    int read_arr[3];
    record read_x;

    fp = fopen("D:\\src\\dat_reader\\test.txt", "rb");
    if(fp == NULL)
    {
        printf("Could not open file\n");
        exit(0);
    }

    // read primatives
    fread(&read_i, sizeof(read_i), 1, fp);
    fread(&read_f, sizeof(read_f), 1, fp);
    fread(&read_d, sizeof(read_d), 1, fp);
    fread(&read_c, sizeof(read_c), 1, fp);

    // read array
    fread(&read_arr, sizeof(read_arr), 1, fp);

    // read structure
    fread(&read_x, sizeof(read_x), 1, fp);

    // display read data
    printf("i=%d\n", read_i);
    printf("f=%f\n", read_f);
    printf("d=%lf\n", read_d);
    printf("c=%c\n\n", read_c);

    for(int j = 0; j<3; j++)
    {
        printf("%d ", read_arr[j]);
    }

    printf("\n\nRoll=%d\n", read_x.roll);
    printf("Name=%s\n", read_x.name);

    fclose(fp);


    return 0;
}