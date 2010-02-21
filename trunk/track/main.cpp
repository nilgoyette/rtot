
#include <boost/thread.hpp>

#include<iostream>

using namespace std;

void QuiFaitUnCOUT() {
	cout << "OMG WTF Ça FONCTIONE";
}

int maIn() {
	
	boost::thread t(QuiFaitUnCOUT);
	
	char c;
	cin >> c;
	return 0;
}
