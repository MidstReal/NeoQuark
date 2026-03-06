#include "global.h"
#include "parser.h"

using namespace std;

int main(int argc, char* argv[]){
    linenum = 1;
    mode64 = false;
    mode32 = false;
    mode16 = false;
    mode8 = false;

    string inputFile = argv[1];
    string outputFile = argv[2];

    if (argc >= 5 && string(argv[2]) == "-o") {
        outputFile = argv[3];
    } else if (argc >= 3) {
        outputFile = argv[2];
    }

    if (argc >= 5 && string(argv[4]) == "64bits"){mode64 = true;}
    else if (argc >= 5 && string(argv[4]) == "32bits"){mode32 = true;}
    else if (argc >= 5 && string(argv[4]) == "16bits"){mode16 = true;}
    else if (argc >= 5 && string(argv[4]) == "8bits"){mode8 = true;}
    else {mode16 = true;}

    in.open(inputFile);
    out.open(outputFile);


    if (in.is_open() && out.is_open())
    {
        while (getline(in, line))
        {
            if(line.empty()) {
                linenum++;
                continue;
            }
            chkcom();
            linenum++;
        }
        
    }
    

    in.close();
    out.close();


}