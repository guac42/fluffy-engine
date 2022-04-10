#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

unsigned char* Image::load(char const *filename, int *x, int *y, int *comp) {
    return stbi_load(filename, x, y, comp, 0);
}

void Image::free(unsigned char* data) {
    stbi_image_free(data);
}

void Image::flip_vertically(int val) {
    stbi_set_flip_vertically_on_load(val);
}