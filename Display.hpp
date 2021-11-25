//Display Class --> responsible for all components of the server's GUI

#pragma once

//color displays
#define ESC "\033["
#define RESET "\033[m"

class Display
{

public:
    
    //*** constructors ***//
    Display(){/*no initializations*/}
    
    //*** destructor ***//
    ~Display(){/*not required*/}
    
    //log in messages
    static string m_logInPrompt;
    static string m_signupPrompt;
    static string m_joinMatchPrompt;
    
    //inside match messages
    static string m_waitForTrumpSelection;
    static string m_waitForHakemPickDiscard;
    
    //colors and formatting
    static Color m_bWhite;
    static Color m_bGreen;
    static Color m_tWhite;
    static Color m_tBlue;
    static Color m_tBlack;
    static Color m_tRed;
    static Color m_tGreen;
    static Color m_tYellow;
    static Color m_blink;
    
    
    //***    displaying the full match on screen     ***//
    
    //to display all player information and their cards on the screen
    static string GetMatchDisplay(vector<Player*>, Player*, const vector<Card>&, const vector<int>&,
                                  const vector<int>&, const int&, const Suit&, const string&);
    
    //to display the stage where Hakem is being determined by distributing cards
    static string GetHakemDeterminationDisplay(vector<Player*>);
    
    //to print a specified number of lines
    static string GetNewLines(int);
    
    //to display a player hand on the screen
    static string GetHandDisplay(const vector<Card>&, const int&, const int&, const bool&);
    
    
    //***    displaying a card     ***//
    
    //to display a card on the screen
    static string GetCardDisplay(const Card&, const int&, const bool&);
    
    //to display the top rank of the card on the screen
    static string GetCardTopRankDisplay(const Card&, const int&, const bool&, const bool&, const bool&);
    
    //to display the card suit on the screen
    static string GetCardSuitDisplay(const Card&, const int&, const bool&, const bool&, const bool&);
    
    //to display the bottom rank of the card on the screen
    static string GetCardBottomRankDisplay(const Card&, const int&, const bool&, const bool&, const bool&);
    
    
    //***    the game menus     ***//
    
    //to display the home page to clients
    static string GetHomePageMenu();
    
    //to display the main menu to clients
    static string GetMainMenu();
    
    //to display the match type menu to clients
    static string GetMatchTypeMenu();
    
    //to display the match mode menu to clients
    static string GetMatchModeMenu();
    
    //to display the NPC intelligence level menu to clients
    static string GetIntelligenceMenu();
    
    
    //***    Hakem determination messages     ***//
    
    //to inform clients about Hakem determination
    static string GetHakemDeterminationMsg();
    
    //to declare Hakem to the clients
    static string GetHakemDeclarationMsg(const string&);
    
    //to inform clients about Hakem Partner determination
    static string GetPartnerDeterminationMsg();
    
    //to declare Hakem's partner to the clients
    static string GetPartnerDeclarationMsg(const string&);
    
    
    //***    informational messages     ***//
    
    //to inform clients about building their hands in 2-player mode
    static string GetBuildHandMsg();
    
    //to declare the winner(s) to clients
    static string GetWinnerDeclarationMsg(vector<Player*>, const Event&, const string&);
    
    //to display the list of players in a match to clients
    static string GetMatchPlayersList(vector<Player*>);
    
    //to display the username requirements for signup
    static string GetUsernameRequirements();
    
    //to display the password requirements for sign up
    static string GetPasswordRequirements();
    
    
    //***    the selection prompts     ***//
    
    //to prompt user to select the trump suit
    static string GetTrumpSuitPrompt(const vector<Card>&);
    
    //to prompt user to discard 2 cards from hand
    static string GetDiscardTwoCardsPrompt(const Suit&);
    
    //to prompt user to pick or discard a card
    static string GetPickOrDiscardPrompt(const Card&);
    
    //to prompt user to select a card
    static string GetCardSelectionPrompt();
    
    
    //***    the server's tables    ***//
    
    //to display the match information
    static string GetMatchInfo(const string&, const int&, const int&, const int&);
    
    //to display the matches that need players to join them
    static string GetOpenMatchesTable(const vector<Match>&);
    
    //to display the leaderboards table which shows client's match records
    static string GetLeaderboards(stringstream, const char, const char, const char);
    
    
    //***   string messages   ***//
    
    //messages displayed to users at different stages
    static string GetWelcomeMsg();
    static string GetSuccessLogInMsg();
    static string GetFailLogInMsg();
    static string GetNoSelectionMsg();
    static string GetInactivityWarningMsg();
    static string GetInvalidSelectionMsg();
    static string GetWaitForOtherPlsMsg();
    static string GetUserLoggedInMsg();
    static string GetSuccessSignupMsg();
    static string GetInvalidCredMsg();
    static string GetTakenUserMsg();
    static string GetSuccessMatchCreationMsg();
    static string GetMatchCreatorLeftMsg();
    static string GetMaxMatchCapReachedMsg();
    static string GetStartMatchMsg();
    static string GetWaitForMatchStartMsg();
    static string GetSuccessJoinMatchMsg();
    static string GetNoOpenMatchMsg();
    static string GetMatchNotFoundMsg();
    static string GetMatchNoLongerOpenMsg();
    static string GetSuccessDiscardCardsMsg();
    static string GetCardAddMsg();
    static string GetInactivityMsg();
    static string GetNoMatchStartMsg();
    static string GetMaxMatchReachedMsg();
    static string GetMaxAccountsReachedMsg();
    static string GetMaxOnlinePlReachedMsg();
    static string GetLogoutMsg();
    static string GetLengthyErrorMsg();
    static string GetPlJoinedMsg(const string&);
    static string GetPlLeftMsg(const string&);
    static string GetPlReplaceMsg(const string&);
    
private:
    
    //*** the game displays ***//
    
    //the horizontal size of the game screen
    static const int m_horizontalScreenSize = 180;
    
    //the vertical size of the game screen
    static const int m_verticalScreenSize = m_horizontalScreenSize / 4;
    
    //the center of the game screen
    static const int m_screenCenter = m_horizontalScreenSize / 2;
    
    //the indentation from the top and bottom of game screen
    static const int m_verticalIndentation = 5;
    
    //the indentation from the left side of the screen
    static const int m_leftIndentation = 5;
    
    
    //*** the card displays ***//
    
    //the vertical space a game card takes on the screen
    static const int m_cardVerticalSize = 3;
    
    //the horizontal space a full card takes on the screen
    static const int m_fullCardHorizontalSize = 5;
    
    //the horizontal space a card in a pile takes on the screen
    static const int m_pileCardHorizontalSize = 2;
    
    //the horizontal space a table card takes on the screen
    static const int m_tableCardHorizontalSize = 3;
    
    
    //*** the title displays ***//
    
    //the number of vertical space the round number and trump suit titles take
    static const int m_verticalTitlesSpace = 2;
    
    //the space between the titles and the first player
    static const int m_titlesPlayerDistance = 3;
    
    
    //*** the player displays ***//
    
    //the vertical space a player information takes on the screen
    static const int m_playerVerticalSpace = m_cardVerticalSize + 3;
    
    //the vertical distance between the top and bottom player information on the screen
    static const int m_verticalTopBottomPlDistance =
    m_verticalScreenSize - (2 * m_playerVerticalSpace) - m_verticalTitlesSpace - m_titlesPlayerDistance;
    
    //the vertical distance between the top and left player information on the screen
    static const int m_verticalTopLeftPlDistance = (m_verticalTopBottomPlDistance / 2) - 2;
    
    //the maximum number of cards in a player hand in 3-player mode
    static const int m_maxCardsThreePlayer = 17;
    
    //the maximum number of cards in a player hand in 2-player and 4-player mode
    static const int m_otherMaxcards = 13;
    
    //the hand width of a player with 17 cards (3-player mode)
    static const int m_seventeenHandWidth = ((m_maxCardsThreePlayer - 1) * m_pileCardHorizontalSize) + m_fullCardHorizontalSize;
    
    //the hand width of a player with 13 cards (2-player and 4-player mode)
    static const int m_thirteenHandWidth = ((m_otherMaxcards - 1) * m_pileCardHorizontalSize) + m_fullCardHorizontalSize;
    
    
    //*** single characters used ***//
    static const char m_space = ' ';
    static const char m_underScore = '_';
    
    
    //*** the match in different modes *** //
    
    //to display a single player information on the screen
    static string GetSinglePlayerDisplay(Player*, const int&, const int&, const int&, const bool&, int, const string&);
    
    //displaying a 2-player match
    static string GetTwoPlMatchDisplay(vector<Player*>, const vector<Card>&, const int&, const int&,
                                           const bool&, const string&);
    
    
    //***                        displaying various states of a 3-player match                         *** //
    
    //3-player match display with no table cards
    static string GetThreePlMatchDisplay(vector<Player*>, const vector<Card>&, const vector<int>&,
                                         const vector<int>&, const bool&, const string&);
    
    //3-player match display with a table card
    static string GetThreePlOneCardDisplay(Player*, const Card&, const int&, const int&, const int&,
                                           const int&, const bool&);
    
    //3-player match display with two table cards
    static string GetThreePlTwoCardsDisplay(Player*, const Card&, const Card&, const int&, const int&, const int&,
                                            const int&, const bool&);
    
    //3-player match display with three table cards
    static string GetThreePlThreeCardsDisplay(Player*, const Card&, const Card&, const Card&, const int&,
                                              const int&, const int&, const int&, const bool&);
    
    
    //***                        displaying various states of a 4-player match                         *** //
    
    //4-player match display for hakem determination
    static string GetFourPlMatchDisplay(vector<Player*>, const vector<Card>&, const vector<int>&, const vector<int>&,
                                        const bool&, const string&);
    
    //4-player match display with no table cards
    static string GetFourPlNoCardDisplay(Player*, Player*, const int&, const int&, const bool&, const int&,
                                         const int&, const int&, const int&, const bool&, const bool&);
    
    //4-player match display with a table card
    static string GetFourPlOneCardDisplay(Player*, Player*, const Card&, const int&, const int&, const int&, const int&,
                                          const int&, const int&, const bool&, const bool&);
    
    //4-player match display with two table cards
    static string GetFourPlTwoCardsDisplay(Player*, Player*, const Card&, const Card&, const int&, const int&, const int&,
                                           const int&, const int&, const int&, const bool&, const bool&);
    
    //4-player match display with three table cards
    static string GetFourPlThreeCardsDisplay(Player*, Player*, const Card&, const Card&, const Card&, const int&,
                                             const int&, const int&, const int&, const int&, const int&, const bool&, const bool&);
    
    //4-player match display with four table cards
    static string GetFourPlFourCardsDisplay(Player*, Player*, const vector<Card>&, const int&, const int&, const int&,
                                            const int&, const int&, const int&, const bool&, const bool&);

    
    //***                        displaying a hand                       *** //
    
    //to find the location to start printing the hand
    static int GetHandStartLocation(const int&, const int&);
    
    //to print the top rank of all cards in a hand
    static string GetCardTopRanksDisplay(const vector<Card>&, const int&, const int&, const bool&);
    
    //to print the suit of all card in a hand
    static string GetCardSuitsDisplay(const vector<Card>&, const int&, const int&, const bool&);
    
    //to print the low rank of all cards in a hand
    static string GetCardBottomRanksDisplay(const vector<Card>&, const int&, const int&, const bool&);
    

    //***                  other components of match display             *** //
    
    //to display a player's name
    static string GetPlNameDisplay(const string&, const int&, const int&);
    
    //to display the points and tricks on the screen
    static string GetScoresDisplay(const int&, const int&, const int&, const int&);
    
    //to print the closing border for player information
    static string GetCloseBoxDisplay(const int&, const int&);
    
    //to find the relative distance between two statements being printed
    static int GetRelativePosition(const string&, int);
    
    //to color the background and foreground of a text
    static void ColorDisplay(string&, const Color&, const Color&);
    
    //to color the foreground of a text
    static void ColorText(string&, const Color&);
};
