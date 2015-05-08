# This is the first competition where I use a programming language
# different from C++. Please, don't judge me!

import sys, fileinput, urllib, random, os, qrtools
from PIL import Image

# Images to be downloaded
numImages = 200

for i in range(numImages):
    username = "operator123"
    password = random.randint(100000, 999999)

    magic = 574381

    partial = password
    for c in username:
        partial += ord(c)

    satellite = magic ^ partial

    date = [2015, 5, 1, 14, 46, 29, satellite]

    fileId = 30000

    for i in date:
        fileId *= 3
        fileId += i

    filename = "{}_{}_{}_{}_{}_{}_{}_{}".format(*([satellite] + date[:-1] + [fileId]))

    url = username + '_' + str(password) + '_' + filename
    
    path = "images/{}.jpeg".format(filename)

    urllib.urlretrieve ("http://54.83.207.93:14333/{}".format(url), path )
    
    im = Image.open(path)
    
    im.crop((517,345,584,404)).save(path[:-5] + '_reduced.bmp')
    
os.system("convert images/*reduced* -evaluate-sequence median qrcode.png")

qr = qrtools.QR()
qr.decode("qrcode.png")
print qr.data

# It's too noisy to work, but it does work in some other readers, so I got the key anyway