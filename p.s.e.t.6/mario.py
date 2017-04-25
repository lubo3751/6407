#  mario.py
# 
# a program that prints out a double half-pyramid of a specified height, 
# per the below:
#
# $ python mario.py
# Height: 4
#    #  #
#   ##  ##
#  ###  ###
# ####  ####

import cs50

def main():

    # get height
    while True:
        print("Height: ", end="")
        height = cs50.get_int()
        if height >= 0 and height < 24:
            break

    # set building variables
    space = " "
    block = "#"
    gap = "  "

    # print pyramid
    for i in range(height):
        print("{}{}{}{}".format(space * (height - i - 1), block * (i + 1), gap, block * (i + 1)))
        
if __name__ == "__main__":
    main()