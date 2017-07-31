// Ryan Jones: toeAgentPatientRun
// CS 4318: Artificial Intelligence
// Final Project: Temple of Ech (TOE), a Frogger Style Game.

#include "toe.h"

action toeAgentPatientRun(ruinMap map, int myX, int myY, bool shouldReset)
{
	/*
		This agent will wait for a safe pattern of arrows before running to
		the next safe-zone at row 5, then wait for a pattern of platforms
		before running to the goal.
	*/
	bool clearToJump = true;	  // signals that it is safe for the agent to run across the platforms.

	// If the agent is told to reset, it does nothing, as it uses no static variables.
	if(shouldReset)
	{
		return doNothing;
	}
	
	//Otherwise, the agent acts normally.
	
	//If on an even numbered row...
	if (myY % 2 == 0)
	{		
		//If on the first row of the board..
		if (myY == 0)
		{
			//and if there are no arrows in a position that would hit the agent as it moves north...
			if(!map.getArrow(myX, myY+1) && !map.getArrow(myX+1, myY+1) && !map.getArrow(myX+2, myY+1) && !map.getArrow(myX-2, myY+2) && !map.getArrow(myX-3, myY+2)&& !map.getArrow(myX-4, myY+2) && !map.getArrow(myX + 5, myY + 3))
			{
				//move north.
				return moveN;
			}
			else
			{
				//if it's not safe, wait until next turn.
				return doNothing;
			}
		}
		// If the agent is in the arrows section...
		if (myY < 5)
		{
			//If the agent wouldn't be killed by moving north...
			if(!map.getArrow(myX, myY+1) && !map.getArrow(myX+1, myY+1) && !map.getArrow(myX+2, myY+1))
			{
				//move north.
				return moveN;
			}
			//If the agent would be killed by moving north, but wouldn't get killed by moving south...
			else if((map.getArrow(myX-1, myY) || map.getArrow(myX-2, myY)) && !map.getArrow(myX, myY-1) && !map.getArrow(myX-1, myY-1) && !map.getArrow(myX-2, myY-1))
			{
				//move south.
				return moveS;
			}
		}
		
		//Otherwise, the agent is in the platform section, and should act accordingly.
		else
		{
			//if the agent knows it's safe to move north...
			if(clearToJump)
			{
				return moveN;
			}
			if(myY+1 == worldSize-1)
			{
				return moveN;
			}
		}	
		return doNothing;
	}
	
	//Otherwise, the agent is in an odd numbered row, and should act accordingly.
	else 
	{
		//If the agent is on row 1
		if (myY == 1)
		{
			//If there isn't an arrow on an impact course with the agent for the next few moves...
			if(!map.getArrow(myX + 2, myY+2) && !map.getArrow(myX + 3, myY+2) &&!map.getArrow(myX + 4, myY+2) &&!map.getArrow(myX - 5, myY+3))
			{
				//move north.
				return moveN;
			}
			//Otherwise, back up.
			else
			{
				return moveS;
			}
		}
		//If the agent is still in the arrow section...
		if (myY < 5)
		{
			//move north if it wouldn't kill the agent.
			if(!map.getArrow(myX, myY+1) && !map.getArrow(myX-1, myY+1) && !map.getArrow(myX-2, myY+1))
			{
				return moveN;
			}
			//Otherwise, if it wouldn't kill the agent to back up, move south.
			else if((map.getArrow(myX+1, myY) || map.getArrow(myX+2, myY)) && !map.getArrow(myX, myY-1) && !map.getArrow(myX+1, myY-1) && !map.getArrow(myX+2, myY-1))
			{
				return moveS;
			}
		}
		//Otherwise, the agent is in the platform section and should act accordingly.
		else
		{		
			//If the agent is in the safe zone and centered...
			if(myY == 5 && myX == 5)
			{
				//make sure clearToJump is set to true.
				clearToJump = true;
				//Loop through the y positions above the agent.
				for(int y = myY; y < worldSize - 2; y++)
				{
					//If there is a platform in a position that would enable the agent to move north safely...
					if(map.getPlatform(myX + (y==5 ? 0 : 2), y + 1) && y % 2 == 1)
					{
						//don't do anything.
						;
					}
					else if(map.getPlatform(myX - (y==6 ? 2 : 4), y + 1) && y % 2 == 0)
					{
						;
					}
					//otherwise, set clearToJump to false.
					else
					{
						clearToJump = false;
					}
				}
			}
			//if it's clear to jump, move north.
			if(clearToJump)
			{
				return moveN;
			}
			//otherwise, if the agent is one step away from the goal, move north.
			else if(myY+1 == worldSize-1)
			{
				return moveN;
			}
		}
		//if all else fails, just don't do anything.
		return doNothing;
	}
}

// Strategy:
// This agent waits for a pattern of arrows or platforms (depending on its position)
// that signals that it is safe to simply run north. In doing so, it attempts to minimize
// deaths and maximize the number of times it escapes. However, it doesn't attempt to pick
// up any idols.

// Performance:
// Upon testing, the agent averages 700+ points, which come from successful escapes,
// rather than the number of idols collected. It does cave in, which decreases its score,
// but as the number of cave-ins is minimal, it doesn't effect its final score too much.

// Side note about probabilities:
// There is a 1/81 chance for arrows to form the needed pattern.
// There is a 1/16 chance for platforms to form the needed pattern.
