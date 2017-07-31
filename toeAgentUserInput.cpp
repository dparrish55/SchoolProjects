#include "toe.h"

action toeAgentUserInput(ruinMap map, int myX, int myY, bool shouldReset)
{
	char input = 'x';
	
	if (shouldReset)
	{
		return doNothing;
	}
	
	cout << "Which direction or stand still? N, S, W, E, or X: ";
	cin >> input;
	
	if (input == 'N' || input == 'n')
	{
		return moveN;
	}
	
	else if(input == 'S' || input == 's')
	{
		return moveS;
	}
	
	else if(input == 'W' || input == 'w')
	{
		return moveW;
	}
	
	else if(input == 'E' || input == 'e')
	{
		return moveE;
	}
	
	else
	{
		return doNothing;
	}
}
