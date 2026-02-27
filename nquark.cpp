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

    string command = "", command2 = "", func = "", asmstr = "";
    vector<string> arg;
    bool inbkt = false, inqt = false, inasm = false, cmdp = true;
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
        if (line[i] == '(') { inbkt = true; cmdp = false; continue; }
        if (line[i] == ')') { inbkt = false; continue; }
        if (line[i] == ',') { curarg++; continue; }
        if (line[i] == ' ' || line[i] == '\t') {
            if (!inbkt && !command.empty()) cmdp = false;
            continue;
        }
        if (!inbkt) {
            if (cmdp) { command += line[i]; func += line[i]; }
            else command2 += line[i]; 
        } else {
            if (curarg >= arg.size()) arg.push_back("");
            arg[curarg] += line[i];
        }
    }

    if (!asmstr.empty()) outtextendl(asmstr);

    bool cmf = false;
    if (!command.empty()) {
        if (command == "func") {
            int b = 0, w = 0, dw = 0, qw = 0;
            for (auto& carg : arg) {
                if (carg == "1b") b++; 
                else if (carg == "2b") w++;
                else if (carg == "4b") dw++; 
                else if (carg == "8b") qw++;
            }
            if (b > maxargsB) maxargsB = b; 
            if (w > maxargsW) maxargsW = w;
            if (dw > maxargsDW) maxargsDW = dw; 
            if (qw > maxargsQW) maxargsQW = qw;

            outtextendl(command2 + ":");
        } 
        else if (command == "if"){
            string type;

            if (arg[4] == "BYTE"){
                type = "byte ";
            } else if(arg[4] == "WORD"){
                type = "word ";
            } else if(arg[4] == "DWORD"){
                type = "dword ";
            } else if(arg[4] == "QWORD"){
                type = "qword ";
            }

            outtextendl("cmp "+ type + arg[0] + ", " + arg[2]);


            if (arg[1] == "!=") out << "jne " << arg[3] << endl;
            else if (arg[1] == "==") out << "je " << arg[3] << endl;
            else if (arg[1] == "<=")  out << "jbe " << arg[3] << endl;
            else if (arg[1] == ">=")  out << "jae " << arg[3] << endl;
            else if (arg[1] == "<")  out << "jb " << arg[3] << endl;
            else if (arg[1] == ">")  out << "ja " << arg[3] << endl;
        }
        else if (command == "end") outtextendl("ret");
        else if (command == ";") {}
        else if (command == "$share") outtextendl("global " + arg[0]);
        else if (command == "$include") outtextendl("%include " + arg[0]);
        else if (command == "goto") outtextendl("jmp " + arg[0]);
        else if (command == "argvars") {
            for (int i = 0; i < maxargsB; i++) outtextendl("CBarg" + to_string(i) + " db 0");
            for (int i = 0; i < maxargsW; i++) outtextendl("CWarg" + to_string(i) + " dw 0");
            for (int i = 0; i < maxargsDW; i++) outtextendl("CDWarg" + to_string(i) + " dd 0");
            for (int i = 0; i < maxargsQW; i++) outtextendl("CQWarg" + to_string(i) + " dq 0");
        } 
        else cmf = true;
    }

    if (!func.empty() && cmf) {
        int div = -1;
        for (int i = 0; i < arg.size(); i++) if (arg[i] == "ARGS") { div = i; break; }

        if (div != -1) {
            int b = 0, w = 0, dw = 0, qw = 0;
            for (int i = div + 1; i < arg.size(); i++) {
                int tpos = i - (div + 1);
                if (tpos < div) {
                    string type = arg[tpos];
                    string val = arg[i];
                    char vt1 = type[0];
                    char vt3 = type[2];
                    string final = "";
                    bool reg = false;

                    if(vt1 == '1') final+="[CBarg" + to_string(b++) + "]";
                    else if(vt1 == '2') final+="[CWarg" + to_string(w++) + "]";
                    else if(vt1 == '4') final+="[CDWarg" + to_string(dw++) + "]";
                    else if(vt1 == '8') final+="[CQWarg" + to_string(qw++) + "]";
                    else if(vt1 == '$') {
                        final+=type.substr(1, type.length()-1);
                        reg = true;
                    }
                    if(!reg){
                        if (vt3 == '1') mov("byte " + final, val);
                        else if (vt3== '2') mov("word " + final, val);
                        else if (vt3 == '4') mov("dword " + final, val);
                        else if (vt3 == '8') mov("qword " + final, val);
                    }
                    else mov(final, val);
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
