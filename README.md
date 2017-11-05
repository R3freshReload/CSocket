# CSocket
This is a small project that wraps the standard C Socket to an object oriented version. 
Not all C-Socket features are implemented.

I have only tested this with MINGW-w64.

# How to compile the example:
With MINGW and CMake installed just navigate into the repository and run ´cmake -G "MinGW Makefiles"´ followed by ´mingw32-make´.
You should get "Server.exe" and a "Client.exe". Done :D

# How to use:
Just include the Socket.h in your project, add ´-lwsock32´ and ´-lws2_32´ to your ´gcc´ command and you should be good to go.
