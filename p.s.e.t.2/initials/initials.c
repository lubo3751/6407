/**
 * initials.c
 *
 * a program that, given a person’s name, prints a person’s initials, 
 * per the below:
 * 
 * $ ./initials
 * Regulus Arcturus Black
 * RAB
 */

#include <cs50.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

// function prototype
void print_init(string name);

int main(void)
{
    // prompt for user input
    string name = GetString();
    
    // print initials
    print_init(name);
    
    return 0;
}

/***** print capitalized initials function *****/
void print_init(string name)
{
    int i = 0;
    
    if (name != NULL)
    {
        // print first initial if there's no space(s)
        if (name[i] != ' ')
        {
            printf("%c", toupper(name[i]));
        }
        
        // print next initials
        for (int len = strlen(name); i < len; i++)
        {
            // if there's a space
            if (name[i] == ' ')
            {
                // if there's a letter after space(s)
                if (name[i + 1] != ' ' && name[i + 1] != '\0')
                {
                    // print initial
                    printf("%c", toupper(name[i + 1]));
                }
            }
        }
        printf("\n");
    }
}

