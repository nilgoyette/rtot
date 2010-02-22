
#include <boost/thread.hpp>

#include <iostream>

using namespace std;

boost::condition_variable cond;
boost::mutex mut;
bool ready;

/*
	
*/

void grabber() {
	unsigned int i = 0;
	while (i < 10) {
		// Prendre les données
		volatile int lol = 0;
		for (unsigned int j = 0; j < 50000000; j++) {
			lol = (float)j / j;
			lol += 1.0;
		}
		// yay, c'est long

		{ // Création d'un scope
			boost::lock_guard<boost::mutex> lock(mut);
			ready = true;
		}

		cout << "notify\n";
		cond.notify_one();

		i++;
	}
}

void traitement() {
	unsigned int i = 0;
	while (i < 10) {
		// Attendre les données
		boost::unique_lock<boost::mutex> lock(mut);
		while (!ready) {
			cond.wait(lock);
		}
		ready = false;

		cout << "notified\n";

		i++;
	}
}

int main() {
	
	boost::thread tG(grabber);
	boost::thread tT(traitement);

	tG.join();
	tT.join();
	
	cout << "End ... \n";
	char c;
	cin >> c;

	return 0;
}
