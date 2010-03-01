
#include <boost/thread.hpp>
#include <iostream>
#include "Grabber.h"

#ifndef _EiC
#include "cv.h"
//#include "highgui.h"
//#include <stdio.h>
//#include <ctype.h>
//#include "BlobResult.h"
#endif



using namespace std;

boost::condition_variable cond;
boost::mutex mut;
bool ready;

/*
	
*/


/*	void grabber() {
		unsigned int i = 0;
		while (i < 10) {
			// Prendre les données
			volatile float lol = 0;
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
	}*/


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

int main(int argc, char** argv) {
	
	Grabber g(0,320,240);

	boost::thread tG( boost::bind( & Grabber::grabber, &g) );
	boost::thread tT(traitement);

	tG.join();
	tT.join();
	
	cout << "End ... \n";
	char c;
	cin >> c;

	return 0;
}
