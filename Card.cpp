#include "stdafx.h"

/**/
/*
 Card::CardIsValid Card::CardIsValid

NAME

        CardIsValid - checks whether a card input is valid

SYNOPSIS

        bool Card::CardIsValid(string a_input);
            a_input --> the user input for a card
            

DESCRIPTION

        This function will check whether the user input matches any
        card in a standard deck of 52 cards.

RETURNS

        True if card input is valid
        False otherwise
 
*/
/**/

bool Card::CardIsValid(string a_input)
{
    //one character for the suit and one character for the rank
    if (a_input.size() != 2)
        return false;
    
    //capitalize the input
    for(auto& i: a_input)
        i = toupper(i);
    
    //identify the rank and suit
    char rankInput = a_input[0];
    char suitInput = a_input[1];
    
    //check the suit input
    if (suitChar.find(suitInput) == suitChar.end())
        return false;
        
    //if a symbolic rank is entered
    if (!isdigit(rankInput))
    {
        //check if a valid symbol is entered
        bool found = false;
        for (auto elem: rankChar)
            if (elem.second == rankInput)
                found = true;
                
        if (!found)
            return false;
    }
        
    //if a numeric rank is entered
    else
    {
        //convert char to int
        int numVal = rankInput - '0';
        
        //check if rank is 1-9
        if (numVal < 1 || numVal > 9)
            return false;
    }
    
    return true;
}
/*bool Card::CardIsValid(string a_input);*/


/**/
/*
 Card::ParseCard Card::ParseCard

NAME

        ParseCard - converts a valid string card input to a card object

SYNOPSIS

        Card Card::ParseCard(string a_card);
            a_input --> the user input for a card
            

DESCRIPTION

        This function constructs a card object based on the specified
        string card input.

RETURNS

        A parsed card object
 
*/
/**/

Card Card::ParseCard(string a_card)
{
    //capitalize the input
    for(auto& i: a_card)
        i = toupper(i);
    
    //identify the rank and suit
    char rankInput = a_card[0];
    char suitInput = a_card[1];
    
    //look up the suit
    Suit cardSuit = suitChar[suitInput];
    
    //will hold the rank
    int cardRank = 0;
    
    //if a symbolic rank is entered
    if (!isdigit(rankInput))
    {
        for (auto elem: rankChar)
            if (elem.second == rankInput)
                cardRank = elem.first;
    }
    
    //if a numeric rank is entered
    else
    {
        //convert char to int
        cardRank = rankInput - '0';
    }
        
    return Card(cardSuit, cardRank);
}
/*Card Card::ParseCard(string a_card);*/


bool Card::operator == (const Card& a_card) const
{
    //if the suit and rank match
    if (m_suit == a_card.m_suit && m_rank == a_card.m_rank)
        return true;
    
    return false;
}


//used to sort player cards based on suit and rank
bool Card::operator < (const Card& a_card) const
{
    if (m_suit < a_card.m_suit)
        return true;
    
    if (m_suit == a_card.m_suit)
    {
        if (m_rank < a_card.m_rank)
            return true;
    }
    
    return false;
}
