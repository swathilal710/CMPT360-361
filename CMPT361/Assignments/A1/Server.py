#Server.py
#Name: Swathi Prema Lal
#Student ID: 3147620
#Assignment #1
#-----------------------------------------------------------


import socket
import sys
import json
import datetime
import os


 
"""
print_menu
Purpose: When called, sends the client socket the menu
Parameters:
    connectionSocket - The TCP socket connected to the client
Returns: None.
  
"""
def print_menu(connectionSocket):

    menu = """\nPlease select the operation:
            1) View uploaded files' information
            2) Upload a file
            3) Terminate the connection 
            Choice: """

    #send the menu to the server
    connectionSocket.sendall(menu.encode("ascii"))



"""
validate_user
Purpose: When called by the server func, function checks if the username 
    input send by the client is the correct one: user1
    Returns True if it passes, False if it's anything other than the required
Parameters:
    connectionSocket - The TCP socket connected to the client
    username - username input send by the client
Returns: True if the username is "user1"; otherwise False.
  
"""
def validate_user(connectionSocket, username):
    if username == "user1":
        return True
        
    else:
        term_msg = "Incorrect username. Connection Terminated."
        connectionSocket.sendall(term_msg.encode("ascii"))
        return False


"""
choices
Purpose: Is called by the server function if the username passes the validation check. 
    Loops, calls the print_menu function to send the menu to the client socket, and 
    calls specific helper functions depending on the choice input by the user.
    The loop continues until the client chooses option 3.
    
Parameters:
    connectionSocket - The TCP socket connected to the client
    
Returns: None.
  
""" 
def choices(connectionSocket):
    #loop through choices
    while True:
        #print menu
        print_menu(connectionSocket)
        #recieve user input choice
        response = connectionSocket.recv(2048).decode("ascii")
        

    
        if response == "1":
            #View files
            view_metadata(connectionSocket)

        elif response == "2":
            #Upload file
            upload_file(connectionSocket)
            

        #close the connection 
        elif response == "3":
            connectionSocket.close()
            exit(1)
        


"""
recv_files
Purpose:  Receives the uploaded file from the client and saves it
    The function continues receiving data until the number of bytes
    received matches the file size.
    
Parameters:
    connectionSocket - The TCP socket connected to the client
    fname - Name of the file
    fsize - file size in bytes
    
Returns: None.
  
"""
def recv_files(connectionSocket, fname, fsize):

    bytes_received = 0

    with open(fname, "wb") as fp:
        #loop until bytes_recieved == fsize
        while bytes_received < fsize:
            remaining = fsize - bytes_received    #leftover bytes to recieve

            data = connectionSocket.recv(min(4096, remaining))
            if not data:
                raise ConnectionError("Connection dropped mid-file transfer")

            fp.write(data)
            bytes_received += len(data)



"""
view_metadata
Purpose: Helper function called when the choice input is 1.
    Sends the Database file entries in a string format to the client socket.
    If the file doesn't exist, raise an error.
    
Parameters:
    connectionSocket - The TCP socket connected to the client
    
Returns: None.
  
"""
def view_metadata(connectionSocket):
    dbfname = "Database.json" 
    dbfname = dbfname.encode("utf-8")       #convert str to byte obj


    #print the string in the correct format with the spaces
    space = " "
    metadata = f"\nName {space:<19} Size (Bytes) {space:<16} Upload Date and Time\n"
   

    #reads the JSON file and prints the entire data
    if os.path.exists(dbfname):
        with open(dbfname, "r") as fp:
            db_data = json.load(fp)

            #loop through the dict to find the key and value
            for filename, data in db_data.items():
                size = data["Size"]
                datetime = data["Date & Time"]

                #attach the metadata string 
                metadata += f"{filename:<25}{size:<30}{datetime}\n"

        connectionSocket.sendall(metadata.encode("ascii"))

    else:
        #if the file doesn't exist, send the client server an error message and raise an error on the server
        fileerror = "\nInvalid operation! No file uploaded to view!"
        connectionSocket.sendall(fileerror.encode("ascii"))
        raise FileExistsError('Database file not found')
    
    
        

"""
upload_data
Purpose: Helper function called when the choice input is 2.
    Add/update the uploaded file's metadata in Databse.json
    Metadata includes: filename, size(bytes), and datetime
    
Parameters:
    fname: The uploaded file's name.
    fsize: The uploaded file's size in bytes.
    
Returns: None.
  
"""
def upload_data(fname, fsize):

    dtime = datetime.datetime.now()     #import the datetime obj
    
    dbfname = "Database.json"
    dbfname = dbfname.encode("utf-8")   #convert str to byte obj

    db_data = {}    #initialize dict

    
    #check if the file exists and is not empty
    if os.path.exists(dbfname) and os.path.getsize(dbfname) > 0:
        with open(dbfname, "r") as fp:
            db_data = json.load(fp)  #load current data into dictionary

    #create empty dict
    else: 
        db_data = {}  #initialize an empty dictionary 

    #add/update the dict with size + time
    db_data[fname] = {"Size": str(fsize), "Date & Time": str(dtime)}
   

    #convert data back to json in str format: dumps
    json_str = json.dumps(db_data)
    with open(dbfname, "w") as f:
        f.write(json_str)
    


"""
upload_file
Purpose: Called when user input is choice 2. 
    The function requests the filename, receives the filename and
    file size, sends “OK [filesize]” message, receives the file contents,
    and updates Database.json with the file metadata.

    
Parameters:
    connectionSocket - The TCP socket connected to the client
    
Returns: None.
  
"""
def upload_file(connectionSocket):
    message = "Please provide the file name: "
    connectionSocket.sendall(message.encode("ascii"))

    #split the str response at \n and turn it into list
    fname = connectionSocket.recv(2048).decode("ascii").split("\n")
    
    filename = fname[0]
    filesize = int(fname[1])    #convert the str size to int
    
    ret_str = "OK" + " " + str(filesize)
    #send string to user side
    connectionSocket.sendall(ret_str.encode("ascii"))

    recv_files(connectionSocket, filename, filesize)


    upload_data(filename, filesize)
    



"""
server
Purpose: Creates TCP socket for server and handles client connections.
    The server listens on port 13000, accepts one client connection at
    a time, validates user inputs, and perform actions requested by the 
    client socket until connection is terminated.
    
Parameters: None.
    
Returns: None.
  
"""
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

    #accept client
    # Wait for a client to connect
    while 1:
        try:
            #Server accepts client connection
            connectionSocket, addr = serverSocket.accept()

            #print welcome msg
            welcome = "Welcome to our system.\nEnter your username: "
            connectionSocket.sendall(welcome.encode("ascii"))

            #recieve username
            message = connectionSocket.recv(2048)
            user_name = message.decode("ascii")

            authorized_user = validate_user(connectionSocket, user_name)
            #if the return is True, call the choices function
            if authorized_user:
                choices(connectionSocket)
        
            #if return is False: break and end connection
            else:
                break        
            
            #Server terminates client connection
            connectionSocket.close()
            

    
        except socket.error as e:
            print('An error occured:',e)
            serverSocket.close() 
            sys.exit(1)        
        except:
            serverSocket.close() 
            sys.exit(0)



server()


