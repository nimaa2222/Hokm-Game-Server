//Round class --> holds together all components of a round in the match

#pragma once

class Round
{

public:
    
    //*** constructors ***//
    Round();
    Round(const Mode&, Player*);
    Round(const Round&);
    
    //*** destrcutor ***//
    ~Round(){/*not required*/}
    
    //*** getters and setters ***//
    Mode GetMode() const {return m_mode;}
    vector<int> GetTrickScores() const {return m_trickScores;}
    Player* GetHakem() const {return m_hakem;}
    Suit GetTrumpSuit() const {return m_trumpSuit;}
    vector<Player*> GetWinner() const {return m_winner;}
    Trick& GetCurrentTrick() {return m_tricks.back();}
    void ResetTrickScores() {fill(m_trickScores.begin(), m_trickScores.end(), 0);}
    
    
    //*** other functions ***//
    
    //to distribute 5 cards to each player and prompt for trump suit selection
    void SetupRound(Deck&, vector<Player*>&, const vector<int>&, const int&);
    
    //to establish the trump suit for the round and deal remaining cards to all players (or prompt discarding cards in 2-player mode)
    void TrumpSuitSelected(Deck&, vector<Player*>&, const vector<int>&, const int&, const Suit&);
    
    //to discard two cards from hand in 2-player mode and then start the pick/discard process
    void TwoCardsDiscarded(Deck&, vector<Player*>&, const vector<int>&, const int&, const Player*, const Card&, const Card&);
    
    //to allow players to pick/discard a card and then call the next player for the same action
    void AskPlayerToPickDiscard(Deck&, vector<Player*>&, const vector<int>&, const int&, int);
    
    //to pick/discard the card and update the deck based on the decision
    void HandlePickDiscardDecision(Deck&, vector<Player*>&, const vector<int>&, const int&, const Decision&, const int&);
    
    //called once human player inputs their decision to pick/discard
    void CardPickedDiscarded(Deck&, vector<Player*>&, const vector<int>&, const int&, const Player*, const Decision&);
    
    //to identify the trick starter and prompt the player to start the trick by playing a card
    void StartNewTrick(vector<Player*>&, const vector<int>&, const int&);
    
    //to update the trick scores at the end of the trick
    void UpdateTrickScores(const vector<Player*>& a_players);
    
    //to check whether the round is over and also identify the round winner
    bool IdentifyRoundWinner(const vector<Player*>);
    
    //to identify the points scored by the winner(s) in this round
    int IdentifyPointsScored(const vector<Player*>);
    
    
private:
    
    // 2-player, 3-player or 4-player
    Mode m_mode;
    
    //the number of tricks taken by each player
    vector<int> m_trickScores;
    
    //the Hakem for the round
    Player* m_hakem;
    
    //the trump suit for the round
    Suit m_trumpSuit;
    
    //the tricks played in the round
    vector<Trick> m_tricks;
    
    //the player that won this round (or team in 4-player mode)
    vector<Player*> m_winner;
    
    
    //*** the scoring points components of the round *** ///
   
    //winning the round by taking all the first seven tricks is called Kot, which is worth 2 points
    static const int m_kot = 2;
    
    //To kot Hakem player(team) is called Hakem koti, which is worth 3 points
    static const int m_hakemKoti = 3;
    
    
    //*** private functions *** ///
    
    //to deal a certain number of cards to each player for a specific number of times (starting with Hakem)
    void DealCards(Deck&, vector<Player*>&, const int&, const int&);
};
