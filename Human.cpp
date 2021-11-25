#include "stdafx.h"

/**/
/*
 Human::SelectTrumpSuit Human::SelectTrumpSuit

NAME

        SelectTrumpSuit - to prompt human player to select the trump suit

SYNOPSIS

        Suit Human::SelectTrumpSuit() const;
            

DESCRIPTION

        This function serves client with a message asking them to choose the
        the trump suit for the round after displaying their hands to them.

RETURNS

        The default suit. Trump suit will be set later on once the user
        responds to the server with their selection.
 
*/
/**/

Suit Human::SelectTrumpSuit() const
{
    //serve the client with the message
    SendMsg(Display::GetTrumpSuitPrompt(m_hand));
    
    //default suit
    return spades;
}
/*Suit Human::SelectTrumpSuit() const;*/


/**/
/*
 Human::DiscardCards Human::DiscardCards

NAME

        DiscardCards - to prompt human player to discard cards from their hand

SYNOPSIS

        void Human::DiscardCards(const Suit& a_trumpSuit);
            a_trumpSuit --> the declared trump suit for the round
            

DESCRIPTION

        This function serves client with a message asking them to discard 2 cards out
        of the 5 cards in their hand after displaying their hand to them.

RETURNS

        None
 
*/
/**/

void Human::DiscardCards(const Suit& a_trumpSuit)
{
    //the client must now respond by specifying the 2 cards to be discarded from hand
    m_stage = mustDiscardTwoCards;
    
    //the message to be sent to the client
    string display = "";
    
    //the position on screen to display the player's hand
    int position = 0;
    
    //the remaining components of the message
    display += Display::GetDiscardTwoCardsPrompt(a_trumpSuit);
    display += Display::GetHandDisplay(m_hand, position, (int)m_hand.size(), false) + "\n";
    display += Display::GetCardSelectionPrompt();
    display += Display::GetNewLines(3);
    
    //serve the client with the message
    SendMsg(display);
}
/*void Human::DiscardCards(const Suit& a_trumpSuit);*/


/**/
/*
 Human::PickOrDiscard Human::PickOrDiscard

NAME

        PickOrDiscard - to prompt human player to either pick or discard a given card

SYNOPSIS

        Decision Human::PickOrDiscard(const Card& a_card, const Suit& a_trumpSuit) const;
            a_card      --> the card to be picked or discarded
            a_trumpSuit --> the declared trump suit for the round
            

DESCRIPTION

        This function displays a card to the client and asks them whether they would like
        to add the card to their hand or discard the card.

RETURNS

        A default decision. The decision will be set later on once the user
        responds to the server with their selection.
 
*/
/**/

Decision Human::PickOrDiscard(const Card& a_card, const Suit& a_trumpSuit) const
{
    //prompt player to pick or discard the card
    string msg = Display::GetPickOrDiscardPrompt(a_card);
    SendMsg(msg);
    
    //defult decision
    return yes;
}
/*Decision Human::PickOrDiscard(const Card& a_card, const Suit& a_trumpSuit) const;*/


/**/
/*
 Human::SendMsg Human::SendMsg

NAME

        SendMsg - to send a message to the human player

SYNOPSIS

        bool Human::SendMsg(const string& a_msg) const;
            a_msg --> the message to be sent to the client
            

DESCRIPTION

        This function sends a message to the human player through
        their socket.

RETURNS

        True if message was sent successfully
        False otherwise
 
*/
/**/

bool Human::SendMsg(const string& a_msg) const
{
    return Server::SendMsg(m_soc, a_msg);
}
/*bool Human::SendMsg(const string& a_msg) const;*/
