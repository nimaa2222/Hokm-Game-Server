#include "stdafx.h"


/**/
/*
 Computer::SelectTrumpSuit Computer::SelectTrumpSuit

NAME

        SelectTrumpSuit - to prompt computer player to select the trump suit

SYNOPSIS

        Suit Computer::SelectTrumpSuit() const;
            

DESCRIPTION

        This function, based on the computer's hand and computer's intelligence
        level chooses one of the suits as the trump suit for the round.

RETURNS

        The selected trump suit
 
*/
/**/

Suit Computer::SelectTrumpSuit() const
{
    //identify all the suits found in hand
    vector<Suit> cardSuits = GetSuits(m_hand);
    
    if (m_intelligence == low || m_intelligence == average)
    {
        //select the suit with the least number of matching cards in hand
        return GetLeastCommonSuit(cardSuits);
    }
        
    //select the suit with the most number of matching cards in hand
    return GetMostCommonSuit(cardSuits);
}
/*Suit Computer::SelectTrumpSuit() const;*/


/**/
/*
 Computer::DiscardCards Computer::DiscardCards

NAME

        DiscardCards - to prompt computer player to discard 2 cards from hand

SYNOPSIS

        void Computer::DiscardCards(const Suit& a_trumpSuit);
            a_trumpSuit --> the declared trump suit for the round
            

DESCRIPTION

        This function, based on the computer's hand, computer's intelligence
        level, and the trump suit, discards 2 out of the 5 cards in hand.

RETURNS

        None
 
*/
/**/

void Computer::DiscardCards(const Suit& a_trumpSuit)
{
    if (m_intelligence == low)
    {
        //identify all cards that are not of type trump suit
        vector<Card> nonTrumpSuitCards = GetNonTrumpSuitCards(m_hand, a_trumpSuit);
        
        //if there is no non-trump-suit cards (all trump suits)
        if (nonTrumpSuitCards.empty())
        {
            //drop the 2 highest trump suit cards
            RemoveFromHand(*max_element(m_hand.begin(), m_hand.end()));
            RemoveFromHand(*max_element(m_hand.begin(), m_hand.end()));
        }
        
        //if there is 3 or 4 trump suit cards
        else if (nonTrumpSuitCards.size() == 1 || nonTrumpSuitCards.size() == 2)
        {
            //get rid of the non-trump-suit card
            vector<Card> trumpSuitCards = GetTrumpSuitCards(m_hand, a_trumpSuit);
            
            //drop the 2 highest ranked trump suit cards
            DropTwoHighest(trumpSuitCards);
        }
        
        //if there is 2 trump suit cards
        else if (nonTrumpSuitCards.size() == 3)
        {
            //identify all trump suit cards
            vector<Card> trumpSuitCards = GetTrumpSuitCards(m_hand, a_trumpSuit);
            
            //drop the 2 trump suit cards
            RemoveFromHand(trumpSuitCards[0]);
            RemoveFromHand(trumpSuitCards[1]);
        }
        
        //if there is 1 trump suit card
        else if (nonTrumpSuitCards.size() == 4)
        {
            //idenitfy the trump suit card
            vector<Card> trumpSuitCards = GetTrumpSuitCards(m_hand, a_trumpSuit);
            
            //drop the trump suit card
            RemoveFromHand(trumpSuitCards[0]);
            
            //identify the card(s) with the highest rank in hand
            vector<Card> highestCards = GetHighestRankedCards(m_hand);
            
            //drop the highest card of the remaining cards
            RemoveFromHand(highestCards[0]);
        }
        
        //if there is no trump suit cards
        else
        {
            //drop the 2 highest cards
            DropTwoHighest(m_hand);
        }
    }
    
    else if (m_intelligence == average)
    {
        //identify all cards that are not of type trump suit
        vector<Card> nonTrumpSuitCards = GetNonTrumpSuitCards(m_hand, a_trumpSuit);
        
        //if there is no non-trump-suit cards (all trump suits)
        if (nonTrumpSuitCards.empty())
        {
            //drop the highest ranked trump suit card
            RemoveFromHand(*max_element(m_hand.begin(), m_hand.end()));
            
            //drop the lowest ranked trump suit card
            RemoveFromHand(*min_element(m_hand.begin(), m_hand.end()));
        }
        
        //if there is 4 trump suit cards
        else if (nonTrumpSuitCards.size() == 1)
        {
            //drop the non-trump-suit card
            RemoveFromHand(nonTrumpSuitCards[0]);
            
            //drop the highest ranked trump suit card
            RemoveFromHand(*max_element(m_hand.begin(), m_hand.end()));
        }
        
        //if there is 3 trump suit cards
        else if (nonTrumpSuitCards.size() == 2)
        {
            //drop a non-trump suit card
            RemoveFromHand(nonTrumpSuitCards[0]);
            
            //drop the highest ranked trump suit card
            vector<Card> trumpSuitCards = GetTrumpSuitCards(m_hand, a_trumpSuit);
            RemoveFromHand(*max_element(trumpSuitCards.begin(), trumpSuitCards.end()));
        }
        
        //2 trump suits
        else if (nonTrumpSuitCards.size() == 3)
        {
            //drop 2 non-trump suit cards
            RemoveFromHand(nonTrumpSuitCards[0]);
            RemoveFromHand(nonTrumpSuitCards[1]);
        }
        
        //if there is 1 or 0 trump suit cards
        else
        {
            //drop the highest ranked non-trump-suit card
            vector<Card> higehestCards = GetHighestRankedCards(nonTrumpSuitCards);
            RemoveFromHand(higehestCards[0]);
            
            //drop the lowest ranked non-trump-suit card
            vector<Card> lowestCards = GetLowestRankedCards(nonTrumpSuitCards);
            RemoveFromHand(lowestCards[0]);
        }
    }
    
    //if intelligence level is high
    else
    {
        //identify all cards that are not of type trump suit
        vector<Card> nonTrumpSuitCards = GetNonTrumpSuitCards(m_hand, a_trumpSuit);
        
        //if there is no non-trump-suit cards (all trump suits)
        if (nonTrumpSuitCards.empty())
        {
            //drop the 2 lowest ranked trump suit cards
            RemoveFromHand(*min_element(m_hand.begin(), m_hand.end()));
            RemoveFromHand(*min_element(m_hand.begin(), m_hand.end()));
        }
        
        //if there is 4 trump suit cards
        else if (nonTrumpSuitCards.size() == 1)
        {
            //drop the non-trump-suit card
            RemoveFromHand(nonTrumpSuitCards[0]);
            
            //drop the lowest ranked trump suit card
            RemoveFromHand(*min_element(m_hand.begin(), m_hand.end()));
        }
        
        //if there is 3 trump suit cards
        else if (nonTrumpSuitCards.size() == 2)
        {
            //drop the 2 non-trump-suit cards
            RemoveFromHand(nonTrumpSuitCards[0]);
            RemoveFromHand(nonTrumpSuitCards[1]);
        }
        
        //if there is 2, 1 or 0 trump suit card(s)
        else
        {
            //drop the 2 lowest ranked non-trump-suit cards
            DropTwoLowest(nonTrumpSuitCards);
        }
    }
}
/*void Computer::DiscardCards(const Suit& a_trumpSuit);*/


/**/
/*
 Computer::PickOrDiscard Computer::PickOrDiscard

NAME

        DiscardCards - to prompt computer player to pick or discard a card from hand

SYNOPSIS

        Decision Computer::PickOrDiscard(const Card& a_card, const Suit& a_trumpSuit) const;
            a_card      --> the card to be picked or discarded
            a_trumpSuit --> the declared trump suit for the round
            

DESCRIPTION

        This function, based on the computer's intelligence level and the trump suit
        decides to pick or discard a given card.

RETURNS

        The computer's decision (an enum) as to pick or discard the card
 
*/
/**/

Decision Computer::PickOrDiscard(const Card& a_card, const Suit& a_trumpSuit) const
{
    if (m_intelligence == low)
    {
        //if the card is of trump suit or a Face card - DO NOT PICK
        if (a_card.GetSuit() == a_trumpSuit || a_card.GetRank() == ace ||
            a_card.GetRank() == king || a_card.GetRank() == queen ||
            a_card.GetRank() == jack)
        {
            return no;
        }
        
        return yes;
        
    }
    
    else if (m_intelligence == average)
    {
        //if the card is a trump suit
        if (a_card.GetRank() == ace || a_card.GetRank() == king ||
            a_card.GetRank() == queen || a_card.GetRank() == jack)
        {
            return yes;
        }
        
        return no;
    }
    
    //high intelligence
    else
    {
        //if the card is of trump suit or a Face card (excluding jack)
        if (a_card.GetSuit() == a_trumpSuit || a_card.GetRank() == ace ||
            a_card.GetRank() == king || a_card.GetRank() == queen)
        {
            return yes;
        }
        
        return no;
    }
}
/*Decision Computer::PickOrDiscard(const Card& a_card, const Suit& a_trumpSuit) const;*/


/**/
/*
 Computer::SelectCard Computer::SelectCard

NAME

        SelectCard - to prompt computer player to select a card from hand

SYNOPSIS

        Card Computer::SelectCard(const Mode& a_mode, const Turn& a_turn,
        const Suit& a_trumpSuit, const Suit& a_suitLed, const vector<Card>& a_tableCards) const;
    
            a_mode       --> the mode of the current match (2, 3, or 4 player)
            a_turn       --> the current player's turn (first, second, third, fourth)
            a_trumpSuit  --> the declared trump suit for the round
            a_suitLed    --> the suit of the first played card in this trick
            a_tableCards --> the cards that have been played so far in the trick
            

DESCRIPTION

        This function, based on the computer's intelligence level and other variables
        picks a card to play from hand during a trick.

RETURNS

        The card selected by computer to be played
 
*/
/**/


Card Computer::SelectCard(const Mode& a_mode, const Turn& a_turn, const Suit& a_trumpSuit, const Suit& a_suitLed,
                          const vector<Card>& a_tableCards) const
{
    //the computer's selection
    Card selectedCard;
    
    //2-player selections
    
    if (m_intelligence == low && a_mode == twoPlayer)
    {
        //if the player is the first player playing a card
        if (a_turn  == first)
            selectedCard = PickLowTwoPlFirst(a_trumpSuit);
        else
        {
            //play the appropriate card based on the turn, match mode, and intelligence level (applies to all functions below)
            selectedCard = PickLowTwoPlSecond(a_trumpSuit, a_suitLed, a_tableCards);
        }
    }
    
    else if (m_intelligence == average && a_mode == twoPlayer)
    {
        if (a_turn  == first)
            selectedCard = PickAverageTwoPlFirst();
        else
            selectedCard = PickAverageTwoPlSecond(a_trumpSuit, a_suitLed, a_tableCards);
    }
    
    else if (m_intelligence == high && a_mode == twoPlayer)
    {
        if (a_turn  == first)
            selectedCard = PickHighTwoPlFirst(a_trumpSuit);
        else
            selectedCard = PickHighTwoPlSecond(a_trumpSuit, a_suitLed, a_tableCards);
    }
    
    //3-player selections
    else if (m_intelligence == low && a_mode == threePlayer)
    {
        if (a_turn  == first)
            selectedCard = selectedCard = PickLowTwoPlFirst(a_trumpSuit);
        else if (a_turn  == second)
            selectedCard = PickLowThreePlSecond(a_trumpSuit, a_suitLed, a_tableCards);
        else
            selectedCard = PickLowTwoPlSecond(a_trumpSuit, a_suitLed, a_tableCards);
    }
    
    else if (m_intelligence == average && a_mode == threePlayer)
    {
        if (a_turn  == first)
            selectedCard = PickAverageTwoPlFirst();
        else if (a_turn  == second)
            selectedCard = PickAverageThreePlSecond(a_trumpSuit, a_suitLed, a_tableCards);
        else
            selectedCard = PickAverageTwoPlSecond(a_trumpSuit, a_suitLed, a_tableCards);
    }
    
    else if (m_intelligence == high && a_mode == threePlayer)
    {
        if (a_turn  == first)
            selectedCard = PickHighTwoPlFirst(a_trumpSuit);
        else if (a_turn  == second)
            selectedCard = PickHighThreePlSecond(a_trumpSuit, a_suitLed, a_tableCards);
        else
            selectedCard = PickHighTwoPlSecond(a_trumpSuit, a_suitLed, a_tableCards);
    }
    
    //4-player selections
    else if (m_intelligence == low && a_mode == fourPlayer)
    {
        if (a_turn  == first)
            selectedCard = PickLowTwoPlFirst(a_trumpSuit);
        else if (a_turn  == second)
            selectedCard = PickLowThreePlSecond(a_trumpSuit, a_suitLed, a_tableCards);
        else if (a_turn  == third)
            selectedCard = PickLowThreePlSecond(a_trumpSuit, a_suitLed, a_tableCards);
        else
            selectedCard = PickLowTwoPlSecond(a_trumpSuit, a_suitLed, a_tableCards);
    }
    
    else if (m_intelligence == average && a_mode == fourPlayer)
    {
        if (a_turn  == first)
            selectedCard = PickAverageTwoPlFirst();
        else if (a_turn  == second)
            selectedCard = PickAverageThreePlSecond(a_trumpSuit, a_suitLed, a_tableCards);
        else if (a_turn  == third)
            selectedCard = PickAverageThreePlSecond(a_trumpSuit, a_suitLed, a_tableCards);
        else
            selectedCard = PickAverageTwoPlSecond(a_trumpSuit, a_suitLed, a_tableCards);
    }
    
    else if (m_intelligence == high && a_mode == fourPlayer)
    {
        if (a_turn  == first)
            selectedCard = PickHighTwoPlFirst(a_trumpSuit);
        else if (a_turn  == second)
            selectedCard = PickHighThreePlSecond(a_trumpSuit, a_suitLed, a_tableCards);
        else if (a_turn  == third)
            selectedCard = PickHighThreePlSecond(a_trumpSuit, a_suitLed, a_tableCards);
        else
            selectedCard = PickHighTwoPlSecond(a_trumpSuit, a_suitLed, a_tableCards);
    }
    
    return selectedCard;
}
/*Card Computer::SelectCard(const Mode& a_mode, const Turn& a_turn,
 const Suit& a_trumpSuit, const Suit& a_suitLed, const vector<Card>& a_tableCards) const;*/


/**/
/*
 Computer::PickHighTwoPlFirst Computer::PickHighTwoPlFirst

NAME

        PickHighTwoPlFirst - to select a card as the first player in 2-player mode (high intelligence)

SYNOPSIS

        Card Computer::PickHighTwoPlFirst(const Suit& a_trumpSuit) const;
            a_trumpSuit  --> the declared trump suit for the round
            

DESCRIPTION

        This function allows a computer player to select a card as the first player
        of the turn in a 2-player mode match. This computer has a high intelligence level.

RETURNS

        The card selected by computer to be played
 
*/
/**/


Card Computer::PickHighTwoPlFirst(const Suit& a_trumpSuit) const
{
    //computer's selection
    Card selectedCard;
    
    //identify all aces in hand
    vector<Card> aces = GetCardsInRank(m_hand, ace);
    
    //identify all kings in hand
    vector<Card> kings = GetCardsInRank(m_hand, king);
    
    //identify all queens in hand
    vector<Card> queens = GetCardsInRank(m_hand, queen);
    
    //identify kings that their ace has been played
    vector<Card> kingsPlayed = GetKingsPlayed(kings);
    
    //identify the queens that their ace and king has been played
    vector<Card> queensPlayed = GetQueensPlayed(queens);
    
    //if there is any aces
    if (!aces.empty())
    {
        //select the appropriate Face Card including ace
        selectedCard = PickFromFaceCards(aces, a_trumpSuit);
    }
    
    //if there is any kings that their ace is played
    else if (!kingsPlayed.empty())
    {
        //select the appropriate Face Card including ace
        selectedCard = PickFromFaceCards(kingsPlayed, a_trumpSuit);
    }
        
    //if there is any queens that their ace and king is played
    else if (!queensPlayed.empty())
    {
        //select the appropriate Face Card including ace
        selectedCard = PickFromFaceCards(kingsPlayed, a_trumpSuit);
        selectedCard = PickFromFaceCards(queensPlayed, a_trumpSuit);
    }
    
    //if there are no Face Cards including ace
    else
    {
        //find the card(s) of the suit with the least number of cards in hand
        
        //identify the suit(s) of the cards in hand
        vector<Suit> suits = GetSuits(m_hand);
        
        //identify the least common suit
        Suit leastCardSuit = GetLeastCommonSuit(suits);
        
        //identify all cards with the least common suit
        vector<Card> leastCommonSuitCards = GetCardsInSuit(m_hand, leastCardSuit);
        
        //pick the highest ranked card of the least common suit
        selectedCard = *max_element(leastCommonSuitCards.begin(), leastCommonSuitCards.end());
    }
    
    return selectedCard;
}
/*Card Computer::PickHighTwoPlFirst(const Suit& a_trumpSuit) const;*/


/**/
/*
 Computer::PickHighTwoPlSecond Computer::PickHighTwoPlSecond

NAME

        PickHighTwoPlSecond - to select a card as the second player in 2-player mode (high intelligence)

SYNOPSIS

        Card Computer::PickHighTwoPlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed,
        const vector<Card>& a_tableCards) const;
 
            a_trumpSuit  --> the declared trump suit for the round
            a_suitLed    --> the suit of the first played card in this trick
            a_tableCards --> the cards played so far in the trick
            

DESCRIPTION

        This function allows a computer player to select a card as the second player
        of the turn in a 2-player mode match. This computer has a high intelligence level.

RETURNS

        The card selected by computer to be played
 
*/
/**/

Card Computer::PickHighTwoPlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed, const vector<Card>& a_tableCards) const
{
    //the computer's selection
    Card selectedCard;
    
    //if there is cards in the suit led in hand
    if (HasSuitLedCards(a_suitLed))
    {
        //identify all the suit led cards in hand
        vector<Card> suitLedCards = GetCardsInSuit(m_hand, a_suitLed);
        
        //if there is only one suit led card in hand
        if (suitLedCards.size() == 1)
        {
            //select this one card
            selectedCard = suitLedCards[0];
        }
        
        //if there is multiple suit led cards in hand
        else
        {
            //identify all the played cards of type suit led
            vector<Card> suitLedTableCards = GetCardsInSuit(a_tableCards, a_suitLed);
            
            //identify highest ranked card of table cards of type suit led
            Card maxTableCard = *max_element(suitLedTableCards.begin(), suitLedTableCards.end());
            
            //check if there is any higher ranked card(s) than the highest ranked suit led table card in hand
            vector<Card> higherCards = GetHigherCards(maxTableCard, suitLedCards);
            
            //if there is any higher cards than the table card
            if (!higherCards.empty())
            {
                //play the first card after the table card
                selectedCard = higherCards[0];
            }
            
            //if there is multiple lower ranked cards than the table card
            else
            {
                //pick the lowest card card of the suit led
                selectedCard = *min_element(suitLedCards.begin(), suitLedCards.end());
            }
        }
    }
    
    //if there is no cards in the suit led
    else
    {
        //check if there is any trump suit cards in hand
        vector<Card> trumpSuitCards = GetTrumpSuitCards(m_hand, a_trumpSuit);
        
        //if there is any trump-suit cards
        if (!trumpSuitCards.empty())
        {
            //if there is only one trump suit card
            if (trumpSuitCards.size() == 1)
            {
                //select this one trump suit card
                selectedCard = trumpSuitCards[0];
            }
            
            //if there is multiple trump suit cards
            else
            {
                //pick the smallest trump suit card
                selectedCard = trumpSuitCards[0];
            }
        }
        
        //if there is no trump suit cards
        else
        {
            //play the smallest ranked card with the suit of the least number of cards in hand
            vector<Card> smallestCards = GetLowestRankedCards(m_hand);
            selectedCard = PickLeastCommonSuitCard(smallestCards);
        }
    }
    
    return selectedCard;
}
/*Card Computer::PickHighTwoPlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed,
 const vector<Card>& a_tableCards) const;*/


/**/
/*
 Computer::PickAverageTwoPlFirst Computer::PickAverageTwoPlFirst

NAME

        PickAverageTwoPlFirst - to select a card as the first player in 2-player mode (average intelligence)

SYNOPSIS

        Card Computer::PickAverageTwoPlFirst() const;

DESCRIPTION

        This function allows a computer player to select a card as the first player
        of the turn in a 2-player mode match. This computer has an average intelligence level.

RETURNS

        The card selected by computer to be played
 
*/
/**/


Card Computer::PickAverageTwoPlFirst() const
{
    //the computer's selection
    Card selectedCard;
        
    //identify all the suit(s) of the cards
    vector<Suit> suits = GetSuits(m_hand);
        
    //identify the most common suit
    Suit mostCommonSuit = GetMostCommonSuit(suits);
        
    //identify all the cards with the most common suit
    vector<Card> mostCommonSuitCards = GetCardsInSuit(m_hand, mostCommonSuit);
        
    //pick the lowest ranked card of the most common suit
    selectedCard = *min_element(mostCommonSuitCards.begin(), mostCommonSuitCards.end());
    
    return selectedCard;
}
/*Card Computer::PickAverageTwoPlFirst() const;*/


/**/
/*
 Computer::PickAverageTwoPlSecond Computer::PickAverageTwoPlSecond

NAME

        PickAverageTwoPlSecond - to select a card as the second player in 2-player mode (average intelligence)

SYNOPSIS

        Card Computer::PickAverageTwoPlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed,
        const vector<Card>& a_tableCards) const;
 
             a_trumpSuit  --> the declared trump suit for the round
             a_suitLed    --> the suit of the first played card in this trick
             a_tableCards --> the cards played so far in the trick

DESCRIPTION

        This function allows a computer player to select a card as the second player
        of the turn in a 2-player mode match. This computer has an average intelligence level.

RETURNS

        The card selected by computer to be played
 
*/
/**/


Card Computer::PickAverageTwoPlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed,
                                      const vector<Card>& a_tableCards) const
{
    //the computer's selection
    Card selectedCard;
    
    //if there is cards in the suit led in hand
    if (HasSuitLedCards(a_suitLed))
    {
        //identify all the suit led cards in hand
        vector<Card> suitLedCards = GetCardsInSuit(m_hand, a_suitLed);
        
        //if there is only one suit led card in hand
        if (suitLedCards.size() == 1)
        {
            //select this one suit led card
            selectedCard = suitLedCards[0];
        }
            
        //if there is multiple suit led cards in hand
        else
        {
            //identify all the suit led cards played
            vector<Card> suitLedTableCards = GetCardsInSuit(a_tableCards, a_suitLed);
            
            //identify the highest ranked suit led card played
            Card maxTableCard = *max_element(suitLedTableCards.begin(), suitLedTableCards.end());
            
            //check if there is any card(s) higher than the highest ranked table card in hand
            vector<Card> higherCards = GetHigherCards(maxTableCard, suitLedCards);
            
            //if there is any higher ranked cards than the table card
            if (!higherCards.empty())
            {
                //if there is only one higher ranked card than the table card
                if (higherCards.size() == 1)
                {
                    //pick this one card
                    selectedCard = higherCards[0];
                }
                
                //if there is multiple higher ranked cards than the table card
                else
                {
                    //pick the highest ranked card
                    selectedCard = *max_element(higherCards.begin(), higherCards.end());
                }
            }
            
            //if there is multiple lower ranked cards than the table card
            else
            {
                //pick the highest ranked lower card
                selectedCard = *max_element(suitLedCards.begin(), suitLedCards.end());
            }
        }
    }
    
    //if there is no cards in the suit led
    else
    {
        //check if there is any trump suit cards in hand
        vector<Card> trumpSuitCards = GetTrumpSuitCards(m_hand, a_trumpSuit);
        
        //if there is any trump-suit cards
        if (!trumpSuitCards.empty())
        {
            //if there is only one trump suit card
            if (trumpSuitCards.size() == 1)
            {
                //pick this one card
                selectedCard = trumpSuitCards[0];
            }
            
            //if there is multiple trump suit cards
            else
            {
                //pick the highest ranked trump suit card
                selectedCard = *max_element(trumpSuitCards.begin(), trumpSuitCards.end());
            }
        }
        
        //if there is no trump suit cards
        else
        {
            //play the smallest ranked card in hand
            vector<Card> smallestCards = GetLowestRankedCards(m_hand);
            selectedCard =  smallestCards[0];
        }
    }
    
    return selectedCard;
}
/*Card Computer::PickAverageTwoPlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed,
 const vector<Card>& a_tableCards) const;*/


/**/
/*
 Computer::PickLowTwoPlFirst Computer::PickLowTwoPlFirst

NAME

        PickLowTwoPlFirst - to select a card as the first player in 2-player mode (low intelligence)

SYNOPSIS

        Card Computer::PickLowTwoPlFirst(const Suit& a_trumpSuit) const;
             a_trumpSuit  --> the declared trump suit for the round
 
DESCRIPTION

        This function allows a computer player to select a card as the first player
        of the turn in a 2-player mode match. This computer has a low intelligence level.

RETURNS

        The card selected by computer to be played
 
*/
/**/

Card Computer::PickLowTwoPlFirst(const Suit& a_trumpSuit) const
{
    //the computer's selection
    Card selectedCard;
    
    //idenitfy all cards that are not of type trump suit
    vector<Card> nonTrumpSuitCards = GetNonTrumpSuitCards(m_hand, a_trumpSuit);
    
    //if there is any non-trump-suit cards
    if (!nonTrumpSuitCards.empty())
    {
        //identify the smallest ranked card(s) that are not of type trump suit
        vector<Card> smallestCards = GetLowestRankedCards(nonTrumpSuitCards);
        
        //if there is only one card with the smallest rank
        if (smallestCards.size() == 1)
        {
            //pick this one card
            selectedCard = smallestCards[0];
        }
        
        //if there is more than one card with the smallest rank
        else
        {
            //pick the card of the suit with the more number of cards in hand
            selectedCard = PickMostCommonSuitCard(smallestCards);
        }
    }
    
    //if there is only trump-suit cards in hand
    else
    {
        //pick the smallest trump suit card
        selectedCard = *min_element(m_hand.begin(), m_hand.end());
    }
    
    return selectedCard;
    
}
/*Card Computer::PickLowTwoPlFirst(const Suit& a_trumpSuit) const;*/


/**/
/*
 Computer::PickLowTwoPlSecond Computer::PickLowTwoPlSecond

NAME

        PickLowTwoPlSecond - to select a card as the second player in 2-player mode (low intelligence)

SYNOPSIS

        Card Computer::PickLowTwoPlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed,
        const vector<Card>& a_tableCards) const;
 
             a_trumpSuit  --> the declared trump suit for the round
             a_suitLed    --> the suit of the first played card in this trick
             a_tableCards --> the cards played so far in the trick
 
DESCRIPTION

        This function allows a computer player to select a card as the second player
        of the turn in a 2-player mode match. This computer has a low intelligence level.

RETURNS

        The card selected by computer to be played
 
*/
/**/

Card Computer::PickLowTwoPlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed, const vector<Card>& a_tableCards) const
{
    //the computer's selection
    Card selectedCard;
    
    //if there is cards in the suit led in hand
    if (HasSuitLedCards(a_suitLed))
    {
        //identify all the suit led cards in hand
        vector<Card> suitLedCards = GetCardsInSuit(m_hand, a_suitLed);
        
        //if there is only one suit led card in hand
        if (suitLedCards.size() == 1)
        {
            //select this one card
            selectedCard = suitLedCards[0];
        }
        
        //if there is multiple suit led cards in hand
        else
        {
            //identify all the played cards of type suit led
            vector<Card> suitLedTableCards = GetCardsInSuit(a_tableCards, a_suitLed);
            
            //identify the highest ranked played card of type suit led
            Card maxTableCard = *max_element(suitLedTableCards.begin(), suitLedTableCards.end());
            
            //check if there is any lower ranked cards than the highest table card in hand
            vector<Card> lowerCards = GetLowerCards(maxTableCard, suitLedCards);
            
            //if there is any lower ranked cards than the table card
            if (!lowerCards.empty())
            {
                //pick the card with the first rank before the table card
                selectedCard = *max_element(lowerCards.begin(), lowerCards.end());
            }
            
            //if there is multiple higher cards than the table card
            else
            {
                //pick the highest ranked card in the suit (to lose a valuable card)
                selectedCard = *max_element(suitLedCards.begin(), suitLedCards.end());
            }
        }
    }
    
    //if there is no cards in the suit led
    else
    {
        //identify all cards that are not of type trump suit
        vector<Card> nonTrumpSuitCards = GetNonTrumpSuitCards(m_hand, a_trumpSuit);
        
        //if there is any non-trump-suit cards
        if (!nonTrumpSuitCards.empty())
        {
            //if there is only one non-trump-suit card
            if (nonTrumpSuitCards.size() == 1)
            {
                //pick this one card
                selectedCard = nonTrumpSuitCards[0];
            }
            
            //if there is multiple non-trump-suit card
            else
            {
                //pick the highest ranked card with suit of the most number of cards
                vector<Card> highestCards = GetHighestRankedCards(nonTrumpSuitCards);
                selectedCard = PickMostCommonSuitCard(highestCards);
            }
        }
        
        //if there is only trump suit cards in hand
        else
        {
            //pick the highest ranked card of the trump suit (to lose a valuable card)
            selectedCard = *max_element(m_hand.begin(), m_hand.end());
        }
    }
    
    return selectedCard;
}
/*Card Computer::PickLowTwoPlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed,
 const vector<Card>& a_tableCards) const;*/


/**/
/*
 Computer::PickHighThreePlSecond Computer::PickHighThreePlSecond

NAME

        PickHighThreePlSecond - to select a card as the second player in 3-player mode (high intelligence)

SYNOPSIS

        Card Computer::PickHighThreePlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed,
        const vector<Card>& a_tableCards) const;
 
             a_trumpSuit  --> the declared trump suit for the round
             a_suitLed    --> the suit of the first played card in this trick
             a_tableCards --> the cards played so far in the trick
 
DESCRIPTION

        This function allows a computer player to select a card as the second player
        of the turn in a 3-player mode match. This computer has a high intelligence level.

RETURNS

        The card selected by computer to be played
 
*/
/**/

Card Computer::PickHighThreePlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed, const vector<Card>& a_tableCards) const
{
    //the computer's selection
    Card selectedCard;
    
    //if there is cards in the suit led in hand
    if (HasSuitLedCards(a_suitLed))
    {
        //identify all the suit led cards in hand
        vector<Card> suitLedCards = GetCardsInSuit(m_hand, a_suitLed);
        
        //if there is only one suit led card in hand
        if (suitLedCards.size() == 1)
        {
            //pick this one card
            selectedCard = suitLedCards[0];
        }
        
        //if there is multiple suit led cards in hand
        else
        {
            //identify all the played cards of type suit led
            vector<Card> suitLedTableCards = GetCardsInSuit(a_tableCards, a_suitLed);
            
            //identify the highest ranked played card of type suit led
            Card maxTableCard = *max_element(suitLedTableCards.begin(), suitLedTableCards.end());
            
            //check if there is any card(s) higher than the highest ranked table card
            vector<Card> higherCards = GetHigherCards(maxTableCard, suitLedCards);
            
            //if there is any higher ranked cards than the table card
            if (!higherCards.empty())
            {
                //play the highest ranked card after the table card to make sure third player does not beat your card
                selectedCard = *max_element(higherCards.begin(), higherCards.end());
            }
            
            //if there is multiple lower ranked cards than the table card
            else
            {
                //pick the lowest ranked card card of the suit led
                selectedCard = *min_element(suitLedCards.begin(), suitLedCards.end());
            }
        }
    }
    
    //if there is no cards in the suit led
    else
    {
        //check if there is any trump suit cards in hand
        vector<Card> trumpSuitCards = GetTrumpSuitCards(m_hand, a_trumpSuit);
        
        //if there is any trump-suit cards
        if (!trumpSuitCards.empty())
        {
            //if there is only one trump suit card
            if (trumpSuitCards.size() == 1)
            {
                //pick this one card
                selectedCard = trumpSuitCards[0];
            }
            
            //if there is multiple trump suit cards
            else
            {
                //pick the highest ranked trump suit card to make sure third player does not beat your card
                selectedCard = *max_element(trumpSuitCards.begin(), trumpSuitCards.end());
            }
        }
        
        //if there is no trump suit cards
        else
        {
            //play the smallest ranked card with the suit of the least number of cards in hand
            vector<Card> smallestCards = GetLowestRankedCards(m_hand);
            selectedCard = PickLeastCommonSuitCard(smallestCards);
        }
    }
    
    return selectedCard;
}
/* Card Computer::PickHighThreePlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed,
 const vector<Card>& a_tableCards) const;*/


/**/
/*
 Computer::PickAverageThreePlSecond Computer::PickAverageThreePlSecond

NAME

        PickAverageThreePlSecond - to select a card as the second player in 3-player mode (average intelligence)

SYNOPSIS

        Card Computer::PickAverageThreePlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed,
        const vector<Card>& a_tableCards) const;
 
             a_trumpSuit  --> the declared trump suit for the round
             a_suitLed    --> the suit of the first played card in this trick
             a_tableCards --> the cards played so far in the trick
 
DESCRIPTION

        This function allows a computer player to select a card as the second player
        of the turn in a 3-player mode match. This computer has an average intelligence level.

RETURNS

        The card selected by computer to be played
 
*/
/**/

Card Computer::PickAverageThreePlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed, const vector<Card>& a_tableCards) const
{
    //the computer's selection
    Card selectedCard;
    
    //if there is cards in the suit led in hand
    if (HasSuitLedCards(a_suitLed))
    {
        //identify all the suit led cards in hand
        vector<Card> suitLedCards = GetCardsInSuit(m_hand, a_suitLed);
        
        //if there is only one suit led card in hand
        if (suitLedCards.size() == 1)
        {
            //pick this one card
            selectedCard = suitLedCards[0];
        }
        
        //if there is multiple suit led cards in hand
        else
        {
            //idenitfy all played cards of type suit led
            vector<Card> suitLedTableCards = GetCardsInSuit(a_tableCards, a_suitLed);
            
            //identify the highest ranked played card of type suit led
            Card maxTableCard = *max_element(suitLedTableCards.begin(), suitLedTableCards.end());
            
            //check if there is any card(s) higher than the highest table card
            vector<Card> higherCards = GetHigherCards(maxTableCard, suitLedCards);
            
            //if there is any higher cards than the table card
            if (!higherCards.empty())
            {
                //if there is only one higher card than the table card
                if (higherCards.size() == 1)
                {
                    //pick this one card
                    selectedCard = higherCards[0];
                }
                
                //if there is multiple higher cards than the table card
                else
                {
                    //play the lowest ranked card (since playing highest card is the best option and for high intelligence)
                    selectedCard = *min_element(higherCards.begin(), higherCards.end());
                }
            }
            
            //if there is multiple lower ranked cards than the table card
            else
            {
                //pick the highest ranked lower card
                selectedCard = *max_element(suitLedCards.begin(), suitLedCards.end());
            }
        }
    }
    
    //if there is no cards in the suit led
    else
    {
        //check if there is any trump suit cards in hand
        vector<Card> trumpSuitCards = GetTrumpSuitCards(m_hand, a_trumpSuit);
        
        //if there is any trump-suit cards
        if (!trumpSuitCards.empty())
        {
            //if there is only one trump suit card
            if (trumpSuitCards.size() == 1)
            {
                //pick this one trump suit card
                selectedCard = trumpSuitCards[0];
            }
            
            //if there is multiple trump suit cards
            else
            {
                //pick the lowest ranked trump suit card
                selectedCard = *min_element(trumpSuitCards.begin(), trumpSuitCards.end());
            }
        }
        
        //if there is no trump suit cards
        else
        {
            //play the smallest ranked card in hand
            vector<Card> smallestCards = GetLowestRankedCards(m_hand);
            selectedCard =  smallestCards[0];
        }
    }
    
    return selectedCard;
    
}
/*Card Computer::PickAverageThreePlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed,
 const vector<Card>& a_tableCards) const;*/


/**/
/*
 Computer::PickLowThreePlSecond Computer::PickLowThreePlSecond

NAME

        PickLowThreePlSecond - to select a card as the second player in 3-player mode (low intelligence)

SYNOPSIS

        Card Computer::PickLowThreePlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed,
        const vector<Card>& a_tableCards) const;
 
             a_trumpSuit  --> the declared trump suit for the round
             a_suitLed    --> the suit of the first played card in this trick
             a_tableCards --> the cards played so far in the trick
 
DESCRIPTION

        This function allows a computer player to select a card as the second player
        of the turn in a 3-player mode match. This computer has a low intelligence level.

RETURNS

        The card selected by computer to be played
 
*/
/**/

Card Computer::PickLowThreePlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed, const vector<Card>& a_tableCards) const
{
    //the computer's selection
    Card selectedCard;
    
    //if there is cards in the suit led in hand
    if (HasSuitLedCards(a_suitLed))
    {
        //identify all the suit led cards in hand
        vector<Card> suitLedCards = GetCardsInSuit(m_hand, a_suitLed);
        
        //if there is only one suit led card in hand
        if (suitLedCards.size() == 1)
        {
            //pick this one card
            selectedCard = suitLedCards[0];
        }
        
        //if there is multiple suit led cards in hand
        else
        {
            //identify the all the played cards of type suit led
            vector<Card> suitLedTableCards = GetCardsInSuit(a_tableCards, a_suitLed);
            
            //identify the highest ranked played card of type suit led
            Card maxTableCard = *max_element(suitLedTableCards.begin(), suitLedTableCards.end());
            
            //check if there is any lower ranked cards than the highest table card
            vector<Card> lowerCards = GetLowerCards(maxTableCard, suitLedCards);
            
            //if there is any lower ranked cards than the table card
            if (!lowerCards.empty())
            {
                //pick the card with first rank before the table card
                selectedCard = *max_element(lowerCards.begin(), lowerCards.end());
            }
            
            //if there is multiple higher cards than the table card
            else
            {
                //pick the first card after the table card (to allow the third player to win)
                selectedCard = *min_element(suitLedCards.begin(), suitLedCards.end());
            }
        }
    }
    
    //if there is no cards in the suit led
    else
    {
        //identify all the cards that are not of type trump suit
        vector<Card> nonTrumpSuitCards = GetNonTrumpSuitCards(m_hand, a_trumpSuit);
        
        //if there is any non-trump-suit cards
        if (!nonTrumpSuitCards.empty())
        {
            //if there is only one non-trump-suit card
            if (nonTrumpSuitCards.size() == 1)
            {
                //pick this one card
                selectedCard = nonTrumpSuitCards[0];
            }
            
            //if there is multiple non-trump-suit card
            else
            {
                //pick the highest ranked card with suit of the most number of cards
                vector<Card> highestCards = GetHighestRankedCards(nonTrumpSuitCards);
                selectedCard = PickMostCommonSuitCard(highestCards);
            }
        }
        
        //if there is only trump suit cards in hand
        else
        {
            //pick the smallest ranked card of the trump suit (to allow the third player to win)
            selectedCard = *min_element(m_hand.begin(), m_hand.end());
        }
    }
    
    return selectedCard;
    
}
/*Card Computer::PickLowThreePlSecond(const Suit& a_trumpSuit, const Suit& a_suitLed,
 const vector<Card>& a_tableCards) const;*/


/**/
/*
 Computer::PickMostCommonSuitCard Computer::PickMostCommonSuitCard

NAME

        PickMostCommonSuitCard  - to pick the card of the suit with the most number of cards in hand

SYNOPSIS

        Card Computer::PickMostCommonSuitCard(const vector<Card>& a_cards) const;
            a_cards --> the collection of cards to choose a card from
 
DESCRIPTION

        This function identifies one of the cards with the most common suit in hand.

RETURNS

        The selected card based on the strategy
 
*/
/**/


Card Computer::PickMostCommonSuitCard(const vector<Card>& a_cards) const
{
    //the selected card
    Card card;
    
    //identify the suit(s) of the cards
    vector<Suit> suits = GetSuits(a_cards);
    
    //identify the most common suit
    Suit mostCardSuit = GetMostCommonSuit(suits);
    
    for (int i=0; i<a_cards.size(); i++)
    {
        if (a_cards[i].GetSuit() == mostCardSuit)
            card = a_cards[i];
    }
    
    return card;
}
/*Card Computer::PickMostCommonSuitCard(const vector<Card>& a_cards) const;*/


/**/
/*
 Computer::PickLeastCommonSuitCard Computer::PickLeastCommonSuitCard

NAME

        PickLeastCommonSuitCard  - to pick the card of the suit with the least number of cards in hand

SYNOPSIS

        Card Computer::PickLeastCommonSuitCard(const vector<Card>& a_cards) const;
            a_cards --> the collection of cards to choose a card from
 
DESCRIPTION

        This function identifies one of the cards with the least common suit in hand.

RETURNS

        The selected card based on the strategy
 
*/
/**/


Card Computer::PickLeastCommonSuitCard(const vector<Card>& a_cards) const
{
    //the selected card
    Card card;
    
    //identify the suit(s) of the cards
    vector<Suit> suits = GetSuits(a_cards);
    
    //identify the least common suit
    Suit leastCardSuit = GetLeastCommonSuit(suits);
    
    for (int i=0; i<a_cards.size(); i++)
    {
        if (a_cards[i].GetSuit() == leastCardSuit)
            card = a_cards[i];
    }
    
    return card;
}
/*Card Computer::PickLeastCommonSuitCard(const vector<Card>& a_cards) const;*/


/**/
/*
 Computer::GetMostCommonSuit Computer::GetMostCommonSuit

NAME

        GetMostCommonSuit - to identify the most common suit in hand

SYNOPSIS

        suit Computer::GetMostCommonSuit(const vector<Suit>& a_suits) const;
            a_suits --> the suits to find the most common suit from
 
DESCRIPTION

        This function identifies the most common suit in hand.

RETURNS

        The most common suit in hand
*/
/**/

Suit Computer::GetMostCommonSuit(const vector<Suit>& a_suits) const
{
    //mapping each suit to the number of times they appear in hand
    map<Suit, int> suitNumCards;
    
    //identify the number of cards in each suit
    for (int i=0; i<a_suits.size(); i++)
    {
        int numCards = GetNumCardsInSuit(a_suits[i]);
        suitNumCards.insert({a_suits[i], numCards});
    }
    
    Suit maxSuit = spades;
    int maxCards = 0;
    
    //find the suit with the most number of cards
    map<Suit, int>::iterator it;
    for (it = suitNumCards.begin(); it != suitNumCards.end(); ++it)
    {
        if (it->second > maxCards)
        {
            maxSuit = it->first;
            maxCards = it->second;
        }
    }
    
    return maxSuit;
}
/*suit Computer::GetMostCommonSuit(const vector<Suit>& a_suits) const;*/


/**/
/*
 Computer::GetLeastCommonSuit Computer::GetLeastCommonSuit

NAME

        GetLeastCommonSuit - to identify the least common suit in hand

SYNOPSIS

        suit Computer::GetLeastCommonSuit(const vector<Suit>& a_suits) const;
            a_suits --> the suits to find the least common suit from
 
DESCRIPTION

        This function identifies the least common suit in hand.

RETURNS

        The least common suit in hand
*/
/**/

Suit Computer::GetLeastCommonSuit(const vector<Suit>& a_suits) const
{
    //mapping each suit to the number of times they appear in hand
    map<Suit, int> suitNumCards;
    
    //identify the number of cards in each suit
    for (int i=0; i<a_suits.size(); i++)
    {
        int numCards = GetNumCardsInSuit(a_suits[i]);
        suitNumCards.insert({a_suits[i], numCards});
    }
    
    Suit minSuit = spades;
    
    //there is only 13 cards in each suit
    int minCards = ace;
    
    //find the suit with the least number of cards
    map<Suit, int>::iterator it;
    for (it = suitNumCards.begin(); it != suitNumCards.end(); ++it)
    {
        if (it->second < minCards)
        {
            minSuit = it->first;
            minCards = it->second;
        }
    }
    
    return minSuit;
}
/*suit Computer::GetLeastCommonSuit(const vector<Suit>& a_suits) const;*/


/**/
/*
 Computer::GetHighestRankedCards Computer::GetHighestRankedCards

NAME

        GetHighestRankedCards - to identify the cards with the highest rank

SYNOPSIS

        vector<Card> Computer::GetHighestRankedCards(const vector<Card>& a_cards) const;
            a_cards --> the cards to choose the highest ranked cards from
 
DESCRIPTION

        This function identifies the highest ranked cards from a collection of cards

RETURNS

        The highest ranked cards
*/
/**/

vector<Card> Computer::GetHighestRankedCards(const vector<Card>& a_cards) const
{
    //identify the highest ranked card
    int maxRank = a_cards[0].GetRank();
    for (int i=1; i<a_cards.size(); i++)
    {
        if (a_cards[i].GetRank() > maxRank)
            maxRank = a_cards[i].GetRank();
    }
    
    //identify the cards with the highest rank
    vector<Card> highestCards;
    for (int i=0; i<a_cards.size(); i++)
    {
        if (a_cards[i].GetRank() == maxRank)
            highestCards.push_back(a_cards[i]);
    }
    
    return highestCards;
}
/*vector<Card> Computer::GetHighestRankedCards(const vector<Card>& a_cards) const;*/


/**/
/*
 Computer::GetLowestRankedCards Computer::GetLowestRankedCards

NAME

        GetLowestRankedCards - to identify the cards with the lowest rank

SYNOPSIS

        vector<Card> Computer::GetLowestRankedCards(const vector<Card>& a_cards) const;
            a_cards --> the cards to choose the lowest ranked cards from
 
DESCRIPTION

        This function identifies the lowest ranked cards from a collection of cards

RETURNS

        The lowest ranked cards
*/
/**/

vector<Card> Computer::GetLowestRankedCards(const vector<Card>& a_cards) const
{
    //identify the lowest ranked card
    int minRank = a_cards[0].GetRank();
    for (int i=1; i<a_cards.size(); i++)
    {
        if (a_cards[i].GetRank() < minRank)
            minRank = a_cards[i].GetRank();
    }
    
    //identify the cards with the lowest rank
    vector<Card> smallestCards;
    for (int i=0; i<a_cards.size(); i++)
    {
        if (a_cards[i].GetRank() == minRank)
            smallestCards.push_back(a_cards[i]);
    }
    
    return smallestCards;
}
/*vector<Card> Computer::GetLowestRankedCards(const vector<Card>& a_cards) const;*/


/**/
/*
 Computer::GetHigherCards Computer::GetHigherCards

NAME

        GetHigherCards - to get the higher ranked cards than a specified card in the same suit

SYNOPSIS

        vector<Card> Computer::GetHigherCards(const Card& a_card, const vector<Card>& a_cards) const;
            a_card  --> the card to check for higher cards
            a_cards --> the collection of cards to check from
 
DESCRIPTION

        This function identifies higher card(s) than a specified card in the same suit.

RETURNS

        A collection of cards
*/
/**/

vector<Card> Computer::GetHigherCards(const Card& a_card, const vector<Card>& a_cards) const
{
    vector<Card> higherCards;
    for (int i=0; i<a_cards.size(); i++)
    {
        if (a_card < a_cards[i])
            higherCards.push_back(a_cards[i]);
    }
    
    return higherCards;
}
/*vector<Card> Computer::GetHigherCards(const Card& a_card, const vector<Card>& a_cards) const;*/


/**/
/*
 Computer::GetLowerCards Computer::GetLowerCards

NAME

        GetLowerCards - to get the lower ranked cards than a specified card in the same suit

SYNOPSIS

        vector<Card> Computer::GetLowerCards(const Card& a_card, const vector<Card>& a_cards) const;
            a_card  --> the card to check for lower cards
            a_cards --> the collection of cards to check from
 
DESCRIPTION

        This function identifies lower card(s) than a specified card in the same suit.

RETURNS

        A collection of cards
*/
/**/

vector<Card> Computer::GetLowerCards(const Card& a_card, const vector<Card>& a_cards) const
{
    vector<Card> lowerCards;
    for (int i=0; i<a_cards.size(); i++)
    {
        if (a_cards[i] < a_card)
            lowerCards.push_back(a_cards[i]);
    }
    
    return lowerCards;
}
/*vector<Card> Computer::GetLowerCards(const Card& a_card, const vector<Card>& a_cards) const;*/


/**/
/*
 Computer::GetTrumpSuitCards Computer::GetTrumpSuitCards

NAME

        GetTrumpSuitCards - to identify all the cards of type trump suit

SYNOPSIS

        vector<Card> Computer::GetTrumpSuitCards(const vector<Card>& a_cards, const Suit& a_trumpSuit) const;
            a_cards     --> the collection of cards to choose cards from
            a_trumpSuit --> the declared trump suit for the round
 
DESCRIPTION

        This function identifies all trump suit cards.

RETURNS

        A collection of cards
*/
/**/

vector<Card> Computer::GetTrumpSuitCards(const vector<Card>& a_cards, const Suit& a_trumpSuit) const
{
    vector<Card> cards;
    for (int i=0; i<a_cards.size(); i++)
    {
        if (a_cards[i].GetSuit() == a_trumpSuit)
            cards.push_back(a_cards[i]);
    }
    
    return cards;
}
/*vector<Card> Computer::GetTrumpSuitCards(const vector<Card>& a_cards, const Suit& a_trumpSuit) const;*/


/**/
/*
 Computer::GetNonTrumpSuitCards Computer::GetNonTrumpSuitCards

NAME

        GetNonTrumpSuitCards - to get all the cards that are not of type trump suit

SYNOPSIS

        vector<Card> Computer::GetNonTrumpSuitCards(const vector<Card>& a_cards, const Suit& a_trumpSuit) const;
            a_cards     --> the collection of cards to choose cards from
            a_trumpSuit --> the declared trump suit for the round
 
DESCRIPTION

        This function identifies all non-trump suit cards.

RETURNS

        A collection of cards
*/
/**/

vector<Card> Computer::GetNonTrumpSuitCards(const vector<Card>& a_cards, const Suit& a_trumpSuit) const
{
    vector<Card> cards;
    for (int i=0; i<a_cards.size(); i++)
    {
        if (a_cards[i].GetSuit() != a_trumpSuit)
            cards.push_back(a_cards[i]);
    }
    
    return cards;
}
/*vector<Card> Computer::GetNonTrumpSuitCards(const vector<Card>& a_cards, const Suit& a_trumpSuit) const;*/


/**/
/*
 Computer::GetKingsPlayed Computer::GetKingsPlayed

NAME

        GetKingsPlayed - to identify the kings that their Ace has been played

SYNOPSIS

        vector<Card> Computer::GetKingsPlayed(const vector<Card>& a_kings) const;
            a_kings --> the king cards to choose from
 
DESCRIPTION

        This function identifies all kings that their Ace has been played in the previous turns

RETURNS

        The kings that their Ace has been played
*/
/**/

vector<Card> Computer::GetKingsPlayed(const vector<Card>& a_kings) const
{
    //kings that their ace has been played
    vector<Card> a_cards;
    
    for (int i=0; i<a_kings.size(); i++)
    {
        //ace of this king
        Card aceOfKing(a_kings[i].GetSuit(), ace);
        
        //if the ace of this king is already played
        if (find(m_cardsPlayed.begin(), m_cardsPlayed.end(), aceOfKing) != m_cardsPlayed.end())
            a_cards.push_back(a_kings[i]);
    }
    
    return a_cards;
}
/*vector<Card> Computer::GetKingsPlayed(const vector<Card>& a_kings) const;*/


/**/
/*
 Computer::GetQueensPlayed Computer::GetQueensPlayed

NAME

        GetQueensPlayed - to get the quuens that their Ace and King has been played

SYNOPSIS

        vector<Card> Computer::GetQueensPlayed(const vector<Card>& a_queens) const;
            a_queens --> the queen cards to choose from
 
DESCRIPTION

        This function identifies all queens that their Ace and King has been played in the previous turns

RETURNS

        The Queens that their Ace and King has been played
*/
/**/

vector<Card> Computer::GetQueensPlayed(const vector<Card>& a_queens) const
{
    //queens that their ace and king has been played
    vector<Card> a_cards;
    
    for (int i=0; i<a_queens.size(); i++)
    {
        //the ace and king of this queen
        Card aceOfQueen(a_queens[i].GetSuit(), ace);
        Card kingOfQueen(a_queens[i].GetSuit(), king);
        
        //if the king and ace of this queen is already played
        if (find(m_cardsPlayed.begin(), m_cardsPlayed.end(), aceOfQueen) != m_cardsPlayed.end() &&
            find(m_cardsPlayed.begin(), m_cardsPlayed.end(), kingOfQueen) != m_cardsPlayed.end())
            a_cards.push_back(a_queens[i]);
    }
    
    return a_cards;
}
/*vector<Card> Computer::GetQueensPlayed(const vector<Card>& a_queens) const;*/


/**/
/*
 Computer::GetCardsInRank Computer::GetCardsInRank

NAME

        GetCardsInRank - to identify all cards with a specified rank

SYNOPSIS

        vector<Card> Computer::GetCardsInRank(const vector<Card>& a_cards, const int& a_rank) const;
            a_cards --> the collection of cards to choose from
            a_rank  --> the rank to look for in the cards
 
DESCRIPTION

        This function identifies all cards of a specific rank.

RETURNS

        The collection of cards with the specified rank
*/
/**/

vector<Card> Computer::GetCardsInRank(const vector<Card>& a_cards, const int& a_rank) const
{
    //the cards with the specified rank
    vector<Card> cards;
    
    for (int i=0; i<a_cards.size(); i++)
    {
        if (a_cards[i].GetRank() == a_rank)
            cards.push_back(a_cards[i]);
    }
    
    return cards;
}
/*vector<Card> Computer::GetCardsInRank(const vector<Card>& a_cards, const int& a_rank) const;*/


/**/
/*
 Computer::GetCardsInSuit Computer::GetCardsInSuit

NAME

        GetCardsInSuit - to identify all cards with a specified suit

SYNOPSIS

        vector<Card> Computer::GetCardsInSuit(const vector<Card>& a_cards, const Suit& a_suit) const;
            a_cards --> the collection of cards to choose from
            a_suit  --> the suit to look for in the cards
 
DESCRIPTION

        This function identifies all cards of a specific suit.

RETURNS

        The collection of cards with the specified suit
*/
/**/


vector<Card> Computer::GetCardsInSuit(const vector<Card>& a_cards, const Suit& a_suit) const
{
    //the cards with the specified suit
    vector<Card> cards;
    
    for (int i=0; i<a_cards.size(); i++)
    {
        if (a_cards[i].GetSuit() == a_suit)
            cards.push_back(a_cards[i]);
    }
    
    return cards;
}
/*vector<Card> Computer::GetCardsInSuit(const vector<Card>& a_cards, const Suit& a_suit) const;*/


/**/
/*
 Computer::HasSuitLedCards Computer::HasSuitLedCards

NAME

        HasSuitLedCards - to check if there is any cards of suit led in hand

SYNOPSIS

        bool Computer::HasSuitLedCards(const Suit& a_suitLed) const;
            a_suitLed --> the suit of the first played card in the trick
 
DESCRIPTION

        This function determines whether there is any cards of the suit led
        in hand

RETURNS

        True if there is any cards of type suit led in hand
        False otherwise
*/
/**/

bool Computer::HasSuitLedCards(const Suit& a_suitLed) const
{
    for (int i=0; i<m_hand.size(); i++)
    {
        if (m_hand[i].GetSuit() == a_suitLed)
            return true;
    }
    
    return false;
}
/*bool Computer::HasSuitLedCards(const Suit& a_suitLed) const;*/


/**/
/*
 Computer::GetNumCardsInSuit Computer::GetNumCardsInSuit

NAME

        GetNumCardsInSuit - to find the total number of cards of a specific suit in hand

SYNOPSIS

        int Computer::GetNumCardsInSuit(const Suit& a_suit) const;
            a_suit --> the suit to look for cards in
 
DESCRIPTION

        This function determines the total number of cards in hand in a specified suit.

RETURNS
 
        The total number of cards in the specified suit
*/
/**/

int Computer::GetNumCardsInSuit(const Suit& a_suit) const
{
    //total number of cards in hand with the specified suit
    int totalCards = 0;
    
    for (int i=0; i<m_hand.size(); i++)
    {
        if (m_hand[i].GetSuit() == a_suit)
            totalCards++;
    }
    
    return totalCards;
}
/*int Computer::GetNumCardsInSuit(const Suit& a_suit) const;*/


/*
 Computer::GetSuits Computer::GetSuits

NAME

        GetSuits - to identify the suit(s) of a collection of cards

SYNOPSIS

        vector<suit> Computer::GetSuits(const vector<Card>& a_cards) const;
            a_cards --> the collection of cards to find suits from
 
DESCRIPTION

        This function identifies the suit(s) of a collection of cards

RETURNS
 
        The collection of suits of cards
*/
/**/

vector<Suit> Computer::GetSuits(const vector<Card>& a_cards) const
{
    //all the suits
    vector<Suit> cardSuits;
    
    for (int i=0; i<a_cards.size(); i++)
        cardSuits.push_back(a_cards[i].GetSuit());
    
    return cardSuits;
}
/*vector<suit> Computer::GetSuits(const vector<Card>& a_cards) const;*/


/*
 Computer::PickFromFaceCards Computer::PickFromFaceCards

NAME

        PickFromFaceCards - to choose a card from a collection of Face Cards including Ace

SYNOPSIS

        Card Computer::PickFromFaceCards(const vector<Card>& a_faceCards, const Suit& a_trumpSuit) const;
            a_faceCards --> the collection of Face cards to choose from
            a_trumpSuit --> the declared trump suit for the round
 
DESCRIPTION

        This function selects a Face card (including Ace)
        from a collection of Face cards (including Ace) to be played.

RETURNS
 
        The selected card
*/
/**/

Card Computer::PickFromFaceCards(const vector<Card>& a_faceCards, const Suit& a_trumpSuit) const
{
    Card selectedCard;
    
    //if there is only one Face Card
    if (a_faceCards.size() == 1)
    {
        //pick this one Face Card
        selectedCard = a_faceCards[0];
    }
    
    //if there is more than one Face Card
    else
    {
        //identify all cards that are not of type trump suit
        vector<Card> nonTrumpSuitFaces = GetNonTrumpSuitCards(a_faceCards, a_trumpSuit);
        
        //if there is any non-trump suit Face Cards
        if (!nonTrumpSuitFaces.empty())
        {
            if (nonTrumpSuitFaces.size() == 1)
            {
                //pick this one card
                selectedCard = nonTrumpSuitFaces[0];
            }
            
            //if there is more than one non-trump suit face card
            else
            {
                //pick the non-trump suit Face card with the least number of cards in hand
                selectedCard = PickLeastCommonSuitCard(a_faceCards);
            }
        }
        
        else
        {
            //pick the Face card of the trump suit
            vector<Card> trumpSuitCards = GetTrumpSuitCards(a_faceCards, a_trumpSuit);
            selectedCard = trumpSuitCards[0];
        }
    }
    
    return selectedCard;
}

/*Card Computer::PickFromFaceCards(const vector<Card>& a_faceCards, const Suit& a_trumpSuit) const;*/


/*
 Computer::DropTwoHighest Computer::DropTwoHighest

NAME

        DropTwoHighest - to drop the 2 highest ranked cards from a collection of cards (hand)

SYNOPSIS

        void Computer::DropTwoHighest(vector<Card> a_cards);
            a_cards --> the collection of cards to choose from
 
DESCRIPTION

        This function identifies and drops the 2 highest ranked cards from a collection of cards

RETURNS
 
        None
*/
/**/

void Computer::DropTwoHighest(vector<Card> a_cards)
{
    //identify the cards with the highest rank
    vector<Card> highestCards = GetHighestRankedCards(a_cards);
    
    if (highestCards.size() >= 2)
    {
        RemoveFromHand(highestCards[0]);
        RemoveFromHand(highestCards[1]);
    }
    
    //if there is only one highest ranked
    else
    {
        //remove the highest ranked card from hand
        RemoveFromHand(highestCards[0]);
        
        //remove the highest ranked card from the cards
        a_cards.erase(remove(a_cards.begin(), a_cards.end(), highestCards[0]), a_cards.end());
        
        //find the highest ranked card of the new hand
        vector<Card> highestCardsTwo = GetHighestRankedCards(a_cards);
        
        //remove the highest ranked card from hand
        RemoveFromHand(highestCardsTwo[0]);
    }
}
/*void Computer::DropTwoHighest(vector<Card> a_cards);*/


/*
 Computer::DropTwoLowest Computer::DropTwoLowest

NAME

        DropTwoLowest - to drop the 2 lowest ranked cards from a collection of cards (hand)

SYNOPSIS

        void Computer::DropTwoHLowest(vector<Card> a_cards);
            a_cards --> the collection of cards to choose from
 
DESCRIPTION

        This function identifies and drops the 2 lowest ranked cards from a collection of cards

RETURNS
 
        None
*/
/**/

void Computer::DropTwoLowest(vector<Card> a_cards)
{
    //identify the cards with the lowest rank
    vector<Card> lowestCards = GetLowestRankedCards(a_cards);
    
    if (lowestCards.size() >= 2)
    {
        RemoveFromHand(lowestCards[0]);
        RemoveFromHand(lowestCards[1]);
    }
    
    //if there is only one smallest rank
    else
    {
        RemoveFromHand(lowestCards[0]);
        
        //remove the highest card from the cards
        a_cards.erase(remove(a_cards.begin(), a_cards.end(), lowestCards[0]), a_cards.end());
        
        //find the lowest ranked card of the new hand
        vector<Card> lowestCardsTwo = GetLowestRankedCards(a_cards);
        
        RemoveFromHand(lowestCardsTwo[0]);
    }
}
