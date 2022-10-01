/*

Command line tool to convert between png <> qoi format

Requires "stb_image.h" and "stb_image_write.h"
Compile with: 
	gcc qoiconv.c -std=c99 -O3 -o qoiconv

Dominic Szablewski - https://phoboslab.org


-- LICENSE: The MIT License(MIT)

Copyright(c) 2021 Dominic Szablewski

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files(the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and / or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions :
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/


#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_NO_LINEAR
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define QOI_IMPLEMENTATION
#include "qoi.h"


#define STR_ENDS_WITH(S, E) (strcmp(S + strlen(S) - (sizeof(E)-1), E) == 0)

int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Usage: qoiconv infile outfile\n");
		printf("Examples:\n");
		printf("  qoiconv image.png image.qoi\n");
		printf("  qoiconv image.qoi image.png\n");
		exit(1);
	}

	unsigned char *pixels = NULL;
	int w, h;
	if (STR_ENDS_WITH(argv[1], ".png")) {
		stbi_set_flip_vertically_on_load(1);
		pixels = (void *)stbi_load(argv[1], &w, &h, NULL, 4);
		// Premultiply alpha.
		for(int i = 0; i < 4*w*h; i += 4){
			pixels[i + 0] = pixels[i + 0]*pixels[i + 3]/255;
			pixels[i + 1] = pixels[i + 1]*pixels[i + 3]/255;
			pixels[i + 2] = pixels[i + 2]*pixels[i + 3]/255;
		}
	}
	else if (STR_ENDS_WITH(argv[1], ".qoi")) {
		pixels = qoi_read(NULL, argv[1], &w, &h, 4);
	}

	if (pixels == NULL) {
		printf("Couldn't load/decode %s\n", argv[1]);
		exit(1);
	}

	int encoded = 0;
	if (STR_ENDS_WITH(argv[2], ".png")) {
		encoded = stbi_write_png(argv[2], w, h, 4, pixels, 0);
	}
	else if (STR_ENDS_WITH(argv[2], ".qoi")) {
		encoded = qoi_write(NULL, argv[2], pixels, w, h, 4);
	}

	if (!encoded) {
		printf("Couldn't write/encode %s\n", argv[2]);
		exit(1);
	}

	return 0;
}
