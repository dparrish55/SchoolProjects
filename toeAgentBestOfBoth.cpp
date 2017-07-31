#include "toe.h"

action toeAgentBestOfBoth(ruinMap map, int myX, int myY, bool shouldReset)
{
	static bool gotIdol[worldSize][worldSize], isIdol[worldSize][worldSize], idolsLocated;
	bool clearToJump = true;
	//gotIdol array keeps track of whether or not the agent has retrieved an idol in a
		//given location.  Sets to true for each space entered as, if an idol is there,
		//entering this space will result in the agent picking it up automatically.
	//isIdol array maintains the locations of all golden idols on the map.
	//idolsLocated variable is used to locate all idols on the first map tick.
	//clearToJump maintains that the agent is moving safely across the platforms based on
		//a specific known-safe pattern.
	
	//when passed that the map has reset, set that the agent has not retrieved idols from
	//any locations, that idols do not yet exist in any locations, and that the idols have
	//yet to be located.
	if (shouldReset)
	{
		idolsLocated = false;
		for(int i = 0; i < worldSize; i++)
		{
			for(int j = 0; j < worldSize; j++)
			{
				gotIdol[i][j] = false;
				isIdol[i][j] = false;
			}
		}
		return doNothing; //return can be anything of type action.
	}
	else
	{
		//if the idols have not yet been located, locate them.
		if(!idolsLocated)
		{
			for(int i = 0; i < worldSize; i++)
			{
				for(int j = 0; j < worldSize; j++)
				{
					if(map.getIdol(i, j))
					{
						isIdol[i][j] = true;
					}
				}
			}
			idolsLocated = true;
		}
		
		//set that any idol in the agent's current space has been retrieved.
		gotIdol[myX][myY] = true;
		
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
			//if the agent is on row 1 and firstStepsClear is true
			if (myY == 1)
			{
				if(!map.getArrow(myX + 2, myY+2) && !map.getArrow(myX + 3, myY+2) &&!map.getArrow(myX + 4, myY+2) &&!map.getArrow(myX - 5, myY+3))
				{
					return moveN;
				}
				else
				{
					return moveS;
				}
			}
			if (myY < 5)
			{
				if(!map.getArrow(myX, myY+1) && !map.getArrow(myX-1, myY+1) && !map.getArrow(myX-2, myY+1))
				{
					return moveN;
				}
				else if((map.getArrow(myX+1, myY) || map.getArrow(myX+2, myY)) && !map.getArrow(myX, myY-1) && !map.getArrow(myX+1, myY-1) && !map.getArrow(myX+2, myY-1))
				{
					return moveS;
				}
			}
			else if (myY == 5)
			{
				//check all squares in the row for an unclaimed idol
				for (int i = 0; i < worldSize; i++)
				{
					if(map.getIdol(i, myY) && !gotIdol[i][myY])
					{ 
						//if such an idol is discovered, move toward it.
						if(i > myX)
						{
							return moveE;
						}
						else if(i < myX)
						{
							return moveW;
						}
					}
				}
				
				//once all safe zone idols have been claimed, move toward the center of the row.
				if (myX < worldSize/2)
				{
					return moveE;
				}
				else if (myX > worldSize/2)
				{
					return moveW;
				}
				
				//if all safe zone idols have been claimed and the agent is centered, jump inside
				//platform detection block via label "banana."
				else
					goto banana;
			}
			else
			{		
				banana:
				if(myY == 5 && myX == 5)
				{
					clearToJump = true;
					for(int y = myY; y < worldSize - 2; y++)
					{
						if(map.getPlatform(myX + (y==5 ? 0 : 2), y + 1) && y % 2 == 1)
						{
							;
						}
						else if(map.getPlatform(myX - (y==6 ? 2 : 4), y + 1) && y % 2 == 0)
						{
							;
						}
						else
						{
							clearToJump = false;
						}
					}
				}
				
				if(clearToJump)
				{
					return moveN;
				}
				else if(myY+1 == worldSize-1)
				{
					return moveN;
				}
			}
			
			return doNothing;
		}
	}
}
