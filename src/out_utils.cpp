#include "out_utils.h"
#include "global.h"

void outtext(string text){
    out << text;
}

void outtextendl(string text){
    out << text << endl;
}
void mov(string arg1, string arg2){
    out << "mov " << arg1 << ", " << arg2 << endl;
}
