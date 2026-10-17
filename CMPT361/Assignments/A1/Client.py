#Client.py
#Name: Swathi Prema Lal
#Student ID: 3147620
#Assignment #1
#-----------------------------------------------------------



import socket
import sys
import os


"""
client
Purpose: Creates TCP socket for client server and communicated with the server through the connection socket.
    The client connects to the server, sends the username,
    displays the menu, handles file uploads and metadata, 
    and terminates the connection.
    
Parameters: None.
    
Returns: None.
  
"""
def client():
    # Server information
    #ask the client user for the server hostname or IP
    serverName = input("Enter the server host name or IP: ")
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
        #display message and ask for user input
        username = input(message)
    
        #send the username to the server
        clientSocket.sendall(username.encode("ascii"))

        #recieve either error message or the menu
        response = clientSocket.recv(2048).decode("ascii")
        
        if response == "Incorrect username. Connection Terminated.":
        #if username != "user1":

            print(response)
            clientSocket.close()

        else:
            #now the message is the menu
            menu = response
            

            while True:
            
                choice = input(menu)

                #if the choice is invalid, continue displaying menu => don't send invalid response to server
                if choice not in {'1', '2', '3'}:
                    print("Invalid input")
                    continue

                #send choice to server
                clientSocket.sendall(choice.encode("ascii"))

                
                    
                #print the metadata 
                if choice == "1":                

                    #recieve metadata
                    metadata = clientSocket.recv(2048).decode("ascii")
                    #if the response is the file exist error, print the response and close the connection
                    if metadata == "\nInvalid operation! No file uploaded to view!":
                        print(metadata)
                        clientSocket.close()
                        break

                    #else print the metadata
                    print(metadata)


                if choice == "2":
                    message = clientSocket.recv(2048).decode("ascii")
                    fname = input(message)

                    if os.path.exists(fname):

                        #get size of file
                        size = os.path.getsize(fname)
                        #send both filename and size in str format" <filename>\n<size>
                        data = fname + "\n" + str(size)
                        #send name to server
                        clientSocket.sendall(data.encode("ascii"))

                        #recive OK[filesize]
                        message = clientSocket.recv(2048).decode("ascii")
                        print(message)

                        #send all the raw data from the file to server all at once
                        with open (fname, "rb") as fp:
                            clientSocket.sendfile(fp)
                            
                        #upload msg
                        print("Upload process completed")

                    #if the file inputted doesn't exist, print the error message and show the menu again
                    else:
                        print("Invalid response! Given file doesnt exist!")
                        continue
                        

                #if the user chose 3, terminate the connection
                if choice == "3":
                    print("Connection terminated.")
                    clientSocket.close()
                    break

               
            
                #call the new menu for choice 2
                menu = clientSocket.recv(2048).decode("ascii")


            #close the socket
            clientSocket.close()



    except socket.error as e:
            print('An error occured:',e)
            clientSocket.close()
            sys.exit(1)
    
#----------
client()