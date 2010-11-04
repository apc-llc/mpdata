#ifndef JPEGAPI_H
#define JPEGAPI_H
/*
 *  This header defines the external api to the jpeg library.
 */
#include "common.h"

JPEG_EXTERN(int)
write_JPEG_file(FILE * file, int image_width, int image_height, unsigned char* image_buffer,
				 int quality);
JPEG_EXTERN(int) read_JPEG_file(const char* filename, unsigned char** imageBuffer, int* width, int* height);

JPEG_EXTERN(void) free_image(unsigned char* imageData);

#endif //JPEGAPI_H
