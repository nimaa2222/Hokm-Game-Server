//Trick class --> holds together all components of a trick in a round

#pragma once


class Trick
{

public:
    
    //*** constructors ***//
    Trick(): m_suitLed(spades), m_tableCards({}), m_winner(NULL) {};
    Trick(const Trick& a_trick): m_suitLed(a_trick.m_suitLed), m_tableCards(a_trick.m_tableCards), m_winner(a_trick.m_winner) {};
    
    //*** destrcutor ***//
    ~Trick(){/*not required*/};
    
    //*** getters ***//
    Suit GetSuitLed() const {return m_suitLed;}
    vector<Card> GetTableCards() const;
    Player* GetWinner() const {return m_winner;}
    Turn GetTurn() const {return (Turn)m_tableCards.size();}
    
    //*** other functions ***//
    
    //checks whether the trick is over and if not prompts next player to play a card
    void PromptPlToPlay(vector<Player*>&, Player*, const vector<int>&, const vector<int>&,
                        const int&, const Suit&, int);
    
    //to place a card on the table
    void PlaceCard(vector<Player*>&, const Card&, const int&);
    
    //called when human inputs a card to play (prompts next player to play afterwards)
    void CardSelected (vector<Player*>&, Player*, const vector<int>&, const vector<int>&,
                       const int&, const Suit&, Player*, const Card&);
    
    //identifies all possible playable cards during a turn
    vector<Card> IdentifyPlayableCards(const vector<Card>&);
    
    
private:
    
    //the suit of the first played card of the trick
    Suit m_suitLed;
    
    //the card and player that played the card
    map<Card, Player*> m_tableCards;
    
    //the winner of this trick
    Player* m_winner;
    

    //***  private functions ***//
    
    Player* IdentifyWinner(const Suit&);
};
