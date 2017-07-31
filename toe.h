// CS 4318, spring 2016
// Final Project: Temple of Ech
//
// Here are the #includes and definitions available to each agent.

#ifndef TOE_H
#define TOE_H

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

const int arrowProbability = 18; // The chance an arrow will spawn coming onto the map.
const int platformProbability = 35;  //The chance a platform will spawn coming onto the map.
const int idolProbability = 4; //The chance that an idol will spawn onto the map
const int worldSize = 11; // The number of rooms square of each Ruin Dive
const int actionLimit = 3 * worldSize * worldSize; // The ruins cave in after this many actions are taken
const int idolCount = 4;  // The number of bonus-point idols to be spawned in the ruins

// The allowed actions for agents to take. Picking up an idol does not cost an action.
enum action {moveW, moveS, moveN, moveE, doNothing};

class ruinMap
{
	private:
		bool isArrow[worldSize][worldSize  / 2];	//maintains arrow locations on front of board
		bool isKillZone[worldSize][worldSize / 2];	//keeps track of which squares arrows killed by moving forward
		bool isPlatform[worldSize][worldSize / 2];	//maintains platform locations on back of board
		bool isIdol[worldSize][worldSize];			//maintains idol locations
        int randomInt(int) const;
	public:
		//see descriptions in toe.cpp
		ruinMap();
		void printRuin(int, int) const;
		void randomize();
		void reset();
		void advanceTick();
		bool getArrow(int, int) const;
		bool getKillZone(int, int) const;
		bool getPlatform(int, int) const;
		bool getIdol(int, int) const;
};

#endif  // #ifndef TOE_H
