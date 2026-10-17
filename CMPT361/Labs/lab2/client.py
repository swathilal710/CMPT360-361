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

        #receive the server's welcome message
        messagerecv = clientSocket.recv(2048)
        message = messagerecv.decode("ascii")

        #loop until break: user chooses to terminate 
        while True:

            #display the message and ask the user for their choice
            choice = input(message)
            #send the user's choice to the server
            clientSocket.send(choice.encode("ascii"))


            #choice 1: Add new entry
            if choice == "1":
                #receive the message
                messagerecv = clientSocket.recv(2048)
                message = messagerecv.decode("ascii")

                #ask user for name
                name = input(message)
                #send the user's name to the server
                clientSocket.send(name.encode("ascii"))

                #receive the name
                namerecv = clientSocket.recv(2048)
                message = namerecv.decode("ascii")

                phone = input(message)
                clientSocket.send(phone.encode("ascii"))

                #recieve the menu message
                menu = clientSocket.recv(2048)
                message = menu.decode("ascii")


            if choice == "2":
                #recieve the message
                messagerecv = clientSocket.recv(2048)
                message = messagerecv.decode("ascii")

                #Ask the user for the search word
                searchWord = input(message)
                #snd the search word to the server
                clientSocket.send(searchWord.encode("ascii"))

                #recieve the search results
                results = clientSocket.recv(2048)
                message = results.decode("ascii")

                #print the search results
                print(message)


                #recieve the menu message
                menu = clientSocket.recv(2048)
                message = menu.decode("ascii")


            #if the user chose 3, terminate the connection
            if choice == "3":
                print("Connection terminated.")
                break



        #Close the connection
        clientSocket.close()

    
    except socket.error as e:
        print('An error occured:',e)
        clientSocket.close()
        sys.exit(1)

#----------
client()