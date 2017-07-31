#include "toe.h"

//Agent SafeZoneScout

//Authored by:  Dylan Parrish

//This agent takes an approach bent on attempting to safely wander through obstacle
//zones.  This wandering allows it to retrieve a larger number of idols than many of
//its counterparts.  To further enhance this strength, the agent always checks for 
//idols housed within the central safe row, retrieving them before wandering the 
//platform section of the map.  However, since idols are only counted should an agent
//manage to escape from the temple, its lack of surety in its safety can prove a 
//limiting factor to its overall performance.  

action toeAgentSafeZoneScout(ruinMap map, int myX, int myY, bool shouldReset)
{
	static bool gotIdol[worldSize][worldSize], isIdol[worldSize][worldSize], idolsLocated;
	//gotIdol array keeps track of whether or not the agent has retrieved an idol in a
		//given location.  Sets to true for each space entered as, if an idol is there,
		//entering this space will result in the agent picking it up automatically.
	//isIdol array maintains the locations of all golden idols on the map.
	//idolsLocated variable is used to locate all idols on the first map tick.
	
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
		
		//if in an even numbered row...: 
		if (myY % 2 == 0)
		{
			//and in the arrow section...:
			if (myY < 5)
			{
				//if the row immediately in front of the agent has no arrows right in front of or up to
				//two columns to the right of the agent, it is safe to jump north, so do it.
				if(!map.getArrow(myX, myY+1) && !map.getArrow(myX+1, myY+1) && !map.getArrow(myX+2, myY+1))
				{
					return moveN;
				}
				
				//if north is unsafe and an arrow will hit the agent for standing still but will not do
				//so if the agent moves in the same direction as the oncoming arrow, move in that direction.
				else if(!map.getArrow(myX-1, myY) && map.getArrow(myX-2, myY))
				{
					return moveE;
				}
				
				//if neither north nor the arrow's direction is safe, move south if it is safe.
				else if((map.getArrow(myX-1, myY) || map.getArrow(myX-2, myY)) && !map.getArrow(myX, myY-1) && !map.getArrow(myX-1, myY-1) && !map.getArrow(myX-2, myY-1))
				{
					return moveS;
				}
			}
			
			//and in the platform section...:
			else
			{
				//if there is a platform to the north of the agent, make the opportunistic jump.
				if(map.getPlatform(myX, myY+1))
				{
					return moveN;
				}
				
				//if there is no platform immediately north  for this turn, but there is a platform
				//diagonally adjacent to the agent's path and a platform beside the agent which will
				//align with the aforementioned diagonal on the next tick, move onto this platform
				//in preparation for the next tick.
				else if(map.getPlatform(myX-1, myY+1) && map.getPlatform(myX+1, myY))
				{
					return moveE;
				}
				
				//if the next row is the exit, neither of the above conditions will have been satisfied,
				//and there is no danger in the exit row.  As such, move north.
				else if(myY+1 == worldSize-1)
				{
					return moveN;
				}
				
				//if a platform has carried the agent to the map boundary...:
				else if(myX == worldSize-1)
				{
					//and there is a platform beside the agent to safely jump on, move onto it.
					if(map.getPlatform(myX-1, myY))
					{
						return moveW;
					}
					//if not, try to back up.  This will be safe if either a platform or the safe row
					//is behind the agent.
					else if(map.getPlatform(myX, myY-1) || myY-1 == worldSize/2)
					{
						return moveS;
					}
					//if none of these is safe, death is assured (the agent has been trapped). As points are
					//calculated based on forward motion, the greatest point return is achieved by taking the
					//pit to the north, so move forward.
					else
					{
						return moveN;
					}
				}
			}
			
			return doNothing;
		}
		
		//else, if in an odd numbered row...:
		else 
		{
			//and in the arrow section...:
			if (myY < 5)
			{
				//if the row immediately in front of the agent has no arrows right in front of or up to
				//two columns to the left of the agent, it is safe to jump north, so do it.
				if(!map.getArrow(myX, myY+1) && !map.getArrow(myX-1, myY+1) && !map.getArrow(myX-2, myY+1))
				{
					return moveN;
				}
				
				//if north is unsafe and an arrow will hit the agent for standing still but will not do
				//so if the agent moves in the same direction as the oncoming arrow, move in that direction.
				else if(!map.getArrow(myX+1, myY) && map.getArrow(myX+2, myY))
				{
					return moveW;
				}
				
				//if neither north nor the arrow's direction is safe, move south if it is safe.
				else if((map.getArrow(myX+1, myY) || map.getArrow(myX+2, myY)) && !map.getArrow(myX, myY-1) && !map.getArrow(myX+1, myY-1) && !map.getArrow(myX+2, myY-1))
				{
					return moveS;
				}
			}
			//and in the safe row...
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
				//label links from safe row detection.  See above for details.
				banana:
				
				//if there is a platform to the north of the agent, make the opportunistic jump.
				if(map.getPlatform(myX, myY+1))
				{
					return moveN;
				}
				
				//if there is no platform immediately north  for this turn, but there is a platform
				//diagonally adjacent to the agent's path and a platform beside the agent which will
				//align with the aforementioned diagonal on the next tick, move onto this platform
				//in preparation for the next tick.
				else if(map.getPlatform(myX+1, myY+1) && map.getPlatform(myX-1, myY))
				{
					return moveW;
				}
				
				//if the next row is the exit, neither of the above conditions will have been satisfied,
				//and there is no danger in the exit row.  As such, move north.
				else if(myY+1 == worldSize-1)
				{
					return moveN;
				}
				
				//if a platform has carried the agent to the map boundary...:
				else if(myX == 0)
				{
					//and there is a platform beside the agent to safely jump on, move onto it.
					if(map.getPlatform(myX+1, myY))
					{
						return moveE;
					}
					//if not, try to back up.  This will be safe if either a platform or the safe row
					//is behind the agent.
					else if(map.getPlatform(myX, myY-1))
					{
						return moveS;
					}
					//if none of these is safe, death is assured (the agent has been trapped). As points are
					//calculated based on forward motion, the greatest point return is achieved by taking the
					//pit to the north, so move forward.
					else
					{
						return moveN;
					}
				}
			}
			return doNothing;
		}
	}
}
