#include "stdafx.h"

//*** overloaded operators ***//

//every player has a unique username excpet for NPCs in different matches
bool Player::operator == (const Player& a_player) const
{
    if (m_username == a_player.m_username)
        return true;
    
    return false;
}


bool Player::operator != (const Player& a_player) const
{
    if (*this == a_player)
        return false;
    
    return true;
}


Player& Player::operator = (const Player& a_player)
{
    m_username = a_player.m_username;
    m_hand = a_player.m_hand;
    m_partner = a_player.m_partner;
    
    return *this;
}

