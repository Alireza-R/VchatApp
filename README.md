VchatApp
========

Voice chat client and server.

Info:

This is the first version of my voice chat system. In future versions, the code will be reviewed, the sound quality should be improved and it should support SSL/TLS communications. In the current version, The client and server are multi threaded and server can handle many clients.

Installation:

The server needs files named Database.txt and online_clients.txt.
For using the codes, you need to install ALSA (Advanced Linux Sound Architecture) SDK on Ubuntu. you can install it by: 
sudo apt-get install libasound-dev

We are working with PortAudio library to connect to the sound system. You need this library to run the program. You can find information about it on:
http://portaudio.com/docs/v19-doxydocs/compile_linux.html

Operation steps:

1. Users should register to the server by client (TCP protocol) and server will keep information of all users in Database.txt. 

2. Each user can sign in to the server and server will keep the socket number, username and IP address of that user in online_client.txt. 

3. Every user that connects to the server can ask for more information about other online users to make a communication or it can wait for other users to connect to it. 

4. If the user wants to make a connection, it will send the name of other online client to the server and will recieve the IP address of other client to make a connection.

5. The user can use the IP address of other client to make a UDP connection.

6. Updates are transfered between the server and client and if one user is not online anymore, the server will notify the client and client will drop the connection.
