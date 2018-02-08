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
	
	auto cd = new CleoDecompiler();
	
	if (cd->loadScript(argv[1]))
		cout << "Load script" << endl;
	else {
		cout << "Script not loaded" << endl;
		return 1;
	}
	
	try{
		auto txt = cd->decompile();
		for(auto str : txt)
			cout << str << endl;
	} catch (std::exception e) {
		cout << e.what() << endl;
	}
	
	delete cd;
	cout << "End test" << endl;
	return 0;
}
