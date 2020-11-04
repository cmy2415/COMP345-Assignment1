#include "GameObservers.h"
#include "GameEngine.h"
#include <iostream>
#include <list>

using namespace std;

void Subject::attach(Observer* observer) {
	observers.push_back(observer);
}

void Subject::detach(Observer* detach_observer) {
	int count = observers.size();
	int i;

	for (i = 0; i < count; i++) {
		if (observers[i] == detach_observer)
			break;
	}
	if (i < count)
		observers.erase(observers.begin() + i);
}

void Subject::notify() {
	for (Observer* observer : observers) {
		observer->update();
	}
}

void PhaseObserver::update() {
	//currentPhase = GameEngine:: getCurrentPhase();
	//currentPlayer = GameEngine:: getCurrentPlayer();
}

void PhaseObserver::PrintPhaseDetails() {
	
	cout << "Current Phase: " << endl;
	cout << "Current Player: " << endl;
}

void GameStatisticsObserver::update() {

}
