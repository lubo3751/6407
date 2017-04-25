#  credit.py
# 
# a program that determines whether a provided credit card number is valid 
# according to Luhn’s algorithm, per the below:
#
# $ python credit.py
# Number: 378282246310005
# AMEX

import cs50
import math

def main():

    # prompt for user input
    print("Number: ", end="")
    number = cs50.get_int()
    str_number = str(number)
    list_number = list(str_number)
    
    # multiply every other digit
    multiply = []
    for i in range(len(list_number)//2):
        multiply.append(int(list_number[len(list_number)-(i*2)-2]) * 2)
    multiply.reverse()
    
    # sum those digits
    sum_multiply = 0
    for i in multiply:
        sum_multiply += sum_digits(i)
    
    # add to remaining digits
    sum_total = sum_multiply
    for i in range (math.ceil(len(list_number)/2)):
        sum_total += int(list_number[len(list_number)-1-i*2])

    # validate checksum, company's identifier, and number's length
    if sum_total % 10 != 0:
        print("INVALID")
    elif len(list_number) < 13 or len(list_number) == 14 or len(list_number) > 16:
        print("INVALID")
    elif list_number[0:1] == ['4']:
        print("VISA")
    elif list_number[0:2] == ['3', '4'] or list_number[0:2] == ['3', '5'] or list_number[0:2] == ['3', '6'] or list_number[0:2] == ['3', '7']:
        print("AMEX")
    elif list_number[0:2] == ['5', '1'] or list_number[0:2] == ['5', '2'] or list_number[0:2] == ['5', '3'] or list_number[0:2] == ['5', '4'] or list_number[0:2] == ['5', '5']:
        print("MASTERCARD")
    else:
        print("INVALID")

# sum digits of number
def sum_digits(n):
    s = 0 
    while n:
        s += n % 10
        n //= 10
    return s


if __name__ == "__main__":
    main()