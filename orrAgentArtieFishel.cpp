// YOUR NAME: 
//
// CS 4322, spring 2016
// Agent Challenge 3: Off-road rally
//
// Rename this file and the function below.  For example, if your agent name
// is Jones, rename this orrAgentSmith.cpp file to orrAgentJones.cpp and the
// orrAgentSmith function below to orrAgentJones.  Complete your agent
// function and turn it in on Blackboard.  Random-number generation is not
// allowed; your agent must be entirely deterministic.  Feel free to create
// other agents--each in a separate .cpp file--for yours to race against,
// but turn in only one.  Test your agent(s) with
//
//    bash orrBuild.bash
//
// and then
//
//    ./orrRunSim
//
// Each submitted agent will race on each of at least 100 terrains, with
// sizes ranging from 3x3 to 30x30, to determine the standings, which will
// be posted soon after the agents are due.

#include "orr.h"

int ArtieFishelgetColumn(int, int); //function to return the column number of a given hex
int ArtieFishelgetRow(int, int);	 //function to return the row number of a given hex
int ArtieFishelcMinus(int, int);	 //function to move left one column in a representer
int ArtieFishelcPlus(int, int);	 //function to move a representer right one column
int ArtieFishelrMinus(int, int);	 //function to move a representer South
int ArtieFishelrPlus(int, int);	 //    ''   ''  ''  ''     ''     North
int ArtieFishelcMrP(int, int);	 	 //    ''   ''  ''  ''     ''     Southeast
int ArtieFishelcPrM(int, int);		 //    ''   ''  ''  ''     ''     Northwest
bool ArtieFishelIsTop(int, int);	 //function to test if a hex is at the top of the map
bool ArtieFishelIsBottom(int,int);	 //	   ''   ''  ''  '' '' '' '' '' '' bottom '' '' ''
bool ArtieFishelIsLeft(int,int);	 //	   ''   ''  ''  '' '' '' '' '' '' left   '' '' ''
bool ArtieFishelIsRight(int, int);	 //	   ''   ''  ''  '' '' '' '' '' '' right  '' '' ''

vector<move> orrAgentArtieFishel(TerrainMap &map)
{
   // Your function must end up returning a vector of moves.
   // No random-number generation allowed!
   // map.getSize() gives the size of the terrain: 6 for a 6x6 map, etc.
   // map.getStartHex() gives the number of the start hex.
   // map.getFinishHex() gives the number of the finish hex.
   // map.getMoveTime(fromHex, direction) gives the driving time of one move.
   // map.getNeighborHex(fromHex, direction) gives the hex got to by one move.
    vector<move> route;	//stores the route taken during the race
	int start, 			//holder for the start hex
		finish, 		//holder for the finish hex
		sColumn, 		//stores the start hex's column
		sRow, 			//stores the start hex's row
		fColumn, 		//stores the finish hex's column
		fRow, 			//stores the finish hex's row
		size, 			//stores the number of tiles wide the map is
		i,				//keeps track of place in loops
		position = 0,		//a changing position used to create the heuristic
		posColumn, 		//position's column
		posRow, 		//position's row
		jumpCount, 		//number of jumps made to goal from each tile for heuristic
		nextPick,		//holder for possible next element on frontier
		gValue,			//g Value for a given position (to compare and store in moveValues[n][1])
		lowHex,			//lowest hex number on frontier not yet evaluated
		locus;			//an iterative position for finding a path to the goal
	bool firstMove = true;	//will be used to test if finish is adjacent to start.
	
	//fill variables such as map size and key hexes
	size = map.getSize();
	start = map.getStartHex();
	finish = map.getFinishHex();
	locus = start;
	
	vector<move> bestPath[size*size];	//vector array to hold the best path found to each hex on frontier
	int moveValues[size*size][3];		//[n][0] holds h values;[n][1] holds g; [n][2] holds f; n = hexNumber
	bool onFrontier[size*size];			//keeps track of which hexes have been checked from the frontier
	bool frontierAdjacent[size*size];	//those hexes which have been assigned g values but haven't been evaluated
	
	
	//get columns and rows for key hexes
	sRow = ArtieFishelgetRow(start, size);
	sColumn = ArtieFishelgetColumn(start, size);
	fRow = ArtieFishelgetRow(finish, size);
	fColumn = ArtieFishelgetColumn(finish, size);
	
	//generate Heuristic using a default cost of 2 points per jump (the cheapest possible route from each hex)
	for (i=0; i < (size * size); i++)
	{
		position = i;
		jumpCount = 0;
		while(position != finish)
		{	
			posColumn = ArtieFishelgetColumn(position, size);
			posRow = ArtieFishelgetRow(position, size);
			if (posColumn < fColumn && posRow > fRow)
				position = ArtieFishelcPrM(position, size);
			else if (posColumn > fColumn && posRow < fRow)
				position = ArtieFishelcMrP(position, size);
			else if (posColumn < fColumn)
				position = ArtieFishelcPlus(position, size);
			else if (posColumn > fColumn)
				position = ArtieFishelcMinus(position, size);
			else if (posRow < fRow)
				position = ArtieFishelrPlus(position, size);
			else
				position = ArtieFishelrMinus(position, size);
			jumpCount++;
		}
		
		moveValues[i][0] = 2 * jumpCount;
	}
	
	//initialize g (moveValues[n][1]) and f (moveValues[n][2]) values to large numbers until checks below
	//add them to the frontier.  Also, initialize frontier booleans to false.
	for (i=0; i < (size * size); i++)
	{
		if (i==start)
			moveValues[i][1] = 0;
		else
			moveValues[i][1] = 10000;
		moveValues[i][2] = moveValues[i][1] + moveValues[i][0];
		
		onFrontier[i] = false;
		frontierAdjacent[i] = false;
	}
	
	
	while (!onFrontier[finish])  //continue looping until optimal finish path discovered (finish has lowest f value).
	{
		if (firstMove)
		{
			if(!ArtieFishelIsTop(start, size))
				if(map.getNeighborHex(start, moveN)==finish)
				{
					bestPath[finish].push_back(moveN);
					onFrontier[finish]=true;
				}
			else if(!ArtieFishelIsTop(start, size) || !ArtieFishelIsLeft(start, size))
				if(map.getNeighborHex(start, moveK)==finish)
				{
					bestPath[finish].push_back(moveK);
					onFrontier[finish]=true;
				}
			else if(!ArtieFishelIsLeft(start, size))
				if(map.getNeighborHex(start, moveW)==finish)
				{
					bestPath[finish].push_back(moveW);
					onFrontier[finish]=true;
				}
			else if(!ArtieFishelIsBottom(start, size))
				if(map.getNeighborHex(start, moveS)==finish)
				{
					bestPath[finish].push_back(moveS);
					onFrontier[finish]=true;
				}
			else if(!ArtieFishelIsBottom(start, size) || !ArtieFishelIsRight(start, size))
				if(map.getNeighborHex(start, moveX)==finish)
				{
					bestPath[finish].push_back(moveX);
					onFrontier[finish]=true;
				}
			else if(!ArtieFishelIsRight(start, size))
				if(map.getNeighborHex(start, moveE)==finish)
				{
					bestPath[finish].push_back(moveE);
					onFrontier[finish]=true;
				}
			
			firstMove = false;	
		}
		lowHex = 0;
		while (onFrontier[lowHex])  //loop sets lowHex to lowest Hex not already evaluated
		{
			lowHex++;
		}
		
		nextPick = lowHex;
		//pick the element with the lowest f Value (moveValues[n][2]) which is not already on the
		//evaluated frontier to analyze next
		//if finish < start, evaluate hexes with equal f values closer to finish by taking the greater
		//hexNumber.  Otherwise, take the lower HexNumber for evaluation first.
		if (finish < start)
			for (i=lowHex; i < (size * size); i++)
			{
				if(moveValues[i][2] < moveValues[nextPick][2] && !onFrontier[i])
					nextPick = i;
			}
		else
			for (i=lowHex; i < (size * size); i++)
			{
				if(moveValues[i][2] <= moveValues[nextPick][2] && !onFrontier[i])
					nextPick = i;
			}
		
		//if finish is the number with the lowest f value, best path to finish already discovered. 
		if (nextPick == finish)
		{	
			locus = finish;
		}
		
		//set the chosen hex to analyze as the current position and add it to the frontier
		position = nextPick;
		onFrontier[position] = true;
		
		//find the fValues for all hexes adjacent to position, setting it as the new fValue only
		//if it is better than any fValue previously discovered for that element (default 10,000)
		//test in order: N,K,W,S,X,E.
		//only analyze these under the conditions that:
		//a) the neighborHex is not already on the evaluated list
		//b) the move is legal based on current position
		//c) the current Hex is not the finish Hex
		//d) the heuristic for the current hex is no more than 6 larger than that of the startHex
			//(keeps from evaluating too far out and still optimal through over 100,000 iterations)
		if(!ArtieFishelIsTop(position, size) && !onFrontier[finish] && moveValues[position][0] <= moveValues[start][0] + 6 && !onFrontier[map.getNeighborHex(position, moveN)])
		{
			gValue = map.getMoveTime(position, moveN) + moveValues[position][1];
			if (gValue < moveValues[map.getNeighborHex(position, moveN)][1])
			{
				moveValues[map.getNeighborHex(position, moveN)][1] = gValue;
				moveValues[map.getNeighborHex(position, moveN)][2] = moveValues[map.getNeighborHex(position, moveN)][1] + moveValues[map.getNeighborHex(position, moveN)][0];
				bestPath[map.getNeighborHex(position, moveN)].assign(bestPath[position].begin(), bestPath[position].end());
				bestPath[map.getNeighborHex(position, moveN)].push_back(moveN);
			}
			frontierAdjacent[map.getNeighborHex(position, moveN)] = true;
		}
		if(!ArtieFishelIsTop(position, size) || !ArtieFishelIsLeft(position, size) && !onFrontier[finish] && moveValues[position][0] <= moveValues[start][0] + 6 && !onFrontier[map.getNeighborHex(position, moveW)])
		{
			gValue = map.getMoveTime(position, moveK) + moveValues[position][1];
			if (gValue < moveValues[map.getNeighborHex(position, moveK)][1])
			{
				moveValues[map.getNeighborHex(position, moveK)][1] = gValue;
				moveValues[map.getNeighborHex(position, moveK)][2] = moveValues[map.getNeighborHex(position, moveK)][1] + moveValues[map.getNeighborHex(position, moveK)][0];
				bestPath[map.getNeighborHex(position, moveK)].assign(bestPath[position].begin(), bestPath[position].end());
				bestPath[map.getNeighborHex(position, moveK)].push_back(moveK);
			}
			frontierAdjacent[map.getNeighborHex(position, moveK)] = true;
		}
		if(!ArtieFishelIsLeft(position, size) && !onFrontier[finish] && moveValues[position][0] <= moveValues[start][0] + 6 && !onFrontier[map.getNeighborHex(position, moveW)])
		{
			gValue = map.getMoveTime(position, moveW) + moveValues[position][1];
			if (gValue < moveValues[map.getNeighborHex(position, moveW)][1])
			{
				moveValues[map.getNeighborHex(position, moveW)][1] = gValue;
				moveValues[map.getNeighborHex(position, moveW)][2] = moveValues[map.getNeighborHex(position, moveW)][1] + moveValues[map.getNeighborHex(position, moveW)][0];
				bestPath[map.getNeighborHex(position, moveW)].assign(bestPath[position].begin(), bestPath[position].end());
				bestPath[map.getNeighborHex(position, moveW)].push_back(moveW);
			}
			frontierAdjacent[map.getNeighborHex(position, moveW)] = true;
		}
		if(!ArtieFishelIsBottom(position, size) && !onFrontier[finish] && moveValues[position][0] <= moveValues[start][0] + 6 && !onFrontier[map.getNeighborHex(position, moveS)])
		{
			gValue = map.getMoveTime(position, moveS) + moveValues[position][1];
			if (gValue < moveValues[map.getNeighborHex(position, moveS)][1])
			{
				moveValues[map.getNeighborHex(position, moveS)][1] = gValue;
				moveValues[map.getNeighborHex(position, moveS)][2] = moveValues[map.getNeighborHex(position, moveS)][1] + moveValues[map.getNeighborHex(position, moveS)][0];
				bestPath[map.getNeighborHex(position, moveS)].assign(bestPath[position].begin(), bestPath[position].end());
				bestPath[map.getNeighborHex(position, moveS)].push_back(moveS);
			}
			frontierAdjacent[map.getNeighborHex(position, moveS)] = true;
		}
		if(!ArtieFishelIsBottom(position, size) || !ArtieFishelIsRight(position, size) && !onFrontier[finish] && moveValues[position][0] <= moveValues[start][0] + 6 && !onFrontier[map.getNeighborHex(position, moveX)])
		{
			gValue = map.getMoveTime(position, moveX) + moveValues[position][1];
			if (gValue < moveValues[map.getNeighborHex(position, moveX)][1])
			{
				moveValues[map.getNeighborHex(position, moveX)][1] = gValue;
				moveValues[map.getNeighborHex(position, moveX)][2] = moveValues[map.getNeighborHex(position, moveX)][1] + moveValues[map.getNeighborHex(position, moveX)][0];
				bestPath[map.getNeighborHex(position, moveX)].assign(bestPath[position].begin(), bestPath[position].end());
				bestPath[map.getNeighborHex(position, moveX)].push_back(moveX);
			}
			frontierAdjacent[map.getNeighborHex(position, moveX)] = true;
		}
		if(!ArtieFishelIsRight(position, size) && !onFrontier[finish] && moveValues[position][0] <= moveValues[start][0] + 6 && !onFrontier[map.getNeighborHex(position, moveE)])
		{
			gValue = map.getMoveTime(position, moveE) + moveValues[position][1];
			if (gValue < moveValues[map.getNeighborHex(position, moveE)][1])
			{
				moveValues[map.getNeighborHex(position, moveE)][1] = gValue;
				moveValues[map.getNeighborHex(position, moveE)][2] = moveValues[map.getNeighborHex(position, moveE)][1] + moveValues[map.getNeighborHex(position, moveE)][0];
				bestPath[map.getNeighborHex(position, moveE)].assign(bestPath[position].begin(), bestPath[position].end());
				bestPath[map.getNeighborHex(position, moveE)].push_back(moveE);
			}
			frontierAdjacent[map.getNeighborHex(position, moveE)] = true;
		}
	}
	route.assign(bestPath[finish].begin(), bestPath[finish].end());
	
    return route;
}

int ArtieFishelgetColumn(int position, int size)
{
	return (position / size) + 1;
}

int ArtieFishelgetRow(int position, int size)
{
	return (position % size) + 1;
}

int ArtieFishelcMinus(int position, int size)
{
	return position -= size;
}
int ArtieFishelcPlus(int position, int size)
{
	return position += size;
}
int ArtieFishelrMinus(int position, int size)
{
	return position -= 1;
}	
int ArtieFishelrPlus(int position , int size)
{
	return position += 1;
}
int ArtieFishelcMrP(int position, int size)
{
	return position -= (size-1);
}	
int ArtieFishelcPrM(int position, int size)
{
	return position += (size-1);
}
bool ArtieFishelIsTop(int position, int size)
{
	if(ArtieFishelgetRow(position, size) == size)
		return true;
	else
		return false;
}
bool ArtieFishelIsBottom(int position,int size)	
{
	if(ArtieFishelgetRow(position, size) == 1)
		return true;
	else
		return false;
}
bool ArtieFishelIsLeft(int position,int size)	 
{
	if(ArtieFishelgetColumn(position, size) == 1)
		return true;
	else
		return false;	
}
bool ArtieFishelIsRight(int position,int size)
{
	if(ArtieFishelgetColumn(position, size) == size)
		return true;
	else
		return false;
}
	
/*

 - First, carefully comment your code above.
 - This agent centers on an A* approach.  It holds an advantage over other A* agents by restricting
   the amount of backwards analysis it performs, keeping a relative focus toward the goal.  The heuristic
   is simple Manhattan distance muliplied by 2 to reflect the minimum cost it could possibly take to reach
   the goal from a given Hex.  
 - Worked with Ryan Jones.

*/
