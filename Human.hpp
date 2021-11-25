//Human class --> Used to represent a human player in the game
//inherits from Player class

#pragma once

class Human: public Player
{

public:
    
    //*** constructors ***//
    Human():Player(), m_soc(0), m_stage(insideMatch) {};
    Human(string a_username, SOCKET a_soc):Player(a_username), m_soc(a_soc), m_stage(insideMatch) {};
    
    //*** destructor ***//
    ~Human(){/*not required*/};
    
    //*** getters ***//
    SOCKET GetSocket() const {return m_soc;}
    ClientStage GetStage() const {return m_stage;}
    bool IsHuman() const {return true;}
    
    //*** mutators ***//
    void SetStage(const ClientStage& a_stage){m_stage = a_stage;}
    void SetSocket(const SOCKET& a_soc){m_soc = a_soc;}
    
    //*** decision making ***//
    Suit SelectTrumpSuit() const;
    void DiscardCards(const Suit&);
    Decision PickOrDiscard(const Card&, const Suit&) const;
    Card SelectCard(const Mode&, const Turn&, const Suit&, const Suit&, const vector<Card>&) const {return Card();}
    
    bool SendMsg(const string&) const;
    
    //computer-related functions
    void RememberCard(const Card&){};
    void ClearMemory(){};
    
private:
    
    //the connected client socket number
    SOCKET m_soc;
    
    //the client status in the server
    ClientStage m_stage;
};
