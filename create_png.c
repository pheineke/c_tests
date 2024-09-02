#include <stdio.h>
#include <stdlib.h>
#include <png.h>

// Funktion zur Erstellung eines PNG-Bildes
void write_png_file(const char *filename, png_bytep *row_pointers, int width, int height) {
    FILE *fp = fopen(filename, "wb");
    if(!fp) abort();

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) abort();

    png_infop info = png_create_info_struct(png);
    if (!info) abort();

    if (setjmp(png_jmpbuf(png))) abort();

    png_init_io(png, fp);

    // Bildattribute setzen
    png_set_IHDR(
        png,
        info,
        width, height,
        8, // bit depth
        PNG_COLOR_TYPE_RGBA, // color type
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
    );
    png_write_info(png, info);

    // Bilddaten schreiben
    png_write_image(png, row_pointers);
    png_write_end(png, NULL);

    // Speicher freigeben
    for(int y = 0; y < height; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);

    fclose(fp);
    png_destroy_write_struct(&png, &info);
}

// Funktion zur Erstellung eines Bildes und Manipulation der Pixel
png_bytep *create_image(int width, int height) {
    png_bytep *row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(width * 4);
        for(int x = 0; x < width; x++) {
            png_bytep px = &(row_pointers[y][x * 4]);
            // Hintergrundfarbe (blau)
            px[0] = 0;    // Rot
            px[1] = 0;    // Grün
            px[2] = 255;  // Blau
            px[3] = 255;  // Alpha (Deckkraft)
        }
    }

    // Manipulation: Ein rotes Quadrat in der Mitte
    for(int y = height/4; y < 3*height/4; y++) {
        for(int x = width/4; x < 3*width/4; x++) {
            png_bytep px = &(row_pointers[y][x * 4]);
            px[0] = 255;  // Rot
            px[1] = 0;    // Grün
            px[2] = 0;    // Blau
            px[3] = 255;  // Alpha
        }
    }

    return row_pointers;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <width> <height>\n", argv[0]);
        return 1;
    }

    int width = atoi(argv[1]);
    int height = atoi(argv[2]);

    if (width <= 0 || height <= 0) {
        fprintf(stderr, "Error: width and height must be positive integers.\n");
        return 1;
    }

    png_bytep *row_pointers = create_image(width, height);
    write_png_file("output.png", row_pointers, width, height);
    printf("PNG-Bild 'output.png' mit Breite %d und Höhe %d wurde erstellt.\n", width, height);
    return 0;
}
