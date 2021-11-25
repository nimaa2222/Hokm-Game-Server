//Match class --> holds together all components of a match

#pragma once

class Match
{

public:
    
    //*** constructors ***//
    Match(const Mode&, Player*);
    Match(const Match&);
    
    //*** destrcutor ***//
    ~Match(){/*not required*/};
    
    //*** getters and setters ***//
    Mode GetMode() const {return m_mode;}
    Deck& GetDeck() {return m_deck;}
    vector<Player*>& GetPlayers() {return m_players;}
    Player* GetPlByUsername(const string&);
    Round& GetCurrentRound() {return m_rounds.back();};
    vector<int> GetPoints() const {return m_points;}
    vector<Player*> GetWinner() const {return m_winner;}
    Player* GetMatchCreater() const {return m_players[0];}
    int GetCurrentNumPls() const {return (int)m_players.size();}
    int GetNumHumanPls() const;
    int GetNumCompPls() const {return m_mode - GetNumHumanPls();}
    int GetRoundNum() {return (int)m_rounds.size();}
    void AddPlayer(Player* a_player) {m_players.push_back(a_player);}
    void RemovePlayer(const string&);
    
    //to replace an existing human player with an NPC
    void ReplacePlayer(const string&, Player*);
    
    //*** other functions ***//
    
    //to check whether a player is in this match (username check)
    bool IsInMatch(const string&) const;
    
    //to perform a client-requested action inside a game on the server
    bool HandleMatchMove(const string&, const ClientStage&, const string&);
    
    //to identify Hakem for this round and prompt for trump suit selection in the round
    void StartNewRound();
    
    //to identify the Hakem for this round and empty the player hands
    int SetupMatch();
    
    //to shuffle the deck and determine the very first Hakem (and teams in 4-player mode)
    Player* IdentifyInitialHakem();
    
    //to identify the first player to recieve an Ace among other players
    Player* IdentifyAcePlayer(const Deck, vector <Player*>);
    
    //to find the index of a player in the match
    int GetPlayerIndex(const string&) const;
    
    //to declare the round winner and update the game points using number of points scored during the round
    void PerformAfterRound();
    
    //to check if the match is over
    bool MatchIsOver() const;
    
    //to clear game table and declare the game winnner
    void PerformAfterMatch();

    //to identify and announce the winner of the game
    void DeclareMatchWinner();


private:
    
    // 2-player, 3-player or 4-player match
    Mode m_mode;
    
    //the game deck
    Deck m_deck;
    
    //the game players
    vector <Player*> m_players;
    
    //the rounds played
    vector<Round> m_rounds;
    
    //the points scored by players
    vector<int> m_points;
    
    //the match winner (or team in 4-player mode)
    vector<Player*> m_winner;
};

//function to swap players
void swap(vector<Player*>&, int, int);
