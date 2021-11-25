//Deck class --> Used to represent a standard game deck

#pragma once

class Deck
{

public:
    
    //*** constructors ***//
    Deck();
    Deck(const Deck& a_deck): m_cards(a_deck.m_cards){};
    
    //*** destrcutor ***//
    ~Deck(){/*not required*/};
    
    //*** getters and setters ***//
    int GetStandardSize() const {return m_size;}
    int GetCurrentSize() const {return (int)m_cards.size();}
    Card GetCard(const int& a_index) const {return m_cards[a_index];}
    Card GetTopDeck() const {return m_cards[0];}
    
    //*** other functions ***//
    
    //swaps the cards in the deck
    void Shuffle();
    
    void RemoveCardByIndex(const int& a_index){m_cards.erase(m_cards.begin() + a_index);}
    void RemoveCard(const Card& a_card){m_cards.erase(remove(m_cards.begin(), m_cards.end(), a_card), m_cards.end());}
    
private:
    
    //the number of cards in a standard deck
    static const int m_size = 52;
    
    //the current cards in the deck
    vector<Card> m_cards;
};
