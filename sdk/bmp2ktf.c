#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct bmp_head
{
    uint16_t sign;
    uint32_t size;
    uint32_t _reserved;
    uint32_t offset;
};

struct bmp_info
{
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bpp;       //bits per pixel
    uint32_t compress;
    uint32_t input_size;
    uint32_t xppm;      //pix/meter
    uint32_t yppm;
    uint32_t col_used;
    uint32_t col_imp;
};

static FILE * input = NULL;
static FILE * output = NULL;

void panic(void)
{
    if (input)
        fclose(input);

    if (output)
        fclose(output);
}

int main(int argc, char * argv[])
{
    if (argc != 3) {
        printf(
            "Usage: bmp2ktf input_file output_file\n"
            "input_file must be in .bmp format\n"
            "output_file must be in .ktf format\n"
        );
        return 1;
    }
    atexit(panic);

    struct bmp_head head;
    struct bmp_info info;

    input = fopen(argv[1], "rb");
    if (!input) {
        fprintf(stderr, "Error: File \"%s\" does not exist\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    output = fopen(argv[2], "wb");
    if (!output) {
        fprintf(stderr, "Error: File \"%s\" does not exist\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    fread(&head.sign, 2, 1, input);
    fread(&head.size, 4, 1, input);
    fread(&head._reserved, 4, 1, input);
    fread(&head.offset, 4, 1, input);

    fread(&info.size, 4, 1, input);
    fread(&info.width, 4, 1, input);
    fread(&info.height, 4, 1, input);
    fread(&info.planes, 2, 1, input);
    fread(&info.bpp, 2, 1, input);
    fread(&info.compress, 4, 1, input);
    fread(&info.input_size, 4, 1, input);
    fread(&info.xppm, 4, 1, input);
    fread(&info.yppm, 4, 1, input);
    fread(&info.col_used, 4, 1, input);
    fread(&info.col_imp, 4, 1, input);

    if (head.sign != 0x4d42) {
        fprintf(stderr, "Error: File \"%s\" is not a .bmp file\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    if (info.compress != 0) {
        fprintf(stderr, "Error: File \"%s\" compression != 0\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    if (info.bpp != 24) {
        fprintf(stderr, "Error: File \"%s\" bits/pixel != 24\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    /*
    fseek(input, head.offset + info.height * (info.width - 1), SEEK_SET);

    for (uint32_t i = 0; i < info.height - 1; i++) {
        for (uint32_t j = 0; j < info.width - 1; j++) {
            uint8_t data[3] = {0};
            fread(data, 1, 3, input);
            fwrite(data, 1, 3, output);
            fseek(input, 1, SEEK_CUR);
        }
        fseek(input, head.offset + info.height * (info.width - 2 - i), SEEK_SET);
    }
    */

    fseek(input, head.offset, SEEK_SET);
    for (uint32_t i = 0; i < info.width; i++) {
        for (uint32_t j = 0; j < info.width; j++) {
            uint8_t rgb[3];
            fread(rgb, 1, 3, input);
            uint8_t temp = rgb[2];
            rgb[2] = rgb[0];
            rgb[0] = temp;
            fwrite(rgb, 1, 3, output);
        }
    }

    return 0;
}
