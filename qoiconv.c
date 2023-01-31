/*

Copyright (c) 2021, Dominic Szablewski - https://phoboslab.org
SPDX-License-Identifier: MIT


Command line tool to convert between png <> qoi format

Requires:
	-"stb_image.h" (https://github.com/nothings/stb/blob/master/stb_image.h)
	-"stb_image_write.h" (https://github.com/nothings/stb/blob/master/stb_image_write.h)
	-"qoi.h" (https://github.com/phoboslab/qoi/blob/master/qoi.h)

Compile with: 
	gcc qoiconv.c -std=c99 -O3 -o qoiconv

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
		puts("Usage: qoiconv <infile> <outfile>");
		puts("Examples:");
		puts("  qoiconv input.png output.qoi");
		puts("  qoiconv input.qoi output.png");
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
		qoi_desc desc;
		pixels = qoi_read(NULL, argv[1], &desc, 4);
		w = desc.width;
		h = desc.height;
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
		encoded = qoi_write(NULL, argv[2], pixels, &(qoi_desc){
			.width = w, .height = h, .channels = 4, .colorspace = QOI_SRGB
		});
	}

	if (!encoded) {
		printf("Couldn't write/encode %s\n", argv[2]);
		exit(1);
	}

	free(pixels);
	return 0;
}
