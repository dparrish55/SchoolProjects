#include "toe.h"

//Default constructor:
//Calls reset and randomize to create a new map.
ruinMap::ruinMap()
{
	reset();
	randomize();
}

void ruinMap::printRuin(int userX, int userY) const
{
   // Output a concise representation of the current Ruin Map.
   int column, row;
   for (row = worldSize - 1; row >= 0; row -= 1)
   {
      for (column = 0; column < worldSize; column += 1)
      {
         cout << "[" << (getArrow(column, row) ? (row % 2 == 1 ? "<" : ">") : " ")
              << (getKillZone(column, row) ? "-" : " ")
			  << (column == userX && row == userY ? "U" : " ")
			  << (getIdol(column, row) ? "&" : " ")
              << (getPlatform(column, row) ? "O" : " ") << "]";
      }
      cout << "\n";
   }
}

int ruinMap::randomInt(int n) const
{
   // Return a random integer between 0 and n - 1.
   int r;
   if (n <= 0)
   {
      return 0;
   }
   do
   {
      r = random();
   }
   while (r >= INT_MAX / n * n);
   return r / (INT_MAX / n);
}

//randomize:
// Loop through the map, placing arrows and platforms based on their spawn probability,
// and place up to four idols based on their spawn probability.
void ruinMap::randomize()
{
	int numIdols = 0;
	for(int x = 0; x <= worldSize - 1; x++)
	{	
		for(int y = 1; y <= worldSize / 2; y++)
		{
			if(y >= 1 && y <= 4 && randomInt(100) < arrowProbability)
			{
				//Spawn arrows
				isArrow[x][y] = true;
				isKillZone[x][y] = true;
			}
			if(y >= 1 && y <= 4 && randomInt(100) < platformProbability)
			{
				//Spawn platforms
				isPlatform[x][y] = true;
			}
		}
		for(int y = 1; y <= worldSize - 2 && numIdols < idolCount; y++)
		{
			if(randomInt(100) < idolProbability)
			{
				//Spawn idols
				isIdol[x][y] = true;
				numIdols += 1;
			}
		}
	}
}

//advanceTick:
// Loop through the map and move the arrows and KillZones 2 spaces (with direction based on their y position)
// and move the platforms 1 space (with direction based on their y position).
void ruinMap::advanceTick()
{
	for(int x = 0; x < worldSize; x++)
	{	
		for(int y = 1; y < worldSize / 2; y++)
		{
			isKillZone[x][y] = false;
		}
	}
	for(int y = 1; y < worldSize / 2; y++)
	{	
		if(y%2 == 1)
		{
			for(int x = 0; x < worldSize; x++)
			{
				if(isArrow[x][y])
				{
					isArrow[x][y] = false;
					isKillZone[x][y] = true;
					if(x-2 >= 0)
					{
						isArrow[x-2][y] = true;
						isKillZone[x-2][y] = true;
						isKillZone[x-1][y] = true;
					}
					else if(x-1>=0)
					{
						isKillZone[x-1][y] = true;
					}
				}
				if(isPlatform[x][y])
				{
					isPlatform[x][y] = false;
					if(x-1 >= 0)
					{
						isPlatform[x-1][y] = true;
					}
				}
			}
		}
		else
		{
			for(int x = worldSize-1; x >= 0; x--)
			{
				if(isArrow[x][y])
				{
					isArrow[x][y] = false;
					isKillZone[x][y] = true;
					if(x+2 <= worldSize-1)
					{
						isArrow[x+2][y] = true;
						isKillZone[x+2][y] = true;
						isKillZone[x+1][y] = true;
					}
					else if(x+1 <= worldSize-1)
					{
						isKillZone[x+1][y] = true;
					}
				}
				if(isPlatform[x][y])
				{
					isPlatform[x][y] = false;
					if(x+1 <= worldSize-1)
					{
						isPlatform[x+1][y] = true;
					}
				}
			}
		}
	}
	for(int y = 1; y < worldSize / 2; y++)
	{
		if(y % 2 == 0)
		{
			for(int x = 0; x < 2; x++)
			{
				if(x == 0)
				{
					if(randomInt(100) < arrowProbability)
					{
						isArrow[x][y] = true;
						isKillZone[x][y] = true;
					}
					if(randomInt(100) < platformProbability)
					{
						isPlatform[x][y] = true;
					}
				}		
				else
				{
					if(randomInt(100) < arrowProbability)
					{
						isArrow[x][y] = true;
						isKillZone[x][y] = true;
						isKillZone[x - 1][y] = true;
					}
				}
			}
		}
		else
		{
			for(int x = worldSize - 1; x > worldSize - 3; x--)
			{
				if(x == worldSize - 1)
				{
					if(randomInt(100) < arrowProbability)
					{
						isArrow[x][y] = true;
						isKillZone[x][y] = true;
					}
					if(randomInt(100) < platformProbability)
					{
						isPlatform[x][y] = true;
					}
				}
				else
				{
					if(randomInt(100) < arrowProbability)
					{
						isArrow[x][y] = true;
						isKillZone[x][y] = true;
						isKillZone[x + 1][y] = true;
					}
				}
			}
		}	
	}
}

//getArrow:
//returns whether or not there is an arrow at the given x and y position.
bool ruinMap::getArrow(int x, int y) const 
{
	if (y > 4)
		return false;
	return isArrow[x][y];
}
//getKillZone:
//returns whether or not there is a KillZone at the given x and y position.
bool ruinMap::getKillZone(int x, int y) const 
{
	if (y>4)
		return false;
	return isKillZone[x][y];
}
//getPlatform:
//returns whether or not there is a platform at the given x and y position.
bool ruinMap::getPlatform(int x, int y) const
{
	if (y <= 4 || y==11)
		return false;
	y -= 5;
	return isPlatform[x][y];
}

//getIdol:
//returns whether or not there is an idol at the given x and y position.
bool ruinMap::getIdol(int x, int y) const
{
	return isIdol[x][y];
}

//reset:
//Clears out the entire map, setting all values to false.
void ruinMap::reset()
{
	for(int x = 0; x < worldSize; x++)
	{
		for(int y = 0; y < worldSize; y++)
		{
			if(y < worldSize / 2)
			{
				isArrow[x][y] = false;
				isKillZone[x][y] = false;
				isPlatform[x][y] = false;
			}
			isIdol[x][y] = false;
		}
	}
}
