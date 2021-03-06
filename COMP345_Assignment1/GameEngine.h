#pragma once
#include "MapLoader.h"
#include "Map.h"
#include "Cards.h"
#include "GameObservers.h"

using namespace std;

class GameEngine : public Subject
{
public:
	~GameEngine();
	void startGame();
	void startupPhase();
	Map* getMap();
	Deck* getDeck();
	void mainGameLoop(); // void for now
	vector<Player*> getPlayers();
	string getPhase();

private:
	Map* map;
	vector<Player*> players;
	Player* firstPlayer;
	Deck* deck;
	int numOfPlayers;
	bool observerOn;
	string phase;

	void selectMap();
	void createComponents();
	void setObservers();
	void setInitialArmies();
	void setRandomOrder();
	void setRandomTerritory();
	void reinforcementPhase(Player* currPlayer); // void for now
	void issueOrdersPhase(Player* currPlayer);
	void executeOrdersPhase(Player* currPlayer);
	void kickPlayers(); // checks if a Player has lost the game
	Player* checkWinner(); // checks if a Player has won
	void endGamePhase(Player* winner); // launches end game winner message
};