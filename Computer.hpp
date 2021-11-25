//Computer class --> Used to represent a computer player in the game

#pragma once

class Computer: public Player
{
    
public:
    
    //*** constructors ***//
    Computer(): Player(), m_intelligence(average), m_cardsPlayed({}) {};
    Computer(string a_name, Intelligence a_intelligence):Player(a_name), m_intelligence(a_intelligence), m_cardsPlayed({}) {};
    
    //*** destructor ***//
    ~Computer(){/*not required*/};
    
    //*** getters ***//
    bool IsHuman() const {return false;}
    
    //*** mutators ***//
    void RememberCard(const Card& a_card){m_cardsPlayed.push_back(a_card);}
    void ClearMemory(){m_cardsPlayed.clear();}
    
    //human-related functions
    SOCKET GetSocket() const {return 0;}
    void SetSocket(const SOCKET&){};
    ClientStage GetStage() const {return insideMatch;}
    void SetStage(const ClientStage& a_stage){}
    bool SendMsg(const string&) const {return true;}
    
    //*** decision making ***//
    Suit SelectTrumpSuit() const;
    void DiscardCards(const Suit&);
    Decision PickOrDiscard(const Card&, const Suit&) const;
    Card SelectCard(const Mode&, const Turn&, const Suit&, const Suit&, const vector<Card>&) const;
    

private:
    
    //the computer player intelligence level in terms of decision making
    Intelligence m_intelligence;
    
    //the memory of the computer (the cards played during a ronud by all the players)
    vector<Card> m_cardsPlayed;
    
    
    
    //***    2-Player mode selections   ***//
    
    // --> for high intelligence card selection
    
    //to select a card as the first player in the turn in 2-player mode with high intelligence level
    Card PickHighTwoPlFirst(const Suit&) const;
    
    //to select a card as the second player in the turn in 2-player mode with high intelligence level
    Card PickHighTwoPlSecond(const Suit&, const Suit&, const vector<Card>&) const;
    
    
    // --> for average intelligence card selection
    
    //to select a card as the first player in the turn in 2-player mode with average intelligence level
    Card PickAverageTwoPlFirst() const;
    
    //to select a card as the second player in the turn in 2-player mode with average intelligence level
    Card PickAverageTwoPlSecond(const Suit&, const Suit&, const vector<Card>&) const;
    
    
    // --> for low intelligence card selection
    
    //to select a card as the first player in the turn in 2-player mode with low intelligence level
    Card PickLowTwoPlFirst(const Suit&) const;
    
    //to select a card as the second player in the turn in 2-player mode with low intelligence level
    Card PickLowTwoPlSecond(const Suit&, const Suit&, const vector<Card>&) const;
    
    
    //***    3-Player mode selections   ***//
    
    //to select a card as the second player in the turn in 3-player mode with high intelligence level
    Card PickHighThreePlSecond(const Suit&, const Suit&, const vector<Card>&) const;
    
    //to select a card as the second player in the turn in 3-player mode with average intelligence level
    Card PickAverageThreePlSecond(const Suit&, const Suit&, const vector<Card>&) const;
    
    //to select a card as the second player in the turn in 3-player mode with low intelligence level
    Card PickLowThreePlSecond(const Suit&, const Suit&, const vector<Card>&) const;
    
    
    //***     other helper functions for computer strategy    ***//
    
    //to select the card with the most number of matching suits in hand
    Card PickMostCommonSuitCard(const vector<Card>&) const;
    
    //to select the card with the least number of matching suits in hand
    Card PickLeastCommonSuitCard(const vector<Card>&) const;
    
    //to find the most common suit in hand
    Suit GetMostCommonSuit(const vector<Suit>&) const;
    
    //to find the least common suit in hand
    Suit GetLeastCommonSuit(const vector<Suit>&) const;
    
    //to find the highest ranked cards in hand
    vector<Card> GetHighestRankedCards(const vector<Card>&) const;
    
    //to find the lowest ranked cards in hand
    vector<Card> GetLowestRankedCards(const vector<Card>&) const;
    
    //to find the highest card of a specified suit
    vector<Card> GetHigherCards(const Card&, const vector<Card>&) const;
    
    //to find the lowest card of a specified suit
    vector<Card> GetLowerCards(const Card&, const vector<Card>&) const;
    
    //to find all the cards of trump suit
    vector<Card> GetTrumpSuitCards(const vector<Card>&, const Suit&) const;
    
    //to find all the cards that their suit is not the trump suit
    vector<Card> GetNonTrumpSuitCards(const vector<Card>&, const Suit&) const;
    
    //to find all the kings that their Ace has been played
    vector<Card> GetKingsPlayed(const vector<Card>&) const;
    
    //to find all the queens that their Ace and King has been played
    vector<Card> GetQueensPlayed(const vector<Card>&) const;
    
    //to find all cards of a specified rank
    vector<Card> GetCardsInRank(const vector<Card>&, const int&) const;
    
    //to find all cards of a specified suit
    vector<Card> GetCardsInSuit(const vector<Card>&, const Suit&) const;
    
    //to find if there is any cards of type suit led in hand
    bool HasSuitLedCards(const Suit&) const;
    
    //to find the total number of cards of a specific suit in hand
    int GetNumCardsInSuit(const Suit&) const;
    
    //to identify the suits of a collection of cards
    vector<Suit> GetSuits(const vector<Card>&) const;
    
    //to select a card from the Face Cards including Ace
    Card PickFromFaceCards(const vector<Card>&, const Suit&) const;
    
    //to discard the 2 highest ranked cards from hand
    void DropTwoHighest(vector<Card>);
    
    //to discard the 2 lowest ranked cards from hand
    void DropTwoLowest(vector<Card>);
};
