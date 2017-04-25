/**
 * water.c
 * 
 * a program that reports a user’s water usage, 
 * converting minutes spent in the shower to bottles of drinking water, 
 * per the below:
 * 
 * $ ./water
 * Minutes: 10
 * Bottles: 120
 */

#include <cs50.h>
#include <stdio.h>

int main(void)
{
    printf("Minutes: ");
    int minutes = GetInt();
    int bottles = minutes * 12;
    printf("Bottles: %i\n", bottles);
    
    return 0;
}