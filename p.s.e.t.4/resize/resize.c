/**
 * resize.c
 *
 * a program that resizes BMPs, per the below:
 *
 * $ ./resize 4 small.bmp large.bmp
 */
       
#include <stdio.h>
#include <stdlib.h>
#include "bmp.h"

static int TRIPLE = sizeof(RGBTRIPLE);

// function prototypes
void copy_scanline(BITMAPINFOHEADER old_bi, FILE *inptr, FILE *outptr, int factor);
void add_padding(int old_padding, int padding, FILE *inptr, FILE *outptr);

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./resize factor infile outfile\n");
        return 1;
    }

    // factor and filenames
    int factor = atoi(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];
    
    // ensure that factor is in [1, 100]
    if ((factor < 1) || (factor > 100))
    {
        printf("Usage: factor musts be within [1, 100]\n");
        return 1;
    }

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);
    
    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // keep old BITMAPINFOHEADER values
    BITMAPINFOHEADER old_bi;
    old_bi.biWidth = bi.biWidth;
    old_bi.biHeight = bi.biHeight;

    // create new BITMAPINFOHEADER values
    bi.biWidth *= factor;
    bi.biHeight *= factor;
    
    if ((factor == 2) || (factor == 3))
    {
        int intWidthPx;
        float fWidthPx, f_biSizeImg;
        
        intWidthPx = (4 - (bi.biWidth * TRIPLE) % 4) % 4;
        fWidthPx = (float) intWidthPx;
        
        f_biSizeImg = ((float) bi.biWidth + fWidthPx / 
                        (float) TRIPLE)
        * (float) abs(bi.biHeight) * (float) TRIPLE; 
        bi.biSizeImage = (int) f_biSizeImg;
    }
    else
    {
        bi.biSizeImage = abs(bi.biHeight) * TRIPLE * 
        (bi.biWidth + 
        ((4 - (bi.biWidth * TRIPLE) % 4) % 4) / TRIPLE);
    }
    
    bf.bfSize = bi.biSizeImage + bf.bfOffBits;

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // determine padding for scanlines
    int old_padding =  (4 - (old_bi.biWidth * TRIPLE) % 4) % 4;
    int new_padding = (4 - (bi.biWidth * TRIPLE) % 4) % 4;

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(old_bi.biHeight); i < biHeight; i++)
    {
        // resize horizontally
        copy_scanline(old_bi, inptr, outptr, factor);
        add_padding(old_padding, new_padding, inptr, outptr);
        
        // resize vertically
        int offset = old_bi.biWidth * (int) TRIPLE + old_padding;
        
        for (int j = 0; j < (factor - 1); j++)
        {
            fseek(inptr, -offset, SEEK_CUR);
            copy_scanline(old_bi, inptr, outptr, factor);
            add_padding(old_padding, new_padding, inptr, outptr);
        }
    }

    new_padding = 0;

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // that's all folks
    return 0;
}

void copy_scanline(BITMAPINFOHEADER old_bi, FILE *inptr, FILE *outptr, int factor)
{
    // iterate over pixels in scanline
    for (int i = 0; i < old_bi.biWidth; i++)
    {    
        {
            // temporary storage
            RGBTRIPLE triple;
        
            // read RGB triple from infile
            fread(&triple, TRIPLE, 1, inptr);
            
            // write RGB triple to outfile
            fwrite(&triple, TRIPLE, 1, outptr);
            
            // add pixel n - 1 times
            for (int j = 0; j < (factor - 1); j++)
            {
                fseek(inptr, -(int) TRIPLE, SEEK_CUR);
                fread(&triple, TRIPLE, 1, inptr);
                fwrite(&triple, TRIPLE, 1, outptr);
            }
        }
    }
}

void add_padding(int old_padding, int new_padding, FILE *inptr, FILE *outptr)
{
    // skip over padding, if any
    fseek(inptr, old_padding, SEEK_CUR);

    // then add it back (to demonstrate how)
    for (int k = 0; k < new_padding; k++)
    {
        fputc(0x00, outptr);
    }    
}