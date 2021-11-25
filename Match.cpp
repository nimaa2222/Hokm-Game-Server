#include "stdafx.h"

//the number of points required to win the match
static const int m_winPoints = 7;

Match::Match(const Mode& a_mode, Player* a_creator)
{
    m_mode = a_mode;
    m_deck = Deck();
    m_players.push_back(a_creator);
    m_rounds = {};
    
    //initially all players have 0 points
    for (int i=0; i<a_mode; i++)
        m_points.push_back(0);
    
    m_winner = {};
}

Match::Match(const Match& a_game)
{
    m_mode = a_game.m_mode;
    m_deck = a_game.m_deck;
    m_players = a_game.m_players;
    m_rounds = a_game.m_rounds;
    m_points = a_game.m_points;
    m_winner = a_game.m_winner;
}

//to find a player by their username
Player* Match::GetPlByUsername(const string& a_username)
{
    Player* player = NULL;
    for (auto pl: m_players)
        if (pl->GetUsername() == a_username)
            player = pl;
    
    return player;
}

//to identify the number of human players in the match
int Match::GetNumHumanPls() const
{
    int numPl = 0;
    for (auto player: m_players)
        if (player->IsHuman())
            numPl++;
    
    return numPl;
}

//to remove a player from the match
void Match::RemovePlayer(const string& a_username)
{
    for (int i=0; i<m_players.size(); i++)
        if (m_players[i]->GetUsername() == a_username)
            m_players.erase(m_players.begin() + i);
}

//to check whether a given player is in this match
bool Match::IsInMatch(const string& a_username) const
{
    for (auto pl: m_players)
        if (pl->GetUsername() == a_username)
            return true;
    
    return false;
}


/**/
/*
 Match::ReplacePlayer Match::ReplacePlayer

NAME

        ReplacePlayer - to replace an existing player with a different player

SYNOPSIS

        void Match::ReplacePlayer(const string& a_username, Player* a_player);
            a_username --> the username of the player to be removed
            a_player   --> the player to replace the removed player
            
DESCRIPTION

        This function replaces a match player with a different player and "hands over"
        the player's hand and partner to the new player.

RETURNS

        None
 
*/
/**/

void Match::ReplacePlayer(const string& a_username, Player* a_player)
{
    //find the player
    int plIndex = GetPlayerIndex(a_username);
    
    //get current player's hand and partner (if applicable)
    m_players[plIndex]->SetSocket(0);
    vector<Card> hand = m_players[plIndex]->GetHand();
    Player* partner = m_players[plIndex]->GetPartner();
    
    //restore NPC username
    a_player->SetPartner(partner);
    a_player->SetHand(hand);
    
    //replace with this player
    m_players[plIndex] = a_player;
}
/*void Match::ReplacePlayer(const string& a_username, Player* a_player);*/


/**/
/*
 Match::HandleMatchMove Match::HandleMatchMove

NAME

        HandleMatchMove - to allow a player to take actions in different stages of the match
 
SYNOPSIS

        bool Match::HandleMatchMove(const string& a_username,
                                const ClientStage& a_stage, const string& a_input);
            a_username --> the username of the player making a move
            a_stage    --> the client's stage in the match
            a_input    --> the human player input
            
DESCRIPTION

        This function responds to player input by taking appropriate actions based on the
        stage of the player in the match.

RETURNS

        Returns true if the match is over
        Returns false otherwise
*/
/**/

bool Match::HandleMatchMove(const string& a_username, const ClientStage& a_stage,
                            const string& a_input)
{
    //identify the player in the match with this username
    Player* pl = NULL;
    
    for (int i=0; i<m_players.size(); i++)
    {
        if (m_players[i]->GetUsername() == a_username)
            pl = m_players[i];
    }
    
    ClientStage stage;
    
    //if a computer player is making a move
    if (a_username.find("NPC") == 0)
    {
        //stage is passed in
        stage = a_stage;
    }
    
    else
    {
        //identify the stage of the player
        stage = pl->GetStage();
    }
    
    //if the player is waiting for other players to make a move
    if (stage == mustWaitForOtherPls)
    {
        //inform player that they must keep waiting
        Server::SendMsg(pl->GetSocket(), Display::GetWaitForOtherPlsMsg());
    }
        
    //if client has been prompted to select the trump suit for the round
    else if (stage == mustSelectTrumpSuit)
    {
        if (a_input.size() != 1)
        {
            //inform client about the invalid input
            Server::SendMsg(pl->GetSocket(), Display::GetInvalidSelectionMsg());
            return false;
        }
        
        char suitInput = toupper(a_input[0]);
        
        //if input is invalid
        if (suitInput != 'S' && suitInput != 'C' && suitInput != 'H' && suitInput != 'D')
        {
            //inform client about the invalid input
            Server::SendMsg(pl->GetSocket(), Display::GetInvalidSelectionMsg());
            return false;
        }
        
        //set the trump suit for the current round of this match and take the next necessary actions
        GetCurrentRound().TrumpSuitSelected(m_deck, m_players, m_points, (int)m_rounds.size(), suitChar[suitInput]);
    }
    
    //if client has been prompted to discard 2 cards
    else if (stage == mustDiscardTwoCards)
    {
        //if there is no comma in the input or there is space in the input
        if (a_input.find(',') == string::npos || a_input.find(' ') != string::npos)
        {
            //inform client about the invalid input
            Server::SendMsg(pl->GetSocket(), Display::GetInvalidSelectionMsg());
            return false;
        }
        
        //parse the 2 discarded cards
        string cardOne = a_input.substr(0, a_input.find(",", 0));
        string cardTwo = a_input.substr(a_input.find(",") + 1);
        
        //if the cards are not inputted correctly
        if (!Card::CardIsValid(cardOne) || !Card::CardIsValid(cardTwo))
        {
            //inform client about the invalid input
            Server::SendMsg(pl->GetSocket(), Display::GetInvalidSelectionMsg());
            return false;
        }
        
        //establish the cards
        Card firstCard = Card::ParseCard(cardOne);
        Card secondCard = Card::ParseCard(cardTwo);
        
        //if cards are not in hand
        if (!pl->IsInHand(firstCard) || !pl->IsInHand(secondCard))
        {
            //inform client about the invalid input
            Server::SendMsg(pl->GetSocket(), Display::GetInvalidSelectionMsg());
            return false;
        }
        
        //discard the cards from the player hands and take the next necessary actions in the match
        GetCurrentRound().TwoCardsDiscarded(m_deck, m_players, m_points, (int)m_rounds.size(), pl, firstCard, secondCard);
    }
    
    //if client has been prompted to pick or discard a card for building hand
    else if (stage == mustPickOrDiscard)
    {
        //if the input is not one charcater or not numeric
        if (a_input.size() != 1 || !isdigit(a_input[0]))
        {
            //inform client about the invalid input
            Server::SendMsg(pl->GetSocket(), Display::GetInvalidSelectionMsg());
            return false;
        }
        
        //establish the selection
        Decision selection = (Decision) (a_input[0] - '0');
        
        //if user did not select a valid option
        if (selection != yes && selection != no)
        {
            //inform client about the invalid input
            Server::SendMsg(pl->GetSocket(), Display::GetInvalidSelectionMsg());
            return false;
        }
        
        //pick or discard the card for this player and take the next necessary actions in the match
        GetCurrentRound().CardPickedDiscarded(m_deck, m_players, m_points, (int)m_rounds.size(), pl, selection);
        
        //if there is only 2 cards left in the deck (i.e. the pick/discard procedure is completed)
        if (m_deck.GetCurrentSize() == 2)
        {
            //the last 2 cards are discarded
            m_deck.RemoveCardByIndex(1);
            m_deck.RemoveCardByIndex(0);
            
            for (auto player: m_players)
            {
                //sort hand based on suit and rank
                player->SortHand();
                
                //clear the computer memory of the cards played in the previous round
                player->ClearMemory();
            }
            
            //the round can now be started
            GetCurrentRound().StartNewTrick(m_players, m_points, (int)m_rounds.size());
        }
    }
    
    //if client is prompted to play a card during a trick
    else if (stage == mustSelectCard)
    {
        //if the card input is invalid
        if (!Card::CardIsValid(a_input))
        {
            //inform client about the invalid input
            Server::SendMsg(pl->GetSocket(), Display::GetInvalidSelectionMsg());
            return false;
        }
        
        //establish the selected card
        Card selectedCard = Card::ParseCard(a_input);
        
        //play the card for this player in the current trick of this match and take the next necessary actions
        GetCurrentRound().GetCurrentTrick().CardSelected(m_players, GetCurrentRound().GetHakem(),m_points,
                                                               GetCurrentRound().GetTrickScores(), (int)m_rounds.size(),
                                                               GetCurrentRound().GetTrumpSuit(), pl, selectedCard);
        //if the trick is over
        if (GetCurrentRound().GetCurrentTrick().GetTableCards().size() == m_mode)
        {
            //update the trick scores
            GetCurrentRound().UpdateTrickScores(m_players);
            
            //if the round is not over
            if (!GetCurrentRound().IdentifyRoundWinner(m_players))
            {
                //start another trick
                GetCurrentRound().StartNewTrick(m_players, m_points, (int)m_rounds.size());
            }
            
            //if the round is over
            else
            {
                //update the game points using number of points scored during the round
                PerformAfterRound();
                
                //if match is not completed
                if (!MatchIsOver())
                {
                    //start the next round
                    StartNewRound();
                }
                    
                else
                {
                    //identify and declare winner
                    PerformAfterMatch();
                    return true;
                }
            }
        }
    }
    
    //i.e. eneterd match but still not prompted for anything
    else
        Server::SendMsg(pl->GetSocket(), "\nYou Have Just Entered A Match! Please Wait!\n");
    
    return false;
}
/*bool Match::HandleMatchMove(const string& a_username, const ClientStage& a_stage,
 const string& a_input)*/


/**/
/*
 Match::StartNewRound Match::StartNewRound

NAME

        StartNewRound - to preapre and start a new round
 
SYNOPSIS

        void Match::StartNewRound();
            
DESCRIPTION

        This function identifies the Hakem for the round, deals 5 cards to each player,
        prompts Hakem for trump suit selection and starts off the round.

RETURNS

        None
 
*/
/**/

void Match::StartNewRound()
{
    //identify the hakem for this round and empty the player hands
    int hakemIndex = SetupMatch();
    
    //start a new round (specifying mode and the Hakem)
    Round newRound(m_mode, m_players[hakemIndex]);
    m_rounds.push_back(newRound);
    
    //deal 5 cards to each player and prompt for trump suit selection
    m_rounds.back().SetupRound(m_deck, m_players, m_points, (int)m_rounds.size());
}
/*void Match::StartNewRound();*/


/**/
/*
 Match::SetupMatch Match::SetupMatch

NAME

        SetupMatch - to identify the Hakem for the round and set up the game deck for the match
 
SYNOPSIS

        int Match::SetupMatch();
            
DESCRIPTION

        This function identifies the Hakem based on the round number. It also
        generates and shuffles the deck for this match.

RETURNS
 
        Returns the index of the Hakem in the list of match players
 
*/
/**/

int Match::SetupMatch()
{
    int hakemIndex = 0;
    
    //identify the Hakem for the first round
    if (m_rounds.size() == 0)
    {
        //repeatedly swap the cards in the deck
        m_deck.Shuffle();
        
        //hakem is the first player to recieve an Ace
        IdentifyInitialHakem();
    
        hakemIndex = 0;
    }
    
    else
    {
        //identify the winner of the previous round
        vector<Player*> roundWinners = GetCurrentRound().GetWinner();
        
        if (m_mode == twoPlayer || m_mode == threePlayer)
        {
            //the winner of the previous round is Hakem for this round
            hakemIndex = GetPlayerIndex(roundWinners[0]->GetUsername());
        }
            
        //if 4-player mode
        else
        {
            //identify the Hakem's index for the previous round
            Player* lastHakem = GetCurrentRound().GetHakem();
            for (int i=0; i<m_players.size(); i++)
                if (*m_players[i] == *lastHakem)
                    hakemIndex = i;
            
            //if hakem's team did not win
            if (roundWinners[0] != m_players[hakemIndex] && roundWinners[1] != m_players[hakemIndex])
            {
                //the next player becomes Hakem
                hakemIndex++;
            }
            
            //go back to the first player
            if (hakemIndex == m_mode)
                hakemIndex = 0;
        }
        
        //create a new deck (since current deck is empty after playing the round)
        m_deck = Deck();
        
        //shuffle the deck for the next round
        m_deck.Shuffle();
    }
    
    //empty all player hands
    for (auto player: m_players)
        player->EmptyHand();
    
    return hakemIndex;
}
/*int Match::SetupMatch();*/


/**/
/*
 Match::IdentifyInitialHakem Match::IdentifyInitialHakem

NAME

        IdentifyInitialHakem - to identify the Hakem for the very first round
 
SYNOPSIS

        Player* Match::IdentifyInitialHakem();
            
DESCRIPTION

        This function identifies the Hakem for the first round (the first player
        to recieve an Ace while distributing cards to players). It also identifies
        the Hakem's partner in 4-player mode (the next person to recieve an Ace
        while distributing the cards to the remaining players).

RETURNS

        Returns the Hakem for the first round
 
*/
/**/

Player* Match::IdentifyInitialHakem()
{
    //let all clients know Hakem is being determined
    for (auto pl: m_players)
        pl->SendMsg(Display::GetHakemDeterminationMsg());
    
    //the Hakem of the first round (TBD)
    Player* hakem = NULL;
    
    if (m_mode == twoPlayer)
    {
        //the first player to recieve an ace becomes Hakem
        hakem = IdentifyAcePlayer(m_deck, m_players);
        
        //position Hakem as first player
        if (*m_players[0] != *hakem)
            swap(m_players, 0, 1);
    }
    
    else if (m_mode == threePlayer)
    {
        //remove one of 2s from the game deck
        
        //set to true when a 2 is found
        bool cardFound = false;
        
        //the index of the card in the deck
        int deckIndex = 0;
        
        //the rank to be removed
        int rankToRemoved = 2;
        
        //remove the card from the deck
        while (!cardFound)
        {
            if (m_deck.GetCard(deckIndex).GetRank() == rankToRemoved)
            {
                cardFound = true;
                m_deck.RemoveCardByIndex(deckIndex);
            }
            
            deckIndex++;
        }
        
        //the first player to recieve an ace becomes Hakem
        hakem = IdentifyAcePlayer(m_deck, m_players);
        
        //position Hakem as first player
        if (*m_players[0] != *hakem)
        {
            if (*m_players[1] == *hakem)
                swap(m_players, 0, 1);
            
            //if hakem = m_Players[2]
            else
                swap(m_players, 0, 2);
        }
    }
    
    else if (m_mode == fourPlayer)
    {
        //the first player to recieve an ace becomes Hakem
        hakem = IdentifyAcePlayer(m_deck, m_players);
        
        //inform clients about who the Hakem is
        Display::GetHakemDeclarationMsg(hakem->GetUsername());
        
        //swap the cards in the deck
        m_deck.Shuffle();
        
        //inform clients about the fact that Hakem's partner is now being determined
        Display::GetPartnerDeterminationMsg();
        
        for (int i=0; i<m_mode; i++)
            m_players[i]->EmptyHand();
        
        //make a copy of the players
        vector <Player*> players = m_players;
        
        //remove Hakem from the list
        players.erase(remove(players.begin(), players.end(), hakem), players.end());
        Player* hakemPartner;
        
        //the next player to recieve an ace becomes Hakem's partner
        hakemPartner = IdentifyAcePlayer(m_deck, players);
        
        //inform clients about who the Hakem's partner is
        Display::GetPartnerDeclarationMsg(hakemPartner->GetUsername());
        
        //remove Hakem's partner from the list
        players.erase(remove(players.begin(), players.end(), hakemPartner), players.end());
        
        //establish partnership for Hakem team
        hakem->SetPartner(hakemPartner);
        hakemPartner->SetPartner(hakem);
        
        //establish partnership for other team
        Player* otherPlOne = players[0];
        Player* otherPlTwo = players[1];
        
        otherPlOne->SetPartner(otherPlTwo);
        otherPlTwo->SetPartner(otherPlOne);
        
        //rearrange players based on Hakem and partnership
        m_players[playerOne] = hakem;
        m_players[playerTwo] = otherPlOne;
        m_players[playerThree] = hakemPartner;
        m_players[playerFour] = otherPlTwo;
    }
    
    if (m_mode != fourPlayer)
    {
        for (auto pl: m_players)
            pl->SendMsg(Display::GetHakemDeclarationMsg(hakem->GetUsername()));
    }
    
    return hakem;
}
/*Player* Match::IdentifyInitialHakem();*/


/**/
/*
 Match::IdentifyAcePlayer Match::IdentifyAcePlayer

NAME

        IdentifyAcePlayer - to distribute cards to players and identify the first player to recieve an Ace
 
SYNOPSIS

        Player* Match::IdentifyAcePlayer(const Deck a_deck, vector <Player*> a_players);
            a_deck    --> the game deck
            a_players --> the match players
            
DESCRIPTION

        This function distributes cards to each player (one at a time) until a player
        recieves an Ace.

RETURNS

        The first player that recieved an Ace
 
*/
/**/

Player* Match::IdentifyAcePlayer(const Deck a_deck, vector <Player*> a_players)
{
    //the index of the card in the game deck
    int deckIndex = 0;
    
    //the index of players in the match
    int playerIndex = 0;
    
    //set to true once a player recieves an ace
    bool aceFound = false;
    
    //the first player to recieve an ace
    Player* aceReciever = NULL;
    
    while (!aceFound)
    {
        //add card to player hand
        a_players[playerIndex]->AddToHand(a_deck.GetCard(deckIndex));
        
        //if the card is an ace
        if (a_deck.GetCard(deckIndex).GetRank() == ace)
        {
            aceFound = true;
            aceReciever = a_players[playerIndex];
        }
        
        deckIndex++;
        playerIndex++;
        
        //if all players recieved a card, go back to the first player
        if (playerIndex == a_players.size())
            playerIndex = 0;
    }
    
    //send the display for the card each player is recieving
    for (auto pl: m_players)
        pl->SendMsg(Display::GetHakemDeterminationDisplay(a_players));
    
    return aceReciever;
}
/*Player* Match::IdentifyAcePlayer(const Deck a_deck, vector <Player*> a_players);*/


//to get the index of a specific player (username lookup)
int Match::GetPlayerIndex(const string& a_username) const
{
    int index = 0;
    for (int i=0; i<m_players.size(); i++)
        if (m_players[i]->GetUsername() == a_username)
            index = i;
    
    return index;
}


/**/
/*
 Match::PerformAfterRound Match::PerformAfterRound

NAME

        PerformAfterRound - to perform a series of actions after a round is completed
 
SYNOPSIS

        void Match::PerformAfterRound();
            
DESCRIPTION

        This function clears the game table to declare the round winner. It also
        updates the player points based on the number of points scored during
        the round.

RETURNS

        None
 
*/
/**/

void Match::PerformAfterRound()
{
    //clear the table
    vector<Card> emptyTable;
    
    //display the final state of the round to all clients
    for (auto pl: m_players)
    {
        string display = Display::GetMatchDisplay(m_players, GetCurrentRound().GetHakem(), emptyTable, m_points,
        GetCurrentRound().GetTrickScores(), (int)m_rounds.size(), GetCurrentRound().GetTrumpSuit(), pl->GetUsername());
        
        pl->SendMsg(display);
    }
    
    //identify and declare the round winner(s)
    vector<Player*> roundWinners = GetCurrentRound().GetWinner();
    
    for (auto pl: m_players)
    {
        string msg = Display::GetWinnerDeclarationMsg(roundWinners, ROUND, pl->GetUsername());
        pl->SendMsg(msg);
    }
        
    
    //identify the number of points scored during the round
    int pointsScored = GetCurrentRound().IdentifyPointsScored(m_players);
    
    //update the game points using the points scored during the round
    for (int i=0; i<m_mode; i++)
    {
        for (int j=0; j<roundWinners.size(); j++)
            if (*m_players[i] == *roundWinners[j])
                m_points[i] += pointsScored;
    }
}
/*void Match::PerformAfterRound();*/


//to check whether the match is completed
bool Match::MatchIsOver() const
{
    //if a player has 7 or more points
    for (auto point: m_points)
        if (point >= m_winPoints)
            return true;
    
    return false;
}


/**/
/*
 Match::PerformAfterMatch Match::PerformAfterMatch

NAME

        PerformAfterMatch - to perform a series of actions after a match is completed
 
SYNOPSIS

        void Match::PerformAfterMatch();
            
DESCRIPTION

        This function clears the game table to declare the final state of the match
        and to declare the player(s) that won the match.

RETURNS

        None
 
*/
/**/

void Match::PerformAfterMatch()
{
    //clear the table
    vector<Card> emptyTable;
    
    //reset trick scores (not required)
    GetCurrentRound().ResetTrickScores();

    for (auto pl: m_players)
    {
        //display the final state of the game
        string display = Display::GetMatchDisplay(m_players, GetCurrentRound().GetHakem(), emptyTable, m_points,
        GetCurrentRound().GetTrickScores(), (int)m_rounds.size(), GetCurrentRound().GetTrumpSuit(), pl->GetUsername());
        
        pl->SendMsg(display);
    }
    
    DeclareMatchWinner();
}
/*void Match::PerformAfterMatch();*/


/**/
/*
 Match::DeclareMatchWinner Match::DeclareMatchWinner

NAME

        DeclareMatchWinner - to declare the winner of the match
 
SYNOPSIS

        void Match::DeclareMatchWinner();
            
DESCRIPTION

        This function identifies the player(s) with 7 or more points (i.e. match winner(s)).
        The match winners are then declared.

RETURNS

        None
 
*/
/**/

void Match::DeclareMatchWinner()
{
    //identify the index of the player with 7 or more points
    int winnerIndex = 0;
    for (int i=0; i<m_points.size(); i++)
        if (m_points[i] >= m_winPoints)
            winnerIndex = i;
        
    //record the winner
    m_winner.push_back(m_players[winnerIndex]);
    
    //record partner as winner (if 4-player mode)
    if (m_mode == fourPlayer)
        m_winner.push_back(m_players[winnerIndex]->GetPartner());
    
    //inform clients about who won the match
    for (auto pl: m_players)
    {
        string display = Display::GetWinnerDeclarationMsg(m_winner, MATCH, pl->GetUsername());
        pl->SendMsg(display);
    }
}
/*void Match::DeclareMatchWinner();*/

//to swap two match players based on two given indices
void swap(vector<Player*>& a_players, int a_numOne, int a_numTwo)
{
    Player* temp = a_players[a_numOne];
    a_players[a_numOne] = a_players[a_numTwo];
    a_players[a_numTwo] = temp;
}
