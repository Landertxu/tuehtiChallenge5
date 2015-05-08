# This is the first competition where I use a programming language
# different from C++. Please, don't judge me!

import sys, base64, binascii

def getInt (bits, endianness, reverse):
    '''Return the integer corresponding to the given
    bit string and endianness'''
    
    # If the endianess is little endian we revert the order
    # of the bytes
    if endianness == 'L':
        byteList = []
        for i in range(0, len(bits), 8):
            byteList.append(bits[i:i+8])
        bits = ''.join(reversed(byteList))
    
    # If we have to reverse we do it now
    
    if (reverse): bits = bits[::-1]
    
    return int(bits, 2)

# First read the encoded data
encodedData = sys.stdin.readline()

# Decode base64

decodedString = base64.b64decode(encodedData)

# String with decoded bytes
hexString = binascii.hexlify(decodedString)

# Binary string. Add extra 1 to avoid loosing leading 0
binString = bin(int('1' + hexString, 16))[3:]

# Skip one line
numCases = sys.stdin.readline()

# Last character used
last = 0
for line in sys.stdin.readlines():
    # Remove the endline
    line = line.strip()
    # The format depends on whether the last character is an R
    if line[-1] == 'R':
        print getInt(binString[last:last + int(line[:-2])], line[-2], True)
        last += int(line[:-2])
    else:
        print getInt(binString[last:last + int(line[:-1])], line[-1], False)
        last += int(line[:-1])