#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "data_and_interpolation.h"
#include "stdio.h"


void dispose()
{
    img *tmp;
    if(final_img && final_img->format->BytesPerPixel == 1)
        free(final_img->format->palette->colors);
    while(img_init)
    {
        free(img_init->r);
        free(img_init->g);
        free(img_init->b);
        free(img_init->a);
        tmp = img_init;
        img_init = img_init->nxt;
        free(tmp);
    }
}
void interpolate (const char* meth)
{
    img *curr, *start, *nw, *sample;
    curr = start = img_init;
    Uint32 floorx, i, k;
    int icurr = 0, j;
    double step = final_length == 1 ? 0 : (init_length - 1.0) / (final_length - 1.0), x = final_length == 1 ? (init_length - 1.0) / 2 : 0, lanczos_ker;
    double *lanczos_r, *lanczos_g, *lanczos_b, *lanczos_a;
    lanczos_r = malloc(init_width * init_height * sizeof(double));
    lanczos_g = malloc(init_width * init_height * sizeof(double));
    lanczos_b = malloc(init_width * init_height * sizeof(double));
    lanczos_a = malloc(init_width * init_height * sizeof(double));
    for(k = 0; k < final_length; ++k, x = k * step)
    {
        floorx = (Uint32)x;
        // ustalenie miejsca curr do wstawiania nowego elemtntu nw listy oraz poczatkowego elementu start do interpolacji nowego elementu nw metoda Lanczosa
        for(; icurr < floorx; icurr++)
        {
            curr = curr->nxt;
            if(icurr - KERSIZE > -2)
                do
                {
                    start = start->nxt;
                }
                while(!start->ifinit);
        }
        nw = malloc(sizeof(img));
        nw->nxt = curr->nxt;
        curr->nxt = nw;
        curr = nw;
        curr->ifinit = 0;
        curr->r = malloc(final_width * final_height);
        curr->g = malloc(final_width * final_height);
        curr->b = malloc(final_width * final_height);
        curr->a = malloc(final_width * final_height);
        // Interpolacja Lanczosa
        for(i = 0; i < init_width * init_height; i++)
        {
            lanczos_r[i] = lanczos_g[i] = lanczos_b[i] = lanczos_a[i] = 0;
            sample = start;
            for(j = floorx - KERSIZE + 1; j <= (int)(floorx + KERSIZE); j++)
            {
                lanczos_ker = get_lanczos_ker(x - j);
                lanczos_r[i]+=sample->r[i] * lanczos_ker;
                lanczos_g[i]+=sample->g[i] * lanczos_ker;
                lanczos_b[i]+=sample->b[i] * lanczos_ker;
                lanczos_a[i]+=sample->a[i] * lanczos_ker;
                if(j >= 0 && j < init_length - 1)
                    do
                    {
                        sample = sample->nxt;
                    }
                    while(!sample->ifinit);
            }
        }
        // Skalowania nowego obrazu nw wybrana metoda meth
        if(!strcmp(meth, "NEIGHBOR"))
        {
            neighbor_interp(curr->r, lanczos_r);
            neighbor_interp(curr->g, lanczos_g);
            neighbor_interp(curr->b, lanczos_b);
            neighbor_interp(curr->a, lanczos_a);
        }
        else if (!strcmp(meth, "LINEAR"))
        {
            lin_interp(curr->r, lanczos_r);
            lin_interp(curr->g, lanczos_g);
            lin_interp(curr->b, lanczos_b);
            lin_interp(curr->a, lanczos_a);
        }
        else
        {
            bic_interp(curr->r, lanczos_r);
            bic_interp(curr->g, lanczos_g);
            bic_interp(curr->b, lanczos_b);
            bic_interp(curr->a, lanczos_a);
        }
    }
    free(lanczos_r);
    free(lanczos_g);
    free(lanczos_b);
    free(lanczos_a);
}

double get_lanczos_ker(double x)
{
    return (x ? KERSIZE * sin(M_PI * x) * sin(M_PI * x / KERSIZE) / (M_PI * M_PI * x * x) : 1);
}

void neighbor_interp(Uint8 *dest, const double *src)
{
    long i, j, x0 = final_width == 1 ? (long)round((init_width - 1.0) / 2) : 0, y0;
	double dx = final_width == 1 ? 0 : (init_width - 1.0) / (final_width - 1.0), dy = final_height == 1 ? 0 :(init_height - 1.0) / (final_height - 1.0), res;
	for(i = 0; i < final_width; ++i, x0 = (long)round(i*dx))
		for(j = 0, y0 = final_height == 1 ? (long)round((init_height - 1.0) / 2) : 0; j < final_height; ++j, y0 = (long)round(j*dy))
        {
            res = src[y0 * init_width + x0];
            if(res < 0)
                dest[j * final_width + i] = 0;
            else if(res > 255)
                dest[j * final_width + i] = 255;
            else
                dest[j * final_width + i] = (Uint8)round(res);
        }
}

void lin_interp(Uint8 *dest, const double *src)
{
    long i, j, x0, y0;
	double x = final_width == 1 ? (init_width - 1.0) / 2 : 0, dx = final_width == 1 ? 0 : (init_width - 1.0) / (final_width - 1.0), dy = final_height == 1 ? 0 : (init_height - 1.0) / (final_height - 1.0), res;
	for(i = 0; i < final_width; ++i, x = i*dx)
	{
		if((x0 = (long)x) == init_width - 1)
			x0--;
		for(j = 0, y0 = final_height == 1 ? (long)round((init_height - 1.0) / 2) : 0; j < final_height; ++j, y0 = (long)round(j*dy))
        {
            res = (x - x0) * (src[y0 * init_width + x0 + 1] - src[y0 * init_width + x0]) + src[y0 * init_width + x0];
            if(res < 0)
                dest[j * final_width + i] = 0;
            else if(res > 255)
                dest[j * final_width + i] = 255;
            else
                dest[j * final_width + i] = (Uint8)round(res);
        }
	}
}

void bic_interp(Uint8 *dest, const double *src)
{
    long i, j, x0, y0;
    int k;
	double x = final_width == 1 ? (init_width - 1.0) / 2 : 0, y, dx = final_width == 1 ? 0 : (init_width - 1.0) / (final_width - 1.0), dy = final_height == 1 ? 0 :(init_height - 1.0) / (final_height - 1.0), a, b, c, res;
	double intrdts[3];
	for(i = 0; i < final_width; ++i, x = i*dx)
	{
		if((x0 = (long)round(x)) == init_width - 1)
			x0--;
		else if (!x0)
			x0++;
		for(j = 0, y = final_height == 1 ? (init_height - 1.0) / 2 : 0; j < final_height; ++j, y = j*dy)
		{
			if((y0 = (long)round(y)) == init_height - 1)
				y0--;
			else if (!y0)
				y0++;
			for (k = -1; k < 2; k++)
			{
				a = 0.5 * (src[(y0 + k) * init_width + x0 - 1] - 2 * src[(y0 + k) * init_width + x0] + src[(y0 + k) * init_width + x0 + 1]);
				b = a * (1 - 2 * x0) + src[(y0 + k) * init_width + x0] - src[(y0 + k) * init_width + x0 - 1];
				c = src[(y0 + k) * init_width + x0] - a * x0 * x0 - b * x0;
				intrdts[k + 1] = a * x * x + b * x + c;
			}
			a = 0.5 * (intrdts[0] - 2 * intrdts[1] + intrdts[2]);
			b = a * (1 - 2 * y0) + intrdts[1] - intrdts[0];
			c = intrdts[1] - a * y0 * y0 - b * y0;
			res = a * y * y + b * y + c;
			if(res < 0)
				dest[j * final_width + i] = 0;
			else if(res > 255)
				dest[j * final_width + i] = 255;
			else
				dest[j * final_width + i] = (Uint8)round(res);
		}
	}
}
