import socket
import sys

def client():
    # Server Information
    serverName = '127.0.0.1' #'localhost'
    serverPort = 14000
    
    #Create client socket that useing IPv4 and TCP protocols 
    try:
        clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error as e:
        print('Error in client socket creation:',e)
        sys.exit(1)    
    
    try:
        #Client connect with the server
        clientSocket.connect((serverName,serverPort))
        message = clientSocket.recv(2048)
        num = input(message.decode('ascii'))
        
        #Client send message to the server
        sent_size = clientSocket.send(num.encode("ascii"))
        
        # Client receives a message from the server and print it
        message = clientSocket.recv(2048)
        print(message.decode('ascii'))
        
        # Client terminate connection with the server
        clientSocket.close()
        
    except socket.error as e:
        print('An error occured:',e)
        clientSocket.close()
        sys.exit(1)

#----------
client()