#include "stdafx.h"

//the title of the event the player won
static map<Event, string> eventTitle =
{
    {TRICK, "Trick"} ,
    {ROUND, "Round"},
    {MATCH, "Match"}
};

//the style of a card display
enum CardDisplayStyle
{
    //used for the first card in the pile
    full,
    
    //used for the remaining cards in the pile
    partial
};

//the maximum number of cards in the hand of a player in different modes
static map<Mode, int> maxCards =
{
    {twoPlayer, 13},
    {threePlayer, 17},
    {fourPlayer, 13}
};

//the specification of the card display style using a boolean
static map<CardDisplayStyle, bool> cardDisplay =
{
    {full, true} ,
    {partial, false}
};

//used to diaplay the leaderboards
struct ClientData
{
    //the client's username
    string username;
    
    //the total number of wins in multi-player matches
    int wins;
    
    //the total number of losses in multi-player matches
    int losses;
    
    //the win rate based on the number of wins and losses
    int winRate;
    
    bool operator < (const ClientData& client) const
    {
        return winRate > client.winRate;
    }
    
    ClientData(string a_user, int a_win, int a_loss)
    {
        username = a_user;
        wins = a_win;
        losses = a_loss;
        winRate = round ((double) wins / (wins + losses) * 100);
    }
};

//***         colors        ***\\

//background colors
string Display::m_bWhite = "107";
string Display::m_bGreen = "42";

//text colors
string Display::m_tBlack = "30";
string Display::m_tWhite = "97";
string Display::m_tBlue = "96";
string Display::m_tRed = "91";
string Display::m_tGreen = "32";
string Display::m_tYellow = "93";

//to make text display blink
string Display::m_blink = "5";

//prompts
string Display::m_logInPrompt = "\nEnter Username And Password In This Format: username=password\n";
string Display::m_signupPrompt = "\nEnter The Username And Password You Would Like In This Format: username=password\n";
string Display::m_joinMatchPrompt = "\nEnter The Match Creator Name For The Match You Would Like To Join\n";
string Display::m_waitForTrumpSelection = "Please Wait For Hakem To Declare The Trump Suit\n";
string Display::m_waitForHakemPickDiscard = "\nWaiting For Hakem To Pick Or Discard The Very First Card...\n";

//*** single characters used ***//
static const string m_line = "━";
static const string m_pipe = "┃";
static const string m_upperScore = "‾";

//*** titles used ***//
static const string m_roundTitle = "ROUND:";
static const string m_roundHakem = "HAKEM:";
static const string m_trumpTitle = "TRUMP:";
static const string m_pointsTitle = "POINTS:";
static const string m_tricksTitle = "TRICKS:";


/**/
/*
 Display::GetMatchDisplay Display::GetMatchDisplay

NAME

        GetMatchDisplay - to get the elements of the game to be displayed on the screen
 
SYNOPSIS

        string Display::GetMatchDisplay(vector<Player*> a_players, Player* a_hakem, const vector<Card>& a_tableCards,
                                 const vector<int>& a_points, const vector<int>& a_tricks, const int& a_roundNum,
                                 const Suit& a_trumpSuit, const string& a_username);
 
            a_players    --> the match players
            a_hakem      --> the hakem for the round
            a_tableCards --> the cards played so far in the trick
            a_points     --> the player match points
            a_tricks     --> the player round trick scores
            a_roundNum   --> the cuurent round number
            a_trumpSuit  --> the trump suit for the round
            a_username   --> the username of player that will recieve the match display
            
DESCRIPTION

        This function creates the game display based on the current situation in the match
        and the player hands. The display is sent to clients through their sockets within
        the other parts of the program.
    

RETURNS

        The game display in string format
 
*/
/**/

string Display::GetMatchDisplay(vector<Player*> a_players, Player* a_hakem, const vector<Card>& a_tableCards,
                                const vector<int>& a_points, const vector<int>& a_tricks, const int& a_roundNum,
                                const Suit& a_trumpSuit, const string& a_username)
{
    //the game display
    string display;
    
    //add necessary vertical spacing
    string vSpacing = GetNewLines(m_verticalIndentation);
    ColorDisplay(vSpacing, m_bGreen, m_tGreen);
    display += vSpacing;
    
    //the space of text being displayed from the beginning of the line
    int spaceFromStart = 0;

    //add the round number display
    string roundSection;
    string roundInfo = m_roundTitle + m_space + to_string(a_roundNum);
    spaceFromStart = GetRelativePosition(roundInfo, m_screenCenter);
    roundSection = AddText(m_space, spaceFromStart) + roundInfo + '\n';
    
    ColorDisplay(roundSection, m_bGreen, m_tWhite);
    display += roundSection;
    
    //add the the trump suit display
    string trumpSection;
    string trumpInfo = AddText(m_space, 2) + m_trumpTitle + m_space;
    spaceFromStart = GetRelativePosition(trumpInfo, m_screenCenter) - 2;
    trumpInfo = AddText(m_space, spaceFromStart) + trumpInfo;
    ColorDisplay(trumpInfo, m_bGreen, m_tWhite);
    
    //color the trump suit
    string trumpSuit = suitSymbol[a_trumpSuit];
    if (a_trumpSuit == hearts || a_trumpSuit == diamonds)
        ColorDisplay(trumpSuit, m_bGreen, m_tRed);
    else
        ColorDisplay(trumpSuit, m_bGreen, m_tBlack);
    
    string newLine = "\n";
    ColorDisplay(newLine, m_bGreen, m_tWhite);
    
    trumpSection = trumpInfo + trumpSuit + newLine;
    display += trumpSection;
    
    //add the Hakem name display
    string hakemSection;
    string hakemInfo = m_roundHakem + m_space + a_hakem->GetUsername();
    spaceFromStart = GetRelativePosition(hakemInfo, m_screenCenter);
    hakemSection += AddText(m_space, spaceFromStart) + hakemInfo + '\n';
    
    ColorDisplay(hakemSection, m_bGreen, m_tWhite);
    display += hakemSection;
    
    //add neecessary vertical spacing
    vSpacing = GetNewLines(m_titlesPlayerDistance) + '\n';
    ColorDisplay(vSpacing, m_bGreen, m_tGreen);
    display += vSpacing;
    
    //the match mode
    int matchMode = (int)a_players.size();
    
    //set to true if cards are being distributed to determine the Hakem
    bool displayingHakem = false;
    
    //add the display for the player on the top part of the screen
    display += GetSinglePlayerDisplay(a_players[playerOne], a_points[playerOne], a_tricks[playerOne],
                                      matchMode, displayingHakem, m_screenCenter, a_username);
    
    if (matchMode == twoPlayer)
    {
        //add the display for a 2-player match
        display += GetTwoPlMatchDisplay(a_players, a_tableCards, a_points[playerTwo],
                                            a_tricks[playerTwo], displayingHakem, a_username);
    }
        
    else if (matchMode == threePlayer)
    {
        //add the display for a 3-player match
        display += GetThreePlMatchDisplay(a_players, a_tableCards, a_points,
                                          a_tricks, displayingHakem, a_username);
    }
        
    else
    {
        //add the display for  a 4-player match
        display += GetFourPlMatchDisplay(a_players, a_tableCards, a_points, a_tricks, displayingHakem, a_username);
    }
        
    //add necessary vertical spacing
    vSpacing = GetNewLines(m_verticalIndentation - 1);
    ColorDisplay(vSpacing, m_bGreen, m_tGreen);
    display += vSpacing + '\n';
    
    //return the completed match display
    return display;
}
/* string Display::GetMatchDisplay(vector<Player*> a_players, Player* a_hakem,
 const vector<Card>& a_tableCards, const vector<int>& a_points, const vector<int>& a_tricks,
 const int& a_roundNum, const Suit& a_trumpSuit, const string& a_username);*/


/**/
/*
 Display::GetHakemDeterminationDisplay Display::GetHakemDeterminationDisplay

NAME

        GetHakemDeterminationDisplay - to get the elements of distributing cards to players to determine the Hakem
 
SYNOPSIS

        string Display::GetHakemDeterminationDisplay(vector<Player*> a_players);
            a_players --> the match players
            
DESCRIPTION

        This function creates the game display based for when cards are being distributed to all
        the players in an attempt to determine the Hakem for the first round (i.e. the first player
        to recieve an Ace).

RETURNS

        The game display for Hakem determination in string format
 
*/
/**/

string Display::GetHakemDeterminationDisplay(vector<Player*> a_players)
{
    //the game display
    string display;
    
    //add necessary vertical spacing
    string vSpacing = GetNewLines(m_verticalIndentation);
    ColorDisplay(vSpacing, m_bGreen, m_tWhite);
    display += vSpacing;
    
    //establish the match mode
    Mode matchMode = (Mode)a_players.size();
    
    //used to simply call functions with default values
    vector <Card> defaultTableCards;
    vector <int> defaultPoints = {0};
    vector <int> defaultTricks = {0};
    
    //indicating that Hakem is being determined (no full match display)
    bool displayingHakem = true;
    
    //add the hand display for the player on the top part of the screen
    display += GetSinglePlayerDisplay(a_players[playerOne], defaultPoints[0], defaultTricks[0],
                                      matchMode, displayingHakem, m_screenCenter, a_players[0]->GetUsername());
    
    if (matchMode == twoPlayer)
    {
        //add the display for a 2-player Hakem determination
        display += GetTwoPlMatchDisplay(a_players, defaultTableCards, defaultPoints[0], defaultTricks[0],
                                            displayingHakem, a_players[0]->GetUsername());
    }
        
    else if (matchMode == threePlayer)
    {
        //add the display for a 3-player Hakem determination
        display += GetThreePlMatchDisplay(a_players, defaultTableCards, defaultPoints, defaultTricks,
                                          displayingHakem, a_players[0]->GetUsername());
    }

    else
    {
        //add the display for a 4-player Hakem determination
        display += GetFourPlMatchDisplay(a_players, defaultTableCards, defaultPoints, defaultTricks,
                                         displayingHakem, a_players[0]->GetUsername());
        
    }

    //add necessary vertical spacing
    vSpacing = GetNewLines(m_verticalIndentation - 2);
    ColorDisplay(vSpacing, m_bGreen, m_tWhite);
    display += vSpacing + '\n';
    
    //the completed display
    return display;
}
/*string Display::GetHakemDeterminationDisplay(vector<Player*> a_players);*/


/**/
/*
 Display::GetSinglePlayerDisplay Display::GetSinglePlayerDisplay

NAME

        GetSinglePlayerDisplay - to get the player box display of a single player
 
SYNOPSIS

        string Display::GetSinglePlayerDisplay(Player* a_player, const int& a_points, const int& a_tricks,
                                        const int& a_matchMode, const bool& a_hakemDetermination,
                                        int a_relativeCenter, const string& a_username);
 
            a_player             --> the player whose information will be added
            a_points             --> the player match points
            a_tricks             --> the player trick scores
            a_matchMode          --> the number of players in the match
            a_hakemDetermination --> set to true if display is for Hakem determination purposes
            a_relativeCenter     --> the relative center of where the player information will be displayed
            a_username           --> the username of player who will recieve this display
 
            
DESCRIPTION

        This function creates and adds the game display for a single player, located on
        the screen. The player information includes player's username, its hand,
        the points and tricks earned by the player.

RETURNS

        The game display for a single player's information
 
*/
/**/

string Display::GetSinglePlayerDisplay(Player* a_player, const int& a_points, const int& a_tricks,
                                       const int& a_matchMode, const bool& a_hakemDetermination, int a_relativeCenter,
                                       const string& a_username)
{
    //the screen display
    string display;
    
    //add the player name display
    string plNameDisplay = GetPlNameDisplay(a_player->GetUsername(), a_relativeCenter, a_matchMode) + "\n";
    ColorDisplay(plNameDisplay, m_bGreen, m_tWhite);
    display += plNameDisplay;
    
    int handSize = a_player->GetHandSize();
    
    //relative center is the hand's start location
    int handStart = a_relativeCenter;
        
    //if the hand is not empty
    if (handSize != 0)
    {
        //subtract half of the the full card size
        handStart -= (m_fullCardHorizontalSize / 2);
            
        //subtract half of the remaining partial cards
        handStart -= handSize -  1;
    }
    
    //if set to true, the symbol and rank of the cards in a player hand will be "hidden"
    bool maskedHand;
    
    //if Hakem is being determined or it is the hand of this player
    if (a_hakemDetermination || a_player->GetUsername() == a_username)
        maskedHand = false;
    else
        maskedHand = true;
        
    //add the hand display
    
    //used to fill up the screen to the end with green background
    string newLine = "\n";
    ColorDisplay(newLine, m_bGreen, m_tGreen);
    
    //add the top rank for all the cards
    display += GetCardTopRanksDisplay(a_player->GetHand(), handStart, maxCards[(Mode)a_matchMode], maskedHand) + newLine;
    
    //add the suit for all the cards
    display += GetCardSuitsDisplay(a_player->GetHand(), handStart, maxCards[(Mode)a_matchMode], maskedHand) + newLine;
    
    //add the bottom rank for all the cards
    display += GetCardBottomRanksDisplay(a_player->GetHand(), handStart, maxCards[(Mode)a_matchMode], maskedHand) + newLine;
   
    //if the display is not for when Hakem is being determined
    if (!a_hakemDetermination)
    {
        //add the scores display
        display += GetScoresDisplay(a_points, a_tricks, a_matchMode, a_relativeCenter);
        string closeBox = "\n" + GetCloseBoxDisplay(a_relativeCenter, a_matchMode) + "\n";
        ColorDisplay(closeBox, m_bGreen, m_tWhite);
        display += closeBox;
    }
    
    //the completed display
    return display;
}
/* string Display::GetSinglePlayerDisplay(Player* a_player, const int& a_points, const int& a_tricks,
 const int& a_matchMode, const bool& a_hakemDetermination, int a_relativeCenter,
 const string& a_username);*/


/*
 Display::GetTwoPlMatchDisplay Display::GetTwoPlMatchDisplay

NAME

        GetTwoPlMatchDisplay - to get the elements of a 2-player match display
 
SYNOPSIS

        string Display::GetTwoPlMatchDisplay(vector<Player*> a_players, const vector<Card>& a_tableCards,
                                          const int& a_plTwoPoints, const int& a_plTwoTricks,
                                          const bool& a_hakemDetermination, const string& a_username);
 
            a_players            --> the match players
            a_tableCards         --> the cards played in the trick
            a_plTwoPoints        --> the second player's points
            a_plTwoTricks        --> the second player's trick scores
            a_hakemDetermination --> set to true if display is for Hakem determination purposes
            a_username           --> the username of player who will recieve this display
 
            
DESCRIPTION

        This function puts together the elements of a 2-player match display. The player information
        and table cards are displayed.

RETURNS

        The game display for a 2-player match in string format
 
*/
/**/

string Display::GetTwoPlMatchDisplay(vector<Player*> a_players, const vector<Card>& a_tableCards,
                                         const int& a_plTwoPoints, const int& a_plTwoTricks,
                                         const bool& a_hakemDetermination, const string& a_username)
{
    //the display to be recieved by client
    string display;
    
    //the number of cards played by the players (on table)
    int numTableCards = (int)a_tableCards.size();
    
    //if the display is not for when Hakem is being determined
    if (!a_hakemDetermination)
    {
        if (numTableCards == 0)
        {
            //add necessary vertical spacing
            string vSpacing = GetNewLines(m_verticalTopLeftPlDistance * 2 + m_cardVerticalSize);
            ColorDisplay(vSpacing, m_bGreen, m_tGreen);
            display += vSpacing;
        }
            
        else if (numTableCards == 1)
        {
            //identify the location of the table card
            int cardLoc = m_screenCenter - (m_tableCardHorizontalSize / 2);
            
            //add the table card display in the appropriate location
            string vSpacing = GetNewLines(m_verticalTopLeftPlDistance);
            ColorDisplay(vSpacing, m_bGreen, m_tGreen);
            display += vSpacing;
            
            //add the card display
            
            //whether card rank and suit should be masked
            bool masked = false;
            
            //used to fill up the screen to the end with green background
            string newLine = "\n";
            ColorDisplay(newLine, m_bGreen, m_tGreen);
            
            //add the top rank for the card
            display += GetCardTopRankDisplay(a_tableCards[0], cardLoc, cardDisplay[full], masked, true) + newLine;
            
            //add the suit for the card
            display += GetCardSuitDisplay(a_tableCards[0], cardLoc, cardDisplay[full], masked, true) + newLine;
            
            //add the bottom rank for the card
            display += GetCardBottomRankDisplay(a_tableCards[0], cardLoc, cardDisplay[full], masked, true) + newLine;
            
            display += vSpacing;
        }
        
        //if there is two cards on the table
        else
        {
            //identify the location of both cards
            int cardOneLoc = m_screenCenter - m_tableCardHorizontalSize - 1;
            int cardTwoLoc = 2;  //relative to card one
            
            Card firstCard = a_tableCards[0];
            Card secondCard = a_tableCards[1];
            
            string vSpacing = GetNewLines(m_verticalTopLeftPlDistance);
            ColorDisplay(vSpacing, m_bGreen, m_tGreen);
            
            //add necessary vertical spacing
            display += vSpacing;
            
            //to fill up the screen with background
            string newLine = "\n";
            ColorDisplay(newLine, m_bGreen, m_tGreen);
            
            //add the top rank for both cards
            display += GetCardTopRankDisplay(firstCard, cardOneLoc, cardDisplay[full], false, true);
            display += GetCardTopRankDisplay(secondCard, cardTwoLoc, cardDisplay[full], false, true) + newLine;
           
            //add the suit for both cards
            display += GetCardSuitDisplay(firstCard, cardOneLoc, cardDisplay[full], false, true);
            display += GetCardSuitDisplay(secondCard, cardTwoLoc, cardDisplay[full], false, true) + newLine;
            
            //add the bottom rank for both cards
            display += GetCardBottomRankDisplay(firstCard, cardOneLoc, cardDisplay[full], false, true);
            display += GetCardBottomRankDisplay(secondCard, cardTwoLoc, cardDisplay[full], false, true) + newLine;
            
            //add necessary spacing
            display += vSpacing;
        }
    }
    
    //if Hakem is being determined
    else
    {
        //do not display table cards
        //do not display scores
        
        //add necessary vertical spacing
        string vSpacing = GetNewLines(m_verticalTopBottomPlDistance);
        ColorDisplay(vSpacing, m_bGreen, m_tGreen);
        display += vSpacing;
    }

    //display the second player information on the bottom of the screen
    display += GetSinglePlayerDisplay(a_players[playerTwo], a_plTwoPoints, a_plTwoTricks,
                                      twoPlayer, a_hakemDetermination, m_screenCenter, a_username);
    
    //the display that will be sent to client
    return display;
}
/*string Display::GetTwoPlMatchDisplay(vector<Player*> a_players, const vector<Card>& a_tableCards,
 const int& a_plTwoPoints, const int& a_plTwoTricks,
 const bool& a_hakemDetermination, const string& a_username);*/


/*
 Display::GetThreePlMatchDisplay Display::GetThreePlMatchDisplay

NAME

        GetTwoPlMatchDisplay - to get the elements of a 3-player match display
 
SYNOPSIS

        string Display::GetThreePlMatchDisplay(vector<Player*> a_players, const vector<Card>& a_tableCards,
                                        const vector<int>& a_points, const vector<int>& a_tricks,
                                        const bool& hakemDetermination, const string& a_username);
 
            a_players            --> the match players
            a_tableCards         --> the cards played in the trick
            a_points             --> the player's match points
            a_tricks             --> the player's trick scores
            a_hakemDetermination --> set to true if display is for Hakem determination purposes
            a_username           --> the username of player who will recieve this display
 
            
DESCRIPTION

        This function puts together the elements of a 3-player match display by calling
        the appropriate function that matches the current match status.
        The player information and table cards are displayed.

RETURNS

        The game display for a 3-player match in string format
 
*/
/**/

string Display::GetThreePlMatchDisplay(vector<Player*> a_players, const vector<Card>& a_tableCards,
                                       const vector<int>& a_points, const vector<int>& a_tricks,
                                       const bool& hakemDetermination, const string& a_username)
{
    //the display sent to client
    string display;
    
    //identify the number of table cards
    int numTableCards = (int)a_tableCards.size();
    
    //identify the location of the second player's box center
    int pTwoBoxCenterLoc = (m_seventeenHandWidth / 2) + m_leftIndentation;
    
    //add necessary vertical spacing
    string vSpacing;
    if (numTableCards != 3)
        vSpacing = GetNewLines(m_verticalTopLeftPlDistance);
        
    else
        vSpacing = GetNewLines(m_verticalTopLeftPlDistance - 1);
    
    ColorDisplay(vSpacing, m_bGreen, m_tWhite);
    display += vSpacing;
    
    //if the display is not for when Hakem is being determined
    if (!hakemDetermination)
    {
        //identify the location of the second player's card
        int pTwoCardStartLoc = GetHandStartLocation(pTwoBoxCenterLoc, a_players[playerTwo]->GetHandSize());
        
        //if set to true, the symbol and rank of the cards in a player hand will be "hidden"
        bool maskedHand;
        
        if (a_players[playerTwo]->GetUsername() == a_username)
            maskedHand = false;
        else
            maskedHand = true;
        
        if (numTableCards == 0)
        {
            //display the second player info
            display += GetSinglePlayerDisplay(a_players[playerTwo], a_points[playerTwo], a_tricks[playerTwo],
                                              threePlayer, hakemDetermination, pTwoBoxCenterLoc, a_username);
        }
        
        else if (numTableCards == 1)
        {
            //add the 3-player match display with a table card
            display += GetThreePlOneCardDisplay(a_players[playerTwo], a_tableCards[0], a_points[playerTwo],
                                  a_tricks[playerTwo], pTwoBoxCenterLoc, pTwoCardStartLoc, maskedHand);
        }
        
        else if (numTableCards == 2)
        {
            //add the 3-player match display with two table cards
            display += GetThreePlTwoCardsDisplay(a_players[playerTwo], a_tableCards[0], a_tableCards[1], a_points[playerTwo],
                                   a_tricks[playerTwo], pTwoBoxCenterLoc, pTwoCardStartLoc, maskedHand);
        }
            
        else
        {
            //add the 3-player match display with three table cards
            display += GetThreePlThreeCardsDisplay(a_players[playerTwo], a_tableCards[0], a_tableCards[1], a_tableCards[2],
                                     a_points[playerTwo], a_tricks[playerTwo], pTwoBoxCenterLoc, pTwoCardStartLoc, maskedHand);
        }
    }
    
    //if Hakem is being determined
    else
    {
        //do not display table cards
        //do not display scores
        
        //add the second player info
        display += GetSinglePlayerDisplay(a_players[playerTwo], a_points[playerTwo], a_tricks[playerTwo],
                                          threePlayer, hakemDetermination, pTwoBoxCenterLoc, a_username);
    }
    
    //add necesary vertical spacing
    if (numTableCards != 3)
        vSpacing = GetNewLines(m_verticalTopLeftPlDistance);
    else
        vSpacing = GetNewLines(m_verticalTopLeftPlDistance - 2);
    
    ColorDisplay(vSpacing, m_bGreen, m_tWhite);
    display += vSpacing;
    
    //add the third player info
    display += GetSinglePlayerDisplay(a_players[playerThree], a_points[playerThree], a_tricks[playerThree],
                                      threePlayer, hakemDetermination, m_screenCenter, a_username);
    
    //the completed display
    return display;
}
/*string Display::GetThreePlMatchDisplay(vector<Player*> a_players, const vector<Card>& a_tableCards,
 const vector<int>& a_points, const vector<int>& a_tricks,
 const bool& hakemDetermination, const string& a_username);*/


/*
 Display::GetThreePlOneCardDisplay Display::GetThreePlOneCardDisplay
NAME

        GetThreePlOneCardDisplay - to get the elements of a 3-player match display with a table card
 
SYNOPSIS

        string Display::GetThreePlOneCardDisplay(Player* a_player, const Card& a_card, const int& a_points, const int& a_tricks,
                                          const int& a_pTwoBoxCenterLoc, const int& a_pTwoCardStartLoc, const bool& a_masked);
 
            a_player             --> the second player in the match
            a_card               --> the table card
            a_points             --> the player match points
            a_tricks             --> the player trick scores
            a_pTwoBoxCenterLoc   --> the center of the second player's information box
            a_pTwoCardStartLoc   --> the location of where the player hands should be printed
            a_masked             --> whether suit and rank of cards in hand should be displayed
            
DESCRIPTION

        This function puts together the elements of a 3-player match display that has
        a table card.

RETURNS

        The game display for a 3-player match with a table card in string format
 
*/
/**/

string Display::GetThreePlOneCardDisplay(Player* a_player, const Card& a_card, const int& a_points, const int& a_tricks,
                                         const int& a_pTwoBoxCenterLoc, const int& a_pTwoCardStartLoc, const bool& a_masked)
{
    //the display sent to client
    string display;
    
    //establish the player's hand
    vector<Card> playerTwoHand = a_player->GetHand();
    
    //to fill screen with background
    string newLine = "\n";
    ColorDisplay(newLine, m_bGreen, m_tWhite);
    
    //establish the location of the table card
    int cardLoc = (m_screenCenter - m_seventeenHandWidth - m_leftIndentation) - (m_fullCardHorizontalSize/2);
    
    //add the the 2nd player's name
    string plName = GetPlNameDisplay(a_player->GetUsername(), a_pTwoBoxCenterLoc, threePlayer) + "\n";
    ColorDisplay(plName, m_bGreen, m_tWhite);
    display += plName;
    
    //add the top rank for all cards in 2nd player's hand
    display += GetCardTopRanksDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[threePlayer], a_masked);
    
    //add the top rank for the table card
    display += GetCardTopRankDisplay(a_card, cardLoc, cardDisplay[full], false, true) + newLine;
        
    //add the card suit for all cards in 2nd player's hand
    display += GetCardSuitsDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[threePlayer], a_masked);
    
    //add the card suit for the table card
    display += GetCardSuitDisplay(a_card, cardLoc, cardDisplay[full], false, true) + newLine;
            
    //add the card bottom rank for all cards in 2nd player's hand
    display += GetCardBottomRanksDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[threePlayer], a_masked);
    
    //add the card bottom rank for the table card
    display += GetCardBottomRankDisplay(a_card, cardLoc, cardDisplay[full], false, true) + newLine;
        
    //add the score board for 2nd player
    display += GetScoresDisplay(a_points, a_tricks, threePlayer, a_pTwoBoxCenterLoc) + newLine;

    //close the player box
    string closeBox = GetCloseBoxDisplay(a_pTwoBoxCenterLoc, threePlayer);
    ColorDisplay(closeBox, m_bGreen, m_tWhite);
    display += closeBox;
    display += newLine;
    
    //the completed display
    return display;
}
/* string Display::GetThreePlOneCardDisplay(Player* a_player, const Card& a_card, const int& a_points,
 const int& a_tricks, const int& a_pTwoBoxCenterLoc, const int& a_pTwoCardStartLoc, const bool& a_masked);*/


/*
 Display::GetThreePlTwoCardsDisplay Display::GetThreePlTwoCardsDisplay
NAME

        GetThreePlTwoCardsDisplay - to get the elements of a 3-player match display with 2 table cards
 
SYNOPSIS

        string Display::GetThreePlTwoCardsDisplay(Player* a_player, const Card& a_cardOne, const Card& a_cardTwo,
                                           const int& a_points, const int& a_tricks, const int& a_pTwoBoxCenterLoc,
                                           const int& a_pTwoCardStartLoc, const bool& a_masked);
 
            a_player             --> the second player in the match
            a_cardOne            --> the first table card
            a_cardTwo            --> the second table card
            a_points             --> the player match points
            a_tricks             --> the player trick scores
            a_pTwoBoxCenterLoc   --> the center of the second player's information box
            a_pTwoCardStartLoc   --> the location of where the player hands should be printed
            a_masked             --> whether suit and rank of cards in hand should be displayed
            
DESCRIPTION

        This function puts together the elements of a 3-player match display that has
        2 table cards.

RETURNS

        The game display for a 3-player match with 2 table cards in string format
 
*/
/**/

string Display::GetThreePlTwoCardsDisplay(Player* a_player, const Card& a_cardOne, const Card& a_cardTwo,
                                          const int& a_points, const int& a_tricks, const int& a_pTwoBoxCenterLoc,
                                          const int& a_pTwoCardStartLoc, const bool& a_masked)
{
    //the display sent to client
    string display = "";
    
    //establish the player hand
    vector<Card> playerTwoHand = a_player->GetHand();
    
    //identify the location of table cards
    int cardOneLoc = m_screenCenter - m_seventeenHandWidth - m_leftIndentation - m_tableCardHorizontalSize;
    int cardTwoLoc = 2; //relative to card one
    
    //to fill screen with background
    string newLine = "\n";
    ColorDisplay(newLine, m_bGreen, m_tWhite);
    
    //add the 2nd player's name
    string plName = GetPlNameDisplay(a_player->GetUsername(), a_pTwoBoxCenterLoc, threePlayer) + "\n";
    ColorDisplay(plName, m_bGreen, m_tWhite);
    display += plName;
    
    //add the card top rank for all cards in 2nd player's hand
    display += GetCardTopRanksDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[threePlayer], a_masked);
    
    //add the card top rank for the first table card
    display += GetCardTopRankDisplay(a_cardOne, cardOneLoc, cardDisplay[full], false, true);
    
    //add the card top rank for the second table card
    display += GetCardTopRankDisplay(a_cardTwo, cardTwoLoc, cardDisplay[full], false, true) + newLine;
    
    //add the card suit for all cards in 2nd player's hand
    display += GetCardSuitsDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[threePlayer], a_masked);
    
    //add the card suit for the first table card
    display += GetCardSuitDisplay(a_cardOne, cardOneLoc, cardDisplay[full], false, true);
    
    //add the card suit for the second table card
    display += GetCardSuitDisplay(a_cardTwo, cardTwoLoc, cardDisplay[full], false, true) + newLine;
    
    //add the card bottom rank for all cards in 2nd player's hand
    display += GetCardBottomRanksDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[threePlayer], a_masked);
    
    //add the card bottom rank for the first table card
    display += GetCardBottomRankDisplay(a_cardOne, cardOneLoc, cardDisplay[full], false, true);
    
    //add the card bottom rank for the second table card
    display += GetCardBottomRankDisplay(a_cardTwo, cardTwoLoc, cardDisplay[full], false, true) + newLine;
    
    //add the score board for 2nd player
    display += GetScoresDisplay(a_points, a_tricks, threePlayer, a_pTwoBoxCenterLoc) + newLine;
    
    //add the closing score board line
    string closeBox = GetCloseBoxDisplay(a_pTwoBoxCenterLoc, threePlayer);
    ColorDisplay(closeBox, m_bGreen, m_tWhite);
    display += closeBox + newLine;
    
    //the completed display
    return display;
}
/*string Display::GetThreePlTwoCardsDisplay(Player* a_player, const Card& a_cardOne, const Card& a_cardTwo,
 const int& a_points, const int& a_tricks, const int& a_pTwoBoxCenterLoc,
 const int& a_pTwoCardStartLoc, const bool& a_masked);*/


/*
 Display::GetThreePlThreeCardsDisplay Display::GetThreePlThreeCardsDisplay
NAME

        GetThreePlThreeCardsDisplay - to get the elements of a 3-player match display with 3 table cards
 
SYNOPSIS

        string Display::GetThreePlThreeCardsDisplay(Player* a_player, const Card& a_cardOne, const Card& a_cardTwo,
                                             const Card& a_cardThree, const int& a_points, const int& a_tricks,
                                             const int& a_pTwoBoxCenterLoc, const int& a_pTwoCardStartLoc,
                                             const bool& a_masked);
 
            a_player             --> the second player in the match
            a_cardOne            --> the first table card
            a_cardTwo            --> the second table card
            a_cardThree          --> the third table card
            a_points             --> the player match points
            a_tricks             --> the player trick scores
            a_pTwoBoxCenterLoc   --> the center of the second player's information box
            a_pTwoCardStartLoc   --> the location of where the player hands should be printed
            a_masked             --> whether suit and rank of cards in hand should be displayed
            
DESCRIPTION

        This function puts together the elements of a 3-player match display that has
        3 table cards.

RETURNS

        The game display for a 3-player match with 3 table cards in string format
 
*/
/**/

string Display::GetThreePlThreeCardsDisplay(Player* a_player, const Card& a_cardOne, const Card& a_cardTwo,
                                            const Card& a_cardThree, const int& a_points, const int& a_tricks,
                                            const int& a_pTwoBoxCenterLoc, const int& a_pTwoCardStartLoc,
                                            const bool& a_masked)
{
    //the display to be sent to client
    string display;
    
    //establish second player's hand
    vector<Card> playerTwoHand = a_player->GetHand();
    
    //identify the location of the first table card
    int cardOneHorizontalLoc = m_screenCenter - (m_tableCardHorizontalSize / 2);
    int cardOneHorizontalLocPl = cardOneHorizontalLoc - m_seventeenHandWidth - m_leftIndentation + 1;
    
    //identify the location of the second table card
    int cardTwoLoc = m_screenCenter - m_seventeenHandWidth - m_leftIndentation - m_tableCardHorizontalSize;
    
    //the relative position of card three to card two
    int cardThreeLoc = 2;
    
    //identify the new location of the cards
    int bottomCardHorizontal = m_screenCenter - m_tableCardHorizontalSize;
    
    //add the 2nd player's name
    string plName = GetPlNameDisplay(a_player->GetUsername(), a_pTwoBoxCenterLoc, threePlayer);
    ColorDisplay(plName, m_bGreen, m_tWhite);
    display += plName;
    
    //to fill screen with background
    string newLine = "\n";
    ColorDisplay(newLine, m_bGreen, m_tWhite);
    
    //add the card top rank for the first table card
    display += GetCardTopRankDisplay(a_cardOne, cardOneHorizontalLocPl, cardDisplay[full], false, true) + newLine;
    
    //add the card top rank for all cards in 2nd player's hand
    display += GetCardTopRanksDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[threePlayer], a_masked);
    
    //add the card suit for the first table card
    display += GetCardSuitDisplay(a_cardOne, cardOneHorizontalLocPl, cardDisplay[full], false, true) + newLine;
    
    //add the card suit for all cards in 2nd player's hand
    display += GetCardSuitsDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[threePlayer], a_masked);
    
    //add the card bottom rank for the first table card
    display += GetCardBottomRankDisplay(a_cardOne, cardOneHorizontalLocPl, cardDisplay[full], false, true) + newLine;
    
    //add the card bottom rank for all cards in 2nd player's hand
    display += GetCardBottomRanksDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[threePlayer], a_masked) + newLine;
    
    //add the score board for 2nd player
    display += GetScoresDisplay(a_points, a_tricks, threePlayer, a_pTwoBoxCenterLoc);
    
    //add the card top rank for the second table card
    display += GetCardTopRankDisplay(a_cardTwo, cardTwoLoc, cardDisplay[full], false, true);
    
    //add the card top rank for the third table card
    display += GetCardTopRankDisplay(a_cardThree, cardThreeLoc, cardDisplay[full], false, true) + newLine;
    
    //add the closing score board line
    string closeBox = GetCloseBoxDisplay(a_pTwoBoxCenterLoc, threePlayer);
    ColorDisplay(closeBox, m_bGreen, m_tWhite);
    display += closeBox;
    
    //add the card suit for the second table card
    display += GetCardSuitDisplay(a_cardTwo, cardTwoLoc, cardDisplay[full], false, true);
    
    //add the card suit for the third table card
    display += GetCardSuitDisplay(a_cardThree, cardThreeLoc, cardDisplay[full], false, true) + newLine;
    
    //add the card bottom rank for the second table card
    display += GetCardBottomRankDisplay(a_cardTwo, bottomCardHorizontal, cardDisplay[full], false, true);
    
    //add the card bottom rank for the third table card
    display += GetCardBottomRankDisplay(a_cardThree, cardThreeLoc, cardDisplay[full], false, true) + newLine;
    
    //the completed display
    return display;
}
/* string Display::GetThreePlThreeCardsDisplay(Player* a_player, const Card& a_cardOne, const Card& a_cardTwo,
 const Card& a_cardThree, const int& a_points, const int& a_tricks,
 const int& a_pTwoBoxCenterLoc, const int& a_pTwoCardStartLoc,
 const bool& a_masked);*/


/*
 Display::GetFourPlMatchDisplay Display::GetFourPlMatchDisplay

NAME

        GetFourPlMatchDisplay - to get the elements of a 4-player match display
 
SYNOPSIS

        string Display::GetFourPlMatchDisplay(vector<Player*> a_players, const vector<Card>& a_tableCards,
                                       const vector<int>& a_points, const vector<int>& a_tricks,
                                       const bool& a_hakemDetermination, const string& a_username);
 
            a_players            --> the match players
            a_tableCards         --> the cards played in the trick
            a_points             --> the players match points
            a_tricks             --> the players trick scores
            a_hakemDetermination --> set to true if display is for Hakem determination purposes
            a_username           --> the username of player who will recieve this display
            
DESCRIPTION

        This function puts together the elements of a 4-player match disply by calling
        the appropriate function that matches the current match status.
        The player information and table cards are displayed.

RETURNS

        The game display for a 4-player match in string format
 
*/
/**/

string Display::GetFourPlMatchDisplay(vector<Player*> a_players, const vector<Card>& a_tableCards,
                                      const vector<int>& a_points, const vector<int>& a_tricks,
                                      const bool& a_hakemDetermination, const string& a_username)
{
    //the display to be sent to client
    string display;
    
    //the number of cards on the table
    int numTableCards = (int)a_tableCards.size();
    
    //these are the same for all states of a 4-player display
    int pTwoBoxCenter = (m_thirteenHandWidth / 2) + m_leftIndentation;
    int pTwoHandStart = GetHandStartLocation(pTwoBoxCenter, a_players[playerTwo]->GetHandSize());
    
    //the player hands to be displayed
    vector<Card> pTwoHand = a_players[playerTwo]->GetHand();
    vector<Card> pFourHand = a_players[playerFour]->GetHand();
    
    //add necessary vertical spacing
    string vSpacing;
    if (numTableCards != 3 && numTableCards != 4)
        vSpacing = GetNewLines(m_verticalTopLeftPlDistance);
    else
        vSpacing = GetNewLines(m_verticalTopLeftPlDistance - 1);
    
    ColorDisplay(vSpacing, m_bGreen, m_tWhite);
    display += vSpacing;
    
    //if the display is not for when Hakem is being determined
    if (!a_hakemDetermination)
    {
        //whether second player's hand will be hidden
        bool plTwoMasked = true;
        
        //whether fourth player's hand will be hidden
        bool plFourMasked = true;
        
        if (a_players[playerTwo]->GetUsername() == a_username)
            plTwoMasked = false;
        else if (a_players[playerFour]->GetUsername() == a_username)
            plFourMasked = false;
        
        if (numTableCards == 0)
        {
            //add the display for a 4-player match with no table cards
            display += GetFourPlNoCardDisplay(a_players[playerTwo], a_players[playerFour], pTwoBoxCenter, pTwoHandStart,
                                              a_hakemDetermination, a_points[playerTwo], a_points[playerFour], a_tricks[playerTwo],
                                              a_tricks[playerFour], plTwoMasked, plFourMasked);
        }
        
        else if (numTableCards == 1)
        {
            //add the display for a 4-player match with 1 table card
            display += GetFourPlOneCardDisplay(a_players[playerTwo], a_players[playerFour], a_tableCards[0], pTwoBoxCenter,
                                               pTwoHandStart, a_points[playerTwo], a_points[playerFour], a_tricks[playerTwo],
                                               a_tricks[playerFour], plTwoMasked, plFourMasked);
        }
        
        else if (numTableCards == 2)
        {
            //add the display for a 4-player match with 2 table cards
            display += GetFourPlTwoCardsDisplay(a_players[playerTwo], a_players[playerFour], a_tableCards[0], a_tableCards[1],
                                                pTwoBoxCenter, pTwoHandStart, a_points[playerTwo], a_points[playerFour],
                                                a_tricks[playerTwo], a_tricks[playerFour], plTwoMasked, plFourMasked);
        }
        
        else if (numTableCards == 3)
        {
            //add the display for a 4-player match with 3 table cards
            display += GetFourPlThreeCardsDisplay(a_players[playerTwo], a_players[playerFour], a_tableCards[0], a_tableCards[1],
                                    a_tableCards[2], pTwoBoxCenter, pTwoHandStart, a_points[playerTwo], a_points[playerFour],
                                                  a_tricks[playerTwo], a_tricks[playerFour], plTwoMasked, plFourMasked);
        }
        
        else
        {
            //add the display for a 4-player match with 4 table cards
            display += GetFourPlFourCardsDisplay(a_players[playerTwo], a_players[playerFour], a_tableCards, pTwoBoxCenter,
                                                 pTwoHandStart, a_points[playerTwo], a_points[playerFour], a_tricks[playerTwo], a_tricks[playerFour], plTwoMasked, plFourMasked);
        }
    }
    
    //if Hakem is being determined
    else
    {
        //do not display table cards
        //do not display scores
        
        //add the 2nd and 4th player hands display
        display += GetFourPlNoCardDisplay(a_players[playerTwo], a_players[playerFour], pTwoBoxCenter, pTwoHandStart,
                                          a_hakemDetermination, a_points[playerTwo], a_points[playerFour], a_tricks[playerTwo],
                                          a_tricks[playerFour], false, false);
    }
    
    //add necessary vertical spacing
    if (numTableCards != 3 && numTableCards != 4)
        vSpacing = GetNewLines(m_verticalTopLeftPlDistance);
    else
        vSpacing = GetNewLines(m_verticalTopLeftPlDistance - 2);
    
    ColorDisplay(vSpacing, m_bGreen, m_tWhite);
    display += vSpacing;
    
    //add the third player box display
    display += GetSinglePlayerDisplay(a_players[playerThree], a_points[playerThree], a_tricks[playerThree], fourPlayer,
                                      a_hakemDetermination, m_screenCenter, a_username);
    
    //the completed display
    return display;
}
/* string Display::GetFourPlMatchDisplay(vector<Player*> a_players, const vector<Card>& a_tableCards,
 const vector<int>& a_points, const vector<int>& a_tricks, const bool& a_hakemDetermination,
 const string& a_username);*/


/*
 Display::GetFourPlNoCardDisplay Display::GetFourPlNoCardDisplay
NAME

        GetFourPlNoCardDisplay - to get the elements of a 4-player match display with no table cards
 
SYNOPSIS

        string Display::GetFourPlNoCardDisplay(Player* a_plTwo, Player* a_plFour, const int& a_pTwoBoxCenter, const int& a_pTwoHandStart,
                                        const bool& a_hakemDetermination, const int& a_plTwoPoints, const int& a_plFourPoints,
                                        const int& a_plTwoTricks, const int& a_plFourTricks, const bool& a_plTwoMasked,
                                        const bool& a_plFourMasked);
 
            a_plTwo              --> the second player in the match
            a_plFour             --> the fourth player in the match
            a_pTwoBoxCenterLoc   --> the center of the second player's information box
            a_pTwoHandStart      --> the location of where the player hands should be printed
            a_hakemDetermination --> set to true if display is for Hakem determination purposes
            a_plTwoPoints        --> the second player's points
            a_plFourPoints       --> the fourth player's points
            a_plTwoTricks        --> the second player's trick scores
            a_plFourTricks       --> the fourth player's trick scores
            a_plTwoMasked        --> whether suit and rank of cards in second player's hand should be displayed
            a_plFourMasked       --> whether suit and rank of cards in fourth player's hand should be displayed
 
            
DESCRIPTION

        This function puts together the elements of a 4-player match display that has
        no table cards.

RETURNS

        The game display for a 4-player match with no table cards in string format
 
*/
/**/

string Display::GetFourPlNoCardDisplay(Player* a_plTwo, Player* a_plFour, const int& a_pTwoBoxCenter, const int& a_pTwoHandStart,
                                       const bool& a_hakemDetermination, const int& a_plTwoPoints, const int& a_plFourPoints, const int& a_plTwoTricks,
                                       const int& a_plFourTricks, const bool& a_plTwoMasked, const bool& a_plFourMasked)
{
    //the display to be sent to client
    string display;
    
    //establish the player's hands
    vector<Card> playerTwoHand = a_plTwo->GetHand();
    vector<Card> playerFourHand = a_plFour->GetHand();
    
    //estbalish the location of the fourth player's box
    int pFourBoxCenter = (2 * (m_screenCenter - m_leftIndentation - m_thirteenHandWidth) ) + (m_thirteenHandWidth / 2);
    int pFourHandStart = GetHandStartLocation(pFourBoxCenter, a_plFour->GetHandSize());
    
    //to fill screen with background
    string newLine = "\n";
    ColorDisplay(newLine, m_bGreen, m_tWhite);
    
    //add the second player's name
    string plNames = GetPlNameDisplay(a_plTwo->GetUsername(), a_pTwoBoxCenter, fourPlayer);
    
    //add the fourth player's name
    plNames += GetPlNameDisplay(a_plFour->GetUsername(), pFourBoxCenter, fourPlayer) + "\n";
    ColorDisplay(plNames, m_bGreen, m_tWhite);
    display += plNames;
    
    //add the top rank of cards for second and fourth player hands
    display += GetCardTopRanksDisplay(playerTwoHand, a_pTwoHandStart, maxCards[fourPlayer], a_plTwoMasked);
    display += GetCardTopRanksDisplay(playerFourHand, pFourHandStart, maxCards[fourPlayer], a_plFourMasked) + newLine;
    
    //add the suit of cards for second and fourth player hands
    display += GetCardSuitsDisplay(playerTwoHand, a_pTwoHandStart, maxCards[fourPlayer], a_plTwoMasked);
    display += GetCardSuitsDisplay(playerFourHand, pFourHandStart, maxCards[fourPlayer], a_plFourMasked) + newLine;
    
    //add the bottom rank of cards for second and fourth player hands
    display += GetCardBottomRanksDisplay(playerTwoHand, a_pTwoHandStart, maxCards[fourPlayer], a_plTwoMasked);
    display += GetCardBottomRanksDisplay(playerFourHand, pFourHandStart, maxCards[fourPlayer], a_plFourMasked) + newLine;
    
    //if the display is not for when Hakem is being determined
    if (!a_hakemDetermination)
    {
        //add second player's points and tricks
        display += GetScoresDisplay(a_plTwoPoints, a_plTwoTricks, fourPlayer, a_pTwoBoxCenter);
            
        //add fourth player's points and tricks
        display += GetScoresDisplay(a_plFourPoints, a_plFourTricks, fourPlayer, pFourBoxCenter) + newLine;
            
        //close both player boxes
        string boxes = GetCloseBoxDisplay(a_pTwoBoxCenter, fourPlayer);
        boxes += GetCloseBoxDisplay(pFourBoxCenter, fourPlayer) + "\n";
        ColorDisplay(boxes, m_bGreen, m_tWhite);
        display += boxes;
    }
    
    //the completed display
    return display;
}
/* string Display::GetFourPlNoCardDisplay(Player* a_plTwo, Player* a_plFour, const int& a_pTwoBoxCenter, const int& a_pTwoHandStart,
 const bool& a_hakemDetermination, const int& a_plTwoPoints, const int& a_plFourPoints,
 const int& a_plTwoTricks, const int& a_plFourTricks, const bool& a_plTwoMasked,
 const bool& a_plFourMasked);*/


/*
 Display::GetFourPlOneCardDisplay Display::GetFourPlOneCardDisplay
NAME

        GetFourPlOneCardDisplay - to get the elements of a 4-player match display with one table card
 
SYNOPSIS

        string Display::GetFourPlOneCardDisplay(Player* a_plTwo, Player* a_plFour, const Card& a_card, const int& a_pTwoBoxCenterLoc,
                                         const int& a_pTwoCardStartLoc, const int& a_plTwoPoints, const int& a_plFourPoints,
                                         const int& a_plTwoTricks, const int& a_plFourTricks, const bool& a_plTwoMasked,
                                         const bool& a_plFourMasked);
 
            a_plTwo              --> the second player in the match
            a_plFour             --> the fourth player in the match
            a_card               --> the table card
            a_pTwoBoxCenterLoc   --> the center of the second player's information box
            a_pTwoCardStartLoc   --> the location of where the player hands should be printed
            a_plTwoPoints        --> the second player's points
            a_plFourPoints       --> the fourth player's points
            a_plTwoTricks        --> the second player's trick scores
            a_plFourTricks       --> the fourth player's trick scores
            a_plTwoMasked        --> whether suit and rank of cards in second player's hand should be displayed
            a_plFourMasked       --> whether suit and rank of cards in fourth player's hand should be displayed
 
            
DESCRIPTION

        This function puts together the elements of a 4-player match display that has
        one table card.

RETURNS

        The game display for a 4-player match with one table card in string format
 
*/
/**/

string Display::GetFourPlOneCardDisplay(Player* a_plTwo, Player* a_plFour, const Card& a_card, const int& a_pTwoBoxCenterLoc,
                                        const int& a_pTwoCardStartLoc, const int& a_plTwoPoints, const int& a_plFourPoints,
                                        const int& a_plTwoTricks, const int& a_plFourTricks, const bool& a_plTwoMasked,
                                        const bool& a_plFourMasked)
{
    //the display to be sent to client
    string display;
    
    //establish the player hands
    vector<Card> playerTwoHand = a_plTwo->GetHand();
    vector<Card> playerFourHand = a_plFour->GetHand();
    
    //establish the location of the table card
    int cardLoc = (m_screenCenter - m_thirteenHandWidth - m_leftIndentation) - (m_tableCardHorizontalSize / 2);
    
    //establish the location of the fourth player box
    int outerBoxCenter = 2 * (m_screenCenter - m_thirteenHandWidth - m_leftIndentation) + (m_thirteenHandWidth / 2);
    int pFourBoxCenter = cardLoc + (m_thirteenHandWidth / 2) - 1;
    int pFourCardStartLoc = GetHandStartLocation(pFourBoxCenter, a_plFour->GetHandSize());
    
    //to fill screen with background
    string newLine = "\n";
    ColorDisplay(newLine, m_bGreen, m_tWhite);
    
    //add the 2nd player's name
    string plNames = GetPlNameDisplay(a_plTwo->GetUsername(), a_pTwoBoxCenterLoc, fourPlayer);
    
    //add the 4th player's name
    plNames += GetPlNameDisplay(a_plFour->GetUsername(), outerBoxCenter, fourPlayer) + "\n";
    
    //color the display
    ColorDisplay(plNames, m_bGreen, m_tWhite);
    display += plNames;
        
    //add the card top rank for all cards in 2nd player's hand
    display += GetCardTopRanksDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[fourPlayer], a_plTwoMasked);
    
    //add the card top rank for the table card
    display += GetCardTopRankDisplay(a_card, cardLoc, cardDisplay[full], false, true);
    
    //add the card top rank for all cards in 4th player's hand
    display += GetCardTopRanksDisplay(playerFourHand, pFourCardStartLoc, maxCards[fourPlayer], a_plFourMasked) + newLine;
        
    //add the card suit for all cards in 2nd player's hand
    display += GetCardSuitsDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[fourPlayer], a_plTwoMasked);
    
    //add the card suit for the table card
    display += GetCardSuitDisplay(a_card, cardLoc, cardDisplay[full], false, true);
    
    //add the card suit for all cards in 4th player's hand
    display += GetCardSuitsDisplay(playerFourHand, pFourCardStartLoc, maxCards[fourPlayer], a_plFourMasked) + newLine;
        
    //add the bottom rank for all cards in 2nd player's hand
    display += GetCardBottomRanksDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[fourPlayer], a_plTwoMasked);
    
    //add the bottom rank for the table card
    display += GetCardBottomRankDisplay(a_card, cardLoc, cardDisplay[full], false, true);
    
    //add the bottom rank for all cards in 4th player's hand
    display += GetCardBottomRanksDisplay(playerFourHand, pFourCardStartLoc, maxCards[fourPlayer], a_plFourMasked) + newLine;
    
    //add scoreboard for second player
    display += GetScoresDisplay(a_plTwoPoints, a_plTwoTricks, fourPlayer, a_pTwoBoxCenterLoc);
    
    //add scoreboard for fourth player
    display += GetScoresDisplay(a_plFourPoints, a_plFourTricks, fourPlayer, outerBoxCenter) + newLine;
    
    //close player 2 hand
    string plBoxes = GetCloseBoxDisplay(a_pTwoBoxCenterLoc, fourPlayer);
    
    //close player 4 hand
    plBoxes += GetCloseBoxDisplay(outerBoxCenter, fourPlayer) + "\n";
    
    //color the display
    ColorDisplay(plBoxes, m_bGreen, m_tWhite);
    display += plBoxes;
    
    //the completed display
    return display;
    
}
/* string Display::GetFourPlOneCardDisplay(Player* a_plTwo, Player* a_plFour, const Card& a_card, const int& a_pTwoBoxCenterLoc,
 const int& a_pTwoCardStartLoc, const int& a_plTwoPoints, const int& a_plFourPoints,
 const int& a_plTwoTricks, const int& a_plFourTricks, const bool& a_plTwoMasked,
 const bool& a_plFourMasked);*/


/*
 Display::GetFourPlTwoCardsDisplay Display::GetFourPlTwoCardsDisplay
NAME

        GetFourPlTwoCardsDisplay - to get the elements of a 4-player match display with 2 table cards
 
SYNOPSIS

        string Display::GetFourPlTwoCardsDisplay(Player* a_plTwo, Player* a_plFour, const Card& a_cardOne, const Card& a_cardTwo,
                                          const int& a_pTwoBoxCenterLoc, const int& a_pTwoCardStartLoc,
                                          const int& a_plTwoPoints, const int& a_plFourPoints, const int& a_plTwoTricks,
                                          const int& a_plFourTricks, const bool& a_plTwoMasked, const bool& a_plFourMasked);
 
            a_plTwo              --> the second player in the match
            a_plFour             --> the fourth player in the match
            a_cardOne            --> the first table card
            a_cardTwo            --> the second table card
            a_pTwoBoxCenterLoc   --> the center of the second player's information box
            a_pTwoCardStartLoc   --> the location of where the player hands should be printed
            a_plTwoPoints        --> the second player's points
            a_plFourPoints       --> the fourth player's points
            a_plTwoTricks        --> the second player's trick scores
            a_plFourTricks       --> the fourth player's trick scores
            a_plTwoMasked        --> whether suit and rank of cards in second player's hand should be displayed
            a_plFourMasked       --> whether suit and rank of cards in fourth player's hand should be displayed
 
            
DESCRIPTION

        This function puts together the elements of a 4-player match display that has
        2 table cards.

RETURNS

        The game display for a 4-player match with 2 table cards in string format
 
*/
/**/

string Display::GetFourPlTwoCardsDisplay(Player* a_plTwo, Player* a_plFour, const Card& a_cardOne, const Card& a_cardTwo,
                                         const int& a_pTwoBoxCenterLoc, const int& a_pTwoCardStartLoc,
                                         const int& a_plTwoPoints, const int& a_plFourPoints, const int& a_plTwoTricks,
                                         const int& a_plFourTricks, const bool& a_plTwoMasked, const bool& a_plFourMasked)
{
    //the display to be sent to client
    string display;
    
    //establish the player hands
    vector<Card> playerTwoHand = a_plTwo->GetHand();
    vector<Card> playerFourHand = a_plFour->GetHand();
    
    //identify the location of the first table card
    int cardOneLoc = m_screenCenter - m_thirteenHandWidth - m_leftIndentation - m_tableCardHorizontalSize - 1;
    int cardTwoLoc = 2; //relative to card one
    
    //identify the location of the fourth player's box
    int outerBoxCenter = 2 * (m_screenCenter - m_thirteenHandWidth - m_leftIndentation) + (m_thirteenHandWidth / 2);
    int pFourBoxCenter = cardOneLoc + (m_thirteenHandWidth / 2);
    int pFourCardStartLoc = GetHandStartLocation(pFourBoxCenter, a_plFour->GetHandSize());
    
    //to fill screen with background
    string newLine = "\n";
    ColorDisplay(newLine, m_bGreen, m_tWhite);
     
    //add the 2nd player's name
    string plNames = GetPlNameDisplay(a_plTwo->GetUsername(), a_pTwoBoxCenterLoc, fourPlayer);
     
    //add the 4th player's name
    plNames += GetPlNameDisplay(a_plFour->GetUsername(), outerBoxCenter, fourPlayer) + "\n";
    
    //color the display
    ColorDisplay(plNames, m_bGreen, m_tWhite);
    display += plNames;
         
    //add the card top rank for all cards in 2nd player's hand
    display += GetCardTopRanksDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[fourPlayer], a_plTwoMasked);
     
    //add the card top rank for the first table card
    display += GetCardTopRankDisplay(a_cardOne, cardOneLoc, cardDisplay[full], false, true);
    
    //add the card top rank for the second table card
    display += GetCardTopRankDisplay(a_cardTwo, cardTwoLoc, cardDisplay[full], false, true);
    
    //add the card top rank for all cards in 4th player's hand
    display += GetCardTopRanksDisplay(playerFourHand, pFourCardStartLoc, maxCards[fourPlayer], a_plFourMasked) + newLine;
         
    //add the card suit for all cards in 2nd player's hand
    display += GetCardSuitsDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[fourPlayer], a_plTwoMasked);
     
    //add the card suit for the first table card
    display += GetCardSuitDisplay(a_cardOne, cardOneLoc, cardDisplay[full], false, true);
    
    //add the card suit for the second table card
    display += GetCardSuitDisplay(a_cardTwo, cardTwoLoc, cardDisplay[full], false, true);
     
    //add the card suit for all cards in 4th player's hand
    display += GetCardSuitsDisplay(playerFourHand, pFourCardStartLoc, maxCards[fourPlayer], a_plFourMasked) + newLine;
     
    //add the bottom rank for all cards in 2nd player's hand
    display += GetCardBottomRanksDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[fourPlayer], a_plTwoMasked);
     
    //add the bottom rank for the first table card
    display += GetCardBottomRankDisplay(a_cardOne, cardOneLoc, cardDisplay[full], false, true);
    
    //add the bottom rank for the second table card
    display += GetCardBottomRankDisplay(a_cardTwo, cardTwoLoc, cardDisplay[full], false, true);
     
    //add the bottom rank for all cards in 4th player's hand
    display += GetCardBottomRanksDisplay(playerFourHand, pFourCardStartLoc, maxCards[fourPlayer], a_plFourMasked) + newLine;
     
    //add scoreboard for the second player
    display += GetScoresDisplay(a_plTwoPoints, a_plTwoTricks, fourPlayer, a_pTwoBoxCenterLoc);
    
    //add scoreboard for the fourth player
    display += GetScoresDisplay(a_plFourPoints, a_plFourTricks, fourPlayer, outerBoxCenter) + newLine;
     
    //close player 2 hand
    string plBoxes = GetCloseBoxDisplay(a_pTwoBoxCenterLoc, fourPlayer);
     
    //close player 4 hand
    plBoxes += GetCloseBoxDisplay(outerBoxCenter, fourPlayer) + "\n";
    
    //color the display
    ColorDisplay(plBoxes, m_bGreen, m_tWhite);
    display += plBoxes;
    
    //the completed display
    return display;
}
/* string Display::GetFourPlTwoCardsDisplay(Player* a_plTwo, Player* a_plFour, const Card& a_cardOne, const Card& a_cardTwo,
 const int& a_pTwoBoxCenterLoc, const int& a_pTwoCardStartLoc,
 const int& a_plTwoPoints, const int& a_plFourPoints, const int& a_plTwoTricks,
 const int& a_plFourTricks, const bool& a_plTwoMasked, const bool& a_plFourMasked);*/


/*
 Display::GetFourPlThreeCardsDisplay Display::GetFourPlThreeCardsDisplay
NAME

        GetFourPlThreeCardsDisplay - to get the elements of a 4-player match display with 3 table cards
 
SYNOPSIS

        string Display::GetFourPlThreeCardsDisplay(Player* a_plTwo, Player* a_plFour, const Card& a_cardOne, const Card& a_cardTwo,
                                            const Card& a_cardThree, const int& a_pTwoBoxCenterLoc, const int& a_pTwoCardStartLoc,
                                            const int& a_plTwoPoints, const int& a_plFourPoints, const int& a_plTwoTricks,
                                            const int& a_plFourTricks, const bool& a_plTwoMasked, const bool& a_plFourMasked);
 
            a_plTwo              --> the second player in the match
            a_plFour             --> the fourth player in the match
            a_cardOne            --> the first table card
            a_cardTwo            --> the second table card
            a_cardThree          --> the third table card
            a_pTwoBoxCenterLoc   --> the center of the second player's information box
            a_pTwoCardStartLoc   --> the location of where the player hands should be printed
            a_plTwoPoints        --> the second player's points
            a_plFourPoints       --> the fourth player's points
            a_plTwoTricks        --> the second player's trick scores
            a_plFourTricks       --> the fourth player's trick scores
            a_plTwoMasked        --> whether suit and rank of cards in second player's hand should be displayed
            a_plFourMasked       --> whether suit and rank of cards in fourth player's hand should be displayed
 
            
DESCRIPTION

        This function puts together the elements of a 4-player match display that has
        3 table cards.

RETURNS

        The game display for a 4-player match with 3 table cards in string format
 
*/
/**/

string Display::GetFourPlThreeCardsDisplay(Player* a_plTwo, Player* a_plFour, const Card& a_cardOne, const Card& a_cardTwo,
                                           const Card& a_cardThree, const int& a_pTwoBoxCenterLoc, const int& a_pTwoCardStartLoc,
                                           const int& a_plTwoPoints, const int& a_plFourPoints, const int& a_plTwoTricks,
                                           const int& a_plFourTricks, const bool& a_plTwoMasked, const bool& a_plFourMasked)
{
 
    //the display to be sent to client
    string display;
    
    //establish the player hands
    vector<Card> playerTwoHand = a_plTwo->GetHand();
    vector<Card> playerFourHand = a_plFour->GetHand();
    
    //identify the various locations of the first table card
    int cardOneFirstRowLoc = m_screenCenter - (m_tableCardHorizontalSize / 2);
    int cardOneRestRowLoc = cardOneFirstRowLoc - m_thirteenHandWidth - m_leftIndentation;
    
    //identify the location of the fourth player box
    int pFourBoxCenterOne = cardOneRestRowLoc + (m_thirteenHandWidth / 2);
    int pFourHandStartOne = GetHandStartLocation(pFourBoxCenterOne, a_plFour->GetHandSize()) - 1;
    
    //identify the various locations of the second table card
    int cardTwoLoc = m_screenCenter - m_thirteenHandWidth - m_leftIndentation - m_tableCardHorizontalSize - 1;
    int cardThreeLoc = 2; //relative to card two
    
    //identify the next location of the fourth player box
    int pFourBoxCenterOtherOne = cardTwoLoc + (m_thirteenHandWidth / 2);
    
    //calculate empty lines and spacing between table cards
    int horizontalSpaceLowerCards = m_screenCenter - m_tableCardHorizontalSize - 1;
    int emptyLineSpace = 2 * (m_screenCenter - m_thirteenHandWidth - m_leftIndentation) + (m_thirteenHandWidth / 2);
    int emptyLineStart = GetHandStartLocation(emptyLineSpace, a_plFour->GetHandSize());
    
    //to fill screen with background
    string newLine = "\n";
    ColorDisplay(newLine, m_bGreen, m_tWhite);
    
    //add the 2nd player's name
    string plTwoName = GetPlNameDisplay(a_plTwo->GetUsername(), a_pTwoBoxCenterLoc, fourPlayer);
    ColorDisplay(plTwoName, m_bGreen, m_tWhite);
    display += plTwoName;
    
    //add the top rank for the first table card
    display += GetCardTopRankDisplay(a_cardOne, cardOneRestRowLoc, cardDisplay[full], false, true);
    
    //add the 4th player's name
    string plFourName = GetPlNameDisplay(a_plFour->GetUsername(), pFourBoxCenterOne, fourPlayer) + "\n";
    ColorDisplay(plFourName, m_bGreen, m_tWhite);
    display += plFourName;
    
    //add the card top rank for all cards in 2nd player's hand
    display += GetCardTopRanksDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[fourPlayer], a_plTwoMasked);
    
    //add the card suit for the first table card
    display += GetCardSuitDisplay(a_cardOne, cardOneRestRowLoc, cardDisplay[full], false, true);
         
    //add the card top rank for all cards in 4th player's hand
    display += GetCardTopRanksDisplay(playerFourHand, pFourHandStartOne, maxCards[fourPlayer], a_plFourMasked) + newLine;

    //add the card suit for all cards in 2nd player's hand
    display += GetCardSuitsDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[fourPlayer], a_plTwoMasked);
    
    //add the card bottom rank for the first table card
    display += GetCardBottomRankDisplay(a_cardOne, cardOneRestRowLoc, cardDisplay[full], false, true);
     
    //add the card suit for all cards in 4th player's hand
    display += GetCardSuitsDisplay(playerFourHand, pFourHandStartOne, maxCards[fourPlayer], a_plFourMasked) + newLine;
         
    //add the card bottom rank for all cards in 2nd player's hand
    display += GetCardBottomRanksDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[fourPlayer], a_plTwoMasked);
    
    //add the card bottom rank for all cards in 4th player's hand
    display += GetCardBottomRanksDisplay(playerFourHand, emptyLineStart, maxCards[fourPlayer], a_plFourMasked) + newLine;
     
    //display scoreboard for player 2
    display += GetScoresDisplay(a_plTwoPoints, a_plTwoTricks, fourPlayer, a_pTwoBoxCenterLoc);
    
    //display the card top rank for the second table card
    display += GetCardTopRankDisplay(a_cardTwo, cardTwoLoc, cardDisplay[full], false, true);
    
    //display the card top rank for the third table card
    display += GetCardTopRankDisplay(a_cardThree, cardThreeLoc, cardDisplay[full], false, true);
     
    //display scoreboard for player 4
    display += GetScoresDisplay(a_plFourPoints, a_plFourTricks, fourPlayer, pFourBoxCenterOtherOne) + newLine;
     
    //close player 2 hand
    string closePlTwo = GetCloseBoxDisplay(a_pTwoBoxCenterLoc, fourPlayer);
    
    //add color
    ColorDisplay(closePlTwo, m_bGreen, m_tWhite);
    display += closePlTwo;
    
    //add the card suit for the second table card
    display += GetCardSuitDisplay(a_cardTwo, cardTwoLoc, cardDisplay[full], false, true);
    
    //add the card suit for the third table card
    display += GetCardSuitDisplay(a_cardThree, cardThreeLoc, cardDisplay[full], false, true);
     
    //close player 4 hand
    string closePlFour = GetCloseBoxDisplay(pFourBoxCenterOtherOne, fourPlayer) + "\n";
    
    //add color
    ColorDisplay(closePlFour, m_bGreen, m_tWhite);
    display += closePlFour;
    
    //add the card bottom rank for the second table card
    display += GetCardBottomRankDisplay(a_cardTwo, horizontalSpaceLowerCards, cardDisplay[full], false, true);
    
    //add the card bottom rank for the third table card
    display += GetCardBottomRankDisplay(a_cardThree, cardThreeLoc, cardDisplay[full], false, true) + newLine;
    
    //the completed display
    return display;
}
/*string Display::GetFourPlThreeCardsDisplay(Player* a_plTwo, Player* a_plFour, const Card& a_cardOne, const Card& a_cardTwo,
 const Card& a_cardThree, const int& a_pTwoBoxCenterLoc, const int& a_pTwoCardStartLoc,
 const int& a_plTwoPoints, const int& a_plFourPoints, const int& a_plTwoTricks,
 const int& a_plFourTricks, const bool& a_plTwoMasked, const bool& a_plFourMasked);*/


/*
 Display::GetFourPlFourCardsDisplay Display::GetFourPlFourCardsDisplay
NAME

        GetFourPlFourCardsDisplay - to get the elements of a 4-player match display with 4 table cards
 
SYNOPSIS

        string Display::GetFourPlFourCardsDisplay(Player* a_plTwo, Player* a_plFour, const vector<Card>& a_cards,
                                           const int& a_pTwoBoxCenterLoc, const int& a_pTwoCardStartLoc,
                                           const int& a_plTwoPoints, const int& a_plFourPoints, const int& a_plTwoTricks,
                                           const int& a_plFourTricks, const bool& a_plTwoMasked, const bool& a_plFourMasked);
 
            a_plTwo              --> the second player in the match
            a_plFour             --> the fourth player in the match
            a_cards              --> the table cards
            a_pTwoBoxCenterLoc   --> the center of the second player's information box
            a_pTwoCardStartLoc   --> the location of where the player hands should be printed
            a_plTwoPoints        --> the second player's points
            a_plFourPoints       --> the fourth player's points
            a_plTwoTricks        --> the second player's trick scores
            a_plFourTricks       --> the fourth player's trick scores
            a_plTwoMasked        --> whether suit and rank of cards in second player's hand should be displayed
            a_plFourMasked       --> whether suit and rank of cards in fourth player's hand should be displayed
 
            
DESCRIPTION

        This function puts together the elements of a 4-player match display that has
        4 table cards.

RETURNS

        The game display for a 4-player match with 4 table cards in string format
 
*/
/**/

string Display::GetFourPlFourCardsDisplay(Player* a_plTwo, Player* a_plFour, const vector<Card>& a_cards,
                                          const int& a_pTwoBoxCenterLoc, const int& a_pTwoCardStartLoc,
                                          const int& a_plTwoPoints, const int& a_plFourPoints, const int& a_plTwoTricks,
                                          const int& a_plFourTricks, const bool& a_plTwoMasked, const bool& a_plFourMasked)
{
    //the display to be sent to client
    string display;
    
    //establish the player hands
    vector<Card> playerTwoHand = a_plTwo->GetHand();
    vector<Card> playerFourHand = a_plFour->GetHand();
    
    //establish the cards
    Card cardOne = a_cards[0];
    Card cardTwo = a_cards[1];
    Card cardThree = a_cards[2];
    Card cardFour = a_cards[3];
    
    //identify the locations associated with first table card
    int cardOneFirstRowLoc = m_screenCenter - m_tableCardHorizontalSize - 1;
    int plFourNamePosition = cardOneFirstRowLoc - (m_thirteenHandWidth / 2) - m_leftIndentation - 1;
    int cardOneRestRowLoc = cardOneFirstRowLoc - m_thirteenHandWidth - m_leftIndentation;
    int plFourBoxCenterTwo = cardOneRestRowLoc + (m_thirteenHandWidth / 2);
    int plFourHandStartTwo = GetHandStartLocation(plFourBoxCenterTwo, a_plFour->GetHandSize());
    
    //identify the locations associated with the other table cards
    int cardTwoLoc = 2; //relative to card one
    int cardThreeLoc = m_screenCenter - m_thirteenHandWidth - m_leftIndentation - m_tableCardHorizontalSize - 1;
    int cardFourLoc = 2; //relative to card two
    
    //calculate the spacing between the table cards
    int emptyLineSpace = 2 * (m_screenCenter - m_thirteenHandWidth - m_leftIndentation) + (m_thirteenHandWidth / 2);
    int emptyLineStart = GetHandStartLocation(emptyLineSpace, a_plFour->GetHandSize());
    int bottomCardsSpace = m_screenCenter - m_tableCardHorizontalSize - 1;
    
    //to fill screen with background
    string newLine = "\n";
    ColorDisplay(newLine, m_bGreen, m_tWhite);
    
    //add the 2nd player's name
    string plTwoName = GetPlNameDisplay(a_plTwo->GetUsername(), a_pTwoBoxCenterLoc, fourPlayer);
    ColorDisplay(plTwoName, m_bGreen, m_tWhite);
    display += plTwoName;
    
    //add the card top rank for the first table card
    display += GetCardTopRankDisplay(cardOne, cardOneRestRowLoc, cardDisplay[full], false, true);
    
    //add the card top rank for the second table card
    display += GetCardTopRankDisplay(cardTwo, cardTwoLoc, cardDisplay[full], false, true);
    
    //add the 4th player's name
    string plFourName = GetPlNameDisplay(a_plFour->GetUsername(), plFourNamePosition, fourPlayer) + "\n";
    ColorDisplay(plFourName, m_bGreen, m_tWhite);
    display += plFourName;
    
    //add the card top rank for all cards in 2nd player's hand
    display += GetCardTopRanksDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[fourPlayer], a_plTwoMasked);
    
    //add the card suit for the first table card
    display += GetCardSuitDisplay(cardOne, cardOneRestRowLoc, cardDisplay[full], false, true);
    
    //add the card suit for the second table card
    display += GetCardSuitDisplay(cardTwo, cardTwoLoc, cardDisplay[full], false, true);
    
    //add the card top rank for all cards in 4th player's hand
    display += GetCardTopRanksDisplay(playerFourHand, plFourHandStartTwo, maxCards[fourPlayer], a_plFourMasked) + newLine;
    
    //add the card suit for all cards in 2nd player's hand
    display += GetCardSuitsDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[fourPlayer], a_plTwoMasked);
    
    //add the card bottom rank for the first table card
    display += GetCardBottomRankDisplay(cardOne, cardOneRestRowLoc, cardDisplay[full], false, true);
    
    //add the card bottom rank for the second table card
    display += GetCardBottomRankDisplay(cardTwo, cardTwoLoc, cardDisplay[full], false, true);
    
    //add the card suit for all cards in 4th player's hand
    display += GetCardSuitsDisplay(playerFourHand, plFourHandStartTwo, maxCards[fourPlayer], a_plFourMasked) + newLine;
     
    //add the card bottom rank for all cards in 2nd player's hand
    display += GetCardBottomRanksDisplay(playerTwoHand, a_pTwoCardStartLoc, maxCards[fourPlayer], a_plTwoMasked);
    
    //add the card bottom rank for all cards in 4th player's hand
    display += GetCardBottomRanksDisplay(playerFourHand, emptyLineStart, maxCards[fourPlayer], a_plFourMasked) + newLine;
     
    //add scoreboard for player 2
    display += GetScoresDisplay(a_plTwoPoints, a_plTwoTricks, fourPlayer, a_pTwoBoxCenterLoc);
    
    //add the card top rank for the third table card
    display += GetCardTopRankDisplay(cardThree, cardThreeLoc, cardDisplay[full], false, true);
    
    //add the card top rank for the fourth table card
    display += GetCardTopRankDisplay(cardFour, cardFourLoc, cardDisplay[full], false, true);
     
    //add scoreboard for player 4
    display += GetScoresDisplay(a_plFourPoints, a_plFourTricks, fourPlayer, plFourBoxCenterTwo) + newLine;
     
    //close player 2 hand
    string plTwoClose = GetCloseBoxDisplay(a_pTwoBoxCenterLoc, fourPlayer);
    ColorDisplay(plTwoClose, m_bGreen, m_tWhite);
    display += plTwoClose;
    
    //add the card suit for the third table card
    display += GetCardSuitDisplay(cardThree, cardThreeLoc, cardDisplay[full], false, true);
    
    //add the card suit for the fourth table card
    display += GetCardSuitDisplay(cardFour, cardFourLoc, cardDisplay[full], false, true);
     
    //close player 4 hand
    string plFourClose = GetCloseBoxDisplay(plFourBoxCenterTwo, fourPlayer) + "\n";
    ColorDisplay(plFourClose, m_bGreen, m_tWhite);
    display += plFourClose;
    
    //add the card bottom rank for the third table card
    display += GetCardBottomRankDisplay(cardThree, bottomCardsSpace, cardDisplay[full], false, true);
    
    //add the card bottom rank for the fourth table card
    display += GetCardBottomRankDisplay(cardFour, cardFourLoc, cardDisplay[full], false, true) + newLine;
    
    //the completed display
    return display;
}
/*string Display::GetFourPlFourCardsDisplay(Player* a_plTwo, Player* a_plFour, const vector<Card>& a_cards,
 const int& a_pTwoBoxCenterLoc, const int& a_pTwoCardStartLoc,
 const int& a_plTwoPoints, const int& a_plFourPoints, const int& a_plTwoTricks,
 const int& a_plFourTricks, const bool& a_plTwoMasked, const bool& a_plFourMasked);*/


//to append multiple new lines together
string Display::GetNewLines(int num)
{
    string newLines;
    for (int i=0; i<num; i++)
        newLines += '\n';
    
    return newLines;
}


/*
 Display::GetHandDisplay Display::GetHandDisplay
NAME

        GetHandDisplay - to get the elements of a player hand
 
SYNOPSIS

        string Display::GetHandDisplay(const vector<Card>& a_hand, const int& a_position,
                                const int& a_maxCards, const bool& a_masked);

            a_hand     --> the player's hand
            a_position --> the position on the screen to display the hand
            a_maxCards --> the maximum cards that could be in the hand
            a_masked   --> whether the suit and rank of the cards should be hidden
            
DESCRIPTION

        This function creates the display for a player hand using the
        cards in their hand.

RETURNS

        The hand display in string format
 
*/
/**/

string Display::GetHandDisplay(const vector<Card>& a_hand, const int& a_position,
                               const int& a_maxCards, const bool& a_masked)
{
    string display;
    string newLine = "\n";
    
    display += newLine;
    
    //add the top rank for all the cards
    display += GetCardTopRanksDisplay(a_hand, a_position, a_maxCards, a_masked) + newLine;
    
    //add the suit for all the cards
    display += GetCardSuitsDisplay(a_hand, a_position, a_maxCards, a_masked) + newLine;
    
    //add the bottom rank for all the cards
    display += GetCardBottomRanksDisplay(a_hand, a_position, a_maxCards, a_masked) + newLine;
    
    return display;
}
/*string Display::GetHandDisplay(const vector<Card>& a_hand, const int& a_position,
 const int& a_maxCards, const bool& a_masked);*/


/*
 Display::GetCardDisplay Display::GetCardDisplay
NAME

        GetCardDisplay - to get the interface of a card
 
SYNOPSIS

        string Display::GetCardDisplay(const Card& a_card, const int& a_position, const bool& a_fullCard);

            a_card     --> the card to be displayed
            a_position --> the position on the screen to display the card
            a_fullCard --> to display a full or partial card
            
DESCRIPTION

        This function creates the display for a card.

RETURNS

        The card display in string format
 
*/
/**/

string Display::GetCardDisplay(const Card& a_card, const int& a_position, const bool& a_fullCard)
{
    //the display to be sent to client
    string display;
    
    //whether card rank and suit should be masked
    bool masked = false;
    string newLine = "\n";
    
    //add the top rank for the card
    display += GetCardTopRankDisplay(a_card, a_position, a_fullCard, masked, true) + newLine;
    
    //add the suit for the card
    display += GetCardSuitDisplay(a_card, a_position, a_fullCard, masked, true) + newLine;
    
    //add the bottom rank for the card
    display += GetCardBottomRankDisplay(a_card, a_position, a_fullCard, masked, true) + newLine;
    
    //the completed display
    return display;
}
/*string Display::GetCardDisplay(const Card& a_card, const int& a_position, const bool& a_firstCardInHand);*/


/*
 Display::GetCardTopRankDisplay Display::GetCardTopRankDisplay
NAME

        GetCardTopRankDisplay - to get the interface of the top rank of a card
 
SYNOPSIS

        string Display::GetCardTopRankDisplay(const Card& a_card, const int& a_position,
                                       const bool& a_fullCard, const bool& a_masked, const bool& a_sCard);

            a_card     --> the card which its top rank will be displayed
            a_position --> the position on the screen to display this component
            a_fullCard --> to display top rank for a full or partial card
            a_masked   --> whether the rank should be masked or not
            a_sCard    --> whether the card is being displayed as a single card
            
DESCRIPTION

        This function creates the top rank of a card.

RETURNS

        The card top rank display in string format
 
*/
/**/

string Display::GetCardTopRankDisplay(const Card& a_card, const int& a_position,
                                      const bool& a_fullCard, const bool& a_masked, const bool& a_sCard)
{
    //add necessary spacing
    string display;
    
    string spacing = AddText(m_space, a_position);
    ColorDisplay(spacing, m_bGreen, m_tGreen);
    
    string firstPart;
    if (a_fullCard)
    {
        //if the card is being displayed as a single card
        if (a_sCard)
            firstPart = "";
        else
            firstPart = m_pipe;
        
        ColorDisplay(firstPart, m_bWhite, m_tBlack);
    }
    
    string secondPart;
    
    //if the hand information is not shown
    if (a_masked)
    {
        secondPart += m_space;
        ColorDisplay(secondPart, m_bWhite, m_bWhite);
    }
    
    else
    {
        //the number where ten and the Face cards start
        int symbolicCardStart = 10;
        
        //if the rank is symbolic
        if (a_card.GetRank() > symbolicCardStart)
            secondPart = rankChar[(SymbolicCards)a_card.GetRank()];
        
        //if the rank is 10
        else if (a_card.GetRank() == symbolicCardStart)
            secondPart = rankChar[ten];
        
        //if the rank is numeric
        else
            secondPart = to_string(a_card.GetRank());
        
        if (a_card.GetSuit() == hearts || a_card.GetSuit() == diamonds)
            ColorDisplay(secondPart, m_bWhite, m_tRed);
        else
            ColorDisplay(secondPart, m_bWhite, m_tBlack);
    }
    
    //if the full card is being displayed
    string thirdPart;
    if (a_fullCard)
    {
        thirdPart = AddText(m_space, 2);
        ColorDisplay(thirdPart, m_bWhite, m_tBlack);
    }
    
    string fourthPart;
    
    //if the card is being displayed as a single card
    if (a_sCard)
        fourthPart = "";
    else
        fourthPart = m_pipe;
    
    ColorDisplay(fourthPart, m_bWhite, m_tBlack);
    display += spacing + firstPart + secondPart + thirdPart + fourthPart;
    
    //the completed display
    return display;
}
/*string Display::GetCardTopRankDisplay(const Card& a_card, const int& a_position,
 const bool& a_fullCard, const bool& a_masked, const bool& a_sCard);*/


/*
 Display::GetCardSuitDisplay Display::GetCardSuitDisplay
NAME

        GetCardSuitDisplay - to get the interface of the suit of a card
 
SYNOPSIS

        string Display::GetCardSuitDisplay(const Card& a_card, const int& a_position,
        const bool& a_fullCard, const bool& a_masked, const bool& a_sCard);

            a_card     --> the card which its suit will be displayed
            a_position --> the position on the screen to display this component
            a_fullCard --> to display the suit for a full or partial card
            a_masked   --> whether the suit should be masked or not
            a_sCard    --> whether the card is being displayed as a single card
            
DESCRIPTION

        This function creates the suit of a card.

RETURNS

        The card suit display in string format
 
*/
/**/

string Display::GetCardSuitDisplay(const Card& a_card, const int& a_position, const bool& a_fullCard, const bool& a_masked, const bool& a_sCard)
{
    //add necessary spacing
    string display;
    
    string spacing = AddText(m_space, a_position);
    ColorDisplay(spacing, m_bGreen, m_tGreen);
    
    string firstPart;
    
    //if a full card is being displayed
    if (a_fullCard)
    {
        //if it's a single card
        if (a_sCard)
            firstPart = "";
        else
            firstPart = m_pipe;
            
        firstPart += AddText(m_space, 1);
        ColorDisplay(firstPart, m_bWhite, m_tBlack);
    }
    
    string secondPart;
    
    //if hand info is being hidden
    if (a_masked)
    {
        secondPart = m_space;
        ColorDisplay(secondPart, m_bWhite, m_tGreen);
    }
        
    else
    {
        secondPart = suitSymbol[a_card.GetSuit()];
        
        //color the suit (hearts and diamonds)
        if (a_card.GetSuit() == hearts || a_card.GetSuit() == diamonds)
            ColorDisplay(secondPart, m_bWhite, m_tRed);
        else
            ColorDisplay(secondPart, m_bWhite, m_tBlack);
    }
        
    string thirdPart;
    if (a_fullCard)
    {
        thirdPart = AddText(m_space, 1);
        ColorDisplay(thirdPart, m_bWhite, m_tGreen);
    }
    
    string fourthPart;
    if (a_sCard)
        fourthPart = "";
    else
        fourthPart = m_pipe;
        
    ColorDisplay(fourthPart, m_bWhite, m_tBlack);
    display += spacing + firstPart + secondPart + thirdPart + fourthPart;
    
    return display;
}
/*string Display::GetCardSuitDisplay(const Card& a_card, const int& a_position,
 const bool& a_fullCard, const bool& a_masked, const bool& a_sCard);*/


/*
 Display::GetCardBottomRankDisplay Display::GetCardBottomRankDisplay
NAME

        GetCardBottomRankDisplay - to get the interface of the bottom rank of a card
 
SYNOPSIS

        string Display::GetCardBottomRankDisplay(const Card& a_card, const int& a_position,
        const bool& a_fullCard, const bool& a_masked, const bool& a_sCard);

            a_card     --> the card which its bottom rank will be displayed
            a_position --> the position on the screen to display this component
            a_fullCard --> to display bottom rank for a full or partial card
            a_masked   --> whether the rank should be masked or not
            a_sCard    --> whether the card is being displayed as a single card
            
DESCRIPTION

        This function creates the bottom rank of a card.

RETURNS

        The card bottom rank display in string format
 
*/
/**/

string Display::GetCardBottomRankDisplay(const Card& a_card, const int& a_position, const bool& a_fullCard,
                                         const bool& a_masked, const bool& a_sCard)
{
    //add necessary spacing
    string display;
    
    string spacing = AddText(m_space, a_position);
    ColorDisplay(spacing, m_bGreen, m_tGreen);
    
    string firstPart;
    
    //if a full card is being displayed
    if (a_fullCard)
    {
        //if it's a single card
        if (a_sCard)
            firstPart = "";
        else
            firstPart = m_pipe;
    }
    
    //add necessary spacing
    firstPart += AddText(m_space, 1);
    ColorDisplay(firstPart, m_bWhite, m_tBlack);
    
    string secondPart, thirdPart;
    if (a_fullCard)
    {
        //add necessary spacing
        secondPart += AddText(m_space, 1);
        
        //if hand info is being hidden
        if (a_masked)
            secondPart += m_space;
        
        else
        {
            //the number where ten and the face cards start
            int symbolicCardStart = 10;
            
            //if the rank is symbolic
            if (a_card.GetRank() > symbolicCardStart)
                thirdPart = rankChar[(SymbolicCards)a_card.GetRank()];
            
            //if the rank is 10
            else if (a_card.GetRank() == symbolicCardStart)
                thirdPart = rankChar[ten];
            
            //if the rank is numeric
            else
                thirdPart = to_string(a_card.GetRank());
            
            //color the suit (hearts and diamonds)
            if (a_card.GetSuit() == hearts || a_card.GetSuit() == diamonds)
                ColorDisplay(thirdPart, m_bWhite, m_tRed);
            else
                ColorDisplay(thirdPart, m_bWhite, m_tBlack);
        }
        
        ColorDisplay(secondPart, m_bWhite, m_tGreen);
    }
    
    string fourthPart;
    
    //if it's a single card
    if (a_sCard)
        fourthPart = "";
    else
        fourthPart = m_pipe;
    
    ColorDisplay(fourthPart, m_bWhite, m_tBlack);
    
    display += spacing + firstPart + secondPart + thirdPart + fourthPart;
    
    //the completed display 
    return display;
}
/*string Display::GetCardBottomRankDisplay(const Card& a_card, const int& a_position,
 const bool& a_fullCard, const bool& a_masked, const bool& a_sCard);*/


/*
 Display::GetHandStartLocation Display::GetHandStartLocation
NAME

        GetHandStartLocation - to identify the location to start displaying the hand on the screen
 
SYNOPSIS

        int Display::GetHandStartLocation(const int& a_boxCenter, const int& a_handSize);

            a_boxCenter --> the center of a player's box
            a_handSize  --> the number of cards in the player's hand
            
DESCRIPTION

        This function identifies the location where the cards in hand
        should be printed based on the number of cards in the player's hand.

RETURNS

        The location to print the hand
 
*/
/**/

int Display::GetHandStartLocation(const int& a_boxCenter, const int& a_handSize)
{
    //initially the starting location is the center of box
    int handStartLoc = a_boxCenter;
        
    //if there is any cards in the player hand
    if (a_handSize != 0)
    {
        //subtract half of the the full card size
        handStartLoc -= (m_fullCardHorizontalSize / 2);
            
        //subtract half of the remaining partial cards
        handStartLoc -= a_handSize - 1;
    }
    
    return handStartLoc;
}
/*int Display::GetHandStartLocation(const int& a_boxCenter, const int& a_handSize);*/


/*
 Display::GetCardTopRanksDisplay Display::GetCardTopRanksDisplay
NAME

        GetCardTopRanksDisplay - to get the interface of the top rank of all the cards in a hand
 
SYNOPSIS

        string Display::GetCardTopRanksDisplay(const vector<Card>& a_hand, const int& a_position,
        const int& a_maxCards, const bool& a_masked);

            a_hand     --> the player's hand
            a_position --> the position on the screen to display the ranks
            a_maxCards --> the maximum number of cards in the player's hand
            a_masked   --> whether or not the ranks should be masked
            
DESCRIPTION

        This function creates the interface for displaying the top ranks of all the
        cards in a player's hand.

RETURNS

        The card top ranks interface in string format
 
*/
/**/

string Display::GetCardTopRanksDisplay(const vector<Card>& a_hand, const int& a_position, const int& a_maxCards, const bool& a_masked)
{
    //add necessary spacing
    string spacing = AddText(m_space, a_position);
    ColorDisplay(spacing, m_bGreen, m_tGreen);
    string display = spacing;
    
    int handSize = (int)a_hand.size();
    
    //the position to start printing the ranks
    int startIndex;
    
    if (handSize == 0)
    {
        //prevent first loop from running
        startIndex = - 1;
    }
    
    else
        startIndex = handSize - 1;
    
    //whether a card is the first card in the hand
    bool first;
    
    //the space between the cards in the hand
    int spaceBetweenCards = 0;
    
    for (int i = startIndex; i >= 0; i--)
    {
        if (i == startIndex)
            first = true;
        else
            first = false;
        
        //get the interface for the top rank
        display += GetCardTopRankDisplay(a_hand[i], spaceBetweenCards, first, a_masked, false);
    }
    
    //add empty spaces for remaining spots
    string remainingSpaces;
    
    //first add empty space for the full card (if applicable)
    if (handSize == 0)
        remainingSpaces += AddText(m_space, m_fullCardHorizontalSize / 2);
    
    //add empty space for the remaining cards
    for (int i=0; i < a_maxCards - handSize; i++)
        remainingSpaces += AddText(m_space, m_pileCardHorizontalSize / 2);
    
    ColorDisplay(remainingSpaces, m_bGreen, m_tGreen);
    display += remainingSpaces;
    
    //the completed display
    return display;
}


/*
 Display::GetCardSuitsDisplay Display::GetCardSuitsDisplay
NAME

        GetCardSuitsDisplay - to get the interface of the suit of all the cards in a hand
 
SYNOPSIS

        string Display::GetCardSuitsDisplay(const vector<Card>& a_hand, const int& a_position,
        const int& a_maxCards, const bool& a_masked);

            a_hand     --> the player's hand
            a_position --> the position on the screen to display the suits
            a_maxCards --> the maximum number of cards in the player's hand
            a_masked   --> whether or not the suits should be masked
            
DESCRIPTION

        This function creates the interface for displaying the suits of all the
        cards in a player's hand.

RETURNS

        The card suits interface of a hand in string format
 
*/
/**/

string Display::GetCardSuitsDisplay(const vector<Card>& a_hand, const int& a_position, const int& a_maxCards, const bool& a_masked)
{
    //add necessary spacing
    string spacing = AddText(m_space, a_position);
    ColorDisplay(spacing, m_bGreen, m_tWhite);
    string display = spacing;
    
    int handSize = (int)a_hand.size();
    
    //the position to start printing the suits
    int startIndex;
    
    if (handSize == 0)
    {
        //prevent first loop from running
        startIndex = - 1;
    }
    
    else
        startIndex = handSize - 1;
    
    //whether a card is the first card in the pile
    bool first;
    
    //the space between the cards in the hand
    int spaceBetweenCards = 0;
    
    for (int i = startIndex; i >= 0; i--)
    {
        if (i == startIndex)
            first = true;
        else
            first = false;
        
        //get the interface for the suit
        display += GetCardSuitDisplay(a_hand[i], spaceBetweenCards, first, a_masked, false);
    }
    
    //add empty spaces for remaining spots
    string remainingSpaces;
    
    //first add empty space for the full card (if applicable)
    if (handSize == 0)
        remainingSpaces += AddText(m_space, m_fullCardHorizontalSize / 2);
    
    //add empty space for the remaining cards
    for (int i=0; i< a_maxCards - handSize; i++)
        remainingSpaces += AddText(m_space, m_pileCardHorizontalSize / 2);
    
    ColorDisplay(remainingSpaces, m_bGreen, m_tWhite);
    display += remainingSpaces;
    
    //the completed display
    return display;
}
/*string Display::GetCardSuitsDisplay(const vector<Card>& a_hand, const int& a_position,
 const int& a_maxCards, const bool& a_masked)*/


/*
 Display::GetCardBottomRanksDisplay Display::GetCardBottomRanksDisplay
NAME

        GetCardBottomRanksDisplay - to get the interface of the bottom rank of all the cards in a hand
 
SYNOPSIS

        string Display::GetCardBottomRanksDisplay(const vector<Card>& a_hand, const int& a_position,
                                           const int& a_maxCards, const bool& a_masked);

            a_hand     --> the player's hand
            a_position --> the position on the screen to display the ranks
            a_maxCards --> the maximum number of cards in the player's hand
            a_masked   --> whether or not the ranks should be masked
            
DESCRIPTION

        This function creates the interface for displaying the bottom ranks of all the
        cards in a player's hand.

RETURNS

        The card bottom ranks interface in string format
 
*/
/**/

string Display::GetCardBottomRanksDisplay(const vector<Card>& a_hand, const int& a_position,
                                          const int& a_maxCards, const bool& a_masked)
{
    //add necessary spacing
    string spacing = AddText(m_space, a_position);
    ColorDisplay(spacing, m_bGreen, m_tWhite);
    string display = spacing;
    
    int handSize = (int)a_hand.size();
    
    //the position to start printing the ranks
    int startIndex;
    
    if (handSize == 0)
    {
        //prevent first loop from running
        startIndex = - 1;
    }
    
    else
        startIndex = handSize - 1;
    
    //whether a card is the first card in the hand
    bool first;
    
    //the space between the cards in the hand
    int spaceBetweenCards = 0;
    
    for (int i = startIndex; i >= 0; i--)
    {
        if (i == startIndex)
            first = true;
        else
            first = false;
        
        //get the bottom rank interface
        display += GetCardBottomRankDisplay(a_hand[i], spaceBetweenCards, first, a_masked, false);
    }
    
    //add empty spaces for remaining spots
    string remainingSpaces;
    
    //first add empty space for the full card (if applicable)
    if (handSize == 0)
        remainingSpaces += AddText(m_space, m_fullCardHorizontalSize / 2);
    
    //add empty space for the remaining cards
    for (int i=0; i< a_maxCards - handSize; i++)
        remainingSpaces += AddText(m_space, m_pileCardHorizontalSize / 2);
    
    ColorDisplay(remainingSpaces, m_bGreen, m_tWhite);
    display += remainingSpaces;
    
    //the completed display
    return display;
}
/* string Display::GetCardBottomRanksDisplay(const vector<Card>& a_hand, const int& a_position,
 const int& a_maxCards, const bool& a_masked);*/


/*
 Display::GetPlNameDisplay Display::GetPlNameDisplay
NAME

        GetPlNameDisplay - to get the interface for displaying a username on the screen
 
SYNOPSIS

        string Display::GetPlNameDisplay(const string& a_username, const int& a_relativeCenter,
        const int& a_matchMode);
 
            a_username       --> the username to be displayed
            a_relativeCenter --> the relative center of the player's box
            a_matchMode      --> the number of players in the match
            
DESCRIPTION

        This function creates the interface for displaying a player's username relative
        to their box on the screen.

RETURNS

        The player's username interface in string format
 
*/
/**/

string Display::GetPlNameDisplay(const string& a_username, const int& a_relativeCenter, const int& a_matchMode)
{
    //the display to be sent to client
    string display;
    
    //add necessary spacing
    int spaceFromStart = GetRelativePosition(a_username, a_relativeCenter);
    string spaces = AddText(m_space, spaceFromStart);
    display = spaces + a_username;
    
    //the maximum spaces it takes to cover the player's hand
    int maxHandWidth;
    
    if (a_matchMode == threePlayer)
        maxHandWidth = m_seventeenHandWidth;
    else
        maxHandWidth = m_thirteenHandWidth;
    
    //calculate the remaining spaces to add
    int takenSpaces = spaceFromStart + (int)a_username.size();
    int fullSpace = maxHandWidth + m_leftIndentation;
    
    //add empty spaces for remaining spots
    for (int i=0; i < fullSpace - takenSpaces; i++)
        display += m_space;
    
    //ColorDisplay(display, m_bGreen, m_tWhite);
    
    //the completed display
    return display;
}
/* string Display::GetPlNameDisplay(const string& a_username, const int& a_relativeCenter,
 const int& a_matchMode);*/


/*
 Display::GetScoresDisplay Display::GetScoresDisplay
NAME

        GetScoresDisplay - to get the interface for displaying player points and tricks on the screen
 
SYNOPSIS

        string Display::GetScoresDisplay(const int& a_points, const int& a_tricks, const int& a_matchMode,
        const int& a_relativeCenter);
 
            a_points         --> the points to be displayed
            a_tricks         --> the trick score to be displayed
            a_matchMode      --> the number of players in the match
            a_relativeCenter --> the relative center of the player's box
            
DESCRIPTION

        This function creates the interface for displaying a player's score board
        on the screen.

RETURNS

        The player's score board interface in string format
 
*/
/**/

string Display::GetScoresDisplay(const int& a_points, const int& a_tricks, const int& a_matchMode,
                                 const int& a_relativeCenter)
{
    //create the player's points and tricks display
    string pointsInfo = m_pointsTitle + m_space + to_string(a_points);
    string tricksInfo = m_tricksTitle + m_space + to_string(a_tricks);
    
    //the maximum spaces it takes to cover the player's hand
    int maxHandWidth;
    
    if (a_matchMode == threePlayer)
        maxHandWidth = m_seventeenHandWidth;
    else
        maxHandWidth = m_thirteenHandWidth;
    
    //identify the space between the points and tricks
    int numSpaces = maxHandWidth - (int)pointsInfo.size() - (int)tricksInfo.size();
    string spaceBetween = AddText(m_space, numSpaces);
    
    //create the scoreboard display
    string scoresInfo = pointsInfo + spaceBetween + tricksInfo;
    int spaceFromStart = GetRelativePosition(scoresInfo, a_relativeCenter);
    
    //add necessary spacing
    string display = AddText(m_space, spaceFromStart) + scoresInfo;
    
    ColorDisplay(display, m_bGreen, m_tWhite);
    
    //the completed display
    return display;
}
/*string Display::GetScoresDisplay(const int& a_points, const int& a_tricks, const int& a_matchMode,
 const int& a_relativeCenter);*/


/*
 Display::GetCloseBoxDisplay Display::GetCloseBoxDisplay
NAME

        GetCloseBoxDisplay - to get the interface for closing the player box on the screen
 
SYNOPSIS

        string Display::GetCloseBoxDisplay(const int& a_relativeCenter, const int& a_matchMode);

            a_matchMode      --> the number of players in the match
            a_relativeCenter --> the relative center of the player's box
            
DESCRIPTION

        This function creates the interface for displaying the closing line of the player's box.

RETURNS

        The player's closing line interface in string format
 
*/
/**/

string Display::GetCloseBoxDisplay(const int& a_relativeCenter, const int& a_matchMode)
{
    //the maximum spaces it takes to cover the player's hand
    int maxHandWidth;
    
    if (a_matchMode == threePlayer)
        maxHandWidth = m_seventeenHandWidth;
    else
        maxHandWidth = m_thirteenHandWidth;
    
    //add the necessary spacing and lines
    string scoreBox = AddText(m_line, maxHandWidth);
    int spaceFromStart = GetRelativePosition(scoreBox, a_relativeCenter);
    string display = AddText(m_space, spaceFromStart + maxHandWidth) + scoreBox;
    
    //the completed display
    return display;
}


/*
 Display::GetRelativePosition Display::GetRelativePosition
NAME

        GetRelativePosition - to find the relative position of a text to a location
 
SYNOPSIS

        int Display::GetRelativePosition(const string& a_text, int a_relativeCenter);

            a_text           --> the text
            a_relativeCenter --> the relative center to where the text is being printed
            
DESCRIPTION

        This function creates the interface for finding the relative position of a text
        to a relative center by finding the number of spaces in between them.

RETURNS

        The relative position of the text in relation to the relative center
 
*/
/**/

int Display::GetRelativePosition(const string& a_text, int a_relativeCenter)
{
    int textSize = (int)a_text.size();
    
    //find the necessary spaces
    int spaceFromStart = a_relativeCenter - (textSize / 2);
    
    return spaceFromStart;
}
/*int Display::GetRelativePosition(const string& a_text, int a_relativeCenter);*/


/*
 Display::GetTrumpSuitPrompt Display::GetTrumpSuitPrompt
NAME

        GetTrumpSuitPrompt- to get the prompt for trump suit selection
 
SYNOPSIS

        string Display::GetTrumpSuitPrompt(const vector<Card>& a_hand);
            a_hand --> the player's hand to be displayed

DESCRIPTION

        This function creates the prompt for the client to choose
        the trump suit.

RETURNS

        The prompt in string format
 
*/
/**/

string Display::GetTrumpSuitPrompt(const vector<Card>& a_hand)
{
    //the display to be sent to client
    string msg;
    
    //the position on screen to display the player's hand
    int position = 0;
    
    //add the prompt
    msg = "\nSelect The Trump Suit\n";
    
    //the cards in client's hand
    msg += Display::GetHandDisplay(a_hand, position, (int)a_hand.size(), false);
    
    //add the suit options
    msg += "\nSUITS: \n";
    msg += "S --> SPADES\n";
    msg += "C --> CLUBS\n";
    msg += "H --> HEARTS\n";
    msg += "D --> DIAMONDS\n\n";
    
    //the entire prompt
    return msg;
}
/*string Display::GetTrumpSuitPrompt(const vector<Card>& a_hand);*/


/*
 Display::GetDiscardTwoCardsPrompt Display::GetDiscardTwoCardsPrompt
NAME

        GetDiscardTwoCardsPrompt - to get the prompt to discard two cards from hand
 
SYNOPSIS

        string Display::GetDiscardTwoCardsPrompt(const Suit& a_trumpSuit);
            a_trumpSuit --> the selected trump suit for the round

DESCRIPTION

        This function creates the prompt for the client to discard two cards
        from their hand as a requirement in 2-player mode.

RETURNS

        The prompt in string format
 
*/
/**/

string Display::GetDiscardTwoCardsPrompt(const Suit& a_trumpSuit)
{
    //the display to be sent to client
    string msg;
    
    //inform client about the selected trump suit
    msg += "Hakem Declared Trump Suit As " + suitSymbol[a_trumpSuit] + "\n";
    
    //add necessary vertical spaces
    msg += Display::GetNewLines(2) + "2 Cards Need To Be Discarded From Hand\n";
    
    //add the prompt
    msg += "Enter Two Cards Separated With Comma And No Spaces\n";
    
    return msg;
}
/*string Display::GetDiscardTwoCardsPrompt(const Suit& a_trumpSuit);*/


/*
 Display::GetDiscardTwoCardsPrompt Display::GetDiscardTwoCardsPrompt
NAME

        GetDiscardTwoCardsPrompt - to get the prompt to select a card
 
SYNOPSIS

        string Display::GetCardSelectionPrompt();

DESCRIPTION

        This function creates the prompt for the client to choose to play
        a card during the match.

RETURNS

        The prompt in string format
 
*/
/**/

string Display::GetCardSelectionPrompt()
{
    //the display to be sent to client
    string display;
    
    //add the prompt
    display += "Enter A Rank Followed By A Suit" + Display::GetNewLines(2);
        
    //add the suit options
    display += "SUITS: \n";
    display += "S --> SPADES\n";
    display += "C --> CLUBS\n";
    display += "H --> HEARTS\n";
    display += "D --> DIAMONDS\n";
        
    display += "\n";
        
    //add the rank options
    display += "RANKS: \n";
    display += "T --> TEN\n";
    display += "J --> JACK\n";
    display += "Q --> QUEEN\n";
    display += "K --> KING\n";
    display += "A --> ACE\n";
        
    display += "\n";
        
    display += "Example: QS --> QUEEN OF SPADES\n";
    
    //the completed display
    return display;
}
/*string Display::GetCardSelectionPrompt();*/


/*
 Display::GetPickOrDiscardPrompt Display::GetPickOrDiscardPrompt
NAME

        GetPickOrDiscardPrompt - to get the prompt to select a card
 
SYNOPSIS

        string Display::GetPickOrDiscardPrompt(const Card& a_card);
            a_card     --> the card being offered to the player
        

DESCRIPTION

        This function creates the prompt for the client to pick or discard
        a card offered to them.

RETURNS

        The prompt in string format
 
*/
/**/

string Display::GetPickOrDiscardPrompt(const Card& a_card)
{
    //the display to be sent to client
    string display;
    
    //add necessary vertical space
    display += Display::GetNewLines(3);
    
    //add the prompt
    display += "Would You Like To Take This Card?\n\n";
    
    //add the card display
    display += Display::GetCardDisplay(a_card, 0, true);
    display += '\n';
    
    //add the options
    string optionOne = "1) Yes\n";
    string optionTwo = "2) No\n";
    string options = optionOne + optionTwo;
    ColorText(options, m_tBlue);
    display += options;
    
    //the completed prompt
    return display;
}
/*string Display::GetPickOrDiscardPrompt(const Card& a_card);*/


/*
 Display::GetBuildHandMsg Display::GetBuildHandMsg
NAME

        GetBuildHandMsg - to get the message informing player about building hands
 
SYNOPSIS

        string Display::GetBuildHandMsg();

DESCRIPTION

        This function creates the message informing clients about how they
        will be builing their hands in the next part by picking or discarding
        cards repetitively.

RETURNS

        The message in string format
 
*/
/**/

string Display::GetBuildHandMsg()
{
    string display = "";
    display += "In This Next Part You Will Build Your Own Hand\n";
    display += "You Are Offered A Card\n";
    display += "If You Pick The Card, It Will Be Added To Your Hand\n";
    display += "Otherwise The Next Card On The Deck Is Added To Your Hand\n";
    
    return display;
}
/*string Display::GetBuildHandMsg();*/


/*
 Display::GetHomePageMenu Display::GetHomePageMenu
NAME

        GetBuildHandMsg - to get the home page of the server
 
SYNOPSIS

        string Display::GetHomePageMenu();

DESCRIPTION

        This function creates the home page of the server by adding
        the prompt and the options to choose from.

RETURNS

        The server's home page in string format
 
*/
/**/

string Display::GetHomePageMenu()
{
    //to be sent to client
    string homePage;
    
    //the prompt
    string prompt = "\nSelect An Option\n";
    
    //the options
    string optionOne = "1) Login\n";
    string optionTwo = "2) Create Account\n";
    string options = optionOne + optionTwo;
    
    ColorText(options, m_tBlue);
    
    //create the home page
    homePage = prompt + options;
    
    //the completed home page
    return homePage;
}
/*string Display::GetHomePageMenu()*/


/*
 Display::GetMatchModeMenu Display::GetMatchModeMenu
NAME

        GetMatchModeMenu - to get the match mode menu of the server
 
SYNOPSIS

        string Display::GetMatchModeMenu();

DESCRIPTION

        This function creates the match mode menu of the server by adding
        the prompt and the options to choose from.

RETURNS

        The server's match mode menu in string format
 
*/
/**/

string Display::GetMatchModeMenu()
{
    //the prompt
    string prompt = "\nSelect The Match Mode\n";
    
    //the options
    string optionOne = "1) Two-Player\n";
    string optionTwo = "2) Three-Player\n";
    string optionThree = "3) Four-Player\n";
    string options = optionOne + optionTwo + optionThree;
    ColorText(options, m_tBlue);
    
    //create the menu
    string menu = prompt + options;
    
    //the completed menu
    return menu;
}
/*string Display::GetMatchModeMenu();*/


/*
 Display::GetMatchTypeMenu Display::GetMatchTypeMenu
NAME

        GetMatchTypeMenu - to get the match type menu of the server
 
SYNOPSIS

        string Display::GetMatchTypeMenu();

DESCRIPTION

        This function creates the match type menu of the server by adding
        the prompt and the options to choose from.

RETURNS

        The server's match type menu in string format
 
*/
/**/

string Display::GetMatchTypeMenu()
{
    //the prompt
    string prompt = "\nSelect The Match Type\n";
    
    //the options
    string optionOne = "1) NPCs (Solo)\n";
    string optionTwo = "2) RGPs (Multiplayer)\n";
    string options = optionOne + optionTwo;
    
    ColorText(options, m_tBlue);
    
    //create the menu
    string menu = prompt + options;
    
    //the completed menu
    return menu;
}
/*string Display::GetMatchTypeMenu();*/


/*
 Display::GetIntelligenceMenu Display::GetIntelligenceMenu
NAME

        GetIntelligenceMenu - to get the NPC intelligence level menu of the server
 
SYNOPSIS

        string Display::GetIntelligenceMenu();

DESCRIPTION

        This function creates the NPC intelligence level menu of the server by adding
        the prompt and the options to choose from.

RETURNS

        The server's NPC intelligence level menu in string format
 
*/
/**/

string Display::GetIntelligenceMenu()
{
    //the prompt
    string prompt  = "\nSelect An Intelligence Level for the NPCs\n";
    
    //the options
    string optionOne = "1) Low\n";
    string optionTwo = "2) Average\n";
    string optionThree = "3) High\n";
    string options = optionOne + optionTwo + optionThree;
    ColorText(options, m_tBlue);
    
    //create the menu
    string menu = prompt + options;
    
    //the completed menu
    return menu;
}
/*string Display::GetIntelligenceMenu();*/


/*
 Display::GetMainMenu Display::GetMainMenu
NAME

        GetMainMenu - to get the main menu of the server
 
SYNOPSIS

        string Display::GetMainMenu();

DESCRIPTION

        This function creates the main menu of the server by adding
        the prompt and the options to choose from.

RETURNS

        The server's main menu in string format
 
*/
/**/

string Display::GetMainMenu()
{
    //to be sent to client
    string mainMenu;
    
    //the prompt
    string prompt = "\nSelect An Option\n";
    
    //the options
    string optionOne = "1) Create A Match\n";
    string optionTwo = "2) Join A Match\n";
    string optionThree = "3) Leaderboards\n";
    string optionFour = "4) Log Out\n";
    string options = optionOne + optionTwo + optionThree + optionFour;
    
    ColorText(options, m_tBlue);
    
    //create the menu
    mainMenu = prompt + options;
    
    //the completed menu
    return mainMenu;
}
/*string Display::GetMainMenu();*/


//to inform clients about who is selected as Hakem
string Display::GetHakemDeclarationMsg(const string& a_username)
{
    string msg = "\nThe Hakem For The First Round is " + a_username + "\n";
    return msg;
}

//to inform clients about who is selected as Hakem's partner
string Display::GetPartnerDeclarationMsg(const string& a_username)
{
    string msg = "The Hakem's Partner is " + a_username + "\n";
    return msg;
}


/*
 Display::GetHakemDeterminationMsg Display::GetHakemDeterminationMsg
NAME

        GetHakemDeterminationMsg - to get the message informing players about how Hakem is selected
 
SYNOPSIS

        string Display::GetHakemDeterminationMsg();

DESCRIPTION

        This function creates the message that informs players about
        how the Hakem for the first round of the match will be
        selected.

RETURNS

        The message in string format
 
*/
/**/

string Display::GetHakemDeterminationMsg()
{
    //create the message
    string lineOne = "\nThe Hakem For The First Round Must Be Identified\n";
    string lineTwo = "Cards Will Be Dealt To Players Until A Player Recieves An Ace\n";
    string lineThree = "The First Player To Recieve An Ace Becomes Hakem!\n";
    string msg = lineOne + lineTwo + lineThree;
    
    return msg;
}
/*string Display::GetHakemDeterminationMsg();*/


/*
 Display::GetPartnerDeterminationMsg Display::GetPartnerDeterminationMsg
NAME

        GetPartnerDeterminationMsg - to get the message informing players about how Hakem's partner is selected
 
SYNOPSIS

        string Display::GetPartnerDeterminationMsg();

DESCRIPTION

        This function creates the message that informs players about
        how the Hakem's partner is selected.

RETURNS

        The message in string format
 
*/
/**/

string Display::GetPartnerDeterminationMsg()
{
    //create the message
    string msg = "The First Player To Recieve An Ace Becomes Hakem's Partner!";
    msg += "\n";
    msg += "Identifying Hakem'S Partner...";
    
    //the completed message
    return msg;
}
/*string Display::GetPartnerDeterminationMsg();*/


/*
 Center Center
NAME

        Center - to center a text inside a table row
 
SYNOPSIS

        string Center(const string& a_text, int a_width);
            a_text  --> the text to be displyed in the row
            a_width --> the row's width

DESCRIPTION

        This function centers a text inside a table row.

RETURNS

        The properly formatted table row in string format
 
*/
/**/

string Center(const string& a_text, int a_width)
{
    //used for formatting
    stringstream ss, spaces;
    
    //count excess room to pad
    int padding = a_width - (int)a_text.size();
    
    for(int i=0; i < padding / 2; i++)
        spaces << " ";
    
    //format with padding
    ss << spaces.str() << a_text << spaces.str();
    
    //if odd #, add a space
    if(padding > 0 && padding % 2 != 0)
        ss << " ";
    
    return ss.str();
}
/*string Center(const string& a_text, int a_width);*/


/*
 Display::GetWinnerDeclarationMsg Display::GetWinnerDeclarationMsg
NAME

        GetWinnerDeclarationMsg - to get the message informing clients about the winner of an event
 
SYNOPSIS

        string Display::GetWinnerDeclarationMsg(vector<Player*> a_winner, const Event& a_event, const string& a_reciever);
            a_winner   --> the list of players that won
            a_event    --> the event the players won (trick, round or match)
            a_reciever --> the client that will reiceve this message

DESCRIPTION

        This function creates the message that informs players about which player
        won this trick, round or match.

RETURNS

        The message in string format
 
*/
/**/

string Display::GetWinnerDeclarationMsg(vector<Player*> a_winner, const Event& a_event, const string& a_reciever)
{
    //to be sent to client
    string msg;
    
    //if there is only one winner (2-player or 3-player mode)
    if (a_winner.size() == 1)
    {
        if (a_winner[0]->GetUsername() == a_reciever)
            msg += "You";
        else
            msg += a_winner[0]->GetUsername();
    }
    
    //if winner is a team (4-player mode)
    else
    {
        bool found = false;
        for (int i=0; i<a_winner.size(); i++)
        {
            if (a_winner[i]->GetUsername() == a_reciever)
                found = true;
        }
        
        //if winners are recieving this message
        if (found)
            msg += "You And Your Partner";
        else
            msg += a_winner[0]->GetUsername() + " And " + a_winner[1]->GetUsername();
    }
        
    msg += " Won The " + eventTitle[a_event] + '!';
    
    //display in the center of the screen
    msg = Center(msg, m_horizontalScreenSize);
    msg = '\n' + msg + '\n';
    
    //the completed message
    return msg;
}
/*string Display::GetWinnerDeclarationMsg(vector<Player*> a_winner,
 const Event& a_event, const string& a_reciever);*/


/*
 Display::GetMatchInfo Display::GetMatchInfo
NAME

        GetMatchInfo - to get the match information for displaying purposes
 
SYNOPSIS

        string Display::GetMatchInfo(const string& a_username, const int& a_currentPls,
        const int& a_mode, const int& a_colWidth);
 
            a_username   --> the match creator's username
            a_currentPls --> the current number of players in the match
            a_mode       --> the maximum number of players in the match
            a_colWidth   --> the width of the column to display the match information

DESCRIPTION

        This function creates the match information that will be displayed in the
        open matches table of the server.

RETURNS

        The match information in string format
 
*/
/**/

string Display::GetMatchInfo(const string& a_username, const int& a_currentPls, const int& a_mode, const int& a_colWidth)
{
    //to be used in the open matches table
    string info;
    
    //add the match creator's name
    string nameCol = Center(a_username, a_colWidth);
    ColorText(nameCol, m_tBlue);
    info += m_pipe + nameCol + m_space + m_pipe + m_space;
    
    //add the current number of players
    string currentPlCol = Center(to_string(a_currentPls), a_colWidth);
    ColorText(currentPlCol, m_tBlue);
    info += currentPlCol + m_space + m_pipe + m_space;
    
    //add the maximum number of players
    string modeCol = Center(to_string(a_mode)+"-Player", a_colWidth);
    ColorText(modeCol, m_tBlue);
    info += modeCol + m_pipe + "\n";
    
    //the completed message
    return info;
}
/*string Display::GetMatchInfo(const string& a_username, const int& a_currentPls,
 const int& a_mode, const int& a_colWidth);*/


/*
 Display::GetLeaderboards Display::GetLeaderboards
NAME

        GetLeaderboards - to get the table showing the match records of the clients
 
SYNOPSIS

        string Display::GetLeaderboards(stringstream a_data, const char a_userPassDelim,
        const char a_credentialDelim, const char a_scoreDelim);
 
            a_data            --> the client accounts information read from the server's database
            a_userPassDelim   --> the delimeter to seperate the username and password
            a_credentialDelim --> the delimeter to seperate username=password from client match record
            a_scoreDelim      --> the delimeter to seperate the client's number of wins and losses

DESCRIPTION

        This function creates a table where for each client their number of wins,
        number of losses, and win rate will be displayed as long as they have completed a
        multiplayer match.

RETURNS

        The leaderboards table in string format
 
*/
/**/

string Display::GetLeaderboards(stringstream a_data, const char a_userPassDelim, const char a_credentialDelim, const char a_scoreDelim)
{
    //to be displayed to client
    string table;
    
    //the clients with their associated record
    vector<ClientData> rankedClients;
    
    //one line of databse file
    string line;
    
    while (getline (a_data, line))
    {
        if (line.empty())
            continue;
        
        //parse number of wins
        
        //find the position of first delimiter
        int firstDelPos = (int)line.find(a_credentialDelim);
        
        //find the position of second delimiter
        int secondDelPos = (int)line.find(a_scoreDelim);
        
        //get the number of wins
        string numWin = line.substr(firstDelPos + 1, secondDelPos - firstDelPos - 1);
        int wins = stoi(numWin);
        
        //parse number of losses
        int delPos = (int)line.find(a_scoreDelim);
        string numLoss = line.substr(delPos + 1, line.size() - delPos);
        int losses = stoi(numLoss);
        
        //if player has completed at least one multi-player match
        if (wins + losses != 0)
        {
            //parse the username
            string username = line.substr(0, line.find(a_userPassDelim, 0));
            
            //record this client
            ClientData newClient(username, wins, losses);
            rankedClients.push_back(newClient);
        }
    }
    
    //if there are no clients in the leaderboards table
    if (rankedClients.empty())
    {
        table = "\nNo Records Found!\n";
        ColorText(table, m_tRed);
        return table;
    }
    
    //otherwise create a table with the clients
    
    //sort based on win rate
    sort(rankedClients.begin(), rankedClients.end());
    
    //the width of each column
    int colWidth = 15;
    
    //the total number of columns
    int numColumns = 5;
    
    //the title
    string title = "LEADERBOARDS";
    title = Center(title, colWidth * numColumns + 2 * 5 + 4);
    title = '\n' + title + '\n';
    
    //add the title row
    table += string(colWidth * numColumns + 2 * 5 + 4, '-') + "\n";
    table += m_pipe + Center("RANK", colWidth) + m_space + m_pipe + m_space;
    table += Center("USERNAME", colWidth) + m_space + m_pipe + m_space;
    table += Center("WINS", colWidth) + m_space + m_pipe + m_space;
    table += Center("LOSSES", colWidth) + m_space + m_pipe + m_space;
    table += Center("WIN RATE", colWidth) + m_pipe + "\n";
    table += string(colWidth * numColumns + 2 * 5 + 4, '-') + "\n";
    
    int rank = 0;
    for (int i=0; i<rankedClients.size(); i++)
    {
        //if it is the first client or a different percentage from previous client
        if (i == 0 || rankedClients[i].winRate != rankedClients[i - 1].winRate)
            rank++;
        
        //add the client's rank
        string rankCol = Center(to_string(rank), colWidth);
        ColorText(rankCol, m_tBlue);
        table += m_pipe + rankCol + m_space + m_pipe + m_space;
        
        //add the client's username
        string userCol = Center(rankedClients[i].username, colWidth);
        ColorText(userCol, m_tBlue);
        table += userCol + m_space + m_pipe + m_space;
        
        //add the client's number of wins
        string winCol = Center(to_string(rankedClients[i].wins), colWidth);
        ColorText(winCol, m_tBlue);
        table += winCol + m_space + m_pipe + m_space;
        
        //add the client's number of losses
        string lossCol = Center(to_string(rankedClients[i].losses), colWidth);
        ColorText(lossCol, m_tBlue);
        table += lossCol + m_space + m_pipe + m_space;
        
        //add the client's win rate
        string winRateCol = Center(to_string(rankedClients[i].winRate) + '%', colWidth);
        ColorText(winRateCol, m_tBlue);
        table += winRateCol + m_pipe + "\n";
    }
    
    //close the table
    table += string(colWidth * numColumns + 2 * 5 + 4, '-') + "\n";
    
    //ColorText(table, m_tYellow);
    
    //the leaderboards table
    return title + table;
}
/*string Display::GetLeaderboards(stringstream a_data, const char a_userPassDelim,
 const char a_credentialDelim, const char a_scoreDelim);*/


/*
 Display::GetOpenMatchesTable Display::GetOpenMatchesTable
NAME

        GetOpenMatchesTable - to get the table showing the matches that need players to join them
 
SYNOPSIS

        string Display::GetOpenMatchesTable(const vector<Match>& a_matches);
            a_matches --> the open matches on the server

DESCRIPTION

        This function creates a table that shows the matches that have not reached
        their maximum capacity. The matches need players to join them in order
        for them to be started.

RETURNS

        The open matches table in string format
 
*/
/**/

string Display::GetOpenMatchesTable(const vector<Match>& a_matches)
{
    //table to be displayed to clients
    string table;
    
    //the width of a table column
    int colWidth = 15;
    
    //the number of columns in the table
    int numColumns = 3;
    
    //the title
    string title = "Open Matches";
    title = Center(title, colWidth * numColumns + 2 * 3 + 2);
    title = '\n' + title + '\n';
    
    //add the title row
    table += string(colWidth * numColumns + 2 * 3 + 2, '-') + "\n";
    table += m_pipe + Center("MATCH CREATOR", colWidth) + m_space + m_pipe + m_space;
    table += Center("CURRENT PLAYERS", colWidth) + m_space + m_pipe + m_space;
    table += Center("MODE", colWidth) + m_pipe + "\n";
    table += string(colWidth * numColumns + 2 * 3 + 2, '-') + "\n";
    
    //add the open match information
    for (auto match: a_matches)
    {
        //add the match to the table
        table += GetMatchInfo(match.GetMatchCreater()->GetUsername(), match.GetCurrentNumPls(), match.GetMode(), colWidth);
    }
        
    //close the table
    table += string(colWidth * numColumns + 2 * 3 + 2, '-') + "\n";
    
    //the completed open matches table
    return title + table;
}
/*string Display::GetOpenMatchesTable(const vector<Match>& a_matches);*/


/*
 Display::GetMatchPlayersList Display::GetMatchPlayersList
NAME

        GetMatchPlayersList - to get the list of players that are currently in a match
 
SYNOPSIS

        string Display::GetMatchPlayersList(vector<Player*> a_players);
            a_players --> the players in the match

DESCRIPTION

        This function creates a list of players that are inside a match
        and waiting for more players to join in order for them match
        to be started.

RETURNS

        The list of match players in string format
 
*/
/**/

string Display::GetMatchPlayersList(vector<Player*> a_players)
{
    //the list to be displayed to the clients
    string list;
    
    //add the title
    string title = "\nList Of Current Players In The Match:\n";
    
    //add all the player usernames in the list
    for (auto player: a_players)
    {
        list += "• ";
        list += player->GetUsername();
        list += "\n";
    }
    
    ColorText(list, m_tBlue);
    
    //the completed list
    return title + list;
}
/*string Display::GetMatchPlayersList(vector<Player*> a_players);*/


/*
 Display::GetUsernameRequirements Display::GetUsernameRequirements
NAME

        GetUsernameRequirements - to get the message informing clients about the server's username requirements
 
SYNOPSIS

        string Display::GetUsernameRequirements();

DESCRIPTION

        This function creates the message that informs clients about the
        requirements of a username when creating an account.

RETURNS

        The username requirements in string format
 
*/
/**/

string Display::GetUsernameRequirements()
{
    //to be sent to clients
    string display;
    
    //add the title
    string title = "\nUsername Must Meet The Following Requirements:\n";
    
    //add the requirements
    display += "• It Must Be 6-10 Characters Long\n";
    display += "• Only Letters (a-z) And Numbers (0-9) Are Allowed\n";
    display += "• It Cannot Start With \"NPC\" (Reserved) \n";
    ColorText(display, m_tBlue);
    
    //the completed message
    return title + display;
}
/*string Display::GetUsernameRequirements();*/


/*
 Display::GetPasswordRequirements Display::GetPasswordRequirements
NAME

        GetPasswordRequirements - to get the message informing clients about the server's password requirements
 
SYNOPSIS

        string Display::GetPasswordRequirements();

DESCRIPTION

        This function creates the message that informs clients about the
        requirements of a password when creating an account.

RETURNS

        The password requirements in string format
 
*/
/**/

string Display::GetPasswordRequirements()
{
    //to be sent to clients
    string display;
    
    //add the title
    string title = "\nPassword Must Meet The Following Requirements:\n";
    
    //add the requirements
    display += "• It Must Be 8-20 Characters Long\n";
    display += "• It Cannot Contain The Characters Space, Comma, Dash, And The Equal Sign\n";
    display += "• It Must Contain At Least One Letter And One Number\n";
    ColorText(display, m_tBlue);
    
    //the completed message
    return title + display;
}
/*string Display::GetPasswordRequirements();*/


/*
 Display::ColorDisplay Display::ColorDisplay
NAME

        ColorDisplay - to color the foreground and background color of a text
 
SYNOPSIS

        void Display::ColorDisplay(string& a_text, const Color& a_backCol, const Color& a_textCol);
            a_text    --> the text to get colored
            a_backCol --> the background color of the text
            a_textCol --> the text color

DESCRIPTION

        This function adds color to the foreground and background color of a
        text. It returns the colored text by reference.

RETURNS

        None
 
*/
/**/

void Display::ColorDisplay(string& a_text, const Color& a_backCol, const Color& a_textCol)
{
    //the colored text
    string coloredDisplay;
    
    //color the foreground and background
    coloredDisplay += ESC;
    coloredDisplay += a_backCol;
    coloredDisplay += ";";
    coloredDisplay += a_textCol;
    coloredDisplay += "m";
    coloredDisplay += a_text;
    coloredDisplay += RESET;
    a_text = coloredDisplay;
}


/*
Display::ColorText Display::ColorText
NAME

        ColorText - to color the foreground of a text
 
SYNOPSIS

        void Display::ColorText(string& a_text, const Color& a_textCol);
            a_text    --> the text to get colored
            a_textCol --> the background color of the text

DESCRIPTION

        This function adds color to the foreground color of a
        text. It returns the colored text by reference.

RETURNS

        None
 
*/
/**/

void Display::ColorText(string& a_text, const Color& a_textCol)
{
    string coloredText;
    coloredText += ESC;
    coloredText += "38";
    coloredText += ";";
    coloredText += a_textCol;
    coloredText += "m";
    coloredText += a_text;
    coloredText += RESET;
    a_text = coloredText;
}


/*
 Display::GetWelcomeMsg Display::GetWelcomeMsg
NAME

        GetWelcomeMsg - to display a logo when user connects to the server
 
SYNOPSIS

        void Display::ColorDisplay();

DESCRIPTION

        This function creates the logo that is displayed to the user
        right after connecting to the server.

RETURNS

        None
 
*/
/**/

string Display::GetWelcomeMsg()
{
    string display;
    
    display += GetNewLines(50);
    
    display += "||     ||  //‾‾\\\\  || //    //\\\\      //\\\\"    ; display += '\n';
    display += "||_____|| //    \\\\ ||//    //  \\\\    //  \\\\"   ; display += '\n';
    display += "||     || \\\\    // ||\\\\   //    \\\\  //    \\\\"  ; display += '\n';
    display += "||     ||  \\\\__//  || \\\\ //      \\\\//      \\\\" ; display += '\n';
    
    display += GetNewLines(2);
    
    ColorDisplay(display, m_blink, m_tBlue);
    
    return display;
}


//*** the following functions are messages displayed to users at various stages  ***\\

string Display::GetSuccessLogInMsg()
{
    string msg = "\nLog In Successful! Have Fun Playing!\n";
    ColorText(msg, m_tGreen);
    return msg;
}

string Display::GetFailLogInMsg()
{
    string msg = "\nLog In Failed! Please Try Again!\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetNoSelectionMsg()
{
    string msg = "\nYou Did Not Make A Selection On Time... You Were Removed From The Match!\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetInactivityWarningMsg()
{
    string msg = "\nYou Have Less Than 10 Seconds To Make A Selection Before Being Removed From The Match\n";
    ColorDisplay(msg, m_blink, m_tYellow);
    return msg;
}

string Display::GetInvalidSelectionMsg()
{
    string msg = "\nInvalid Selection. Please Try Again!\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetWaitForOtherPlsMsg()
{
    string msg = "\nYou Must Wait For Other Players!\n";
    ColorText(msg, m_tYellow);
    return msg;
}

string Display::GetUserLoggedInMsg()
{
    string msg = "\nUser is Already Logged In. You Must Log Out Before Logging In Again!\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetSuccessSignupMsg()
{
    string msg = "\nAccount Sign Up Successful!\n";
    ColorText(msg, m_tGreen);
    return msg;
}

string Display::GetInvalidCredMsg()
{
    string msg = "\nImproperly Formatted username=password. Please Try Again!\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetTakenUserMsg()
{
    string msg = "\nThe Username Already Exists. Please Choose A Different Username!\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetSuccessMatchCreationMsg()
{
    string msg = "\nYou Have Successfully Created A Match!\n";
    msg += "Your Match Will Start As Soon As Enough Players Join!\n";
    ColorText(msg, m_tGreen);
    return msg;
}

string Display::GetMatchCreatorLeftMsg()
{
    string msg = "\nThe Match Creator Left. The Match Will Not Be Played!\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetMaxMatchCapReachedMsg()
{
    string msg = "\nMax Capacity Reached. Waiting For Match Creater To Start The Match!\n";
    ColorText(msg, m_tGreen);
    return msg;
}

string Display::GetStartMatchMsg()
{
    string msg = "\nMax Capacity Reached. Type START and Press Enter To Begin The Match!\n";
    ColorText(msg, m_tGreen);
    return msg;
}

string Display::GetWaitForMatchStartMsg()
{
    string msg = "\nYou Must Wait At This Time!\n";
    ColorText(msg, m_tYellow);
    return msg;
}

string Display::GetSuccessJoinMatchMsg()
{
    string msg = "\nYou Successfully Joined The Match!\n";
    ColorText(msg, m_tGreen);
    return msg;
}

string Display::GetNoOpenMatchMsg()
{
    string msg = "\nThere Are No Open Matches To Join At This Time.\n";
    msg += "Please Create A New Match Or Try Joining A Match Later!\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetMatchNotFoundMsg()
{
    string msg = "\nMatch Not Found!\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetMatchNoLongerOpenMsg()
{
    string msg = "\nThe Match Has Reached Its Max Capacity. It Is No Longer Open To Join!\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetSuccessDiscardCardsMsg()
{
    string msg = "\nYou Discarded Two Cards From Your Hand!\nWaiting For The Other Player...\n\n";
    ColorText(msg, m_tGreen);
    return msg;
}

string Display::GetCardAddMsg()
{
    string msg = "\nA Card Was Added To Your Hand!\nWaiting For The Other Player...\n";
    ColorText(msg, m_tGreen);
    return msg;
}

string Display::GetInactivityMsg()
{
    string msg = "\nYour Session Has Timed Out Due To Inactivity!\n\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetNoMatchStartMsg()
{
    string msg = "\nYou Did Not Start The Match On Time... The Match Was Cancelled!\n";
    ColorText(msg, m_tRed);
    return msg;
}


string Display::GetMaxMatchReachedMsg()
{
    string msg = "\nYou Cannot Create A Match At This Time As The Server Has Reached Its Maximum Number Of Matches...\n";
    msg += "Please Try Joining a Match Instead Or Try Again Later!\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetMaxAccountsReachedMsg()
{
    string msg = "\nThe Server Has Reached Its Max Number Of Clients. You Cannot Create An Account At This Time...\n";
    msg += "Please Try Again Later!\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetMaxOnlinePlReachedMsg()
{
    string msg = "\nYou Cannot Connect To The Server At This Time...Please Try Again Later!\n\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetLogoutMsg()
{
    string msg = "\nLogout Successful! See You Soon!\n\n";
    ColorText(msg, m_tGreen);
    return msg;
}

string Display::GetLengthyErrorMsg()
{
    string msg = "\nThe Input Is Too Large. Please Try Again!\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetPlJoinedMsg(const string& a_username)
{
    string msg = "\n" + a_username + " Joined The Match!\n";
    ColorText(msg, m_tGreen);
    return msg;
}

string Display::GetPlLeftMsg(const string& a_username)
{
    string msg = "\n" + a_username + " Left The Match\n";
    ColorText(msg, m_tRed);
    return msg;
}

string Display::GetPlReplaceMsg(const string& a_username)
{
    string msg = a_username + " Replaced This Player!\n";
    ColorText(msg, m_tGreen);
    return msg;
}
