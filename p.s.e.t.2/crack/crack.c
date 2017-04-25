/**
 * crack.c
 *
 * Program that cracks passwords, 
 * hashed with C’s DES-based (not MD5-based) crypt function, 
 * per the below:
 * 
 * $ ./crack 50fkUxYHbnXGw
 * rofl
 */
 
#define _XOPEN_SOURCE

#include <stdio.h> 
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
 
int main(int argc, char *argv[])
{
    // check there is one command-line argument
    if (argc != 2)
    {
        printf("Usage: ./crack hash\n");
        return 1;
    }
    
    // extract hash from argument
    char *hash = argv[1];
    
    // extract salt from hash
    char salt[2];
    salt[0] = hash[0];
    salt[1] = hash[1];
    
    // character array to store password possibilities 
    char key[5] = "A\0\0\0\0";
    
    // loop through password possibilities
    while (true)
    {
        
        // traverse through all four character positions
        for (int i = 0; i < 4; i++)
        {
            
            // after '\0' was incremented to 1 set it to 32 
            if (key[i] == 1)
                key[i] = 'A';
                
            // skip characters between 'Z' and 'a' 
            if (key[i] == 91)
                key[i] += 6;
            
            // after all characters was tried
            if (key[i] > 'z')
            {
                // after all 4 positions was tried
                if (i == 3) 
                {
                    printf("NOT FOUND\n");
                    return 2;
                }
                
                // start again at 'A' and increment '\0' on the left
                key[i] = 'A';
                key[i + 1]++;
            }
        }
        
        // increment first character
        key[0]++;
        
        // password was found
        if (strcmp(hash, crypt(key, salt)) == 0)
            break;
    }
        
    // print password
    printf("%s\n", key);
     
    return 0;
}
 