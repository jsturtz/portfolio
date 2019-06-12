/********************************************************************* 
** Author: Jordan Sturtz
** Date: 06-4-2018
** Description: Class Implementation file for Ship class. 
 *********************************************************************/

#include "Ship.hpp"

/********************************************************************* 
** Description: Constructor. Initializes name to first parameter, 
** length to second. Damage starts at 0.
 *********************************************************************/
Ship::Ship(std::string name, int length) 
{
    this->name = name;
    this->length = length;
    this->damage = 0;
}

/********************************************************************* 
** Description: Getters. 
 *********************************************************************/

std::string Ship::getName()
{ return name;}

int Ship::getLength()
{ return length;}

int Ship::getDamage()
{ return damage;}

/********************************************************************* 
** Description: takeHit increments damage by 1.
 *********************************************************************/

void Ship::takeHit() 
{ damage++; }
