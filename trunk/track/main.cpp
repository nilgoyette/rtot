
#include <boost/thread.hpp>

#include<iostream>

using namespace std;

void QuiFaitUnCOUT() {
	cout << "OMG WTF �a FONCTIONE";
}

int maIn() {
	
	boost::thread t(QuiFaitUnCOUT);
	
	char c;
	cin >> c;
	return 0;
}
