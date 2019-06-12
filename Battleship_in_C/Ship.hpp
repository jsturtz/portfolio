/********************************************************************* 
** Author: Jordan Sturtz
** Date: 06-4-2018
** Description: Header file for Ship class. This ship class is used
** in the BBoard class. A Ship object has three member variables: 
** (1) a name, (2) a length, and (3) its current damage. The 
** constructor takes two parameters, a name and a length, and 
** initializes accordingly while also initializing damage to 0. Aside
** from the relavent getters, the only member function is takeHit, 
** increments damage. 
 *********************************************************************/

#ifndef SHIP_HPP
#define SHIP_HPP
#include<string>

class Ship 
{
    private:
        std::string name;
        int length;
        int damage;

    public:
        Ship(std::string, int);
        
        // adds one to damage
        void takeHit();

        // getters
        std::string getName();
        int getLength();
        int getDamage();
};
 
#endif 
