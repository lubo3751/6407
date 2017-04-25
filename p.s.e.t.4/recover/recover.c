/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * a program that recovers JPEGs from a forensic image, per the below:
 *
 * $ ./recover card.raw
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

typedef uint8_t  BYTE;

// jpg's first four bytes pattern
const BYTE jpg_pattern1[4] = {0xff, 0xd8, 0xff, 0xe0};
const BYTE jpg_pattern2[4] = {0xff, 0xd8, 0xff, 0xe1};

int main(void)
{
    char jpg_name[4];
    int num_of_bytes, pattern1_ok, pattern2_ok, jpg_count = 0;
    BYTE buffer[512];  // size of FAT block
    FILE *img_pt = NULL, *card_pt = NULL;
    
    // open card.raw file (forensic image)
    card_pt = fopen("card.raw", "r");
    if (card_pt == NULL)
    {
        fprintf(stderr, "Could not open card.raw.\n");
        return 1;
    }
    
    // repeat until EOF
    while (1)
    {
        // read 512 bytes from card.raw into a buffer
        num_of_bytes = fread(buffer, 1, sizeof(buffer), card_pt);

        // compare first four bytes pattern
        pattern1_ok = pattern2_ok = 1;
        pattern1_ok = memcmp(buffer, jpg_pattern1, sizeof(jpg_pattern1));
        pattern2_ok = memcmp(buffer, jpg_pattern2, sizeof(jpg_pattern2));
    
        // if first four bytes pattern match
        if ((pattern1_ok == 0) || (pattern2_ok == 0))
        {
            // close jpg file that was written recently
            if (jpg_count > 0)
            {
                fclose(img_pt);
            }
            
            // create jpg name according to jpg counter
            sprintf(jpg_name, "%.3d.jpg", jpg_count);
            
            // open/create a new jpg file
            img_pt = fopen(jpg_name, "a");
            jpg_count++;
        }

        // if a new jpg file has been opened/created
        if (img_pt)
        {
            // write bits into the new jpg file
            fwrite(buffer, 1, num_of_bytes, img_pt);
        }
        
        // if EOF has been reached in card.raw, close opened files, and terminate the loop
        if (feof(card_pt))
        {
            fclose(img_pt);
            fclose(card_pt);
            break;
        }
    }
    return 0;
}