#Socket Programming: server.py
#Name: Swathi Prema Lal
#Student ID: 3147620
#Lab2
#-----------------------------------------------------------

import socket
import sys


def server():
    #Server port
    serverPort = 13000

    # Create a server socket using TCP and IPv4
    try:
        serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error as e:
        print("Error creating server socket:", e)
        sys.exit(1)


    # Bind the socket to port 13000
    try:
        serverSocket.bind(('', serverPort))
    except socket.error as e:
        print("Error binding server socket:", e)
        sys.exit(1)


    #Listen for client connection; can only have 1 in the waiting queue
    serverSocket.listen(1)

    #dictionary as the database for the phonebook; name as the key, phone #s as the value
    phonebook = {}


    #accept client
    # Wait for a client to connect
    while 1:
        try:
            connectionSocket, addr = serverSocket.accept()

            welcomeMessage = """Welcome to the online phone book.

            Please select the operation
            1)Add a new entry
            2)Search
            3)Terminate the connection

            Choice: """
            
            #welcome message is only displayed once: when user opens the server
            connectionSocket.sendall(welcomeMessage.encode("ascii"))
            print("All the message now send.")

            #loop to keep recieving inputs/messages from the client until user chooses to terminate program
            while True:

                #receive the message
                message = connectionSocket.recv(2048)
                #Convert the bytes into a normal string
                choice = message.decode("ascii")

                #add entry
                if choice == '1':
                    #ask for the name
                    connectionSocket.send("Enter the name: ".encode("ascii"))

                    #recieve the name
                    namerecv = connectionSocket.recv(2048)
                    name = namerecv.decode("ascii")

                    #ask for phone number
                    connectionSocket.send("Enter the phone number: ".encode("ascii"))

                    phonerecv = connectionSocket.recv(2048)
                    phone = phonerecv.decode("ascii")

                    #add name and phone number to dict:   
                        #if name already exits: apend number
                        #if name is new, create a new entry and add name and phone
                    
                    if name not in phonebook.keys():
                        phonebook[name] = [phone]

                    else:
                        phonebook[name].append(phone)


                    # Send the menu again after adding the entry
                    menu = """Please select the operation
                    1)Add a new entry
                    2)Search
                    3)Terminate the connection

                    Choice: """

                    connectionSocket.send(menu.encode("ascii"))


                 #Search will be implemented next.
                if choice == "2":
                    #ask the user for the search word
                    connectionSocket.send(
                        "Enter the search word: ".encode("ascii")
                    )

                    #receive the word
                    searchrecv = connectionSocket.recv(2048)
                    searchword = searchrecv.decode("ascii")

                    results = "Name \t\tPhone Number(s)\n"

                    #iterate through each name in the dictionary
                    for name in phonebook:
                        if searchword in name:      #if the word is part of the name: 'ABC' in 'ABC DEF'
                            phoneNumbers = phonebook[name]      #val of the key: [0000000000]
                            results += name + "\t\t" 
                            results += str(phoneNumbers) + '\n'       #turn the numbers into strings: ['0000000000']

                    connectionSocket.send(results.encode("ascii"))  #send the completed search result back to client

                    #send the menu again
                    menu = """\nPlease select the operation
                    1)Add a new entry
                    2)Search
                    3)Terminate the connection

                    Choice: """

                    connectionSocket.send(menu.encode("ascii"))

                #terminate the connection
                if choice == '3':
                    connectionSocket.close()
                    print("DEBUG: connection closed")
                    exit(1)
                    



            #close the client connection
            connectionSocket.close()

        except socket.error as e:
            print("Socket error:", e)       
            serverSocket.close()            #close the server socket
            sys.exit(1)

        except:
            print('Goodbye')
            serverSocket.close() 
            sys.exit(0)
            
        
#-------
server()