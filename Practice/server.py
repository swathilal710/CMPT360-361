import socket
import sys

# This is an example from "Computer Networking: A Top Down Approach" textbook chapter 2
# You can try this with nc localhost 12000
# See the following link for more details about the socket liberary (https://docs.python.org/3/library/socket.html)

def server():
    #Server port
    serverPort = 14000
    
    #Create server socket that uses IPv4 and TCP protocols 
    try:
        serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error as e:
        print('Error in server socket creation:',e)
        sys.exit(1)
    
    #Associate 12000 port number to the server socket
    try:
        serverSocket.bind(('', serverPort))
    except socket.error as e:
        print('Error in server socket binding:',e)
        sys.exit(1)        
        
    print('The server is ready to accept connections')
        
    #The server can only have one connection in its queue waiting for acceptance
    serverSocket.listen(1)
        
    while 1:
        try:
            #Server accepts client connection
            connectionSocket, addr = serverSocket.accept()
            print(addr,'   ',connectionSocket)
            connectionSocket.send("Enter a number: ".encode('ascii'))
            #Server receives client message, decode it and convert it to upper case
            message = connectionSocket.recv(2048)
            modifiedMessage = "The number received is "+message.decode('ascii')+"."
            
            #Server sends the client the modified message
            #print(modifiedMessage)
            connectionSocket.send(modifiedMessage.encode('ascii'))
            
            #Server terminates client connection
            connectionSocket.close()
            
        except socket.error as e:
            print('An error occured:',e)
            serverSocket.close() 
            sys.exit(1)        
        except:
            print('Goodbye')
            serverSocket.close() 
            sys.exit(0)
            
        
#-------
server()