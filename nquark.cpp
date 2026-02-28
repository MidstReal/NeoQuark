#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

ifstream in;
ofstream out;

string line;
int linenum = 1;

int maxargsB = 0;
int maxargsW = 0;
int maxargsDW = 0;
int maxargsQW = 0;

void outtext(string text){
    out << text;
}

void outtextendl(string text){
    out << text << endl;
}
void mov(string arg1, string arg2){
    out << "mov " << arg1 << ", " << arg2 << endl;
}

void chkcom() {
    size_t first = line.find_first_not_of(" \t");
    if (first == string::npos) return;
    line = line.substr(first);

    string command = "", command2 = "", command3 = "", func = "", asmstr = "";
    vector<string> arg;
    bool inbkt = false, inqt = false, inasm = false;
    int cmdp = 1;
    int curarg = 0;

    for (int i = 0; i < line.length(); i++) {
        if (line[i] == '`' && !inqt) { inasm = !inasm; continue; }
        if (inasm) { asmstr += line[i]; continue; }
        if (line[i] == '"' || line[i] == '\'') {
            inqt = !inqt;
            if (inbkt) {
                if (curarg >= arg.size()) arg.push_back("");
                arg[curarg] += line[i];
            }
            continue;
        }
        if (inqt) {
            if (curarg >= arg.size()) arg.push_back("");
            arg[curarg] += line[i];
            continue;
        }
        if (line[i] == '(') { inbkt = true; cmdp = 0; continue; }
        if (line[i] == ')') { inbkt = false; continue; }
        if (line[i] == ',') { curarg++; continue; }
        if (line[i] == ' ' || line[i] == '\t') {
            if (!inqt && !inbkt) cmdp++;
            continue;
        }
        if (!inbkt) {
            if (cmdp == 1) { command += line[i]; func += line[i]; }
            else if(cmdp == 2) command2 += line[i]; 
            else if(cmdp == 3) command3 += line[i]; 
        } else {
            if (curarg >= arg.size()) arg.push_back("");
            arg[curarg] += line[i];
        }
    }

    if (!asmstr.empty()) outtextendl(asmstr);

    bool cmf = false;
    if (!command.empty()) {
        if (command == "func") {
            outtextendl(command2 + ":");

            int div = -1;
            for (int i = 0; i < arg.size(); i++) if (arg[i] == "|") { div = i; break; }

            if (div != -1) {
                int b = 0, w = 0, dw = 0, qw = 0;
                for (int i = div + 1; i < arg.size(); i++) {
                    int tpos = i - (div + 1);
                    if (tpos < div) {
                        string type = arg[tpos];
                        string val = arg[i];
                        char vt1 = type[0];
                        string final = "";
                        bool reg = false;

                        final+=type.substr(1, type.length()-1);

                        if (vt1 == 'B') mov("byte " + final, val);
                        else if (vt1== 'W') mov("word " + final, val);
                        else if (vt1 == 'D') mov("dword " + final, val);
                        else if (vt1 == 'Q') mov("qword " + final, val);
                        else if (vt1 == '$') mov(final, val);
                    }
                }
            }
            
        } 
        else if (command == "if"){
            string type;

            if (arg.size() >= 5){
                if (arg[4] == "BYTE"){
                    type = "byte ";
                } else if(arg[4] == "WORD"){
                    type = "word ";
                } else if(arg[4] == "DWORD"){
                    type = "dword ";
                } else if(arg[4] == "QWORD"){
                    type = "qword ";
                }
            }


            outtextendl("cmp "+ type + arg[0] + ", " + arg[2]);


            if (arg[1] == "!=") out << "jne " << arg[3] << endl;
            else if (arg[1] == "==") out << "je " << arg[3] << endl;
            else if (arg[1] == "<=")  out << "jbe " << arg[3] << endl;
            else if (arg[1] == ">=")  out << "jae " << arg[3] << endl;
            else if (arg[1] == "<")  out << "jb " << arg[3] << endl;
            else if (arg[1] == ">")  out << "ja " << arg[3] << endl;
        }
        else if (command[command.length()-1] == ':') outtextendl(command);
        else if (command == "end") outtextendl("ret");
        else if (command == ";") {}
        else if (command == "$share") outtextendl("global " + arg[0]);
        else if (command == "$include") outtextendl("%include " + arg[0]);
        else if (command == "$section") outtextendl("section " + arg[0]);
        else if (command == "goto") {
            if (arg.size() >= 2) outtextendl(arg[1] + " " + arg[0]);
            else if (arg.size() >= 1) outtextendl("jmp " + arg[0]);
        }
        else if (command == "int") outtextendl("int " + arg[0]);
        else if (command2 == "=") mov(command, command3);
        else if (command2 == "b=") mov("byte "+command, command3);
        else if (command2 == "+=") outtextendl("add " + command + ", " + command3);
        else if (command2 == "-=") outtextendl("sub " + command + ", " + command3);
        else if (command2 == "--") outtextendl("dec " + command);
        else if (command2 == "++") outtextendl("inc " + command);
        else if (command2 == "1=") outtextendl(command + " db " + line.substr(command.length()+4, line.length()-(command.length()+4)));
        else if (command2 == "2=") outtextendl(command + " dw " + line.substr(command.length()+4, line.length()-(command.length()+4)));
        else if (command2 == "4=") outtextendl(command + " dd " + line.substr(command.length()+4, line.length()-(command.length()+4)));
        else if (command2 == "8=") outtextendl(command + " dq " + line.substr(command.length()+4, line.length()-(command.length()+4)));

        else if (command == "1=") outtextendl("db " + line.substr(3, line.length()-3));
        else if (command == "2=") outtextendl("dw " + line.substr(3, line.length()-3));
        else if (command == "4=") outtextendl("dd " + line.substr(3, line.length()-3));
        else if (command == "8=") outtextendl("dq " + line.substr(3, line.length()-3));
        else cmf = true;
    }

    if (!func.empty() && cmf) {
        int div = -1;
        for (int i = 0; i < arg.size(); i++) if (arg[i] == "|") { div = i; break; }

        if (div != -1) {
            int b = 0, w = 0, dw = 0, qw = 0;
            for (int i = div + 1; i < arg.size(); i++) {
                int tpos = i - (div + 1);
                if (tpos < div) {
                    string type = arg[tpos];
                    string val = arg[i];
                    char vt1 = type[0];
                    string final = "";
                    bool reg = false;

                    final+=type.substr(1, type.length()-1);

                    if (vt1 == 'B') mov("byte " + final, val);
                    else if (vt1== 'W') mov("word " + final, val);
                    else if (vt1 == 'D') mov("dword " + final, val);
                    else if (vt1 == 'Q') mov("qword " + final, val);
                    else if (vt1 == '$') mov(final, val);
                }
            }
        }
        outtextendl("call " + func);
    }
}
int main(int argc, char* argv[]){
    string inputFile = argv[1];
    string outputFile = argv[2];

    if (argc >= 4 && string(argv[2]) == "-o") {
        outputFile = argv[3];
    } else if (argc >= 3) {
        outputFile = argv[2];
    }

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
