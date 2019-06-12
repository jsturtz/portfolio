/********************************************************************* 
** Author: Jordan Sturtz
** Date: 06-4-2018
** Description: Class declaration file for BBoard class. The BBoard
** class is designed to be used in creating a Battleship board game.
** The BBoard represents one player 10 x 10 board filled with 
** battleships of different sizes. The BBoard contains three 
** member variables: 
**      (1) attacksArray, a 10 x 10 array of bools to keep track of 
**          which locations have been attacked
**      (2) shipsArray, a 10 x 10 array of pointers-to-Ships to track
**          which spots point to which ships
**      (3) unSunkShips, an int that records the amount of unSunkShips
**          left on the board
** BBoard has getters the elements stored in both 10 x 10 arrays, and
** has a getter for the number of unSunkShips. In addition, 
** BBoard has the following member functions:
**      (1) placeShip, which places a ship and returns true iff the
**          ship can be legitimately placed. 
**      (2) attack, which attacks the given spot, and updates the
**          attacksArray and unSunkShips as necessary
**      (3) allShipsSunk, which returns whether all ships are sunk
 *********************************************************************/

#ifndef BBOARD_HPP
#define BBOARD_HPP
#include "Ship.hpp"

class BBoard 
{
    private:
        bool attacksArray[10][10];
        Ship* shipsArray[10][10];
        int unSunkShips;

    public:
        // constructors and destructor 
        BBoard();
        
        // member functions
        bool getAttacksArrayElement(int, int);
        Ship* getShipsArrayElement (int, int);
        int getNumShipsRemaining();
        bool placeShip(Ship*, int, int, char);
        bool attack(int, int);
        bool allShipsSunk();
};
 
#endif 
