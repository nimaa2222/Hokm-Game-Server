//Server main program

#include "stdafx.h"

/**/
/*
 main() main()

NAME

        main - sets up and starts up the game server (server's client code)

SYNOPSIS

        int main(int argc, char *argv[]);
            argc --> the number of command line arguments
            argv --> the command line arguments
            

DESCRIPTION

        This function will set up the server and a connection to the
        the databse, using the filename passed as command line argument.
        The server will be ready for incoming connection requests afterwards.

RETURNS

        It exists with code 1 in case of database/network disconnection.
 
*/
/**/

int main(int argc, char *argv[])
{
    //set up the game server using the specified file database
    Server gameServer(argc, argv);
    
    //start up the server to allow clients to connect and play
    gameServer.StartUp();
    
    //server will shut down and terminate with exit code 1 if there is netowrk/database disconnections
    
    return 0;
}
/*int main(int argc, char *argv[])*/
