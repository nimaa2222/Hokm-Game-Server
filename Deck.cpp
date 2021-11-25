#include "stdafx.h"

//generates a standard deck
Deck::Deck()
{
    //for each suit in the deck
    for (int i=0; i<4; i++)
    {
        //generate the 13 set of cards (2 - Ace)
        for (int j=2; j<15; j++)
        {
            Card newCard((Suit) i, j);
            m_cards.push_back(newCard);
        }
    }
}


/**/
/*
 Deck::Shuffle Deck::Shuffle

NAME

        Shuffle - to shuffle the game deck

SYNOPSIS

        void Deck::Shuffle();
            

DESCRIPTION

        This function shuffles the game deck by randomly swapping
        the cards in the deck.

RETURNS

        None
 
*/
/**/

void Deck::Shuffle()
{
    //to give each card a chance to be swapped
    for (auto elem: m_cards)
    {
        //swap the 2 cards based on 2 random numbers in range 0 to size of deck - 1
        swap(m_cards[rand() % m_cards.size()], m_cards[rand() % m_cards.size()]);
    }
}
/*void Deck::Shuffle();*/
