#include <iostream>

#include "../../../CleoDecompiler.h"

using namespace std;


int main(int argc, char **argv)
{
	cout << "Start test" << endl;
	
	if (argc == 1){
		cout << "Use script name in args" << endl;
		return 1;
	}
	
	CleoDecompiler cd;
	
	if (cd.loadScript(argv[1]))
		cout << "Load script" << endl;
	else {
		cout << "Script not loaded" << endl;
		return 1;
	}
	
	try{
		cout << cd.readOpcode() << endl;
	} catch (std::exception e) {
		cout << e.what() << endl;
	}
		
	cout << "End test" << endl;
	return 0;
}
