//Server class --> used to allow clients to connect to server and play the game

#pragma once

//options to choose on home page
enum HomePageOptions
{
    login = 1,
    signup
};

//options to choose on main menu
enum MainMenuOptions
{
    createMatch = 1,
    joinMatch,
    leaderboards,
    logout
};

//playing against computer or human players
enum MatchType
{
    //non-player characters
    npc = 1,
    
    //real game players
    rgp
};

//the match client would like to cretate on the server
struct MatchPreference {
    
    //the type of match (npc or rgp)
    MatchType p_type;
    
    //the number of players in the match
    Mode p_mode;
    
    //the npc intelligence level (if in npc mode)
    Intelligence p_intelligence;
    
    MatchPreference()
    {
        //defaults
        p_type = npc;
        p_mode = twoPlayer;
        p_intelligence = average;
    }
};

//information of a client connected to the server
struct Client {
    
    //connected socket of client
    SOCKET soc;
    
    //the client stage
    ClientStage stage;
    
    //the client username
    string username;
    
    //the prefereance of the game to create
    MatchPreference preference;
    
    //the last time an input was recieved from the client
    time_t timer;

    Client(const SOCKET& a_soc)
    {
        soc = a_soc;
        stage = mustSelectFromHomePage;
        username = "";
        preference = MatchPreference();
        timer = time(NULL);
    }
};


class Server
{

public:
    
    //*** constructors ***//
    Server(int, char*[]);
    
    //*** destrcutor ***//
    ~Server();
    
    //*** other functions ***//
    
    //to start up the server and accept incoming requests
    void StartUp();
    
    //to send a message to a connected client
    static bool SendMsg(SOCKET, const string&);
    

private:
    
    //the port number for the service to be provided
    string m_service;
    
    //the connected clients
    vector<Client> m_clients;
    
    //the current ongoing matches on the server
    vector<Match> m_matches;
    
    //file that holds all client account information
    FileAccess m_database;
    
    //the maximum time the function SELECT can take to complete
    timeval* m_timeval;
    
    //the pre-made usernames of NPCs
    map<int, string> m_npcNames = {
        {0, "NPC1"},
        {1, "NPC2"},
        {2, "NPC3"}
    };
    
    //*** other data members ***//
    
    //username --> 6-10 characters
    //delimiter(=) --> 1 character
    //password --> 8-20 characters
    
    //max username          --> 10 characters
    //delimiter             --> 1 character
    //max password          --> 20 characters
    //max username=password --> 10 + 1 + 20 = 31
    
    //the maximum size of a username=password
    static const int m_maxUserPassSize = 31;
    
    //to separate username and password
    static const char m_userPassDelim = '=';
    
    //to separate username=password from client match records
    static const char m_credentialDelim = ',';
    
    //to separate the number of wins and losses
    static const char m_scoreDelim = '-';
    
    //the maximum time client has to respond during a match (90 seconds)
    static const int m_matchInactivity = 90;
    
    //the maximum time client has to respond outside of a match
    static const int m_generalInactivity = 5 * m_matchInactivity;
    
    //the command inputted by match creator to start a match
    static string matchStartCommand;
    
    
    //private functions
    
    
    //to handle an incoming client connection request
    void HandleConnectionRequest(const SOCKET&);
    
    //to step through connected clients to see if any data is pending
    void CheckPendingData(fd_set*);
    
    //to handle a client who has gone idle
    bool HandleIdleClient(const int&);
    
    //to handle a client that unexpectedly disconnects
    void HandleDisconnection(const int&);
    
    //to handle a client input while connected to server
    void HandleRequest(const int&, string);
    
    //to handle client request regarding login or creating an account
    void HandleHomePageSelection(const int&, const string&);
    
    //to login the client if authenticated
    void LogIn(const int&, const string&);
    
    //to check whether a set of username and password exists in the database
    bool Authenticate(string&, const string&);
    
    //to check whether a client is already logged in
    bool IsLoggedIn(string);
    
    //to handle client request to create a new account
    void CreateAccount(const int&, const string&);
    
    //to check whether a valid username=password is entered
    bool CheckCredentials(const int&, const string&);
    
    //to check whether a given username meets the username requirements
    bool UsernameIsValid(const int&, const string&);
    
    //to check whether a given username is already taken
    bool UsernameIsTaken(const string&);
    
    //to check whether a given password meets the password requirements
    bool PasswordIsValid(const string&) const;
    
    //to handle client request regarding starting a match, joining a match, or logging out
    void HandleMainMenuSelection(const int&, const string&);
    
    //to find the total number of matches being created and in progress
    int GetTotalMatches() const;
    
    //to get the matches that need players to join to reach max capacity and be started
    vector<Match> GetOpenMatches() const;
    
    //to handle client request regarding selecting the match type (NPCs, RGPs)
    void HandleMatchTypeSelection(const int&, const string&);
    
    //to handle client request regarding selecting the intelligence of NPCs
    void HandleNPCIntelligenceSelection(const int&, const string&);
    
    //to handle client request regarding the number of players in the match
    void HandleMatchModeSelection(const int&, const string&);
    
    //to handle client request regarding joining a match on the server
    void HandleJoinMatchRequest(const int&, const string&);
    
    //to handle client request regarding starting a multi-player match
    void HandleStartMatchRequest(const int&, string);
    
    //to handle client request while they're inside a match
    void HandleInsideMatchRequest(const int&, const string&);
    
    //to lookup a match by the socket of a player inside the match
    int GetMatchIndexByPlUsername(const string&);
    
    //to find a connecteed client by their username
    int GetClientIndexByUsername(const string&);
};

