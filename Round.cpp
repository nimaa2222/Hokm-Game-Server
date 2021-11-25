#include "stdafx.h"

//the number of tricks required to win in 2-player and 4-player mode
static const int m_winTricks = 7;


Round::Round()
{
    //defaults
    m_mode = twoPlayer;
    m_trickScores = {};
    m_hakem = NULL;
    m_trumpSuit = spades;
    m_tricks = {};
    m_winner = {};
}


Round::Round(const Mode& a_mode, Player* a_hakem)
{
    m_mode = a_mode;
    
    //initially all players have 0 tricks
    for (int i=0; i<a_mode; i++)
        m_trickScores.push_back(0);
    
    m_hakem = a_hakem;
    
    //default
    m_trumpSuit = spades;
    
    m_tricks = {};
    m_winner = {};
}


Round::Round(const Round& a_round)
{
    m_mode = a_round.m_mode;
    m_trickScores = a_round.m_trickScores;
    m_hakem = a_round.m_hakem;
    m_trumpSuit = a_round.m_trumpSuit;
    m_tricks = a_round.m_tricks;
    m_winner = a_round.m_winner;
}


/**/
/*
 Round::SetupRound Round::SetupRound

NAME

        SetupRound - to distribute 5 cards to each player and prompt for trump suit selection

SYNOPSIS

        void Round::SetupRound(Deck& a_deck, vector<Player*>& a_players,
                                const vector<int>& a_points, const int& a_roundNum);
 
            a_deck     --> the game deck
            a_players  --> the players in the match
            a_points   --> the player match points
            a_roundNum --> the current round number
            
DESCRIPTION

        This function deals 5 cards to each player and asks Hakem to declare the trump suit.
        If hakem is a computer player, a trump suit will be selected by the computer. Otherwise
        the match will resume once the human player selects the trump suit.

RETURNS

        None
 
*/
/**/

void Round::SetupRound(Deck& a_deck, vector<Player*>& a_players, const vector<int>& a_points, const int& a_roundNum)
{
    //number of cards to deal to each player
    int numCards = 5;
    
    //the number of times to repeat a dealing procedure
    int repetitions = 1;
    
    //deal 5 cards to each player (1 time)
    DealCards(a_deck, a_players, numCards, repetitions);
    
    //Hakem must declare trump suit
    //computer will select a suit
    //human will send its selection to the server and the server will set trump suit for the round
    
    //prompt hakem to select the trump suit
    Suit selectedSuit = m_hakem->SelectTrumpSuit();
    
    //all other players are prompted to wait
    for (auto player: a_players)
    {
        if (player != m_hakem)
        {
            player->SetStage(mustWaitForOtherPls);
            player->SendMsg(Display::m_waitForTrumpSelection);
        }
    }
    
    //if hakem is a computer player
    if (!m_hakem->IsHuman())
    {
        //trump suit is selected and the next necessary action in the match can be taken
        TrumpSuitSelected(a_deck, a_players, a_points, a_roundNum, selectedSuit);
    }
        
    else
    {
        //the next client input is the trump suit selection
        m_hakem->SetStage(mustSelectTrumpSuit);
    }
        
}
/*void Round::SetupRound(Deck& a_deck, vector<Player*>& a_players,
 const vector<int>& a_points, const int& a_roundNum);*/


/**/
/*
 Round::DealCards Round::DealCards

NAME

        DealCards - to deal a certain number of cards to each player a certain number of times

SYNOPSIS

        void Round::DealCards(Deck& a_deck, vector<Player*>& a_players,
                             const int& a_numCards, const int& a_repetitions);
 
            a_deck       --> the game deck
            a_players    --> the players in the match
            a_numCards   --> the number of cards to distribute to each player
            a_roundNum   --> the number of times to distribute cards to each player
            
DESCRIPTION

        This function deals a specified number of cards to each player in the match
        for a specified number of times. The cards are removed from the deck and added
        to the player hands.

RETURNS

        None
 
*/
/**/

void Round::DealCards(Deck& a_deck, vector<Player*>& a_players, const int& a_numCards, const int& a_repetitions)
{
    for (int k=0; k<a_repetitions; k++)
    {
        for (int i=0; i<a_players.size(); i++)
        {
            for (int j=0; j<a_numCards; j++)
            {
                a_players[i]->AddToHand(a_deck.GetCard(0));
                a_deck.RemoveCardByIndex(0);
            }
        }
    }
}
/*void Round::DealCards(Deck& a_deck, vector<Player*>& a_players,
 const int& a_numCards, const int& a_repetitions);*/


/**/
/*
 Round::TrumpSuitSelected Round::TrumpSuitSelected

NAME

        TrumpSuitSelected - to establish the trump suit for the round

SYNOPSIS

        void Round::TrumpSuitSelected(Deck& a_deck, vector<Player*>& a_players,
            const vector<int>& a_points, const int& a_roundNum, const Suit& a_suit);
 
            a_deck     --> the game deck
            a_players  --> the players in the match
            a_points   --> the player match points
            a_roundNum --> the current round number
            a_suit     --> the selected trump suit
            
DESCRIPTION

        This function establishes the trump suit for the round and takes
        the next necessary action in the match. This would be dealing the remaining
        cards to all players in 3-player and 4-player mode or asking players
        to discard two cards from hand in 2-player mode.
 

RETURNS

        None
 
*/
/**/


void Round::TrumpSuitSelected(Deck& a_deck, vector<Player*>& a_players, const vector<int>& a_points,
                              const int& a_roundNum, const Suit& a_suit)
{
    //establish the trump suit for the round
    m_trumpSuit = a_suit;
    
    //the number of cards to deal to each player
    int numCards;
    
    //the number of times to deal the cards
    int repetitions;
    
    if (m_mode == threePlayer || m_mode == fourPlayer)
    {
        numCards = 4;
        
        if (m_mode == threePlayer)
        {
            //deal 4 cards to each player (3 times)
            repetitions = 3;
        }
        
        //if it's 4-player mode
        else
        {
            //deal 4 cards to each player (2 times)
            repetitions = 2;
        }
            
        DealCards(a_deck, a_players, numCards, repetitions);
        
        //at this point the hands are ready for the match
        for (auto player: a_players)
        {
            //sort based on rank and suit
            player->SortHand();
            
            //clear computer memory of cards from the previous round
            player->ClearMemory();
        }
        
        //begin a new trick (each player plays a card)
        StartNewTrick(a_players, a_points, a_roundNum);
    }
    
    //if it's a 2-player game
    else
    {
        //each player discards 2 cards
        //computer will discard 2 cards while human(s) recieves message to discard 2 cards
        for (int i=0; i<m_mode; i++)
            a_players[i]->DiscardCards(m_trumpSuit);
    }
}
/*void Round::TrumpSuitSelected(Deck& a_deck, vector<Player*>& a_players,
 const vector<int>& a_points, const int& a_roundNum, const Suit& a_suit);*/


/**/
/*
 Round::TwoCardsDiscarded Round::TwoCardsDiscarded

NAME

        TwoCardsDiscarded - to discard two cards from hand in 2-player mode and start the pick/discard process

SYNOPSIS

        void Round::TwoCardsDiscarded(Deck& a_deck, vector<Player*>& a_players, const vector<int>& a_points,
        const int& a_roundNum, const Player* a_selector, const Card& a_cardOne, const Card& a_cardTwo);
 
            a_deck     --> the game deck
            a_players  --> the players in the match
            a_points   --> the player match points
            a_roundNum --> the current round number
            a_selector --> the player discarding the 2 cards
            a_cardOne  --> the first card to be discarded from hand
            a_cardTwo  --> the second card to be discarded from hand
            
DESCRIPTION

        This function is called once a human player inputs 2 cards to be discarded from hand.
        The function initiates the pick/discard procedure once both players have
        discarded 2 cards from hand.
 

RETURNS

        None
 
*/
/**/

void Round::TwoCardsDiscarded(Deck& a_deck, vector<Player*>& a_players, const vector<int>& a_points,
                          const int& a_roundNum, const Player* a_selector, const Card& a_cardOne, const Card& a_cardTwo)
{
    //identify the player discarding the card
    int index = 0;
    for (int i=0; i<a_players.size(); i++)
    {
        if (*a_players[i] == *a_selector)
            index = i;
    }
    
    if (a_players[index]->IsHuman())
    {
        //remove both cards from hand
        a_players[index]->RemoveFromHand(a_cardOne);
        a_players[index]->RemoveFromHand(a_cardTwo);
        
        //the position on the screen to display the hand
        int position = 0;
        
        //display the player's hand after removing the cards
        Server::SendMsg(a_selector->GetSocket(), Display::GetHandDisplay(a_selector->GetHand(), position, a_selector->GetHandSize(), false));
        
        //inform player about potential wait for the other player
        Server::SendMsg(a_players[index]->GetSocket(), Display::GetSuccessDiscardCardsMsg());
        
        //the player must now wait for the other player
        a_players[index]->SetStage(mustWaitForOtherPls);
    }
    
    //5 cards - 2 cards = 3 cards
    bool discardCompleted = true;
    for (int i=0; i<a_players.size(); i++)
        if (a_players[i]->GetHandSize() != 3)
            discardCompleted = false;
    
    //if both players have discarded the 2 cards
    if (discardCompleted)
    {
        //let both players know that they will now build their hands
        string msg = Display::GetBuildHandMsg();
        for (int i=0; i<a_players.size(); i++)
            a_players[i]->SendMsg(msg);
        
        //inform about the wait
        for (auto player: a_players)
            if (player != m_hakem)
                player->SendMsg(Display::m_waitForHakemPickDiscard);
        
        //offer the very first card to Hakem
        int hakemIndex = 0;
        
        //prompt hakem to pick or discard the very first card
        AskPlayerToPickDiscard(a_deck, a_players, a_points, a_roundNum, hakemIndex);
    }
}
/* void Round::TwoCardsDiscarded(Deck& a_deck, vector<Player*>& a_players, const vector<int>& a_points,
 const int& a_roundNum, const Player* a_selector, const Card& a_cardOne, const Card& a_cardTwo);*/


/**/
/*
 Round::AskPlayerToPickDiscard Round::AskPlayerToPickDiscard

NAME

        AskPlayerToPickDiscard - to allow a player to pick/discard a card and then promopt the next player for the same action

SYNOPSIS

        void Round::AskPlayerToPickDiscard(Deck& a_deck, vector<Player*>& a_players,
                    const vector<int>& a_points, const int& a_roundNum, int a_index);
 
            a_deck     --> the game deck
            a_players  --> the players in the match
            a_points   --> the player match points
            a_roundNum --> the current round number
            a_index    --> the index of the player to pick/discard a card
            
DESCRIPTION

        This function prompts a player to pick or discard a given card. If the player
        is a computer player, a the card is either picked or discarded. Otherwise,
        the human player will be sent a prompt to pick or discard a card from hand.
 
RETURNS

        None
 
*/
/**/

void Round::AskPlayerToPickDiscard(Deck& a_deck, vector<Player*>& a_players, const vector<int>& a_points,
                                const int& a_roundNum, int a_index)
{
    //the other player must wait for this player
    for (auto player: a_players)
        if (player != a_players[a_index])
            player->SetStage(mustWaitForOtherPls);
    
    //this player must pick or discard
    a_players[a_index]->SetStage(mustPickOrDiscard);
    
    //identify the card being offered to the player
    Card optionOne = a_deck.GetTopDeck();
    
    //prompt player to pick or discard the card
    Decision result = a_players[a_index]->PickOrDiscard(optionOne, m_trumpSuit);
    
    //if player is a computer player
    if (!a_players[a_index]->IsHuman())
    {
        //update the deck and hand based on the computer decision
        HandlePickDiscardDecision(a_deck, a_players, a_points, a_roundNum, result, a_index);
        
        //identify the next player
        if (a_index == 0)
            a_index = 1;
        else
            a_index = 0;
        
        //if the pick/discard procedure is completed
        if (a_deck.GetCurrentSize() == 2)
            return;
        
        //prompt next player to pick/discard (must be human since this player is computer)
        optionOne = a_deck.GetTopDeck();
        a_players[a_index]->PickOrDiscard(optionOne, m_trumpSuit);
        
        //other clients must wait
        for (auto player: a_players)
            if (player != a_players[a_index])
                player->SetStage(mustWaitForOtherPls);
        
        //the client's next input must be their pick/discard decision
        a_players[a_index]->SetStage(mustPickOrDiscard);
    }
}
/*void Round::AskPlayerToPickDiscard(Deck& a_deck, vector<Player*>& a_players,
 const vector<int>& a_points, const int& a_roundNum, int a_index);*/


/**/
/*
 Round::HandlePickDiscardDecision Round::HandlePickDiscardDecision

NAME

        HandlePickDiscardDecision - to pick/discard a card and update the deck based on the decision

SYNOPSIS

        void Round::HandlePickDiscardDecision(Deck& a_deck, vector<Player*>& a_players, const vector<int>& a_points,
                    const int& a_roundNum, const Decision& a_decision, const int& a_index);
 
            a_deck     --> the game deck
            a_players  --> the players in the match
            a_points   --> the player match points
            a_roundNum --> the current round number
            a_decision --> the decision as to pick or discard the card
            a_index    --> the player making the decision
            
DESCRIPTION

        This function, based on the player's decision, either adds the
        first or second card on the deck to the hand. Both cards are
        discarded from the deck afterwards.
 

RETURNS

        None
 
*/
/**/

void Round::HandlePickDiscardDecision(Deck& a_deck, vector<Player*>& a_players, const vector<int>& a_points,
                         const int& a_roundNum, const Decision& a_decision, const int& a_index)
{
    //the card on top of the deck
    Card optionOne = a_deck.GetTopDeck();
    
    //the card after the card on top of the card
    Card optionTwo = a_deck.GetCard(1);
    
    //if player decided to take the card
    if (a_decision == yes)
    {
        //add the card on top of the deck to the hand
        a_players[a_index]->AddToHand(optionOne);
    }
        
    else
    {
        //add the second card on the deck to the hand
        a_players[a_index]->AddToHand(optionTwo);
    }
        
    //remove both cards from deck
    a_deck.RemoveCard(optionOne);
    a_deck.RemoveCard(optionTwo);
}
/*void Round::HandlePickDiscardDecision(Deck& a_deck, vector<Player*>& a_players, const vector<int>& a_points,
 const int& a_roundNum, const Decision& a_decision, const int& a_index);*/


/**/
/*
 Round::CardPickedDiscarded Round::CardPickedDiscarded

NAME

        CardPickedDiscarded - to allow human player input their decision for picking or discarding a card

SYNOPSIS

        void Round::CardPickedDiscarded(Deck& a_deck, vector<Player*>& a_players,
        const vector<int>& a_points, const int& a_roundNum, const Player* a_player,
        const Decision& a_selection);
 
            a_deck      --> the game deck
            a_players   --> the players in the match
            a_points    --> the player match points
            a_roundNum  --> the current round number
            a_player    --> the player making the decision (pick/discard)
            a_selection --> the decision made by the player (pick/discard)
            
DESCRIPTION

        This function is called once human player inputs their selection as to whether
        they would like to pick or discard the card that is offered to them from the deck.
        The player hand and the deck get updated accordingly.
 

RETURNS

        None
 
*/
/**/

void Round::CardPickedDiscarded(Deck& a_deck, vector<Player*>& a_players, const vector<int>& a_points,
                                const int& a_roundNum, const Player* a_player, const Decision& a_selection)
{
    //players alternate for selecting cards
    //repeats until each player has 13 cards
    //20 times total --> 10 times each player
    
    //identify the player making the decision
    int index = 0;
    for (int i=0; i<a_players.size(); i++)
    {
        if (*a_players[i] == *a_player)
            index = i;
    }
        
    //update the deck and hand based on the decision
    HandlePickDiscardDecision(a_deck, a_players, a_points, a_roundNum, a_selection, index);
        
    //the position on the screen to display the player hand
    int position = 0;
    
    //display the hand with the card added
    string msg = Display::GetHandDisplay(a_players[index]->GetHand(), position, a_players[index]->GetHandSize(), false);
    a_players[index]->SendMsg(msg);
    
    //if the pick/discard procedure is completed
    if (a_deck.GetCurrentSize() == 2)
        return;
    
    //inform current player to wait for next player to pick/discard
    a_players[index]->SendMsg(Display::GetCardAddMsg());
        
    //identify next player
    if (index == 1)
        index = 0;
    else
        index = 1;
    
    //prmopt the other player to pick/discard the next set of cards
    AskPlayerToPickDiscard(a_deck, a_players, a_points, a_roundNum, index);
}
/*void Round::CardPickedDiscarded(Deck& a_deck, vector<Player*>& a_players,
 const vector<int>& a_points, const int& a_roundNum, const Player* a_player,
 const Decision& a_selection);*/


/**/
/*
 Round::StartNewTrick Round::StartNewTrick

NAME

        StartNewTrick - to prepare and initiate a new trick where each player plays a card

SYNOPSIS

        void Round::StartNewTrick(vector<Player*>& a_players, const vector<int>& a_points,
        const int& a_roundNum);
 
            a_players   --> the players in the match
            a_points    --> the player match points
            a_roundNum  --> the current round number
            
DESCRIPTION

        This function identifies the first player to play a card in the trick that
        it will initiate. It prompts the very first player to play a card to start
        off the trick.
 

RETURNS

        None
 
*/
/**/

void Round::StartNewTrick(vector<Player*>& a_players, const vector<int>& a_points, const int& a_roundNum)
{
    //the index of player to place the first card in the trick
    int trickStarterIndex = 0;
    
    //if this is the first trick of the round
    if (m_tricks.size() == 0)
    {
        //Hakem will be the player to place the first card
        for (int i=0; i<a_players.size(); i++)
            if (*a_players[i] == *m_hakem)
                trickStarterIndex = i;
    }
    
    //if this is not the first trick of the round
    else
    {
        //the winner of the previous trick starts this trick
        for (int i=0; i<a_players.size(); i++)
            if (*a_players[i] == *this->GetCurrentTrick().GetWinner())
                trickStarterIndex = i;
    }
    
    //initiate a new trick
    Trick newTrick;
    m_tricks.push_back(newTrick);
    
    //prompt the first player to play a card (this will start the chain of calls on players to play cards)
    GetCurrentTrick().PromptPlToPlay(a_players, m_hakem, a_points, m_trickScores, a_roundNum, m_trumpSuit, trickStarterIndex);
}
/*void Round::StartNewTrick(vector<Player*>& a_players, const vector<int>& a_points,
 const int& a_roundNum);*/


/**/
/*
 Round::UpdateTrickScores Round::UpdateTrickScores

NAME

        UpdateTrickScores - to identify and update the trick scores of the trick winner

SYNOPSIS

        void Round::UpdateTrickScores(const vector<Player*>& a_players);
            a_players  --> the players in the match
            
DESCRIPTION

        This function will identify the trick winner and update their score. It will
        also update the trick winner's partner score if applicable (i.e. if 4-player mode).
 

RETURNS

        None
 
*/
/**/

void Round::UpdateTrickScores(const vector<Player*>& a_players)
{
    //identify the trick winner
    int trickStarterIndex = 0;
    for (int i=0; i<a_players.size(); i++)
        if (*a_players[i] == *GetCurrentTrick().GetWinner())
            trickStarterIndex = i;
    
    //give a point to the player that won the trick
    m_trickScores[(PlayerNum)trickStarterIndex]++;
    
    if (m_mode == fourPlayer)
    {
        //identify winner's partner
        Player* winnerPartner = a_players[(PlayerNum)trickStarterIndex]->GetPartner();
        
        //give a point to the winner's partner
        for (int i=0; i<fourPlayer; i++)
            if (*a_players[i] == *winnerPartner)
                m_trickScores[i]++;
    }
}
/*void Round::UpdateTrickScores(const vector<Player*>& a_players);*/


/**/
/*
 Round::IdentifyRoundWinner Round::IdentifyRoundWinner

NAME

        IdentifyRoundWinner - to check whether the round is over and identify the winner if it is

SYNOPSIS

        bool Round::IdentifyRoundWinner(const vector<Player*> a_players);
            a_players  --> the players in the match
            
DESCRIPTION

        This function, based on the trick scores and the match mode, checks if
        the round is completed. If the round is completed, it will identify the
        winner(s) of the round.
 

RETURNS

        Returns true if the round is over
        Returns false otherwise
 
*/
/**/

bool Round::IdentifyRoundWinner(const vector<Player*> a_players)
{
    //set to true if the round is completed
    bool roundOver = false;
    
    if (m_mode == twoPlayer || m_mode == fourPlayer)
    {
        for (int i=0; i<m_mode; i++)
        {
            //7 tricks required to win in 2-player and 4-player mode
            if (m_trickScores[i] == m_winTricks)
            {
                //establish the winner
                m_winner.push_back(a_players[i]);
                roundOver = true;
            }
        }
    }
    
    //if it's 3-player mode
    else
    {
        //if a player takes all the first 7 tricks, they win (check first)
        
        //if there is a player with 7 tricks
        if (count(m_trickScores.begin(), m_trickScores.end(), m_winTricks))
        {
            vector<int> copyTricks = m_trickScores;
                
            //remove the 7 tricks from the tricks list
            copyTricks.erase(remove(copyTricks.begin(), copyTricks.end(), m_winTricks), copyTricks.end());
                
            //now the remaining values must be zeros
            bool nonZero = false;
            for (int i=0; i<copyTricks.size(); i++)
                if (copyTricks[i] != 0)
                    nonZero = true;
                
            //if all remaining tricks are 0
            if (!nonZero)
            {
                //the player wins by taking the first 7 tricks
                for (int i=0; i<m_trickScores.size(); i++)
                    if (m_trickScores[i] == m_winTricks)
                        m_winner.push_back(a_players[i]);
                
                return true;
            }
        }
        
        //the player with a number of tricks that cannot be beaten wins (check second)
        
        //the total number of tricks in a 3-player mode
        int maxTricks = 17;
        
        //the total number of tricks played so far
        int tricksPlayed = accumulate(m_trickScores.begin(), m_trickScores.end(), 0);
        
        //the number of tricks that can be played afterwards
        int tricksRemaining = maxTricks - tricksPlayed;
        
        //the current maximum tricks of a player
        int highestTrick = *max_element(m_trickScores.begin(), m_trickScores.end());
        
        //identify the index of the the player with the highest trick
        vector<int>::iterator it = find(m_trickScores.begin(), m_trickScores.end(), highestTrick);
        int winnerIndex = (int)distance(m_trickScores.begin(), it);
        
        //move this player to the beginning of the vector (if it is not already)
        swap(m_trickScores[winnerIndex], m_trickScores[0]);
        
        //if there is a number of tricks that cannot be beaten (is equal or more than 7 tricks e.g. 7-4-5, 8-6-2)
        if ( (tricksRemaining + m_trickScores[1] < highestTrick) && (tricksRemaining + m_trickScores[2] < highestTrick) )
        {
            //restore original ordering
            swap(m_trickScores[winnerIndex], m_trickScores[0]);
            
            //establish winner
            m_winner.push_back(a_players[winnerIndex]);
            roundOver = true;
        }
        
        else
        {
            //if two of players take the same possible number of tricks (i.e. 6-6-5, 7-7-3, 8-8-1)
            //the player with the lower tricks wins the round (check third)
            
            //restore original ordering
            swap(m_trickScores[winnerIndex], m_trickScores[0]);
            
            //if all player hands are empty
            if (tricksPlayed == maxTricks)
            {
                //if the first and second player are tied up
                if (m_trickScores[playerOne] == m_trickScores[playerTwo])
                {
                    roundOver = true;
                    
                    //the third player wins
                    m_winner.push_back(a_players[playerThree]);
                }
                
                //if the first and third player are tied up
                else if (m_trickScores[playerOne] == m_trickScores[playerThree])
                {
                    roundOver = true;
                    
                    //the second player wins
                    m_winner.push_back(a_players[playerTwo]);
                }
                
                //if the second and third player are tied up
                else if (m_trickScores[playerTwo] == m_trickScores[playerThree])
                {
                    roundOver = true;
                    
                    //the first player wins
                    m_winner.push_back(a_players[playerOne]);
                }
            }
        }
    }
    
    return roundOver;
}
/*bool Round::IdentifyRoundWinner(const vector<Player*> a_players);*/


/**/
/*
 Round::IdentifyPointsScored Round::IdentifyPointsScored

NAME

        IdentifyPointsScored - to identify the points scored by the winner(s) in this round

SYNOPSIS

        int Round::IdentifyPointsScored(const vector<Player*> a_players);
            a_players  --> the players in the match
            
DESCRIPTION

        This function checks whether the winner(s) scored 1, 2, or 3 points during this round
        based on the number of tricks that players have at the end of the round.

RETURNS

        Returns the number of points scored by the winner(s)
 
*/
/**/

int Round::IdentifyPointsScored(const vector<Player*> a_players)
{
    //winning first 7 tricks as non-Hakem or non-Hakem team --> 3 points
    //winning first 7 tricks as Hakem or Hakem team --> 2 points
    //everything else --> 1 point
    
    //if there is a player with 7 tricks
    if (count(m_trickScores.begin(), m_trickScores.end(), m_winTricks))
    {
        vector<int> copyTricks = m_trickScores;
            
        //remove the 7 tricks from the tricks list
        copyTricks.erase(remove(copyTricks.begin(), copyTricks.end(), m_winTricks), copyTricks.end());
            
        //now the remaining values must be zeros
        bool nonZero = false;
        for (int i=0; i<copyTricks.size(); i++)
            if (copyTricks[i] != 0)
                nonZero = true;
            
        //if all remaining tricks are 0
        if (!nonZero)
        {
            //to check whether Hakem won the round (or was in the winning team)
            bool hakemWon = false;
            for (int i=0; i<m_winner.size(); i++)
                if (*m_winner[i] == *m_hakem)
                    hakemWon = true;
            
            //if winner is not Hakem (or Hakem is not in winning team)
            if (!hakemWon)
            {
                //winner scored 3 points
                return m_hakemKoti;
            }
            else
            {
                //winner scored 2 points
                return m_kot;
            }
        }
    }
    
    //winner scored 1 point
    return 1;
}
/*int Round::IdentifyPointsScored(const vector<Player*> a_players);*/
