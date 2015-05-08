# This is the first competition where I use a programming language
# different from C++. Please, don't judge me!

# Sorry, too late for comments

import sys, fileinput, urllib, random, os, qrtools
from PIL import Image
import ImageChops



path = "codebook.png"

colours = ['red', 'green', 'blue', 'alpha']

for colour in range(4):

    image = Image.open(path)

    imLoad = image.load()

    for i in range(640):
        for j in range(480):
            imLoad[i,j] = (0,0,0,(imLoad[i,j][colour] + 1)%2 * 255)
            
    image.save('img{}.png'.format(colours[colour]))