/********************************************************************* 
** Author: Jordan Sturtz
** Date: 06-4-2018
** Description: Class Implementation file for BBoard class. 
 *********************************************************************/

#include "BBoard.hpp"
#include <iostream>

/********************************************************************* 
** Description: Constructor. Initializes all bools in attacksArray
** to false, all *Ship in shipsArray to null, and unSunkShips to 0.
 *********************************************************************/
BBoard::BBoard() 
{
    unSunkShips = 0;

    for (int i = 0; i < 10; i++) 
    {
        for (int j = 0; j < 10; j++) 
        {
            attacksArray[i][j] = false;
            shipsArray[i][j] = NULL;
        }
    }
}


/********************************************************************* 
** Description: getters
 *********************************************************************/

bool BBoard::getAttacksArrayElement(int row, int col) 
{ return attacksArray[row][col]; }

Ship* BBoard::getShipsArrayElement(int row, int col) 
{ return shipsArray[row][col]; }

int BBoard::getNumShipsRemaining() 
{ return unSunkShips; }

/********************************************************************* 
** Description: The main function of placeShip is to add a new ship
** to shipsArray. It will perform two checks: (1) check whether the
** ship overlaps another ship and (2) check whether the ship can 
** fit in the designated row-column address. If neither, then
** shipsArray will return false. Otherwise, true. What it means to 
** "add" these ships to shipsArray is to make the pointers in the 
** appropriate spot all point to the address of the entered ship.
** Thus, four parameters are needed: 
**      (1) the address of the ship object
**      (2) the row of the lowest-most location of the ship
**      (3) the column of the left-most location of the ship
**      (4) the orientation of the ship ('R' for row, 'C' for column)
 *********************************************************************/
bool BBoard::placeShip (Ship* ship, int row, int col, char orientation)
{
    int length = ship->getLength();

    // code to use if orientation of ship is along the rows (i.e. from left to right)
    if (orientation == 'R')
    {
        // checks to see if placement of ship fits on board
        if (row + length > 10) 
        { return false; }

        // checks to see whether ship overlaps other ship
        for (int i = 0; i < length; i++)
        {
            if (shipsArray[row][col + i] != NULL)
            { return false; }
        }
        
        // makes the slots in shipsArray point to ship
        for (int i = 0; i < length; i++)
        { shipsArray[row][col + i] = ship; }
        
        unSunkShips++;
        return true;
    }

    else if (orientation == 'C')
    {
        // checks to see if placement of ship fits on board
        if (col + length > 10) 
        { return false; }

        // checks to see whether ship overlaps other ship
        for (int i = 0; i < length; i++)
        {
            if (shipsArray[row + i][col] != NULL)
            { return false; }
        }
        
        // makes the slots in shipsArray point to ship
        for (int i = 0; i < length; i++)
        { shipsArray[row + i][col] = ship; }

        unSunkShips++;
        return true;
    }

}

/********************************************************************* 
** Description: attack will add an attack to the board. It takes in
** two parameters: (1) the row and (2) the column of the attack. 
** If the attack hits and the attack has not already been made at 
** the given row-column address, then this function will call the 
** ship's takeHit member function. If the ship has taken damage 
** equivalent to its length, then this function will cout "They sank 
** <ship's name>" and decrement unSunkShips. Regardless of 
** whether the spot has been hit, this function will also update
** attacksArray with a true bool. 
 *********************************************************************/
bool BBoard::attack (int row, int col)
{
    // checks whether spot is filled up with a ship object and has not already been hit
    if (shipsArray[row][col] != NULL && attacksArray[row][col] == false)
    {
        // calls the takeHit member function on the Ship object at row, col
        Ship* ship = getShipsArrayElement(row, col);
        ship->takeHit();

        // checks to see if ship's damage == ship's length
        if (ship->getDamage() == ship->getLength()) 
        {
            std::cout << "They sank " << ship->getName() << "!" << std::endl;
            unSunkShips--;
        }

        // updates attacksArray and returns true
        attacksArray[row][col] = true;
        return true;
    }

    // updates attacksArray and returns false
    attacksArray[row][col] = true;
    return false;
}



/********************************************************************* 
** Description: allShipsSunk returns whether or not all ships have been
** sunk. This is equivalent to checking whether getNumShipsRemaining
** equals 0. 
 *********************************************************************/
bool BBoard::allShipsSunk ()
{ return getNumShipsRemaining() == 0; }
