# MultiThreadGame
Multiplayer game written in C++(mainly C though) to train usage of IPC mechanisms. 
Ncurses is used as graphical interface.

Game works only on Linux (tested on Ubuntu 18.04) and requires ncurses download.

To start:

-place all files in folder

-open terminal in folder

-use './start_server' to start server terminal

-use './start_player' to start player terminal(up to 4 times, each for one player)

Link to simple demo: https://youtu.be/f0XbO9m9M6I

Game rules:  
Player(1,2,3,4) is in labyrinth.  
His task is to collect coins(c,t,T) and carry them to the camp(A), where they get saved into his account.  
If he crashes into another player or npc beast(\*) he dies, leaving all money in place.  
Walking through the bush(#) takes two moves.  
