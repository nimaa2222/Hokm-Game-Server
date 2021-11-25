//Player class --> Used to represent a game player

#pragma once

class Player
{

public:
    
    //*** constructors ***//
    Player(): m_username(""), m_hand({}), m_partner(NULL) {};
    Player(const string& a_username): m_username(a_username), m_hand({}), m_partner(NULL) {};
    Player(const Player& a_pl): m_username(a_pl.m_username), m_hand(a_pl.m_hand), m_partner(a_pl.m_partner) {};
    
    //*** destrcutor ***//
    //~Player(){/*not required*/};
    
    //*** getters ***//
    string GetUsername() const {return m_username;}
    vector<Card> GetHand() const {return m_hand;}
    Player* GetPartner() const {return m_partner;}
    int GetHandSize() const {return (int)m_hand.size();}
    bool IsInHand(const Card& a_card){return find(m_hand.begin(), m_hand.end(), a_card) != m_hand.end();}
    
    //*** mutators ***//
    void SetUsername(string a_username){m_username = a_username;}
    void SetHand(vector<Card> a_hand){m_hand = a_hand;}
    void SetPartner(Player* a_partner){m_partner = a_partner;}
    void SortHand(){sort(m_hand.begin(), m_hand.end());}
    void AddToHand(const Card& a_card){m_hand.push_back(a_card);}
    void RemoveFromHand(const Card& a_card){m_hand.erase(remove(m_hand.begin(), m_hand.end(), a_card), m_hand.end());}
    void EmptyHand(){m_hand.clear();}
    
    //*** overloaded operators ***//
    bool operator == (const Player&) const;
    bool operator != (const Player&) const;
    Player& operator = (const Player&);
    
    //*** virtual functions for computer and human class ***//
    
    //decision making
    
    //to prompt players to select the trump suit
    virtual Suit SelectTrumpSuit() const = 0;
    
    //to prompt players to discard cards from hands
    virtual void DiscardCards(const Suit&) = 0;
    
    //to prompt players to pick or disacrd a card
    virtual Decision PickOrDiscard(const Card&, const Suit&) const = 0;
    
    //to prompt players to select a card
    virtual Card SelectCard(const Mode&, const Turn&, const Suit&, const Suit&, const vector<Card>&) const = 0;
    
    //other functions
    
    //to allow computer player to remember the played cards during the round
    virtual void RememberCard(const Card&) = 0;
    
    //to clear the computer memory of the "memorized" cards
    virtual void ClearMemory() = 0;
    
    //to check whether a player is a computer or human player
    virtual bool IsHuman() const = 0;
    
    //to send a message to a connected client during match
    virtual bool SendMsg(const string&) const = 0;
    
    virtual SOCKET GetSocket() const = 0;
    virtual void SetSocket(const SOCKET&) = 0;
    virtual ClientStage GetStage() const = 0;
    virtual void SetStage(const ClientStage&) = 0;
    
protected:
    
    //the player's username
    string m_username;
    
    //the cards in the player hand
    vector<Card> m_hand;
    
private:
    
    //applicable to 4-player mode (a player's teammate)
    Player* m_partner;
};
