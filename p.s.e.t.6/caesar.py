#  caesar.py
# 
# a program that encrypts messages using Caesar’s cipher, per the below.
#
# python caesar.py 13
# plaintext:  HELLO
# ciphertext: URYYB

import cs50
import sys

# check command-line argument
if len(sys.argv) != 2:
    print("Usage: ./caesar k")
    exit (1)
    
key = int(sys.argv[1])

# prompt for user input
print("plaintext: ", end ="")
text = cs50.get_string()

# print ciphertext
print("ciphertext: ", end ="")
for i in text:
    if i.isalpha():
        if i.islower():
            letter = ord(i)
            letter2 = letter - 96
            result = (letter2 + key) % 26
            print("{}".format(chr(result + 96)), end ="");
        else:
            letter = ord(i)
            letter2 = letter - 64
            result = (letter2 + key) % 26
            print("{}".format(chr(result + 64)), end = "");
    else:
        print(i, end ="")
print()

exit(0)