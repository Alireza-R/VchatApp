VchatApp
========

Voice chat client and server.

Info:
This is the first version of my voice chat system. In future versions, the code will be reviewed, the sound quality should be improved and it should support SSL/TLS communications.

Installation:
The server needs to files named Database.txt and online_clients.txt.
For using the codes, you need to install ALSA (Advanced Linux Sound Architecture) SDK on Ubuntu. you can install it by: 
sudo apt-get install libasound-dev

We are working with PortAudio library to connect to the sound system. You need this library to run the program. You can find information about it on:
http://portaudio.com/docs/v19-doxydocs/compile_linux.html

Operation steps:
At first the user should register to the server by client and server will keep information of all users in Database.txt. Then the user can sign in to the server and server will keep the socket number, name and IP address of that user in online_client.txt. Every user that connects to the server can ask for more information about other online users to make a communication or it can wait for other users to connect to it. Updates are transfered between the server and client and if one user is not online anymore, the server will notify the client and client will drop the connection.
