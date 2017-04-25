/**
 * vigenere.c
 *
 * a program that encrypts messages using Vigenère’s cipher, per the below:
 *
 * $ ./vigenere ABC
 * plaintext:  HELLO
 * ciphertext: HFNLP
 */

#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// function prototypes
bool is_alpha(string key);
void print_cipher(string key, string message);

int main(int argc, string argv[])
{
    // check that there is one command-line argument
    if (argc != 2)
    {
        printf("Usage: ./vigenere k\n");
        return 1;
    }
    else
    {
        // extract a key from command-line argument
        string key = argv[1];
        
        // check that key is alphabetical
        bool alpha = is_alpha(key);
        
        if (!alpha)
        {
            printf("Usage: ./vigenere k\n");
            return 1;
        }
        else
        {
            // prompt for user input
            printf("plaintext: ");
            string message = GetString();
            
            // encrypt and print ciphertext
            print_cipher(key, message);
        }
    }
    return 0;
}

/**
 * function that check key is alphabetilac
 */
bool is_alpha(string key)
{
    for (int i = 0, len = strlen(key); i < len; i++) 
    {
        if (isalpha(key[i]))
            ;
        else
            return false;
    }
    return true;
}

/**
 * function that encipher plaintext and print ciphertext
 */
void print_cipher(string key, string message)
{
    printf("ciphertext: ");
    
    // find length of message and key
    int message_len = strlen(message);
    int key_len = strlen(key);
    
    // loop through chars in message
    for (int i = 0, j = 0; i < message_len; i++)
    {
        // char is alphabetic
        if (isalpha(message[i]))
        {
            // char is lower case
            if (islower(message[i]))
            {
            // encipher and print lower-case letter
                int letter = message[i];
                int letter2 = (letter - 96);
                int result = ((letter2 + ((int) tolower(key[j]) - 97)) % 26);
                printf("%c", (result + 96));    
            }
            else
            {
            // encipher and print upper-case letter
                int letter = message[i];
                int letter2 = (letter - 64);
                int result = ((letter2 + ((int) tolower(key[j]) - 97)) % 26);
                printf("%c", (result + 64)); 
            }
            j = ((j + 1) % key_len);
        }
        else
        {
        // print non-alphabetic char as it is
            printf("%c", message[i]);
        }
    }
    printf("\n");
}