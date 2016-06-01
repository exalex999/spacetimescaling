#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <math.h>
#include "dataio.h"

int imgread(const char *path, const char *tmplt)
{
    char *fullpath;
    // odczyt wedlug szablonu/wszystkich bitmapow w folderze
    if(tmplt)
    {
        fullpath = malloc(strlen(path) + strlen(tmplt) + 1);
        strcpy(fullpath, path);
        int i;
        for(i = 1; i <= init_length; i++)
        {
            extract_tmplt(fullpath + strlen(path), tmplt, i);
            if(!add_cell(fullpath))
            {
                free(fullpath);
                return 0;
            }
        }
        free(fullpath);
    }
    else
    {
        DIR *dp;
        struct dirent *ep;
        if (!(dp = opendir(path)))
            return 0;
        while ((ep = readdir (dp)))
        {
            if(strcmp(ep->d_name + strlen(ep->d_name) - 4, ".bmp") && strcmp(ep->d_name + strlen(ep->d_name) - 4, ".BMP") && strcmp(ep->d_name + strlen(ep->d_name) - 4, ".dib") && strcmp(ep->d_name + strlen(ep->d_name) - 4, ".DIB"))
                continue;
            fullpath = malloc(strlen(path) + strlen(ep->d_name) + 1);
            strcat(strcpy(fullpath, path), ep->d_name);
            if(!add_cell(fullpath))
            {
                free(fullpath);
                closedir(dp);
                return 0;
            }
            init_length++;
            free(fullpath);
        }
        closedir(dp);
        if(!img_init)
            return 0;
    }
    return 1;
}
int imgwrite(const char *path, const char *tmplt)
{
    Uint32 n = 1, i, j, err, pixel, pixelsize = final_img->format->BitsPerPixel < 8 ? final_img->format->BitsPerPixel : 8 * final_img->format->BytesPerPixel;
    char fullpath[strlen(path) + strlen(tmplt) + 1];
    strcpy(fullpath, path);
    img *curr = img_init;
    while(curr)
    {
        if(!curr->ifinit)
        {
            final_img->pixels = malloc((size_t)ceil(final_width * final_height * pixelsize / 8.0));
            if(pixelsize < 8)
                for(i = 0; i < ceil(final_width * final_height * pixelsize / 8.0); i++)
                {
                    pixel = 0;
                    for(j = 0; j < 8 / pixelsize && i * 8 / pixelsize + j < final_width * final_height; j++)
                        pixel |= SDL_MapRGBA(final_img->format, (curr->r)[i * 8 / pixelsize + j], (curr->g)[i * 8 / pixelsize + j], (curr->b)[i * 8 / pixelsize + j], (curr->a)[i * 8 / pixelsize + j]) << (8 - (j + 1) * pixelsize);
                    memcpy(final_img->pixels + i, &pixel, 1);
                }
            else
                for(i = 0; i < final_width * final_height; i++)
                {
                    pixel = SDL_MapRGBA(final_img->format, (curr->r)[i], (curr->g)[i], (curr->b)[i], (curr->a)[i]);
                    if(i == 900)
                        pixel = pixel;
                    memcpy(final_img->pixels + i * final_img->format->BytesPerPixel, &pixel, final_img->format->BytesPerPixel);
                }
            extract_tmplt(fullpath + strlen(path), tmplt, n);
            err = SDL_SaveBMP(final_img, fullpath);
            free(final_img->pixels);
            if(err)
                return 0;
            n++;
        }
        curr = curr->nxt;
    }
    return 1;
}
int add_cell(const char *fullpath)
{
    SDL_Surface *image;
    SDL_RWops *rwop;
    Uint32 i;
    if(!(rwop = SDL_RWFromFile(fullpath, "rb")))
        return 0;
    if(!(image = IMG_LoadBMP_RW(rwop)))
    {
        rwop->close(rwop);
        return 0;
    }
    if(image->format->BitsPerPixel > 32)
    {
        rwop->close(rwop);
        SDL_FreeSurface(image);
        return 0;
    }
    // w liscie juz sa obrazy/nie ma obrazow
    if(lastcell)
    {
        if(init_width != image->w || init_height != image->h)
        {
            rwop->close(rwop);
            SDL_FreeSurface(image);
            return 0;
        }
        // zmiana szablonu obrazow wysciowych
        if(image->format->BitsPerPixel > final_img->format->BitsPerPixel)
        {
            if(final_img->format->BytesPerPixel == 1)
                free(final_img->format->palette->colors);
            if(!(final_img = SDL_CreateRGBSurface(0, final_width, final_height, image->format->BitsPerPixel, image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask)))
            {
                rwop->close(rwop);
                SDL_FreeSurface(image);
                return 0;
            }
            if(image->format->BytesPerPixel == 1)
            {
                final_img->format->palette->colors = malloc(image->format->palette->ncolors * sizeof(SDL_Color));
                memcpy(final_img->format->palette->colors, image->format->palette->colors, image->format->palette->ncolors * sizeof(SDL_Color));
            }
        }
        lastcell->nxt = malloc(sizeof(img));
        lastcell = lastcell->nxt;
    }
    else
    {
        // ustalenie szablonu obrazow wyjsciowych
        if(!(final_img = SDL_CreateRGBSurface(0, final_width, final_height, image->format->BitsPerPixel, image->format->Rmask, image->format->Gmask, image->format->Bmask, image->format->Amask)))
        {
            rwop->close(rwop);
            SDL_FreeSurface(image);
            return 0;
        }
        if(image->format->BytesPerPixel == 1)
        {
            final_img->format->palette->colors = malloc(image->format->palette->ncolors * sizeof(SDL_Color));
            memcpy(final_img->format->palette->colors, image->format->palette->colors, image->format->palette->ncolors * sizeof(SDL_Color));
        }
        init_width = image->w;
        init_height = image->h;
        img_init = lastcell = malloc(sizeof(img));
    }
    lastcell->r = malloc(init_width * init_height);
    lastcell->g = malloc(init_width * init_height);
    lastcell->b = malloc(init_width * init_height);
    lastcell->a = malloc(init_width * init_height);
    // kopiowanie obrazu wejsciowego do listy
    if(image->format->BytesPerPixel == 1)
        for(i = 0; i < init_width * init_height; i++)
            SDL_GetRGBA((Uint32)*((Uint8*)(image->pixels + i * image->format->BitsPerPixel / 8)) << (24 + i * image->format->BitsPerPixel % 8) >> (32 - image->format->BitsPerPixel), image->format, lastcell->r + i, lastcell->g + i, lastcell->b + i, lastcell->a + i);
    else
        for(i = 0; i < init_width * init_height; i++)
            SDL_GetRGBA(*((Uint32*)(image->pixels + i * image->format->BytesPerPixel)), image->format, lastcell->r + i, lastcell->g + i, lastcell->b + i, lastcell->a + i);

    lastcell->ifinit = 1;
    lastcell->nxt = NULL;
    rwop->close(rwop);
    SDL_FreeSurface(image);
    return 1;
}
int check_tmplt(const char *tmplt, const char * n)
{
    Uint32 snumb = 0, end = 0, i;
    if(strlen(tmplt) > 255)
        return 0;
    for(i = 0; *(tmplt + i); i++)
    {
        if(!(strcmp(tmplt + i, ".bmp") && strcmp(tmplt + i, ".BMP") && strcmp(tmplt + i, ".dib") && strcmp(tmplt + i, ".DIB")) && snumb >= strlen(n))
            return 1;
        if(*(tmplt + i) == '/')
            if(end)
                break;
            else
                snumb++;
        else if(snumb)
            end = 1;
    }
    return 0;
}
void extract_tmplt(char *dest, const char *tmplt, Uint32 n)
{
    strcpy(dest, tmplt);
    char *start = strchr(tmplt, '/'), *end = strrchr(tmplt, '/');
    char numb[end - start + 1];
    sprintf(numb, "%d", n);
    memset((void*)((intptr_t)dest + (intptr_t)start - (intptr_t)tmplt), '0', (intptr_t)end - (intptr_t)start - strlen(numb) + 1);
    memcpy((void*)((intptr_t)dest + (intptr_t)end - (intptr_t)tmplt - strlen(numb) + 1), numb, strlen(numb));
}
Uint32 convert_to_int(const char *str)
{
    Uint32 n = 0, i;
    if(*str == '0')
        return 0;
    for(i = 0; *(str + i); i++)
    {
        if(i > 3 || *(str + i) < '0' || *(str + i) > '9')
            return 0;
        n = 10 * n + *(str + i) - '0';
    }
    return n;
}
