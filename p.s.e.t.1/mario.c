/**
 * mario.c
 * 
 * a program that prints out a double half-pyramid of a specified height, 
 * per the below:
 *
 * $ ./mario
 * Height: 4
 *    #  #
 *   ##  ##
 *  ###  ###
 * ####  ####
 */

#include <cs50.h>
#include <stdio.h>

// function prototypes
int get_height(void);
void print_pyr(int height);

int main(void)
{
    // get user input
    int height = get_height();

    // print the pyramid
    print_pyr(height);

    return 0;
}

/*** get height of pyramid (from the user) ***/
int get_height(void)
{
    int height;
   
    do
    {
        printf("Height: ");
        height = GetInt();
    }
    while ((height < 0) || (height > 23));   
    
    return height;
}

/*** print the pyramid ***/
void print_pyr(int height)
{
    // loop throught each floor
    for (int i = height; i > 0; i--)
    {
        // print left spaces
        for (int j = i; j > 1; j--)
            printf(" ");
        
        // print left blocks
        for (int j = i; j <= height; j++)
            printf("#");
        
        // print the gap
        printf("  ");
        
        // print right blocks
        for (int j = i; j <= height; j++)
            printf("#");
            
        printf("\n");
    }
    
    return;
}