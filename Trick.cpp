#include "stdafx.h"

/**/
/*
 Trick::GetTableCards Trick::GetTableCards

NAME

        GetTableCards - to identify the cards played so far in the trick

SYNOPSIS

        vector<Card> Trick::GetTableCards() const;
            

DESCRIPTION

        This function identifies all the played cards during this trick.

RETURNS

        The collection of played cards
 
*/
/**/

vector<Card> Trick::GetTableCards() const
{
    vector<Card> tableCards;
    
    //get all the keys of the table cards map
    transform( m_tableCards.begin(), m_tableCards.end(), back_inserter(tableCards), []
              (const map<Card, Player*>::value_type &pair){return pair.first;});
    
    return tableCards;
}
/*vector<Card> Trick::GetTableCards() const;*/


/**/
/*
 Trick::PromptPlToPlay Trick::PromptPlToPlay

NAME

        PromptPlToPlay - checks whether the trick is over and if not prompts next player to play a card

SYNOPSIS

        void Trick::PromptPlToPlay(vector<Player*> &a_players, Player* a_hakem, const vector<int>& a_points,
                            const vector<int>& a_tricks, const int& a_randNum, const Suit& a_trumpSuit, int a_index);
 
            a_players   --> the match players
            a_hakem     --> the Hakem of this round
            a_points    --> the match points for the players
            a_tricks    --> the round trick scores for the players
            a_randNum   --> the current round number
            a_trumpSuit --> the declared trump suit for this round
            a_index     --> the match index of player being prompted to play a card
            

DESCRIPTION

        This function checks to see whether the trick is over. If the trick is over, it establishes the
        winner. Otheriwse it will prompt the next player to play a card.

RETURNS

        None
 
*/
/**/

void Trick::PromptPlToPlay(vector<Player*> &a_players, Player* a_hakem, const vector<int>& a_points,
                           const vector<int>& a_tricks, const int& a_randNum, const Suit& a_trumpSuit, int a_index)
{
    //display the current match state to all human players
    for (auto pl: a_players)
    {
        string display = Display::GetMatchDisplay(a_players, a_hakem, GetTableCards(), a_points,
                                                  a_tricks, a_randNum, a_trumpSuit, pl->GetUsername());
        pl->SendMsg(display);
    }
    
    //if all players played a card
    if (m_tableCards.size() == a_players.size())
    {
        //establish the trick winner
        IdentifyWinner(a_trumpSuit);
        
        //prepare winner declaration message
        vector<Player*> winner;
        winner.push_back(m_winner);
        
        //announce the trick winner
        for (auto pl: a_players)
        {
            string msg = Display::GetWinnerDeclarationMsg(winner, TRICK, pl->GetUsername());
            pl->SendMsg(msg);
        }
    }
    
    //if players still need to play cards to complete the trick
    else
    {
        //player's next input must be a card input
        a_players[a_index]->SetStage(mustSelectCard);
        
        //other players must wait for this player
        for (auto player: a_players)
        {
            if (player != a_players[a_index])
            {
                player->SetStage(mustWaitForOtherPls);
                player->SendMsg("\nWaiting For " + a_players[a_index]->GetUsername() + " To Play A Card...\n");
            }
        }
        
        if (a_players[a_index]->IsHuman())
        {
            //display the message to prompt player to play a card
            string display;
            display += "\nSelect A Card\n";
            display += Display::GetCardSelectionPrompt();
            a_players[a_index]->SendMsg(display);
        }
        
        //if it's a computer
        else
        {
            //computer selects a card
            Card selectedCard = a_players[a_index]->SelectCard((Mode)a_players.size(), (Turn)m_tableCards.size(),
                                                               a_trumpSuit, m_suitLed, GetTableCards());
            
            //computer places the card
            PlaceCard(a_players, selectedCard, a_index);
            
            for (auto player: a_players)
                player->SendMsg(a_players[a_index]->GetUsername() + " Placed A Card!\n");
            
            //identify index of the next player
            a_index++;
            if (a_index == a_players.size())
                a_index = 0;
            
            //computer prompts next player to play a card
            PromptPlToPlay(a_players, a_hakem, a_points, a_tricks, a_randNum, a_trumpSuit, a_index);
        }
    }
}
/*        void Trick::PromptPlToPlay(vector<Player*> &a_players, Player* a_hakem, const vector<int>& a_points,
 const vector<int>& a_tricks, const int& a_randNum, const Suit& a_trumpSuit, int a_index);*/


/**/
/*
 Trick::IdentifyWinner Trick::IdentifyWinner

NAME

        IdentifyWinner - identifies the winner of the trick

SYNOPSIS

        Player* Trick::IdentifyWinner(const Suit& a_trumpSuit);
            a_trumpSuit --> the declared trump suit for this round

            
DESCRIPTION

        This function identifies the winner of a trick first based on the highest
        played trump suit card and then the highest suit led card played.

RETURNS

        None
 
*/
/**/

Player* Trick::IdentifyWinner(const Suit& a_trumpSuit)
{
    //identify any possible trump suits played
    vector<Card> trumpCardsPlayed;
    for (auto itr = m_tableCards.begin(); itr != m_tableCards.end(); ++itr)
        if (itr->first.GetSuit() == a_trumpSuit)
            trumpCardsPlayed.push_back(itr->first);
    
    //if there is any trump suits
    if (trumpCardsPlayed.size() != 0)
    {
        //the player that played the highest ranked card of the trump suit wins the trick
        
        //the highest ranked card in the trump suit
        Card maxTrumpCard = *max_element(trumpCardsPlayed.begin(), trumpCardsPlayed.end());
        
        //the player that played this card wins (record the winner)
        m_winner = m_tableCards[maxTrumpCard];
    }
    
    else
    {
        //the player that played the highest ranked card of the suit led wins the trick
        
        //identify all cards of the suit led (there is at least one)
        vector<Card> suitLedPlayed;
        for (auto itr = m_tableCards.begin(); itr != m_tableCards.end(); ++itr)
            if (itr->first.GetSuit() == m_suitLed)
                suitLedPlayed.push_back(itr->first);
        
        //the highest ranked card of suit led
        Card maxSuitLedCard = *max_element(suitLedPlayed.begin(), suitLedPlayed.end());
        
        //the player that played this card wins (record the winner)
        m_winner = m_tableCards[maxSuitLedCard];
    }
    
    return m_winner;
}
/*Player* Trick::IdentifyWinner(const Suit& a_trumpSuit);*/


/**/
/*
 Trick::PlaceCard Trick::PlaceCard

NAME

        PlaceCard - to play a card from hand

SYNOPSIS

        void Trick::PlaceCard(vector<Player*> &a_players, const Card& a_card, const int& a_index);
            a_players --> the match players
            a_card    --> the card to be placed on the table
            a_index   --> the index of the player that played this card

            
DESCRIPTION

        This function places a card on the table and takes the necessary actions afterwards.

RETURNS

        None
 
*/
/**/

void Trick::PlaceCard(vector<Player*> &a_players, const Card& a_card, const int& a_index)
{
    //record the suit led on the first turn
    if (m_tableCards.size() == 0)
        m_suitLed = a_card.GetSuit();
    
    //remove the card from player's hand
    a_players[a_index]->RemoveFromHand(a_card);
    
    //record the card played and the associated player
    m_tableCards.insert({a_card, a_players[a_index]});
    
    //computers will also remember the cards played
    for (auto player:a_players)
        player->RememberCard(a_card);
}
/*void Trick::PlaceCard(vector<Player*> &a_players, const Card& a_card, const int& a_index);*/


/**/
/*
 Trick::CardSelected Trick::CardSelected

NAME

        CardSelected - to allow human players to respond to card input prompts

SYNOPSIS

        void Trick::CardSelected(vector<Player*> &a_players, Player* a_hakem, const vector<int>& a_points,
                          const vector<int>& a_tricks, const int& a_randNum, const Suit& a_trumpSuit,
                          Player* a_pl, const Card& a_card);
 
             a_players   --> the match players
             a_hakem     --> the Hakem of this round
             a_points    --> the match points for the players
             a_tricks    --> the round trick scores for the players
             a_randNum   --> the current round number
             a_trumpSuit --> the declared trump suit for this round
             a_pl        --> the player placing the card
             a_card      --> the card to be placed on the table
            
DESCRIPTION

        This function responds to human requests to play a card during the trick.

RETURNS

        None
 
*/
/**/

void Trick::CardSelected(vector<Player*> &a_players, Player* a_hakem, const vector<int>& a_points,
                         const vector<int>& a_tricks, const int& a_randNum, const Suit& a_trumpSuit,
                         Player* a_pl, const Card& a_card)
{
    //identify the player
    int plIndex =  0;
    for(int i=0; i<a_players.size(); i++)
        if (*a_players[i] == *a_pl)
            plIndex = i;
    
    //if selected card is not in hand or not playable
    vector<Card> playableCards = IdentifyPlayableCards(a_players[plIndex]->GetHand());
    if (!(find(playableCards.begin(), playableCards.end(), a_card) != playableCards.end()))
    {
        a_pl->SendMsg(Display::GetInvalidSelectionMsg());
        return;
    }
        
    //place the card on the table
    PlaceCard(a_players, a_card, plIndex);
    
    //inform players about the placement
    for (auto player: a_players)
        player->SendMsg(a_players[plIndex]->GetUsername() + " Placed A Card!\n");
    
    //identify the next player
    plIndex++;
    if (plIndex == a_players.size())
        plIndex = 0;
    
    //prompt next player to make a selection
    PromptPlToPlay(a_players, a_hakem, a_points, a_tricks, a_randNum, a_trumpSuit, plIndex);
}
/*void Trick::CardSelected(vector<Player*> &a_players, Player* a_hakem, const vector<int>& a_points,
 const vector<int>& a_tricks, const int& a_randNum, const Suit& a_trumpSuit,
 Player* a_pl, const Card& a_card);*/


/**/
/*
 Trick::IdentifyPlayableCards Trick::IdentifyPlayableCards

NAME

        IdentifyPlayableCards - to identify the playable cards in a player hand in this turn

SYNOPSIS

        vector<Card> Trick::IdentifyPlayableCards(const vector<Card>& a_hand);
            a_hand --> the collection of cards to choose from
            
DESCRIPTION

        This function identifies all the eligible cards that can be played
        during during this turn.

RETURNS

        All the playable cards in hand
 
*/
/**/

vector<Card> Trick::IdentifyPlayableCards(const vector<Card>& a_hand)
{
    //if the player is starting the trick
    if (m_tableCards.empty())
    {
        //all cards in hand are eligible
        return a_hand;
    }
    
    //otherwise...
    vector<Card> playableCards;
    
    //all cards with the suit led are eligible
    for (int i=0; i<a_hand.size(); i++)
    {
        if (a_hand[i].GetSuit() == m_suitLed)
            playableCards.push_back(a_hand[i]);
    }
    
    //if at least one card with the suit led exists
    if (playableCards.size() != 0)
        return playableCards;
    
    //if there is no card with the suit led, all cards are playable
    return a_hand;
}
/*vector<Card> Trick::IdentifyPlayableCards(const vector<Card>& a_hand);*/
