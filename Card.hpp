//Card class --> Used to represent a card in the game

#pragma once

class Card
{

public:
    
    //*** constructors ***//
    Card(): m_suit(spades), m_rank(ace){}; //default card
    Card(const Suit& a_suit, const int& a_rank): m_suit(a_suit), m_rank(a_rank){};
    Card(const Card& a_card): m_suit(a_card.m_suit), m_rank(a_card.m_rank){};
    
    //*** destrcutor ***//
    ~Card(){/*not required*/};
    
    //*** getters ***//
    Suit GetSuit() const {return m_suit;}
    int GetRank() const {return m_rank;}
    
    //*** other functions ***//
    
    //checks whether a string card input is valid
    static bool CardIsValid(string);
    
    //converts a valid string card input into a card object
    static Card ParseCard(string);
    
    //*** overloaded operators ***//
    bool operator == (const Card&) const;
    bool operator < (const Card&) const;
    
private:
    
    //the suit of the card
    Suit m_suit;
    
    //the rank of the card
    int m_rank;
};
