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
    uint32_t img_size;
    uint32_t xppm;       //pix/meter
    uint32_t yppm;
    uint32_t col_used;
    uint32_t col_imp;
};

int main()
{
    struct bmp_head head;
    struct bmp_info info;
    FILE * img = fopen("img.bmp", "rb");
    fread(&head.sign, 2, 1, img);
    fread(&head.size, 4, 1, img);
    fread(&head._reserved, 4, 1, img);
    fread(&head.offset, 4, 1, img);

    fread(&info.size, 4, 1, img);
    fread(&info.width, 4, 1, img);
    fread(&info.height, 4, 1, img);
    fread(&info.planes, 2, 1, img);
    fread(&info.bpp, 2, 1, img);
    fread(&info.compress, 4, 1, img);
    fread(&info.img_size, 4, 1, img);
    fread(&info.xppm, 4, 1, img);
    fread(&info.yppm, 4, 1, img);
    fread(&info.col_used, 4, 1, img);
    fread(&info.col_imp, 4, 1, img);

    printf("%hu\n", head.sign);
    printf("%lu %ld\n", head.size, head.size);
    printf("%lu %hu\n", head.offset, head.offset);
    printf("%lu %ld\n", info.size, info.size);
    printf("%lu %ld\n", info.width, info.width);
    printf("%lu %ld\n", info.height, info.height);
    printf("%hu %hd\n", info.bpp, info.bpp);
    printf("%lu %ld\n", info.compress, info.compress);
    printf("%lu %ld\n", info.img_size, info.img_size);
    printf("%lu %ld\n", info.xppm, info.height);
    printf("%lu %ld\n", info.yppm, info.height);
    printf("%lu %ld\n", info.col_used, info.height);

    return 0;
}
