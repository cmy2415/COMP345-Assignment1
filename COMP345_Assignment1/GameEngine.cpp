#include <iostream>
#include <string>
#include <ctime>
#include <random>
#include <algorithm>
#include <chrono> 
#include "GameEngine.h"

void GameEngine::startGame()
{
	cout << "Welcome!" << endl; //TODO: change to better message later
	cout << endl;

	selectMap();
	cout << endl;

	createComponents();
	cout << endl;

	setObservers();
	cout << endl;

	//start up phase
	startupPhase();

	/* 
	TODO/FIX:
	(2) the right number of players is created, a deck with the right number of cards is created;
	*/
}

void GameEngine::startupPhase()
{
	setRandomOrder();
	cout << endl;

	setRandomTerritory();
	cout << endl;

	setInitialArmies();
	cout << endl;
}

Map * GameEngine::getMap()
{
	return map;
}

void GameEngine::selectMap()
{
	string dominationMap;
	MapLoader* mapLoader = new MapLoader();
	bool isValid = false;

	do
	{
		cout << "Select the map to play with: ";
		cin >> dominationMap;
		map = mapLoader->GetMap(dominationMap);

		if(map != NULL)
			isValid = map->validate();

		if (map == NULL || !isValid)
		{
			cout << "Map is invalid." << endl;
		}
	} while (map == NULL || !isValid);

	delete mapLoader;
}

void GameEngine::createComponents()
{
	// how many players
	int playernum = 0;
	do
	{
		cout << "Select the number of players (2-5): ";
		cin >> playernum;

		if (playernum < 2 || playernum > 5)
		{
			cout << "Please enter a valid number of players." << endl;
		}
	} while (playernum < 2 || playernum > 5);
	numOfPlayers = playernum; // set number of players

	// create deck and players with set hand
	//TODO:
	//Deck* d;
	//deck = d;

	for (int i = 0; i < playernum; i++)
	{
		Player* p = new Player(); // memory deallocated when calling destructor of GameEngine ?

		// Draw 5 cards from the deck and place it in the player's hand
		for (int i = 0; i < 5; i++)
		{
			//deck->draw(p);
		}

		players.push_back(p);
	}
}

void GameEngine::setObservers()
{
	char answer;
	cout << "Would you like to turn on the observers ? (y/n): ";
	cin >> answer;

	if (answer == 'y')
	{
		observerOn = true;
		cout << "Observers will be on." << endl;
	}
	else if (answer == 'n')
	{
		observerOn = false;
		cout << "Observers will be off." << endl;
	}
}

void GameEngine::setInitialArmies()
{
	// set specific number
	int numOfArmies;
	switch (numOfPlayers)
	{
	case 2:
		numOfArmies = 40;
		break;
	case 3:
		numOfArmies = 35;
		break;
	case 4:
		numOfArmies = 30;
		break;
	case 5:
		numOfArmies = 25;
		break;
	}

	//attach number to all players
	for (int i = 0; i < players.size(); i++)
	{
		players.at(i)->addReinforcements(numOfArmies);
	}
}

// shuffle code from: http://www.cplusplus.com/reference/algorithm/shuffle/
void GameEngine::setRandomOrder()
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(players.begin(), players.end(), std::default_random_engine(seed));

	std::cout << "The playing order: ";
	for (int i = 0; i < numOfPlayers; i++)
	{
		cout << players[i]->getPlayerNumber() << " ";
	}
	cout << endl;
}

void GameEngine::setRandomTerritory()
{
	vector<Territory*> territoriesCopy = map->listOfTerritories;
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	shuffle(territoriesCopy.begin(), territoriesCopy.end(), std::default_random_engine(seed));

	int turn = 0;
	for (int i = 0; i < territoriesCopy.size(); i++)
	{
		players[turn]->addTerritory(territoriesCopy[i]);
		turn++;

		if (turn > (players.size() - 1))
			turn = 0;
	}

	/* FOR DRIVER
	for (int j = 0; j < players.size(); j++)
	{
		cout << "Player " << j << "'s Territories" << endl;
		vector<Territory*> temp = players[j]->getTerritories();
		for (int i = 0; i < players[j]->getTerritories().size(); i++)
		{
			cout << temp[i]->name << endl;
		}
		cout << endl;
	}
	*/
}

// Function that controls the main game
// Loops through 3 phases
void GameEngine::mainGameLoop()
{
	// Reinforcement phase
	cout << "Reinforcement phase:" << endl;
	cout << "====================" << endl;
	for (int i = 0; i < numOfPlayers; i++)
	{
		//cout << "Player " << i << endl;
		reinforcementPhase(players[i]);
	}
	cout << endl;

	// Issuing Orders phase
	cout << "Issuing orders phase:" << endl;
	cout << "=====================" << endl;
	for (int i = 0; i < numOfPlayers; i++)
	{
		issueOrdersPhase(players[i]);
	}
	cout << endl;

	// Orders execution phase

	// check if a Player owns no Territories; if yes, kick them from the game
	
}

// Determines how many armies to add to the input Player's reinforcement pool at the start of each reinforcement phase
void GameEngine::reinforcementPhase(Player* currPlayer)
{
	int newArmies = 3; // minimum number of new armies to assign to Player

	// check if Player owns whole Continent
	int bonusArmies = 0; // if Player gets bonus reinforcements from owning whole Continent
	Continent* currContinent = NULL; // for readability
	for (int i = 0; i < map->listOfContinents.size(); i++ )
	{
		currContinent = map->listOfContinents[i];
		if (currContinent->controlsContinent(currPlayer))
			bonusArmies += currContinent->armyvalue;
	}
	currContinent = NULL;

	// Player gets number of armies equal to their number of Territories / 3, unless this number is less than 3
	if ((currPlayer->getTerritories().size() / 3) < newArmies)
		newArmies = currPlayer->getTerritories().size() / 3;

	cout << "Player " << currPlayer->getPlayerNumber() << " will receive " << newArmies << " new reinforcements " 
		<< "and " << bonusArmies << " bonus reinforcements." << endl;
	currPlayer->addReinforcements(newArmies + bonusArmies); // add armies
}

// Prompts user for Order to be issued and calls issueOrder()
void GameEngine::issueOrdersPhase(Player* currPlayer) {
	
	// issue Deploy orders
	// for simplicity, each Deploy order will deploy all of the Player's reinforcement pool to the first Territory returned by toDefend()
	currPlayer->issueOrder(new Deploy(currPlayer, currPlayer->toDefend()[0], currPlayer->getNumOfArmies()));
	cout << "Player " << currPlayer->getPlayerNumber() << " issued a Deploy order." << endl;

	// issue Advance orders
	// for simplicity, each Advance order will move half the armies from the current Territory to the next Territory
	Territory* toAttack = currPlayer->toAttack()[0]; // first Territory returned by toAttack is the Territory to attack
	Territory* current = NULL; // Territory to move armies from
	for (int i = 0; i < toAttack->listOfAdjTerritories.size(); i++)
	{
		// find the first adjacent Territory of toAttack that the Player owns to take armies from
		if (toAttack->listOfAdjTerritories[i]->owner == currPlayer)
		{
			current = toAttack->listOfAdjTerritories[i];
			break;
		}
	}
	// note: order will still be issued if 0 armies are to be moved (e.g. if numberOfArmies = 1, then numberOfArmies / 2 = 0)
	currPlayer->issueOrder(new Advance(currPlayer, current, toAttack, current->numberOfArmies / 2));
	cout << "Player " << currPlayer->getPlayerNumber() << " issued an Advance order." << endl;

	toAttack = NULL;
	current = NULL;

	// play the first Card in the Player's Hand
	cout << "Player " << currPlayer->getPlayerNumber() << " played a " << currPlayer->getHand()->cardsInHand[0] << "." << endl;
	currPlayer->getHand()->play();
}

void GameEngine::executeOrdersPhase(Player* currPlayer)
{
	// execute deploy orders
	for (int i = 0; i < currPlayer->getOrders().size(); i++)
	{
		if (currPlayer->getOrders()[i]->getType() == "deploy")
			currPlayer->getOrders()[i]->execute();
	}
}