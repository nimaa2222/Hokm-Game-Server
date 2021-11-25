#include "stdafx.h"

//the command inputted by match creator to start a match
string Server::matchStartCommand = "START";

//to prepare to receive connections for a service
static SOCKET PassiveTCP(const string&, const int&);

//to convert a recieved character array message to a string
string ConvertToString(const char*, const ssize_t&);

//to print a message on server side by continuously sleeping the system (server-side)
void PrintSlowMsg(const string& a_msg, const int& a_pause);

Server::Server(int argc, char *argv[]): m_database(argc, argv)
{
    m_service = "7000";
    m_matches = {};
    m_clients = {};
    m_timeval = new timeval{1};
}

Server::~Server(){delete m_timeval;}


/**/
/*
 Server::StartUp Server::StartUp

NAME

        StartUp - to start up the server and accept incoming requests
 
SYNOPSIS

        void Server::StartUp();
            
DESCRIPTION

        This function, after establishing a TCP connection, starts up the server
        to recieve incoming requests to allow clients to connect to the server and
        play the game. The server essentially runs forever unless a problem with the
        databse or network arises (program exits with code 1).

RETURNS

        None
 
*/
/**/

void Server::StartUp()
{
    //initialize random seed for randomization
    srand((unsigned) time (NULL));
    cout<<endl<<endl;
    
    string msg = "CONNECTING TO SERVER'S DATABASE...";
    PrintSlowMsg(msg, 100);
    cout<<endl;
    
    msg = "CONNECTION TO DATABASE ESTABISHED...";
    PrintSlowMsg(msg, 85);
    cout<<endl;
    
    msg = "STARTING THE SERVER...";
    PrintSlowMsg(msg, 100);
    cout<<endl;
    
    msg = "THE SERVER IS NOW UP...";
    PrintSlowMsg(msg, 85);
    cout<<endl;
    
    msg = "The Server Is Listening On Port " + m_service;
    PrintSlowMsg(msg, 85);
    cout<<endl;

    //connect to the server process
    SOCKET ls = PassiveTCP(m_service, 5);

    //wait for connections from clients
    while (true)
    {
        //prepare to wait for data and connection requests
        fd_set readMap;
        FD_ZERO(&readMap);
        FD_SET(ls, &readMap);
        SOCKET maxSoc = ls;
        
        for (size_t is = 0; is < (int)m_clients.size(); is++)
        {
            if (m_clients[is].soc == 0)
                continue;
            
            FD_SET(m_clients[is].soc, &readMap);
            
            if (m_clients[is].soc > maxSoc)
                maxSoc = m_clients[is].soc;
        }

        //wait for something to happen for a specified time interval
        int srv = select(maxSoc + 1, &readMap, NULL, NULL, m_timeval);
        if (srv == SOCKET_ERROR)
        {
            cerr << "Select Call Returned With Error - Terminating" << endl;
            cerr << "The Server Has Unexpectedly Shut Down!"<<endl;
            
            //terminates the server
            exit(1);
        }
        
        //handle connection request first
        if (FD_ISSET(ls, &readMap))
        {
            HandleConnectionRequest(ls);
            continue;
        }
        
        //step through connected clients to see if any data is pending
        CheckPendingData(&readMap);
    }
}
/*void Server::StartUp();*/


/**/
/*
 Server::HandleConnectionRequest Server::HandleConnectionRequest

NAME

        CheckPendingData - to handle connection requests to the server
 
SYNOPSIS

        void Server::HandleConnectionRequest(const SOCKET& a_ls);
            a_ls --> the server's listening socket
            
DESCRIPTION

        This function establishes client connections to the server
        as long as the necessary resources are present. A welcome
        message and the home page is displayed to the user upon
        a successful connection.

RETURNS

        None
 
*/
/**/

void Server::HandleConnectionRequest(const SOCKET& a_ls)
{
    //address of the client
    struct sockaddr fsin;
    
    //length of client address
    socklen_t alen = sizeof(sockaddr_in);
    
    //accept the connection request
    SOCKET s = accept(a_ls, &fsin, &alen);
    
    cout<<"A New Client Connected"<<endl;
    
    if (s == INVALID_SOCKET)
    {
        cerr<<"Error On Accept Call"<<endl;
        return;
    }
    
    //if there is not enough room for another online player
    if (m_clients.size() == INT_MAX)
    {
        //inform client about this
        SendMsg(s, Display::GetMaxOnlinePlReachedMsg());
        
        //deallocate the socket for reuse
        close(s);
        
        return;
    }
    
    //record the new client connection
    Client newClient(s);
    m_clients.push_back(newClient);
    
    //welcome user to the server
    SendMsg(m_clients.back().soc, Display::GetWelcomeMsg());
    
    //display the home page to the user
    SendMsg(m_clients[m_clients.size() - 1].soc, Display::GetHomePageMenu());
}
/*void Server::HandleConnectionRequest(const SOCKET& a_ls);*/


/**/
/*
 Server::CheckPendingData Server::CheckPendingData

NAME

        CheckPendingData - to step through connected clients to see if any data is pending
 
SYNOPSIS

        void Server::CheckPendingData(fd_set* a_readMap);
            a_readMap --> the readmap of the server
            
DESCRIPTION

        This function initially checks to see if any client is idle.
        It then handles disconnected clients and finally clients who
        have have requested resources from the server by sending inputs
        to the server.

RETURNS

        None
 
*/
/**/

void Server::CheckPendingData(fd_set* a_readMap)
{
    //step through connected clients
    for (int isoc = 0; isoc < (int)m_clients.size(); isoc++)
    {
        //check for idle clients
        bool disconnected = HandleIdleClient(isoc);
        if (disconnected)
            isoc--;
        
        //if there is data pending
        if (FD_ISSET(m_clients[isoc].soc, a_readMap))
        {
            //reset client's timer
            m_clients[isoc].timer = time(NULL);
            
            const int MAXSIZE = 1000;
            char buff[MAXSIZE + 1];
            
            //recieve client's message
            ssize_t nb = recv(m_clients[isoc].soc, buff, MAXSIZE, 0);
            
            //if client has disconnected
            if (nb <= 0)
            {
                //deallocate the socket for reuse
                close(m_clients[isoc].soc);
                
                //handle the disconnection based on client's stage
                HandleDisconnection(isoc);
                
                //remove the client from the list of connected clients
                m_clients.erase(m_clients.begin() + isoc);
                
                isoc--;
            }
            
            //if client has entered an input
            else
            {
                string input = ConvertToString(buff, nb);
                
                //max user input is 31 charcaters which is for username=password
                if (input.size() > m_maxUserPassSize)
                {
                    SendMsg(m_clients[isoc].soc, Display::GetLengthyErrorMsg());
                    continue;
                }
                
                //respond to client's input based on their stage
                HandleRequest(isoc, input);
            }
        }
    }
}
/*void Server::CheckPendingData(fd_set* a_readMap);*/


/**/
/*
 Server::HandleIdleClient Server::HandleIdleClient

NAME

        HandleIdleClient - to handle a client who has gone idle
 
SYNOPSIS

        bool Server::HandleIdleClient(const int& a_index);
            a_index --> the index of the idle client
            
DESCRIPTION
        
        This function deals with clients that have not communicated with the
        server for a certain period. It periodically resets the timer for clients
        who are waiting for other players. The other idle clients are either
        disconnected from the server or kicked out of their match based on
        their stage and inactivity time.

RETURNS

         Returns true if client socket is closed and client is disconnected from the server
         Returns false otherwise
 
*/
/**/

bool Server::HandleIdleClient(const int& a_index)
{
    //establish client's stage
    ClientStage cStage = m_clients[a_index].stage;
    
    //if player is waiting to start a match
    if (cStage == mustWaitToStartMatch)
    {
        //reset player's timer to prevent disconnection
        m_clients[a_index].timer = time(NULL);
        
        return false;
    }
    
    //if player is in a match waiting for other players
    if (cStage == insideMatch)
    {
        //find the match associated with this user
        int matchIndex = GetMatchIndexByPlUsername(m_clients[a_index].username);
        
        //find the specific stage of this player inside the match
        ClientStage matchStage =  m_matches[matchIndex].GetPlByUsername(m_clients[a_index].username)->GetStage();
        
        //if player is waiting for other players in the match
        if (matchStage == mustWaitForOtherPls)
        {
            //reset player's timer to prevent disconnection
            m_clients[a_index].timer = time(NULL);
            
            return false;
        }
    }
    
    //if the player is not waiting...
    
    //establish client's inactivity time
    long inactivityTime = time(NULL) - m_clients[a_index].timer;
    
    //if no response for a specific period
    if (inactivityTime > m_matchInactivity - 10)
    {
        //if client is not inside a match
        if (cStage != insideMatch)
        {
            //if it's been more than 5 minutes
            if (inactivityTime > m_generalInactivity)
            {
                //if the player is a match creator who has to give permission to start the match
                if (cStage == mustAllowMatchToStart)
                {
                    //client did not start the match on time
                    SendMsg(m_clients[a_index].soc, Display::GetNoMatchStartMsg());
                    
                    //match is deleted and all clients go back to main menu
                    HandleDisconnection(a_index);
                    
                    //prompt client with the main menu
                    m_clients[a_index].stage = mustSelectFromMainMenu;
                    SendMsg(m_clients[a_index].soc, Display::GetMainMenu());
                    
                    //reset player's timer to prevent immediate disconnection
                    m_clients[a_index].timer = time(NULL);
                }
                
                else
                {
                    //inform player about disconnection (i.e. session ended due to inactivity)
                    SendMsg(m_clients[a_index].soc, Display::GetInactivityMsg());
                    
                    //deallocate the socket for reuse
                    close(m_clients[a_index].soc);
                    
                    //remove the client from the list of connected clients
                    m_clients.erase(m_clients.begin() + a_index);
                    
                    return true;
                }
            }
        }
        
        //if player is inside the match
        else
        {
            //if it's been more than 90 seconds
            if (inactivityTime > m_matchInactivity)
            {
                //player did not make a selection on time
                SendMsg(m_clients[a_index].soc, Display::GetNoSelectionMsg());
                
                //player replaced by NPC
                HandleDisconnection(a_index);
                
                //user goes back to main menu
                m_clients[a_index].stage = mustSelectFromMainMenu;
                SendMsg(m_clients[a_index].soc, Display::GetMainMenu());
                
                //reset player's timer to prevent immediate disconnection
                m_clients[a_index].timer = time(NULL);
            }
            
            //if there is less than 10 seconds left
            else if (inactivityTime == m_matchInactivity - 9)
            {
                //warn the player about getting kicked out of the match
                SendMsg(m_clients[a_index].soc, Display::GetInactivityWarningMsg());
            }
        }
    }
    
    return false;
}
/*bool Server::HandleIdleClient(const int& a_index);*/


/**/
/*
 Server::HandleDisconnection Server::HandleDisconnection

NAME

        HandleDisconnection - to handle a client that unexpectedly disconnects
 
SYNOPSIS

        void Server::HandleDisconnection(const int& a_index);
            a_index --> the index of the disconnected client
            
DESCRIPTION
        
        This function deals with clients that have are unexpectedly disconnected from the server.
        If the player was inside a match, it is replaced by an average NPC (if applicable).
        Otheriwse the client is simply removed from the connected client list.

RETURNS

        None
 
*/
/**/

void Server::HandleDisconnection(const int& a_index)
{
    //establish client's stage
    ClientStage cStage = m_clients[a_index].stage;
    
    //if client disconnects during the match
    if (cStage == insideMatch)
    {
        //find the match associated with this user
        int matchIndex = GetMatchIndexByPlUsername(m_clients[a_index].username);
        
        //if after leaving there is at least one human player left in the match
        //(i.e. there is currently at least 2 human players in the match)
        if (m_matches[matchIndex].GetNumHumanPls() >= 2)
        {
            //find the specific stage of this player inside the match
            ClientStage matchStage =  m_matches[matchIndex].GetPlByUsername(m_clients[a_index].username)->GetStage();
            
            //replace disconnected player by an average NPC
            string compUsername = m_npcNames[m_matches[matchIndex].GetNumCompPls()];
            Player* compPl = new Computer(compUsername, average);
            m_matches[matchIndex].ReplacePlayer(m_clients[a_index].username, compPl);
            
            //inform match players about this replacement
            string msg = Display::GetPlLeftMsg(m_clients[a_index].username);
            msg += Display::GetPlReplaceMsg(compUsername);
            
            for (auto pl: m_matches[matchIndex].GetPlayers())
                pl->SendMsg(msg);
            
            if (matchStage == mustSelectTrumpSuit)
            {
                //manually select the trump suit for the player that left the match
                
                //ask the computer to call the trump suit
                Suit selectedSuit = m_matches[matchIndex].GetPlByUsername(compUsername)->SelectTrumpSuit();
                
                m_matches[matchIndex].GetCurrentRound().TrumpSuitSelected(m_matches[matchIndex].GetDeck(),
                m_matches[matchIndex].GetPlayers(),
                m_matches[matchIndex].GetPoints(),
                m_matches[matchIndex].GetRoundNum(), selectedSuit);
            }
            
            else if (matchStage == mustDiscardTwoCards)
            {
                //manually discard 2 cards for the player that left the match
                m_matches[matchIndex].GetPlByUsername(compUsername)->DiscardCards(m_matches[matchIndex].GetCurrentRound().GetTrumpSuit());
                
                m_matches[matchIndex].GetCurrentRound().TwoCardsDiscarded(m_matches[matchIndex].GetDeck(),
                m_matches[matchIndex].GetPlayers(), m_matches[matchIndex].GetPoints(), m_matches[matchIndex].GetRoundNum(),
                m_matches[matchIndex].GetPlByUsername(compUsername), Card(), Card());
            }
            
            else if (matchStage == mustPickOrDiscard)
            {
                //manually pick/discard for the player that left the match
                Decision decision = m_matches[matchIndex].GetPlByUsername(compUsername)->PickOrDiscard(m_matches[matchIndex].GetDeck().GetTopDeck(),
                m_matches[matchIndex].GetCurrentRound().GetTrumpSuit());
                
                //decision as string
                string decisionStr = to_string(decision);
                
                //make the move for the player
                m_matches[matchIndex].HandleMatchMove(compUsername, mustPickOrDiscard, decisionStr);
            }
            
            else if (matchStage == mustSelectCard)
            {
                //select a card to be played at this time
                Card selectedCard = m_matches[matchIndex].GetPlByUsername(compUsername)->SelectCard(m_matches[matchIndex].GetMode(),
                m_matches[matchIndex].GetCurrentRound().GetCurrentTrick().GetTurn(), m_matches[matchIndex].GetCurrentRound().GetTrumpSuit(),
                m_matches[matchIndex].GetCurrentRound().GetCurrentTrick().GetSuitLed(),
                m_matches[matchIndex].GetCurrentRound().GetCurrentTrick().GetTableCards());
                
                //convert card to string input
                string cardStr;
                
                //convert the rank
                int rank = selectedCard.GetRank();
                
                if (rank > 1 && rank < 10)
                    cardStr += to_string(rank);
                else
                    cardStr += rankChar[(SymbolicCards)rank];
                
                //convert the suit
                Suit suit = selectedCard.GetSuit();
                
                for (auto cardSuit: suitChar)
                    if (cardSuit.second == suit)
                        cardStr += cardSuit.first;
                
                m_matches[matchIndex].HandleMatchMove(compUsername, mustSelectCard, cardStr);
            }
            
            //if simply waiting for other players (mustWaitForOtherPls)
            else
            {
                //no immediate action required by the computer
                //the prompt for the current player simply must be redisplayed
                
                //set to true if players are inside a trick
                bool inTrick = false;
                
                //the player that must select a card at this time
                Player* selector = NULL;
                
                //check if inside a trick
                for (auto player: m_matches[matchIndex].GetPlayers())
                {
                    if (player->GetStage() == mustSelectCard)
                    {
                        inTrick = true;
                        selector = player;
                    }
                }
                
                //if players are in a trick
                if (inTrick)
                {
                    //match must be redisplayed to all users
                    for (auto player: m_matches[matchIndex].GetPlayers())
                    {
                        
                        string display = Display::GetMatchDisplay(m_matches[matchIndex].GetPlayers(),
                        m_matches[matchIndex].GetCurrentRound().GetHakem(),
                        m_matches[matchIndex].GetCurrentRound().GetCurrentTrick().GetTableCards(), m_matches[matchIndex].GetPoints(),
                        m_matches[matchIndex].GetCurrentRound().GetTrickScores(), m_matches[matchIndex].GetRoundNum(),
                        m_matches[matchIndex].GetCurrentRound().GetTrumpSuit(), player->GetUsername());
                        
                        SendMsg(player->GetSocket(), display);
                    }
                        
                    string msg = Display::GetCardSelectionPrompt();
                    
                    //card selection prompt must be redisplayed to player taking turn
                    SendMsg(selector->GetSocket(), msg);
                }
                
                else
                {
                    //all other players must wait
                    for (auto player: m_matches[matchIndex].GetPlayers())
                    {
                        if (player->GetStage() != mustWaitForOtherPls)
                            SendMsg(player->GetSocket(), "Please Continue Playing!\n");
                    }
                }
            }
        }
        
        //if there will be no human players left in the match (i.e. this is the only human player in the match)
        else
        {
            //destroy and remove the match from the list of online matches
            m_matches.erase(m_matches.begin() + matchIndex);
        }
    }
    
    //if client disconnects while waiting to start a match
    else if (cStage == mustWaitToStartMatch || cStage == mustAllowMatchToStart)
    {
        //find the match associated with this user
        int matchIndex = GetMatchIndexByPlUsername(m_clients[a_index].username);
        
        //if the match creator disconnects
        if (m_matches[matchIndex].GetPlayerIndex(m_clients[a_index].username) == 0)
        {
            //remove this player from the match
            m_matches[matchIndex].RemovePlayer(m_clients[a_index].username);
            
            for (auto player: m_matches[matchIndex].GetPlayers())
            {
                //inform players about the match creator leaving
                Server::SendMsg(player->GetSocket(), Display::GetMatchCreatorLeftMsg());
                
                //update the stage of other clients to main menu
                player->SetStage(mustSelectFromMainMenu);
                m_clients[GetClientIndexByUsername(player->GetUsername())].stage = mustSelectFromMainMenu;
                
                //display the main menu to all other clients
                Server::SendMsg(player->GetSocket(), Display::GetMainMenu());
            }
            
            //destroy the match
            m_matches.erase(m_matches.begin() + matchIndex);
        }
        
        //if one of the other players disconnect
        else
        {
            //find the match associated with this user
            int matchIndex = GetMatchIndexByPlUsername(m_clients[a_index].username);
            
            //remove this player from the match
            m_matches[matchIndex].RemovePlayer(m_clients[a_index].username);
            
            for (auto player: m_matches[matchIndex].GetPlayers())
            {
                //inform players about the client leaving
                Server::SendMsg(player->GetSocket(), Display::GetPlLeftMsg(m_clients[a_index].username));
                
                //show the new list of players
                Server::SendMsg(player->GetSocket(), Display::GetMatchPlayersList(m_matches[matchIndex].GetPlayers()));
                
                //update the staus of all the clients
                m_clients[GetClientIndexByUsername(player->GetUsername())].stage = mustWaitToStartMatch;
                player->SetStage(mustWaitToStartMatch);
            }
            
            //continue waiting for other players to join
        }
    }
}
/*void Server::HandleDisconnection(const int& a_index);*/


/**/
/*
 Server::HandleRequest Server::HandleRequest

NAME

        HandleRequest - to handle a client input while connected to server
 
SYNOPSIS

        void Server::HandleRequest(const int& a_index, string a_input);
            a_index   --> the index of client on connected client list
            a_input   --> the client's input
            
DESCRIPTION
        
        This function deals with the various client requests at different stages.
        It calls the appropriate function to handle the request based on the
        client's stage.

RETURNS

         None
 
*/
/**/

void Server::HandleRequest(const int& a_index, string a_input)
{
    //if the client is on the home page
    if (m_clients[a_index].stage == mustSelectFromHomePage)
        HandleHomePageSelection(a_index, a_input);
    
    //if client is entering username and password to log in
    else if (m_clients[a_index].stage == mustLogin)
    {
        //log in the client if authenticated
        LogIn(a_index, a_input);
    }
    
    //if client is entering username and password to create an account
    else if (m_clients[a_index].stage == mustCreateAccount)
    {
        //add the new credentials to the database if it is valid
        CreateAccount(a_index, a_input);
    }
    
    //if client is selecting from the main menu
    else if (m_clients[a_index].stage == mustSelectFromMainMenu)
        HandleMainMenuSelection(a_index, a_input);
    
    //if client is selecting the match type (NPC or RGP)
    else if (m_clients[a_index].stage == mustSelectMatchType)
        HandleMatchTypeSelection(a_index, a_input);
    
    //if client is selecting NPC intelligence (Low, Medium, High)
    else if (m_clients[a_index].stage == mustSelectNPCIntelligence)
        HandleNPCIntelligenceSelection(a_index, a_input);
    
    //if client is selecting the number of players in the game
    else if (m_clients[a_index].stage == mustSelectMatchMode)
        HandleMatchModeSelection(a_index, a_input);
    
    //if client must give permission to start a match
    else if (m_clients[a_index].stage == mustAllowMatchToStart)
        HandleStartMatchRequest(a_index, a_input);
    
    //if client is selecting the match to join
    else if (m_clients[a_index].stage == mustSelectMatchToJoin)
        HandleJoinMatchRequest(a_index, a_input);
    
    //if client is on the list waiting for other player(s) to join
    else if (m_clients[a_index].stage == mustWaitToStartMatch)
    {
        //inform client that they must wait
        SendMsg(m_clients[a_index].soc, Display::GetWaitForMatchStartMsg());
    }
    
    //if a player inside an ongoing match takes an action
    else if (m_clients[a_index].stage == insideMatch)
        HandleInsideMatchRequest(a_index, a_input);
}
/*void Server::HandleRequest(const int& a_index, string a_input);*/


/**/
/*
 Server::HandleHomePageSelection Server::HandleHomePageSelection

NAME

        HandleHomePageSelection - to respond to client selection on the home page
 
SYNOPSIS

        void Server::HandleHomePageSelection(const int& a_index, const string& a_input);
            a_index --> the index of the client on connected client list
            a_input --> the client's input
            
DESCRIPTION
        
        This function checks whether the user would like to log in or
        create an account and takes the necessary actions.

RETURNS

         None
 
*/
/**/

void Server::HandleHomePageSelection(const int& a_index, const string& a_input)
{
    //if input is invalid
    if (a_input.size() != 1 || !isdigit(a_input[0]) || (stoi(a_input) != login && stoi(a_input) != signup))
    {
        //inform client about invalid input
        SendMsg(m_clients[a_index].soc, Display::GetInvalidSelectionMsg());
        
        //redisplay the home page
        SendMsg(m_clients[a_index].soc, Display::GetHomePageMenu());
        
        return;
    }
    
    //if user wants to log in
    if (stoi(a_input) == login)
    {
        //display the login prompt
        SendMsg(m_clients[a_index].soc, Display::m_logInPrompt);
        
        //client must now enter usename and password
        m_clients[a_index].stage = mustLogin;
    }
    
    //if user wants to create an account
    else
    {
        //if there is not enough room to create an account
        if (m_database.GetNumClients() == INT_MAX)
        {
            //inform client
            SendMsg(m_clients[a_index].soc, Display::GetMaxAccountsReachedMsg());
            
            //redisplay the home page
            SendMsg(m_clients[a_index].soc, Display::GetHomePageMenu());
            
            return;
        }
        
        //display the sign up prompt
        SendMsg(m_clients[a_index].soc, Display::m_signupPrompt);
        
        //client must now enter usename and password
        m_clients[a_index].stage = mustCreateAccount;
    }
}
/*void Server::HandleHomePageSelection(const int& a_index, const string& a_input);*/


/**/
/*
 Server::LogIn Server::LogIn

NAME

        LogIn - to log in the client on the server
 
SYNOPSIS

        void Server::LogIn(const int& a_index, const string& a_userpass);
            a_index    --> the index of the client on connected client list
            a_userpass --> the username and password provided by client
            
DESCRIPTION
        
        This function checks whether the provided username and password
        matches a username and password on the database. If there is a match
        and the client is not already logged in, the client will be logged in.

RETURNS

         None
 
*/
/**/

void Server::LogIn(const int& a_index, const string& a_userpass)
{
    //parse the username
    string username = a_userpass.substr(0, a_userpass.find(m_userPassDelim, 0));
    
    //capitalize the username (username is not case sensitive)
    string capUsername = "";
    for(auto i: username)
        capUsername += toupper(i);
    
    //parse the password (case sensitive)
    string password = a_userpass.substr(a_userpass.find(m_userPassDelim) + 1);
    
    //if there is a match for the username and password
    if (Authenticate(capUsername, password))
    {
        //if client is already logged in
        if (IsLoggedIn(username))
        {
            //inform client that they cannot log in
            SendMsg(m_clients[a_index].soc, Display::GetUserLoggedInMsg());
            
            //redisplay the home page
            SendMsg(m_clients[a_index].soc, Display::GetHomePageMenu());
            
            //client must now choose from home page
            m_clients[a_index].stage = mustSelectFromHomePage;
            
            return;
        }
        
        //inform client that log in was successful
        SendMsg(m_clients[a_index].soc, Display::GetSuccessLogInMsg());
        
        //log in client with username specified at sign up
        m_clients[a_index].username = capUsername;
        
        //display the main menu
        SendMsg(m_clients[a_index].soc, Display::GetMainMenu());
        
        //client must now select from the main menu
        m_clients[a_index].stage = mustSelectFromMainMenu;
        
        cout<<capUsername<<" Successfully Logged In"<<endl;
    }
    
    //if there is no match for username or password
    else
    {
        //inform client that log in failed
        SendMsg(m_clients[a_index].soc, Display::GetFailLogInMsg());
        
        //redisplay the login prompt
        SendMsg(m_clients[a_index].soc, Display::m_logInPrompt);
    }
}
/*void Server::LogIn(const int& a_index, const string& a_userpass);*/


/**/
/*
 Server::Authenticate Server::Authenticate

NAME

        Authenticate - to check whether a set of username and password exists in the database
 
SYNOPSIS

        bool Server::Authenticate(string& a_username, const string& a_password);
            a_username --> the username specified by client
            a_password --> the password specified by client
            
DESCRIPTION
        
        This function checks whether a given username and password matches
        a username and password pair on the database.

RETURNS

        Returns true if a match is found
        Returns false otherwise
 
        Returns format of username found in database by reference (a_username)
 
*/
/**/

bool Server::Authenticate(string& a_username, const string& a_password)
{
    //the database
    stringstream buffer = m_database.FetchFileData();
    
    //a line read from database
    string accountInfo;
    
    //read a line from the database
    while (getline (buffer, accountInfo))
    {
        //parse username=password from client account information
        string userCredential = accountInfo.substr(0, accountInfo.find(m_credentialDelim, 0));
        
        //parse the username from the file
        string usernameFromFile = userCredential.substr(0, userCredential.find(m_userPassDelim, 0));
        
        //capitalize the username from file
        string capUserFile = "";
        for(auto i: usernameFromFile)
            capUserFile += toupper(i);
        
        //parse the password from file
        string passFromFile = userCredential.substr(userCredential.find(m_userPassDelim) + 1);
        
        //if username and password matches
        if (a_username == capUserFile && a_password == passFromFile)
        {
            //return the way client has specified their username during sign up
            a_username = usernameFromFile;
            
            return true;
        }
    }
    
    return false;
}
/*bool Server::Authenticate(string& a_username, const string& a_password);*/


/**/
/*
 Server::IsLoggedIn Server::IsLoggedIn

NAME

        IsLoggedIn - to check whether a given client is logged in
 
SYNOPSIS

        bool Server::IsLoggedIn(string a_username);
            a_username --> the username of the client
            
            
DESCRIPTION
        
        This function checks whether a given client is currently logged in
        by looking up its username in the connected list of clients.

RETURNS

        Returns true if client is logged in
        Returns false otherwise
 
*/
/**/

bool Server::IsLoggedIn(string a_username)
{
    //capitalize the username
    for (auto& i: a_username)
        i = toupper(i);
    
    //for all connected clients
    for (auto client: m_clients)
    {
        //the capital form of connected client username
        string capUsername = "";
        
        for (auto i: client.username)
            capUsername += toupper(i);
            
        //if username matches a connected client username
        if (capUsername == a_username)
            return true;
    }

    return false;
}
/*bool Server::IsLoggedIn(string a_username);*/


/**/
/*
 Server::CreateAccount Server::CreateAccount

NAME

        CreateAccount - to handle client request to create a new account
 
SYNOPSIS

        void Server::CreateAccount(const int& a_index, const string& a_userpass);
            a_index    --> the index of the client on the connected clients list
            a_userpass --> a specified username=password for sign up
            
DESCRIPTION
        
        This function adds a new account to the database as long as enough resources
        are present and the username and password match the server's username and password
        requirements.

RETURNS

        None
 
*/
/**/

void Server::CreateAccount(const int& a_index, const string& a_userpass)
{
    //make sure username=password itself is properly formatted
    bool validCred = CheckCredentials(a_index, a_userpass);
    
    //if username=password is valid
    if (validCred)
    {
        //initialze wins and losses for the client
        string scores = ", 0-0";
        string accountInfo = a_userpass + scores;
        
        //enter account information into the databse
        m_database.EnterIntoDatabase(accountInfo);
        
        //inform client about successful sign up
        SendMsg(m_clients[a_index].soc, Display::GetSuccessSignupMsg());

        //display the home page
        SendMsg(m_clients[a_index].soc, Display::GetHomePageMenu());
        
        //client must now select from home page
        m_clients[a_index].stage = mustSelectFromHomePage;
        
        cout<<"An Account Was Created"<<endl;
    }
}
/*void Server::CreateAccount(const int& a_index, const string& a_userpass);*/


/**/
/*
 Server::CheckCredentials Server::CheckCredentials

NAME

        CheckCredentials - to check whether a valid username=password is entered
 
SYNOPSIS

        bool Server::CheckCredentials(const int& a_index, const string& a_credential);
            a_index      --> the index of the client on the connected clients list
            a_credential --> a specified username=password
            
DESCRIPTION
        
        This function makes sure that the username=password is properly formatted,
        the username matches its requirements and the password also matches
        its requirements.

RETURNS

        Returns true if username=password is valid
        Returns false otherwise
 
*/
/**/

bool Server::CheckCredentials(const int& a_index, const string& a_credential)
{
    //check the formatting of username=password
    
    //set to true if input contains '='
    bool delimFound = false;
    
    //set to true if input contains a space
    bool spaceFound = false;
    
    //for all characters in the input
    for (auto ch: a_credential)
    {
        if (ch == ' ')
            spaceFound = true;
        
        //if character is '='
        else if (ch == m_userPassDelim)
            delimFound = true;
    }
    
    //if input contains space or does not contain the delimeter
    if (!delimFound || spaceFound)
    {
        //inform client about invalid username=password
        SendMsg(m_clients[a_index].soc, Display::GetInvalidCredMsg());
        
        //redisplay the sign up prompt
        SendMsg(m_clients[a_index].soc, Display::m_signupPrompt);
        
        return false;
    }
    
    //parse the username from username=password
    string username = a_credential.substr(0, a_credential.find(m_userPassDelim, 0));
    
    //check if username is valid
    if (!UsernameIsValid(a_index, username))
    {
        //redisplay the sign up prompt
        SendMsg(m_clients[a_index].soc, Display::m_signupPrompt);
        
        return false;
    }
    
    //parse the password from username=password
    string password = a_credential.substr(a_credential.find(m_userPassDelim) + 1);
    
    //check if password is valid
    if (!PasswordIsValid(password))
    {
        //display the password requirements
        SendMsg(m_clients[a_index].soc, Display::GetPasswordRequirements());
        
        //redisplay the sign up prompt
        SendMsg(m_clients[a_index].soc, Display::m_signupPrompt);
        
        return false;
    }
    
    return true;
}
/*bool Server::CheckCredentials(const int& a_index, const string& a_credential);*/


/**/
/*
 Server::UsernameIsValid Server::UsernameIsValid

NAME

        UsernameIsValid - to check whether a given username meets the username requirements
 
SYNOPSIS

        bool Server::UsernameIsValid(const int& a_index, const string& a_username);
            a_index    --> the index of the client on the connected clients list
            a_username --> a specified username to create an account
            
DESCRIPTION
        
        This function checks whether the specified username by client to create an
        account meets the username requirements.

RETURNS

        Returns true if username is valid
        Returns false otherwise
 
*/
/**/

bool Server::UsernameIsValid(const int& a_index, const string& a_username)
{
    //username must be 6-10 characters long
    if (a_username.size() < 6 || a_username.size() > 10)
    {
        //inform client about username requirements
        SendMsg(m_clients[a_index].soc, Display::GetUsernameRequirements());
        
        return false;
    }
        
    //only letters and numbers are allowed
    for (auto ch: a_username)
    {
        if (!isalpha(ch) && !isdigit(ch))
        {
            //inform client about username requirements
            SendMsg(m_clients[a_index].soc, Display::GetUsernameRequirements());
            
            return false;
        }
    }
    
    //capitalize the username
    string capUser;
    for (auto ch: a_username)
        capUser += toupper(ch);
    
    //username cannot start with NPC (reserved)
    if (capUser.find("NPC") == 0)
    {
        //inform client about username requirements
        SendMsg(m_clients[a_index].soc, Display::GetUsernameRequirements());
        
        return false;
    }
    
    //if another client has this username
    if (UsernameIsTaken(a_username))
    {
        //inform client about it
        SendMsg(m_clients[a_index].soc, Display::GetTakenUserMsg());
        
        return false;
    }
    
    return true;
}
/*bool Server::UsernameIsValid(const int& a_index, const string& a_username);*/


/**/
/*
 Server::UsernameIsTaken Server::UsernameIsTaken

NAME

        UsernameIsTaken - to check whether a given is username is already taken
 
SYNOPSIS

        bool Server::UsernameIsTaken(const string& a_username);
            a_username --> a specified username to create an account
            
DESCRIPTION
        
        This function checks whether the specified username by client to create an
        account is taken already by another registered client.

RETURNS

        Returns true if username is taken by another client
        Returns false otherwise
 
*/
/**/

bool Server::UsernameIsTaken(const string& a_username)
{
    //read all the database information into a buffer
    stringstream buffer = m_database.FetchFileData();
    
    //line read from buffer
    string credentialsFromFile;
    
    //capitalize username provided
    string capUsername = "";
    for(auto i: a_username)
        capUsername += toupper(i);
    
    while (getline (buffer, credentialsFromFile))
    {
        //parse the username from buffer
        string userFromFile = credentialsFromFile.substr(0, credentialsFromFile.find(m_userPassDelim, 0));
        
        //capitalize username from databse
        for(auto& i: userFromFile)
            i = toupper(i);
        
        //if usernames match
        if (capUsername == userFromFile)
            return true;
    }
    
    return false;
}
/*bool Server::UsernameIsTaken(const string& a_username);*/


/**/
/*
 Server::PasswordIsValid Server::PasswordIsValid

NAME

        PasswordIsValid - to check whether a given password meets the password requirements
 
SYNOPSIS

        bool Server::PasswordIsValid(const string& a_password) const;
            a_password --> a specified password to create an account
            
DESCRIPTION
        
        This function checks whether the specified password by client to create an
        account meets the password requirements.

RETURNS

        Returns true if password is valid
        Returns false otherwise
 
*/
/**/


bool Server::PasswordIsValid(const string& a_password) const
{
    //password must be 8-20 characters long
    if (a_password.size() < 8 || a_password.size() > 20)
        return false;
    
    //must not contain these characters --> (space = , -)
    for (auto ch: a_password)
        if (ch == ' ' || ch == m_userPassDelim || ch == m_credentialDelim || ch == m_scoreDelim)
            return false;
    
    //must contain one letter and one number
    
    //set to true once a letter is found
    bool oneLetter = false;
    
    //set to true once a number is found
    bool oneNum = false;
    
    for (auto ch: a_password)
    {
        if (isalpha(ch))
            oneLetter = true;
        else if (isdigit(ch))
            oneNum = true;
    }
    
    //if it does not have at least one letter or at least one number
    if (!oneLetter || !oneNum)
        return false;
    
    return true;
}
/*bool Server::PasswordIsValid(const string& a_password) const;*/


/**/
/*
 Server::HandleMainMenuSelection Server::HandleMainMenuSelection

NAME

        HandleMainMenuSelection - to respond to client selection on the main menu
 
SYNOPSIS

        void Server::HandleMainMenuSelection(const int& a_index, const string& a_input);
            a_index --> the index of the client on connected client list
            a_input --> the client's input
            
DESCRIPTION
        
        This function checks whether the client would like to create a match,
        join a match, view the leaderboards table or log out and takes appropriate
        actions accordingly.

RETURNS

         None
 
*/
/**/

void Server::HandleMainMenuSelection(const int& a_index, const string& a_input)
{
    //if selection is invalid
    if (a_input.size() != 1 || !isdigit(a_input[0]) || (stoi(a_input) != createMatch && stoi(a_input) != joinMatch &&
                                                        stoi(a_input) != leaderboards && stoi(a_input) != logout) )
    {
        //inform client about invalid selection
        SendMsg(m_clients[a_index].soc, Display::GetInvalidSelectionMsg());
        
        //redisplay the main menu
        SendMsg(m_clients[a_index].soc, Display::GetMainMenu());
        
        return;
    }
    
    //convert string input to integer
    int selection = stoi(a_input);
    
    //if client would like to create a match
    if (selection == createMatch)
    {
        //check if server has reached its max match capacity
        //INT_MAX is the capacity even though the vector could hold more items
        //this is because access by index will not be possible after INT_MAX, the largest integer
        
        //if server has reached its maximum number of matches
        if (GetTotalMatches() == INT_MAX)
        {
            //inform client about this
            SendMsg(m_clients[a_index].soc, Display::GetMaxMatchCapReachedMsg());
            
            //client goes back to main menu
            m_clients[a_index].stage = mustSelectFromMainMenu;
            SendMsg(m_clients[a_index].soc, Display::GetMainMenu());
        
            return;
        }
        
        //prompt client to select match type (NPC or RGP)
        SendMsg(m_clients[a_index].soc, Display::GetMatchTypeMenu());
        m_clients[a_index].stage = mustSelectMatchType;
    }
    
    //if client would like to join a match
    else if (selection == joinMatch)
    {
        //the message to be displayed to client
        string msg = "";
        
        //get all ongoing matches on the server that are waiting for players to join
        vector<Match> openMatches = GetOpenMatches();
        
        //if there are no matches to join
        if (openMatches.empty())
        {
            //inform client about this
            msg += Display::GetNoOpenMatchMsg();
            
            //client goes back to main menu
            m_clients[a_index].stage = mustSelectFromMainMenu;
            msg += Display::GetMainMenu();
        }
        
        else
        {
            //display all the matches that client can join
            msg += Display::GetOpenMatchesTable(openMatches);
            
            //ask client which match they would like to join
            msg += Display::m_joinMatchPrompt;
            m_clients[a_index].stage = mustSelectMatchToJoin;
        }
            
        SendMsg(m_clients[a_index].soc, msg);
    }
    
    //if client wants to view the leaderboards table
    else if (selection == leaderboards)
    {
        //display the leaderboards table to client
        SendMsg(m_clients[a_index].soc, Display::GetLeaderboards(m_database.FetchFileData(), m_userPassDelim, m_credentialDelim, m_scoreDelim));
        
        //redisplay the main menu
        SendMsg(m_clients[a_index].soc, Display::GetMainMenu());
    }
    
    //if player wants to log out
    else
    {
        cout<<m_clients[a_index].username<<" Logged Out"<<endl;
        
        //inform client about the disconnection
        SendMsg(m_clients[a_index].soc, Display::GetLogoutMsg());
        
        //deallocate the socket for reuse
        close(m_clients[a_index].soc);
        
        //remove the client from the lisr of connected clients
        m_clients.erase(m_clients.begin() + a_index);
    }
}
/*void Server::HandleMainMenuSelection(const int& a_index, const string& a_input);*/


/**/
/*
 Server::GetTotalMatches Server::GetTotalMatches

NAME

        GetTotalMatches - to find the total number of matches being created and in progress on the server
 
SYNOPSIS

        int Server::GetTotalMatches() const;
            
DESCRIPTION
        
        This function adds all the number of ongoing matches and the matches that are
        in the process of being created (i.e. selecting the match preferences) on the server.
 
RETURNS

        The total number of matches
 
*/
/**/

int Server::GetTotalMatches() const
{
    int numMatches = 0;
    
    //the clients in the process of creating a match
    for (auto client: m_clients)
        if (client.stage == mustSelectMatchType || client.stage == mustSelectNPCIntelligence || client.stage == mustSelectMatchMode)
            numMatches++;
    
    //+ the matches in progress on the server
    return numMatches + (int)m_matches.size();
}
/*int Server::GetTotalMatches() const;*/


/**/
/*
 Server::GetOpenMatches Server::GetOpenMatches

NAME

        GetOpenMatches - to get the matches that need players to join to reach max capacity to be started
 
SYNOPSIS

        vector<Match> Server::GetOpenMatches() const;
            
DESCRIPTION
        
        This function finds all matches that need more players to join them in order
        for them to be started.
 
RETURNS

        The matches with missing players
 
*/
/**/

vector<Match> Server::GetOpenMatches() const
{
    vector<Match> openMatches;
    for (auto match: m_matches)
        if (match.GetMode() != match.GetCurrentNumPls())
            openMatches.push_back(match);
    
    return openMatches;
}
/*vector<Match> Server::GetOpenMatches() const;*/


/**/
/*
 Server::HandleMatchTypeSelection Server::HandleMatchTypeSelection

NAME

        HandleMatchTypeSelection - to handle client request regarding selecting the match type (NPC or RGP)
 
SYNOPSIS

        void Server::HandleMatchTypeSelection(const int& a_index, const string& a_input);
             a_index --> the index of the client on connected client list
             a_input --> the client's input
            
DESCRIPTION
        
        This function identifies the match that the client would like to create (NPC or RGP)
        and records this information to be used later on for creating the match.
    
 
RETURNS

        None
 
*/
/**/

void Server::HandleMatchTypeSelection(const int& a_index, const string& a_input)
{
    //if selection is invalid
    if (a_input.size() != 1 || !isdigit(a_input[0]) || (stoi(a_input) != npc && stoi(a_input) != rgp))
    {
        //inform client about invalid selection
        SendMsg(m_clients[a_index].soc, Display::GetInvalidSelectionMsg());
        
        //redisplay the match type selection menu
        SendMsg(m_clients[a_index].soc, Display::GetMatchTypeMenu());
        
        return;
    }
    
    int selection = stoi(a_input);
    
    //if client wants to create a match with all computer players
    if (selection == npc)
    {
        //record this preference
        m_clients[a_index].preference.p_type = npc;
        
        //client must now select the intelligence level for NPCs
        m_clients[a_index].stage = mustSelectNPCIntelligence;
        
        //display the menu for selecting NPC intelligence
        SendMsg(m_clients[a_index].soc, Display::GetIntelligenceMenu());
    }
    
    //if client wants to create a match with all human players
    else
    {
        //record this preference
        m_clients[a_index].preference.p_type = rgp;
        
        //client must now select the number of players in the match
        m_clients[a_index].stage = mustSelectMatchMode;
        
        //display the menu for selecting the match mode
        SendMsg(m_clients[a_index].soc, Display::GetMatchModeMenu());
    }
}
/*void Server::HandleMatchTypeSelection(const int& a_index, const string& a_input);*/


/**/
/*
 Server::HandleNPCIntelligenceSelection Server::HandleNPCIntelligenceSelection

NAME

        HandleNPCIntelligenceSelection - to handle client request regarding selecting the intelligence of NPCs
 
SYNOPSIS

        void Server::HandleNPCIntelligenceSelection(const int& a_index, const string& a_input);
             a_index --> the index of the client on connected client list
             a_input --> the client's input
            
DESCRIPTION
        
        This function identifies the NPC intelligence level for the match that
        the client would like to create and records this information to be used
        later on for creating the match.
    
 
RETURNS

        None
 
*/
/**/

void Server::HandleNPCIntelligenceSelection(const int& a_index, const string& a_input)
{
    //if selection is invalid
    if (a_input.size() != 1 || !isdigit(a_input[0]) || (stoi(a_input) != low && stoi(a_input) != average && stoi(a_input) != high))
    {
        //inform client about invalid selection
        SendMsg(m_clients[a_index].soc, Display::GetInvalidSelectionMsg());
        
        //redisplay the NPC intelligence menu
        SendMsg(m_clients[a_index].soc, Display::GetIntelligenceMenu());
        
        return;
    }
    
    //record this preference
    Intelligence selection = (Intelligence)stoi(a_input);
    m_clients[a_index].preference.p_intelligence = selection;
    
    //client must now select the match mode
    m_clients[a_index].stage = mustSelectMatchMode;
    
    //display the menu for selecting match mode
    SendMsg(m_clients[a_index].soc, Display::GetMatchModeMenu());
}
/*void Server::HandleNPCIntelligenceSelection(const int& a_index, const string& a_input);*/


/**/
/*
 Server::HandleMatchModeSelection Server::HandleMatchModeSelection

NAME

        HandleMatchModeSelection - to handle client request regarding the number of players in the match
 
SYNOPSIS

        void Server::HandleMatchModeSelection(const int& a_index, const string& a_input);
             a_index --> the index of the client on connected client list
             a_input --> the client's input
            
DESCRIPTION
        
        This function identifies the mode of the match the client would like to create.
        If the match type is NPC, the match is started right away. Otherwise if the match
        type is RGP, enough players must join in order for the match to start.
    
 
RETURNS

        None
 
*/
/**/

void Server::HandleMatchModeSelection(const int& a_index, const string& a_input)
{
    //if selection is invalid (+1 to match with the menu)
    if (a_input.size() != 1 || !isdigit(a_input[0]) || (stoi(a_input) + 1 != twoPlayer &&
                                                        stoi(a_input) + 1 != threePlayer && stoi(a_input) + 1 != fourPlayer))
    {
        //inform client about invalid selection
        SendMsg(m_clients[a_index].soc, Display::GetInvalidSelectionMsg());
        
        //redisplay the match mode menu
        SendMsg(m_clients[a_index].soc, Display::GetMatchModeMenu());
        
        return;
    }
    
    //+1 to match with the menu
    int selection = stoi(a_input) + 1;
    
    //at this point match creation is guaranteed
    
    //create the match creator
    Player* humanPl = new Human(m_clients[a_index].username, m_clients[a_index].soc);
    
    cout<<humanPl->GetUsername()<<" Created A Match"<<endl;
    
    //record the mode
    m_clients[a_index].preference.p_mode = (Mode)selection;
    
    //create a new match
    Match newMatch(m_clients[a_index].preference.p_mode, humanPl);
    
    //add the match to the ongoing matches on the server
    m_matches.push_back(newMatch);
    
    if (m_clients[a_index].preference.p_type == npc)
    {
        for (int i=0; i<m_matches.back().GetMode() - 1; i++)
        {
            //create the remaining players as NPCs
            Player* compPl = new Computer(m_npcNames[i], m_clients[a_index].preference.p_intelligence);
            m_matches.back().AddPlayer(compPl);
        }
        
        cout<<humanPl->GetUsername()<<" Started A Match"<<endl;
        
        //start the first round of the match
        m_matches.back().StartNewRound();
        
        //client has now entered a match
        m_clients[a_index].stage = insideMatch;
    }
    
    //RGP
    else
    {
        //inform client about the wait to get all the match players
        SendMsg(m_clients[a_index].soc, Display::GetSuccessMatchCreationMsg());
        
        //client must wait for other players to join the match
        m_clients[a_index].stage = mustWaitToStartMatch;
    }
}
/*void Server::HandleMatchModeSelection(const int& a_index, const string& a_input);*/


/**/
/*
 Server::HandleJoinMatchRequest Server::HandleJoinMatchRequest

NAME

        HandleJoinMatchRequest - to handle client request regarding joining a match on the server
 
SYNOPSIS

        void Server::HandleJoinMatchRequest(const int& a_index, const string& a_input);
             a_index --> the index of the client on connected client list
             a_input --> the client's input
            
DESCRIPTION
        
        This function allows a client to be added to a match that needs players
        to join in order for it to be started.
    
 
RETURNS

        None
 
*/
/**/

void Server::HandleJoinMatchRequest(const int& a_index, const string& a_input)
{
    //capitalize the input since it's not case sensitive
    string capInput = "";
    for(auto i: a_input)
        capInput += toupper(i);
    
    //set to true if the match is found on the server
    bool found = false;
    
    //set to true if the match still has empty spots
    bool emptySpot = false;
    
    //the index of the match on the server (TBD)
    int matchIndex = 0;
    
    for (int i=0; i<m_matches.size(); i++)
    {
        //capitalize username to match input (not case sensitive)
        string capUsername = "";
        for(auto i: m_matches[i].GetMatchCreater()->GetUsername())
            capUsername += toupper(i);
        
        if (capUsername == capInput)
        {
            found = true;
            
            if (m_matches[i].GetCurrentNumPls() != m_matches[i].GetMode())
            {
                emptySpot = true;
                matchIndex = i;
            }
        }
    }
    
    //if match is not found or no longer has empty spots
    if (!found || !emptySpot)
    {
        //the meessage to be displayed to the client
        string msg;
        
        //if the match is not found on the server
        if (!found)
        {
            //match not found
            msg = Display::GetMatchNotFoundMsg();
        }
        
        //if the match no longer has empty spots
        else if (!emptySpot)
        {
            //another user joined the match before this user
            msg = Display::GetMatchNoLongerOpenMsg();
        }
        
        //change client status to main menu
        m_clients[a_index].stage = mustSelectFromMainMenu;
        
        //display the main menu
        msg += Display::GetMainMenu();
        SendMsg(m_clients[a_index].soc, msg);
        
        return;
    }
    
    //otherwise add this client to the match
    Player* pl = new Human(m_clients[a_index].username, m_clients[a_index].soc);
    m_matches[matchIndex].AddPlayer(pl);
    
    cout<<pl->GetUsername()<<" Joined a Match"<<endl;
    
    //get the players of this match
    vector<Player*> players = m_matches[matchIndex].GetPlayers();
    
    //inform all players that a client has joined
    string joinMatchOtherPl = Display::GetPlJoinedMsg(players.back()->GetUsername());
    
    //display the list of all players
    string plList = Display::GetMatchPlayersList(players);
    
    //last player (player that just joined) will get a different message
    for (int i=0; i<players.size() - 1; i++)
        SendMsg(players[i]->GetSocket(), joinMatchOtherPl + plList);
    
    //last player's message
    SendMsg(players.back()->GetSocket(), Display::GetSuccessJoinMatchMsg() + plList);
    
    //if match has reached its max capacity
    if (m_matches[matchIndex].GetCurrentNumPls() == m_matches[matchIndex].GetMode())
    {
        //inform match creator to start the match
        SendMsg(players[0]->GetSocket(), Display::GetStartMatchMsg());
        
        //match creator must start the match
        int clientIndex = GetClientIndexByUsername(players[0]->GetUsername());
        m_clients[clientIndex].stage = mustAllowMatchToStart;
        
        //inform other players about max capacity being reached
        //index = 0 --> match creator
        for (int i = 1; i<players.size(); i++)
            SendMsg(players[i]->GetSocket(), Display::GetMaxMatchCapReachedMsg());
    }
    
    //this client must wait for other players join to start the match
    //or wait for match creator to start match if max capacity is reached
    m_clients[a_index].stage = mustWaitToStartMatch;
}
/*void Server::HandleJoinMatchRequest(const int& a_index, const string& a_input);*/


/**/
/*
 Server::HandleStartMatchRequest Server::HandleStartMatchRequest

NAME

        HandleStartMatchRequest - to handle client request regarding starting a multi-player match
 
SYNOPSIS

        void Server::HandleStartMatchRequest(const int& a_index, string a_input);
             a_index --> the index of the client on connected client list
             a_input --> the client's input
            
DESCRIPTION
        
        This function allows the match creator to start the match that has
        reached its maximum capacity as long as the START comman is entered.
    
 
RETURNS

        None
 
*/
/**/

void Server::HandleStartMatchRequest(const int& a_index, string a_input)
{
    //capitalize the input
    for(auto& i: a_input)
        i = toupper(i);
    
    //if client entered the command START to start the match
    if (a_input == matchStartCommand)
    {
        //find the match that the creator wants to start
        int matchIndex = GetMatchIndexByPlUsername(m_clients[a_index].username);
        
        cout<<m_clients[a_index].username<<" Started A Multi-Player Match"<<endl;
        
        //start the first round of this match
        m_matches[matchIndex].StartNewRound();
        
        //identify the players of the match
        vector<Player*> players = m_matches[matchIndex].GetPlayers();
        
        //all clients associated with this match are now inside the match
        for (auto player: players)
        {
            int index = GetClientIndexByUsername(player->GetUsername());
            m_clients[index].stage = insideMatch;
        }
    }
    
    else
        SendMsg(m_clients[a_index].soc, Display::GetInvalidSelectionMsg());
}
/*void Server::HandleStartMatchRequest(const int& a_index, string a_input);*/


/**/
/*
 Server::HandleInsideMatchRequest Server::HandleInsideMatchRequest

NAME

        HandleInsideMatchRequest - to handle client request while they're inside a match
 
SYNOPSIS

        void Server::HandleInsideMatchRequest(const int& a_index, const string& a_input);
             a_index --> the index of the client on connected client list
             a_input --> the client's input
            
DESCRIPTION
        
        This function allows a client to make a move inside the match that they
        are currently in. If the match is over, the database updates the match
        records of the players that were involved in the match (if the match
        was completed as a multiplayer match).
    
RETURNS

        None
 
*/
/**/

void Server::HandleInsideMatchRequest(const int& a_index, const string& a_input)
{
    //find the match index that the player is currently in
    int matchIndex = GetMatchIndexByPlUsername(m_clients[a_index].username);

    //perform the necessary action (make the requested move)
    bool matchIsOver = m_matches[matchIndex].HandleMatchMove(m_clients[a_index].username, insideMatch, a_input);
    
    //if the match has ended
    if (matchIsOver)
    {
        //the players that won this match
        vector<Player*> matchWinners = m_matches[matchIndex].GetWinner();
        
        if (matchWinners.size() == 1)
            cout<<matchWinners[0]->GetUsername()<<" Won A Match"<<endl;
        else
            cout<<matchWinners[0]<<" And "<<matchWinners[1]<<" Won A Match"<<endl;
        
        //update the client match records in the database (if applicable)
        //if the match was completed as a multi-player match
        if (m_matches[matchIndex].GetNumCompPls() == 0)
        {
            //map each player to a win/loss
            map<string, bool> matchOutcome;
            
            for (auto pl: m_matches[matchIndex].GetPlayers())
            {
                for (auto winner: m_matches[matchIndex].GetWinner())
                {
                    if (pl == winner)
                    {
                        //record as a win
                        matchOutcome.insert({pl->GetUsername(), true});
                    }
                    
                    else
                    {
                        //record as a loss
                        matchOutcome.insert({pl->GetUsername(), false});
                    }
                }
            }
            
            //update the database with the outcome of this match
            m_database.UpdateAccounts(matchOutcome, m_userPassDelim, m_credentialDelim, m_scoreDelim);
        }
        
        //all players are now prompted to the main menu
        for (auto& client: m_clients)
        {
            for (auto player: m_matches[matchIndex].GetPlayers())
            {
                if (client.username == player->GetUsername())
                {
                    client.stage = mustSelectFromMainMenu;
                    SendMsg(client.soc, Display::GetMainMenu());
                }
            }
        }
        
        //destroy and remove the match from the server matches
        m_matches.erase(m_matches.begin() + matchIndex);
    }
}
/*void Server::HandleInsideMatchRequest(const int& a_index, const string& a_input);*/


/**/
/*
 Server::SendMsg Server::SendMsg

NAME

        SendMsg - to send a message to a connected client
 
SYNOPSIS

        bool Server::SendMsg(SOCKET a_soc, const string& a_msg);
             a_soc --> the client's socket
             a_msg --> the message to be sent to the client
            
DESCRIPTION
        
        This function sends a message to a specified socket.
    
RETURNS

        Returns true if message was sent successfully
        Returns false otherwise
*/
/**/

bool Server::SendMsg(SOCKET a_soc, const string& a_msg)
{
    if (a_soc == 0)
        return false;
    
    //find the size of message
    const int msgSize = (int)a_msg.length();
    
    //convert string to character array
    char msgBuff[(int)a_msg.length() + 1];
    strcpy(msgBuff, a_msg.c_str());
    
    //null-terminate the message
    msgBuff[(int)a_msg.length()] = '\0';
    
    if (send(a_soc, msgBuff, msgSize, 0) == INVALID_SOCKET)
    {
        cout<<"Send To Client Failed!"<<endl;
        close(a_soc);
        a_soc = 0;
        return false;
    }
    
    return true;
}
/*bool Server::SendMsg(SOCKET a_soc, const string& a_msg);*/


//to find the match that a client is currently in using client's username
int Server::GetMatchIndexByPlUsername(const string& a_username)
{
    int i;
    for (i=0; i<m_matches.size(); i++)
        if (m_matches[i].IsInMatch(a_username))
            return i;
    
    return i;
}


//to find the index of a client on the list of connected clients using their username
int Server::GetClientIndexByUsername(const string& a_username)
{
    int index = 0;
    for (int i=0; i<m_clients.size(); i++)
        if (m_clients[i].username == a_username)
            index = i;
    
    return index;
}


/**/
/*
 PassiveTCP PassiveTCP

NAME

        PassiveTCP - to prepare to receive connections for a service
 
SYNOPSIS

        static SOCKET PassiveTCP(const string& a_service, const int& a_qlen);
             a_service --> the port number
             a_qlen    --> used to indicate being ready for connections
            
DESCRIPTION
        
        This function establishes a TCP connection for the server to recieve
        incoming connection requests.
    
RETURNS

        Returns the listening socket
*/
/**/

static SOCKET PassiveTCP(const string& a_service, const int& a_qlen)
{
    //points to service information
    struct servent* pse;
    
    //internet endpoint address
    struct sockaddr_in sin;

    /* create an end point address for this computer. We use INADDR_ANY
    * for the address of the computer so that we may receive connection
    * requests through any of the computer's IP addresses.  */
    memset((char*)&sin, 0, sizeof(sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    //get the port number for the service
    if ((pse = getservbyname(a_service.c_str(), "tcp")) != NULL)
        sin.sin_port = (u_short)pse->s_port;
    
    else if ((sin.sin_port = htons((u_short)stoi(a_service))) == 0)
    {
        cerr << "Bad Port number/service specified: " << a_service << endl;;
        exit(1);
    }
    
    //allocate a socket
    SOCKET ls;
    
    if ((ls = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        cout<<"Socket Call Failed"<<endl;
        exit(1);
    }
    
    //bind the address to the socket
    if (::bind(ls, (struct sockaddr*)&sin, sizeof(sin)) == INVALID_SOCKET)
    {
        cout<<"Bind Call Failed"<<endl;
        exit(1);
    }
    
    //indicate that we are ready to wait for connections
    if (listen(ls, a_qlen) == INVALID_SOCKET)
    {
        cout<<"Listen Call Failed"<<endl;
        exit(1);
    }
    
    //return the listening socket
    return ls;
}
/*static SOCKET PassiveTCP(const string& a_service, const int& a_qlen);*/


//to convert a character array recieved from client to a string
string ConvertToString(const char* a_buff, const ssize_t& a_nb)
{
    string str;
    
    //-2 because last two bytes are empty characters
    for (int i = 0; i < a_nb - 2; i++)
        str += a_buff[i];
    
    return str;
}

//to print a message while constantly blocking execution of the calling thread during a span of time
void PrintSlowMsg(const string& a_msg, const int& a_time)
{
    for(char c: a_msg)
    {
        this_thread::sleep_for(chrono::milliseconds(a_time));
        cout<<c<<flush;
    }
}
