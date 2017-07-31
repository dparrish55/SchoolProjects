//Main Function for Temple of Ech which rotates through the agents across each
//tick of a Ruin Map.
#include "toe.h"

extern const int numAgents;
extern action (*agentFunc[])(ruinMap, int, int, bool);
extern string agentStr[];

int main()
{
    const int numWorlds = 3;   // Change if you like.
    action applyAction = doNothing;
    int user, i, j, m, n, finishedCount, order[numAgents], score[numAgents],
	    timesKilled[numAgents], totalIdols[numAgents], timesFinished[numAgents],
        idolsCollected[numAgents], timesCavedIn[numAgents], finishedTick[numAgents],
		temp, totalScore[numAgents], agentX[numAgents], agentY[numAgents]; 
	bool keepGoing, hasDied[numAgents], hasFinished[numAgents], gotIdol[numAgents][worldSize][worldSize], cavedIn[numAgents], done[numAgents];
	ruinMap map;
	
	srandom(time(0));

   cout << "AI Final Project: Temple of Ech\n"
        << "Iteration results\n\n";
		
	for (i = 0; i < numAgents; i++)
	{
		totalScore[i] = 0;
		totalIdols[i] = 0;
		timesKilled[i] = 0;
		timesCavedIn[i] = 0;
		timesFinished[i] = 0;
		if(agentStr[i]=="UserInput")
		{
			user = i;
		}
	}
	
	for (i=0; i < numWorlds; i++)
	{
		//At the beginning of each map, reset the obstacle 
		//locations and the map-specific variables
		map.reset();
		for (j = 0; j < numAgents; j++)
		{
			hasDied[j] = false;
			hasFinished[j] = false;
			cavedIn[j] = false;
			agentX[j] = 5;
			agentY[j] = 0;
			finishedTick[j] = 0;
			score[j] = 0;
			idolsCollected[j] = 0;
			done[j]=false;
			for(m=0; m< worldSize; m++)
			{
				for(n = 0; n< worldSize; n++)
				{
					gotIdol[j][m][n] = false;
				}
			}
		}
		
		//tell all agents to reset static variables for the start of the new map
		for (j = 0; j < numAgents; j++)
		{
			(*agentFunc[j])(map, agentX[j], agentY[j], true);
		}
		finishedCount = 0;
		applyAction = doNothing;
		
		//randomize the obstacles already on the map and start agent navigation
		map.randomize();
		for (j = 0; j < actionLimit; j++)
		{
			//the agents manipulate the map in the same way between the following 
			//if statement and its corresponding else block.  The code is input
			//twice due to the minor change in output functionality as the first
			//block outputs every tick of the first and last three maps and the
			//first tick of every other map whereas the else block controls all 
			//other cases and only outputs those map ticks on which an agent dies
			//or successfully exits the map.  Regardless of which block is navi-
			//gated, there will always print a map summary at the end of each map.
			if (i < 3 || i >= numWorlds-3 || j==0)
			{
				cout << "\nRuin Map " << i + 1 << ", Tick " << j << ":\n";
				map.printRuin(agentX[user], agentY[user]);
				
				for (m = 0; m < numAgents; m++)
				{
					//provide a printout that allows a reader to see where each 
					//agent is prior to movement at the current tick.
					cout << setw(20) << left << agentStr[m] << " (" << setw(2) << right
						<< agentX[m] << "," << setw(2) << right << agentY[m] << ")";
					//if an agent that has not died yet is in the updated list of
					//kill zones due to movement of arrows on the last tick, set
					//that the agent has died, no longer allowing it to move about
					//the board.  Calculate score accordingly
					if(map.getKillZone(agentX[m], agentY[m]) && !hasDied[m])
					{
						hasDied[m] = true;
						done[m] = true;
						finishedTick[m] = j;
						score[m] += agentY[m] * 100;
						score[m] -= j + 1;
						score[m] -= 1000;
						timesKilled[m]++;
						totalScore[m] += score[m];					
					}
					
					//if a platform slid off screen on the last tick, thereby pushing
					//the player off into the empty space left behind, set that the 
					//agent has died, no longer allowing it to move about the board.
					//Calculate score accordingly.
					else if(agentY[m] > 5 && agentY[m] < 10 && !map.getPlatform(agentX[m], agentY[m]) && !hasDied[m])
					{
						hasDied[m] = true;
						done[m] = true;
						finishedTick[m] = j;
						score[m] += agentY[m] * 100;
						score[m] -= j + 1;
						score[m] -= 1000;
						timesKilled[m]++;
						totalScore[m] += score[m];
					}

					//include the agent's death or completion of the map to the printout
					//for all subsequent ticks of the clock.
					if(hasDied[m])
					{
						cout << setw(10) << right << "Died" << " on tick " << setw(3) << right << finishedTick[m]; 
					}
					else if(hasFinished[m])
					{
						cout << setw(10) << right << "Finished" << " on tick " << setw(3) << right << finishedTick[m]; 
					}
					cout << "\n";
					
					//if the agent is still exploring the map, pass it the snapshot of
					//the obstacle positions and allow it to make a single move.
					if(hasDied[m] || hasFinished[m])
					{
						; 
					}
					else
					{
						applyAction = (*agentFunc[m])(map, agentX[m], agentY[m], false);
						
						//if it is possible for the agent to move in the direction
						//returned, update the agent's position.
						if(applyAction == moveN)
						{
							if(agentY[m] < worldSize - 1)
								agentY[m]++;					
						}
						else if(applyAction == moveW)
						{
							if(agentX[m] > 0)
								agentX[m]--;
						}
						else if(applyAction == moveS)
						{
							if(agentY[m] > 0)
								agentY[m]--;
						}
						else if(applyAction == moveE)
						{
							if(agentX[m] < worldSize - 1)
								agentX[m]++;
						}
						else
						{
							;
						}
					}
					
					//if the agent has entered a square which contains a golden idol
					//and that agent has not already picked up that idol, pick it up.
					if(map.getIdol(agentX[m], agentY[m]) && !gotIdol[m][agentX[m]][agentY[m]])
					{
						idolsCollected[m]++;
						gotIdol[m][agentX[m]][agentY[m]] = true;
					}
					
					//if the agent jumps into a space on the back half of the map and
					//that space does not contain a floating platform, set that the 
					//agent has died, no longer allowing it to manipulate the map.
					//calculate score accordingly.
					if(agentY[m] > 5 && agentY[m] < 10 && !map.getPlatform(agentX[m], agentY[m]) && !hasDied[m])
					{
						hasDied[m] = true;
						done[m] = true;
						finishedTick[m] = j;
						score[m] += agentY[m] * 100;
						score[m] -= j + 1;
						score[m] -= 1000;
						timesKilled[m]++;
						totalScore[m] += score[m];
					}
					
					//if an agent reaches the final row, set that the agent
					//has successfully escaped the map. Calculate score
					//accordingly.  Add to the score for each golden idol
					//obtained along the way.
					else if (agentY[m] == 10 && !hasFinished[m])
					{
						hasFinished[m] = true;
						timesFinished[m]++;
						done[m] = true;
						finishedTick[m] = j;
						score[m] += 1000;
						totalIdols[m] += idolsCollected[m];
						while(idolsCollected[m] > 0)
						{
							score[m] += 250;
							idolsCollected[m]--;
						}
						totalScore[m] += score[m];
					}
					
					//if the agent is standing on a floating platform, move it
					//along with the platform in the appropriate direction.
					if(map.getPlatform(agentX[m], agentY[m]) && agentY[m] % 2 == 0 && agentX[m] > 0 && !hasDied[m])
					{
						agentX[m] -= 1;
					}
					else if(map.getPlatform(agentX[m], agentY[m]) && agentY[m] % 2 == 1 && agentX[m] < worldSize - 1 && !hasDied[m])
					{
						agentX[m] += 1;
					}
					
					//if the action limit is reached and the agent has
					//neither died nor escaped, set that the agent was
					//victim to a cave-in.  Calculate score accordingly.
					if(j == actionLimit-1 && !hasDied[m] && !hasFinished[m])
					{
						cavedIn[m] = true;
						timesCavedIn[m]++;
						score[m] -= 2000;
						score[m] -= j + 1;
						totalScore[m] += score[m];
					}
				}
				
				//run through a check to see if any agents have neither
				//escaped nor died, breaking from the map if all are done.
				keepGoing = false; 
				
				for(n = 0; n < numAgents; n++)
				{
					if (!done[n])
						keepGoing = true;
				}
				
				if(!keepGoing)
					break;
				
				map.advanceTick();
			}
			
			//This is the aforementioned else block which manipulates the map
			//in all the same ways as the above if block.  It is kept separate
			//only for purposes of different printout structure.  Please refer
			//to comments above for function of internal code.
			else
			{
				for (m = 0; m < numAgents; m++)
				{
					if(map.getKillZone(agentX[m], agentY[m]) && !hasDied[m])
					{
						hasDied[m] = true;
						done[m] = true;
						finishedTick[m] = j;
						score[m] += agentY[m] * 100;
						score[m] -= 1000;
						timesKilled[m]++;
						totalScore[m] += score[m];
						cout << "\nRuin Map " << i + 1 << ", Tick " << j << ":\n";
						map.printRuin(agentX[user], agentY[user]);
						cout << setw(20) << left << agentStr[m] << " (" << setw(2) << right
							<< agentX[m] << "," << setw(2) << right << agentY[m] << ")";
						if(hasDied[m])
						{
							cout << setw(10) << right << "Died" << " on tick " << setw(3) << right << finishedTick[m]; 
						}
						else if(hasFinished[m])
						{
							cout << setw(10) << right << "Finished" << " on tick " << setw(3) << right << finishedTick[m]; 
						}
						cout << "\n";
					
					}
					else if(agentY[m] > 5 && agentY[m] < 10 && !map.getPlatform(agentX[m], agentY[m]) && !hasDied[m])
					{
						hasDied[m] = true;
						done[m] = true;
						finishedTick[m] = j;
						score[m] += agentY[m] * 100;
						score[m] -= 1000;
						timesKilled[m]++;
						totalScore[m] += score[m];
						cout << "\nRuin Map " << i + 1 << ", Tick " << j << ":\n";
						map.printRuin(agentX[user], agentY[user]);
						cout << setw(20) << left << agentStr[m] << " (" << setw(2) << right
							<< agentX[m] << "," << setw(2) << right << agentY[m] << ")";
						if(hasDied[m])
						{
							cout << setw(10) << right << "Died" << " on tick " << setw(3) << right << finishedTick[m]; 
						}
						else if(hasFinished[m])
						{
							cout << setw(10) << right << "Finished" << " on tick " << setw(3) << right << finishedTick[m]; 
						}
					}
					if(hasDied[m] || hasFinished[m])
					{
						; 
					}
					if (!done[m])
					{
						applyAction = (*agentFunc[m])(map, agentX[m], agentY[m], false);
						if(applyAction == moveN)
						{
							if(agentY[m] < worldSize - 1)
								agentY[m]++;					
						}
						else if(applyAction == moveW)
						{
							if(agentX[m] > 0)
								agentX[m]--;
						}
						else if(applyAction == moveS)
						{
							if(agentY[m] > 0)
								agentY[m]--;
						}
						else if(applyAction == moveE)
						{
							if(agentX[m] < worldSize - 1)
								agentX[m]++;
						}
						else
						{
							;
						}
					}
					
					if(map.getIdol(agentX[m], agentY[m]) && !gotIdol[m][agentX[m]][agentY[m]])
					{
						idolsCollected[m]++;
						gotIdol[m][agentX[m]][agentY[m]] = true;
					}
					if(agentY[m] > 5 && agentY[m] < 10 && !map.getPlatform(agentX[m], agentY[m]) && !hasDied[m])
					{
						hasDied[m] = true;
						done[m] = true;
						finishedTick[m] = j;
						score[m] += agentY[m] * 100;
						score[m] -= 1000;
						score[m] -= j + 1;
						timesKilled[m]++;
						totalScore[m] += score[m];
						cout << "\nRuin Map " << i + 1 << ", Tick " << j << ":\n";
						map.printRuin(agentX[user], agentY[user]);
						cout << setw(20) << left << agentStr[m] << " (" << setw(2) << right
							<< agentX[m] << "," << setw(2) << right << agentY[m] << ")";
						if(hasDied[m])
						{
							cout << setw(10) << right << "Died" << " on tick " << setw(3) << right << finishedTick[m]; 
						}
						else if(hasFinished[m])
						{
							cout << setw(10) << right << "Finished" << " on tick " << setw(3) << right << finishedTick[m]; 
						}
						cout << "\n";
					}
					else if (agentY[m] == 10 && !hasFinished[m])
					{
						hasFinished[m] = true;
						done[m] = true;
						finishedTick[m] = j;
						timesFinished[m]++;
						score[m] += 1000;
						score[m] -= j + 1;
						totalIdols[m] += idolsCollected[m];
						while(idolsCollected[m] > 0)
						{
							score[m] += 250;
							idolsCollected[m]--;
						}
						totalScore[m] += score[m];
						cout << "\nRuin Map " << i + 1 << ", Tick " << j << ":\n";
						map.printRuin(agentX[user], agentY[user]);
						cout << setw(20) << left << agentStr[m] << " (" << setw(2) << right
							<< agentX[m] << "," << setw(2) << right << agentY[m] << ")";
						if(hasDied[m])
						{
							cout << setw(10) << right << "Died" << " on tick " << setw(3) << right << finishedTick[m]; 
						}
						else if(hasFinished[m])
						{
							cout << setw(10) << right << "Finished" << " on tick " << setw(3) << right << finishedTick[m]; 
						}
						cout << "\n";
					}
					if(map.getPlatform(agentX[m], agentY[m]) && agentY[m] % 2 == 0 && agentX[m] > 0 && !hasDied[m])
					{
						agentX[m] -= 1;
					}
					else if(map.getPlatform(agentX[m], agentY[m]) && agentY[m] % 2 == 1 && agentX[m] < worldSize - 1 && !hasDied[m])
					{
						agentX[m] += 1;
					}
					if(j == actionLimit-1 && !hasDied[m] && !hasFinished[m])
					{
						cavedIn[m] = true;
						timesCavedIn[m]++;
						score[m] -= 2000;
						score[m] -= j + 1;
						totalScore[m] += score[m];
					}
				}
				keepGoing = false; 
				
				for(n = 0; n < numAgents; n++)
				{
					if (!done[n])
						keepGoing = true;
				}
				
				if(!keepGoing)
					break;
				
				map.advanceTick();
			}			
		}
		
		//print out the end summary of the map which was just completed, denoting the point
		//of death or completion for each agent.
		cout << "\nRuin Map " << i + 1 << " End:\n";
		map.printRuin(agentX[user], agentY[user]);
		for (j = 0; j < numAgents; j++)
		{
			cout << setw(20) << left << agentStr[j] << " (" << setw(2) << right
				<< agentX[j] << "," << setw(2) << right << agentY[j] << ")";
			if(hasDied[j])
			{
				cout << setw(10) << right << "Died" << " on tick " << setw(3) << right << finishedTick[j]; 
			}
			else if(hasFinished[j])
			{
				cout << setw(10) << right << "Finished" << " on tick " << setw(3) << right << finishedTick[j]; 
			}
			else if(cavedIn[j])
			{
				cout << setw(10) << right << "Caved In" << " on tick 362";
			}
			cout << "\n";
		}
	}
	
	//using the following score priority, rank agents on performance: total score, idols collected,
	//and number of cave-ins to which the agent fell victim.
	for (i = 0; i < numAgents; i += 1)
    {
		order[i] = i;
    }
	for (i = 0; i < numAgents; i += 1)
    {
        for (j = i + 1; j < numAgents; j += 1)
        {
			if (totalScore[order[i]] < totalScore[order[j]] || totalScore[order[i]] == totalScore[order[j]] &&
             (totalIdols[order[i]] < totalIdols[order[j]] || totalIdols[order[i]] == totalIdols[order[j]] &&
              (timesCavedIn[order[i]] > timesCavedIn[order[j]] || timesCavedIn[order[i]] == timesCavedIn[order[j]] &&
               agentStr[order[i]] > agentStr[order[j]])))
			{
				temp = order[i];
				order[i] = order[j];
				order[j] = temp;
			}
		}
	}
	
	//print out the agents' statistics in order of rank determined above.
    cout << "\n"
         << "Overall standings:           average       idols     times    times      times\n"
         << "                              points   collected   escaped   killed   caved in\n";
    cout << fixed;
    for (i = 0; i < numAgents; i += 1)
    {
        cout << setw(24) << left << agentStr[order[i]]
             << " " << setprecision(5) << setw(11) << right << static_cast<double>(totalScore[order[i]]) / numWorlds
             << " " << setw(11) << right << totalIdols[order[i]]
             << " " << setw(9) << right << timesFinished[order[i]]
             << " " << setw(8) << right << timesKilled[order[i]]
             << " " << setw(10) << right << timesCavedIn[order[i]] << "\n";
    }
    
    return 0;
}
