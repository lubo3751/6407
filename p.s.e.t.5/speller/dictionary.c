/**
 * dictionary.c
 *
 * Implements a dictionary's functionality.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "dictionary.h"

typedef struct node
{
    char *word;
    struct node *next;
}
node;

node  *hash_table[HT_SIZE];
int word_count = 0;

// function prototype
int hash(const char *word);

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    // TODO
    
    int word_len = strlen(word);
    char lowCase_word[word_len + 1];  // holds a word converted to lowercase
    static node *traverser;
    
    // makes lower case word
    for (int i = 0; word[i]; i++) 
        lowCase_word[i] = tolower(word[i]);
        
    // marks the end of string
    lowCase_word[word_len] = '\0';
    
    // selects the correct-one SLL
    traverser = hash_table[hash(lowCase_word)];
    
    // searches the word in SLL
    while (traverser)
    {
        if (strcmp(traverser->word, lowCase_word) == 0)
            return true;
        else
            traverser = traverser->next;
    }

    return false;
}

/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load(const char *dictionary)
{
    // TODO
    
    char word_holder[LENGTH + 1];  // holds scanned word
    int hash_code;  // output of hash function
    
    // set hash-table's pointers to zero
    for (int i = 0; i < HT_SIZE; i++)
    {
        hash_table[i] = NULL;
    }
    
    // open dictionary
    FILE *dict_ptr = fopen(dictionary, "r");
    if (dict_ptr == NULL)
    {
        return false;
    }
    
    // scan one word a time from dictionary file
    while (fscanf(dict_ptr, "%s\n", word_holder) != EOF)
    {    
        // allocate memory for a new node
        node *node_ptr = malloc(sizeof(node));
        node_ptr->word = malloc(sizeof(word_holder));
        
        // copy scanned word to the node
        strcpy(node_ptr->word, word_holder);
        
        // get a hash code and increment the counter
        hash_code = hash(word_holder);
        word_count++;
        
        // add the node to the hash table
        if (hash_table[hash_code] == NULL)
        {
            hash_table[hash_code] = node_ptr;
            node_ptr->next = NULL;
        }
        else
        {
            node_ptr->next = hash_table[hash_code];
            hash_table[hash_code] = node_ptr;
        }
    }
    fclose(dict_ptr);
    
    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    // TODO
    
    return word_count;
}

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void)
{
    // TODO
    
    node *node_ptr, *nextnode_ptr;
    
    // loops through array of SLL (hash table)
    for (int i = 0; i < HT_SIZE; i++)
    {
        // points to beginning of SLL
        node_ptr = hash_table[i];
        // loop until end of SLL
        while (node_ptr)
        {
            // free allocated memory locations
            free(node_ptr->word);
            nextnode_ptr = node_ptr->next;
            free(node_ptr);
            node_ptr = nextnode_ptr;
        }
        // set hash-table's pointers to NULL
        hash_table[i] = NULL;
    }
    return true;
}

/**
 * Hash function
 */
int hash(const char *word)
{
    
    static int hash;
    hash = 0;
    
    while ( *word )
        hash += *word++;
    
    return hash % HT_SIZE;
}
