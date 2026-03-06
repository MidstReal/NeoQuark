#include "global.h"

ifstream in;
ofstream out;

string line;
int linenum = 1;

bool mode64 = false;
bool mode32 = false;
bool mode16 = false;
bool mode8 = false;