#ifndef GAMEFRAME_IMAGE_H
#define GAMEFRAME_IMAGE_H

class Image {
public:
    static unsigned char* load(char const *filename, int *x, int *y, int *comp);

    static void free(unsigned char* data);

    static void flip_vertically(int val);
};

#endif //GAMEFRAME_IMAGE_H
