#include "BBoard.hpp"
#include <iostream>
#include <assert.h>

using namespace std;
int main() 
{
    BBoard board;
    Ship ship1("Boaty McBoatface", 2);
    board.placeShip(&ship1, 2, 2, 'R');
    cout << "allShipsSunk() should be false: " << board.allShipsSunk();
    board.attack(2, 3);
    cout << "ship1 damage should be 1: " << ship1.getDamage();

    /* BBoard board1; */
    /* BBoard board2; */

    /* Ship boaty("Boaty McBoatFace", 5); */
    /* Ship dipship("Dipship", 3); */
    /* Ship jealous("Jealous Much?", 2); */
    /* Ship fatPhallus("USS Fat Phallus", 4); */
    /* Ship blowhard("USS Blowhard", 6); */

    /* cout << "placing boaty at 0, 0" << endl; */
    /* board1.placeShip(&boaty, 0, 0, 'R'); */
    /* cout << "placing dipship so that it overlaps boaty..." << endl; */
    /* board1.placeShip(&dipship, 0, 4, 'R'); */
    /* cout << "placing in a legitimate spot at 0, 5..." << endl; */
    /* board1.placeShip(&dipship, 0, 5, 'C'); */

    /* cout << "placing jealous so that it goes over row edge ..." << endl; */
    /* board1.placeShip(&jealous, 9, 9, 'R'); */

    /* cout << "placing jealous so that it goes over col edge ..." << endl; */
    /* board1.placeShip(&jealous, 9, 9, 'C'); */

    /* cout << "testing allShipsSunk. Should be 0 for false: " << board1.allShipsSunk() << endl; */

    /* cout << "attacking 0, 0..." << endl; */
    /* board1.attack(0, 0); */
    /* cout << "attacking 0, 0..." << endl; */
    /* board1.attack(0, 0); */
    /* cout << "attacking 0, 5..." << endl; */
    /* board1.attack(0, 5); */
    /* cout << "attacking 0, 1..." << endl; */
    /* board1.attack(0, 1); */
    /* cout << "attacking 0, 2..." << endl; */
    /* board1.attack(0, 2); */
    /* cout << "attacking 0, 3..." << endl; */
    /* board1.attack(0, 3); */
    /* cout << "attacking 0, 4..." << endl; */
    /* board1.attack(0, 4); */


    /* cout<< "attacking 0, 5..." << endl; */
    /* board1.attack(0, 5); */
    /* cout<< "attacking 1, 5..." << endl; */
    /* board1.attack(1, 5); */
    /* cout<< "attacking 2, 5..." << endl; */
    /* board1.attack(2, 5); */

    /* cout << "testing allShipsSunk. Should be 1 for true: " << board1.allShipsSunk() << endl; */

    return 0;
}


