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
            for (auto& s : arg) {
                if (s == "1b") b++; else if (s == "2b") w++;
                else if (s == "4b") dw++; else if (s == "8b") qw++;
            }
            if (b > maxargsB) maxargsB = b; if (w > maxargsW) maxargsW = w;
            if (dw > maxargsDW) maxargsDW = dw; if (qw > maxargsQW) maxargsQW = qw;

            outtextendl(command2 + ":");
        } 
        else if (command == "end") { outtextendl("ret"); }
        else if (command == "argvars") {
            for (int i = 0; i < maxargsB; i++) outtextendl("CBarg" + to_string(i) + " db 0");
            for (int i = 0; i < maxargsW; i++) outtextendl("CWarg" + to_string(i) + " dw 0");
            for (int i = 0; i < maxargsDW; i++) outtextendl("CDWarg" + to_string(i) + " dd 0");
            for (int i = 0; i < maxargsQW; i++) outtextendl("CQWarg" + to_string(i) + " dq 0");
        } 
        else cmf = true;
    }

    if (!func.empty() && cmf) {
        int divider = -1;
        for (int i = 0; i < arg.size(); i++) if (arg[i] == "ARGS") { divider = i; break; }

        if (divider != -1) {
            int b = 0, w = 0, dw = 0, qw = 0;
            for (int i = divider + 1; i < arg.size(); i++) {
                int t_idx = i - (divider + 1);
                if (t_idx < divider) {
                    string type = arg[t_idx];
                    string val = arg[i];
                    if (type == "1b") mov("byte [CBarg" + to_string(b++) + "]", val);
                    else if (type == "2b") mov("word [CWarg" + to_string(w++) + "]", val);
                    else if (type == "4b") mov("dword [CDWarg" + to_string(dw++) + "]", val);
                    else if (type == "8b") mov("qword [CQWarg" + to_string(qw++) + "]", val);
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