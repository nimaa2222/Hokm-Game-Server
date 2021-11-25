//stdafx.h
//include file for standard system include files
//project specific include files that are used frequently, but are changed infrequently

#pragma once

//NPC intelligence level in terms of decision making
enum Intelligence
{
    low = 1,
    average,
    high
};

//the possible events (capitalized for clarification)
enum Event
{
    TRICK,
    ROUND,
    MATCH
};

//the match mode
enum Mode
{
    twoPlayer = 2,
    threePlayer,
    fourPlayer
};

//reference to players inside a match
enum PlayerNum
{
    playerOne,
    playerTwo,
    playerThree,
    playerFour
};

//the client's current status in the server
enum ClientStage
{
    
    //***   not in match  ***//
    
    //user selection is from home page
    mustSelectFromHomePage,
    
    //user input is credentials to log in
    mustLogin,
    
    //user input is credentials to create an account
    mustCreateAccount,
    
    //user selection is from the main menu
    mustSelectFromMainMenu,
    
    //user selection is from match type menu
    mustSelectMatchType,
    
    //user selection is from the NPC intelligence menu
    mustSelectNPCIntelligence,
    
    //user selection is from the match mode menu
    mustSelectMatchMode,
    
    //user selection is from the open matches table
    mustSelectMatchToJoin,
    
    //user input is an attempt to start the match
    mustAllowMatchToStart,
    
    //user cannot do anything at the moment (input ignored)
    mustWaitToStartMatch,
    
    
    //***   inside match  ***//
    
    //user is inside the match
    insideMatch,
    
    //user cannot do anything at the moment (input ignored)
    mustWaitForOtherPls,
    
    //user input is the trump suit
    mustSelectTrumpSuit,
    
    //user input is two cards
    mustDiscardTwoCards,
    
    //user selection is picking or discarding a card
    mustPickOrDiscard,
    
    //user input is a card
    mustSelectCard
};

//the suits of the standard deck
enum Suit
{
    spades,
    hearts,
    clubs,
    diamonds
};

//the Face Cards including Ten and Ace
enum SymbolicCards
{
    ten = 10,
    jack,
    queen,
    king,
    ace
};

//the turn during the trick
enum Turn
{
    first,
    second,
    third,
    fourth
};

//the player's decision during the match
enum Decision
{
    yes = 1,
    no
};

//#pragma comment(lib, "Ws2_32.lib")

//standard include files
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <map>
#include <unordered_map>
#include <numeric>
#include <algorithm>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
using namespace std;

//network-related definitions
typedef int SOCKET;
typedef string Color;
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

//network include files
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

//project include files
#include "Card.hpp"
#include "Deck.hpp"
#include "Player.hpp"
#include "Human.hpp"
#include "Computer.hpp"
#include "Trick.hpp"
#include "Round.hpp"
#include "Match.hpp"
#include "Display.hpp"
#include "FileAccess.hpp"
#include "Server.hpp"


//mapping each character input to a suit
static map<char, Suit> suitChar =
{
    {'S', spades},
    {'C', clubs},
    {'H', hearts},
    {'D', diamonds}
};

//mapping a symbolic card to a character
static map<SymbolicCards, char> rankChar =
{
    //since the number 10 has 2 characters...
    //it is represented by a symbol rather than a number for displaying purposes
    {ten, 'T'} ,
    
    {jack, 'J'} ,
    {queen, 'Q'},
    {king, 'K'},
    {ace, 'A'}
};

//mapping each suit to a the appropriate symbol (displaying purposes)
static map<Suit, string> suitSymbol =
{
    {spades, "♠"},
    {clubs, "♣"},
    {hearts, "♥"},
    {diamonds, "♦"}
};


/**/
/*
 AddText AddText

NAME

        AddText - to put together a series of characters or strings

SYNOPSIS

        string AddText(const T a_text, const int a_num);
            a_text   --> the text to be put together
            a_num    --> the number of times the text should be appended
            
DESCRIPTION

        This function appends a string or character to itself
        for a specified number of times.

RETURNS

        The appended completed string
 
*/
/**/

template <class T>
string AddText(const T a_text, const int a_num)
{
    //holds the a_text a specific number of times
    string fullText;
    
    for (int i=0; i<a_num; i++)
        fullText += a_text;
    
    return fullText;
}
/*string AddText(const T a_text, const int a_num);*/






