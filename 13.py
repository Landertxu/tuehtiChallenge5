import socket
import sys
import time
import base64, binhex, binascii, hashlib, fileinput

HOST, PORT = "54.83.207.93", 12345


# Generate the effort proof
def generateProof (beginning):
    text = beginning + 'aaaaa'
    for i in range (ord('A'),ord('Z')):
        for j in range (ord('A'),ord('Z')):
            for k in range (ord('A'),ord('Z')):
                text2 = text + chr(i) + chr(j) + chr(k)
                m = hashlib.sha1()
                m.update(text2)
                if m.digest()[-1] == '\xff':
                    return text2
    
message = fileinput.input().next()

decodedMessage = base64.b64decode(message)

# We will successively send messages, decrypting one byte at a time
data = ('a' * (len(decodedMessage)))

decrypted = ''

for i in range(len(decodedMessage)):

    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # Connect to server and send data
        sock.connect((HOST, PORT))
        
        # Initial message
        received = sock.recv(256)
        
        # Beginning of the proof
        prefix = len('Send a string starting with ')
        
        beginning = received[prefix:prefix + 16]
        
        proof = generateProof(beginning)
        
        # Send the proof
        sock.sendall(proof + "")        
        
        # Confirmation message
        received = sock.recv(256)
        
        # Send the message to encrypt
        sock.sendall(data + "\n")
        
        # Receive the encrypted message
        received = sock.recv(1024)
        
        # Decrypt a new byte
        # We use the fact that they reuse the IV
        pattern = base64.b64decode(received)
        decrypted += chr(ord(pattern[i]) ^ ord(decodedMessage[i]) ^ ord(data[i]))
        
        # Modify the data to get the next character
        data = decrypted + data[len(decrypted):]
        
    finally:
        sock.close()

print decrypted