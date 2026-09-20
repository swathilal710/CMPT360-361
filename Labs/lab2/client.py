#Socket Programming: client.py
#Name: Swathi Prema Lal
#Student ID: 3147620
#Lab2
#-----------------------------------------------------------

import socket
import sys


def client():
    # Server information
    serverName = "127.0.0.1"
    serverPort = 13000

    #Create client socket that useing IPv4 and TCP 
    try:
        clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error as e:
        print('Error in client socket creation:',e)
        sys.exit(1)    


    try:
        #Connect to the server
        clientSocket.connect((serverName, serverPort))

        #receive the server's message
        message = clientSocket.recv(2048)

        #display the message and ask the user for their choice
        choice = input(message.decode("ascii"))


        #send the user's choice to the server
        sent_size = clientSocket.send(choice.encode("ascii"))
        

        #Close the connection
        clientSocket.close()

    
    except socket.error as e:
        print('An error occured:',e)
        clientSocket.close()
        sys.exit(1)

#----------
client()