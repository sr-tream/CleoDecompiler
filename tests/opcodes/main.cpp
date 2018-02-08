#include <iostream>

#include "../../CleoDecompiler.h"

using namespace std;


int main()
{
	cout << "Start test" << endl;
	CleoDecompiler cd;
	
	auto opcodes = cd.paramsData();
	cout << "Get opcode data" << endl;
	
	for (auto op : opcodes){
		cout << hex << op.first << ":" << endl;
		cout << "\tCount args: " << op.second.count << endl;
		cout << "\tOpcode: " << op.second.opcode << endl;
	}
		
	cout << "End test" << endl;
	return 0;
}
