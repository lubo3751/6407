/** 
 * credit.c
 *
 * a program that determines whether a provided credit card number is valid 
 * according to Luhn’s algorithm, per the below:
 *
 * $ ./credit
 * Number: 378282246310005
 * AMEX
 */

#include <stdio.h>
#include <stdlib.h>
#include <cs50.h>

// stores credit card number
long long int cc_number;

// function prototypes
int cal_checksum(void);
char val_identif(void);
void val_length(char company);

int main(void)
{
    int checksum; 
    char identifier = '\0';
    
    // prompt for user input
    printf("Number: ");
    cc_number = get_long_long();
    
    // calculate checksum
    checksum = cal_checksum();
    
    // validate checksum
    if (checksum % 10 != 0)
    {
        printf("INVALID\n");
        return 0;
    }
    
    // validate company's identifier
    identifier = val_identif();
    
    if (identifier == 'I')
    {
        printf("INVALID\n");
        return 0;
    }
    
    // validate number's lengtt and print result
    val_length(identifier);
    
    return 0;
}


/***** calculates checksum *****/
int cal_checksum(void)
{
    int mltpl = 0, check = 0;
    long long int numb = cc_number;
    
    // add digits to 'check' starting from rightmost
    while (numb != 0)
    {
        check += numb % 10;
        mltpl = ((numb /= 10) % 10) * 2;
        
        // add each digit separately 
        while (mltpl != 0)
        {
            check += mltpl % 10;
            check += (mltpl /= 10) % 10;
            mltpl /= 10;
        }
        
        // get rid of rightmost digit
        numb /= 10;
    }
    return check;
}


/***** validate company's identifier function *****/
char val_identif(void)
{
    char str_n[20], str_n2[2];
    int identif;
    char cmpn = '\0';
    
    // extract identirier
    sprintf(str_n, "%lli", cc_number);
    sprintf(str_n2, "%c%c", str_n[0], str_n[1]);
    identif = atoi(str_n2);
    
    // select company
    if (identif == 34 || identif == 37)
    {
        cmpn = 'A';
    }
    else if (identif > 50 && identif < 56)
    {
        cmpn = 'M';
    }
    else if (identif > 39 && identif < 49)
    {
        cmpn = 'V';
    }
    else
    {
        cmpn = 'I';
    }
    
    return cmpn;
}


/***** validate number's lengtt function *****/
void val_length(char company)
{
    int len = 0;
    
    // calculate length
    while (cc_number != 0)
    {
        len += 1;
        cc_number /= 10;
    }
        
    // validate length and print result
    if (company == 'A')
    {
        if (len == 15)
        {
            printf("AMEX\n");
        }
        else
        {
            printf("INVALID\n");
        }
    }
    else if (company == 'M')
    {
        if (len == 16)
        {
            printf("MASTERCARD\n");
        }
        else
        {
            printf("INVALID\n");
        }
    }
    else if (company == 'V')
    {
        if (len == 13 || len == 16)
        {
            printf("VISA\n");
        }
        else
        {
            printf("INVALID\n");
        }
    }    
}
