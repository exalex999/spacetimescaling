#ifndef DATA_AND_INTERPOLATION_H_INCLUDED
#define DATA_AND_INTERPOLATION_H_INCLUDED

#include "SDL2/SDL_image.h"

#define KERSIZE 3                               // rozmiar jadra Lanczosa

// obraz-element listy
typedef struct img
{
    Uint8 *r, *g, *b, *a;
    int ifinit;                                 // flaga obrazow wejsciowych/wyjsciowych
    struct img *nxt;
} img;

img *img_init, *lastcell;
Uint32 init_width, init_height, init_length, final_width, final_height, final_length;
SDL_Surface *final_img;                         // szablon obrazow wysciowych

void dispose();                                 // usuwa liste obrazow oraz szablon dla obrazow wysciowych z pamieci

void interpolate (const char*);
double get_lanczos_ker(double);
void neighbor_interp(Uint8*, const double*);
void lin_interp(Uint8*, const double*);
void bic_interp(Uint8*, const double*);

#endif // DATA_AND_INTERPOLATION_H_INCLUDED
