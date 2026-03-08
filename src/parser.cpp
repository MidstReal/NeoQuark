#include "out_utils.h"
#include "parser.h"
#include "global.h"

using namespace std;

void chkcom() {
    size_t first = line.find_first_not_of(" \t");
    if (first == string::npos) return;
    line = line.substr(first);

    string command = "", command2 = "", command3 = "", command4 = "", func = "", asmstr = "";
    vector<string> arg;
    bool inbkt = false, inqt = false, inasm = false;
    int cmdp = 1;
    int curarg = 0;

    for (int i = 0; i < line.length(); i++) {
        if (line[i] == ';' && !inqt) break;
        if (line[i] == '`' && !inqt) { inasm = !inasm; continue; }
        if (inasm) { asmstr += line[i]; continue; }
        if (line[i] == '"') {
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
            else if(cmdp == 4) command4 += line[i]; 
        } else {
            if (curarg >= arg.size()) arg.push_back("");
            arg[curarg] += line[i];
        }
    }

    if (!asmstr.empty()) outtextendl(asmstr);

    bool cmf = false;
    if (!command.empty()) {
        if (command == "func") outtextendl(command2 + ":");      
        else if(command == "struct")outtextendl("struc " + command2);
        else if(command == "ends")outtextendl("endstruc");
        else if(command == "new"){
            outtextendl(command2 + ":");
            outtextendl("istruc "+arg[0]);
        }
        else if(command == "endn") outtextendl("iend");
        else if(command == "at") outtext("at " + command2 +", ");
        else if (command == "if"){
            string type;

            if (arg.size() >= 5){
                if (arg[4] == "1b"){
                    type = "byte ";
                } else if(arg[4] == "2b"){
                    type = "word ";
                } else if(arg[4] == "4b"){
                    type = "dword ";
                } else if(arg[4] == "8b"){
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
        else if (command == "syscall") outtextendl("syscall");
        else if (command == "$share") outtextendl("global " + arg[0]);
        else if (command == "$include") outtextendl("%include " + arg[0]);
        else if (command == "$bin") outtextendl("incbin " + arg[0]);
        else if (command == "$section") outtextendl("section " + arg[0]);
        else if (command == "$org") outtextendl("org " + arg[0]);
        else if (command == "$bits") outtextendl("bits " + arg[0]);
        else if (command == "$use") outtextendl("extern " + arg[0]);
        else if (command == "#64mode") {mode64 = true; mode32 = false; mode16 = false; mode8 = false;}
        else if (command == "#32mode") {mode32 = true; mode64 = false; mode16 = false; mode8 = false;}
        else if (command == "#16mode") {mode16 = true; mode32 = false; mode64 = false; mode8 = false;}
        else if (command == "#8mode")  {mode8 = true; mode32 = false; mode16 = false; mode64 = false;}
        else if (command == "goto") {
            if (arg.size() >= 2) outtextendl(arg[1] + " " + arg[0]);
            else if (arg.size() >= 1) outtextendl("jmp " + arg[0]);
        }
        else if (command == "int") outtextendl("int " + arg[0]);
        else if (command == "reserve" || command2 == "reserve") {
            if (arg.size() >= 2) {
                if (arg[1] == "1?") outtextendl("resb " + arg[0]);
                else if (arg[1] == "2?") outtextendl("resw " + arg[0]);
                else if (arg[1] == "4?") outtextendl("resd " + arg[0]);
                else if (arg[1] == "8?") outtextendl("resq " + arg[0]);
                else outtextendl("times " + arg[0] + " db " + arg[1]);
            } else if (arg.size() >= 1) outtextendl("resb " + arg[0]);
        }
        else if (command == "align") outtextendl("align " + arg[0]);
        else if (command == "lea") outtextendl("lea " + arg[0] + ", " + arg[1]);

        else if (command2 == "=") mov(command, command3);
        else if (command2 == "b=") mov("byte "+command, command3);
        else if (command2 == "w=") mov("word "+command, command3);
        else if (command2 == "d=") mov("dword "+command, command3);
        else if (command2 == "q=") mov("qword "+command, command3);
        else if (command2 == "+=") outtextendl("add " + command + ", " + command3);
        else if (command2 == "-=") outtextendl("sub " + command + ", " + command3);
        else if (command2 == "*=") outtextendl("imul " + command + ", " + command3);
        else if (command2 == "/=") {
            outtextendl("xor rdx, rdx");
            outtextendl("idiv " + command3);
        }
        else if (command2 == "%=") {
            outtextendl("xor rdx, rdx");
            outtextendl("idiv " + command3);
            outtextendl("mov " + command + ", rdx");
        }

        else if (command2 == "--") outtextendl("dec " + command);
        else if (command2 == "b--") outtextendl("dec byte " + command);
        else if (command2 == "w--") outtextendl("dec word " + command);
        else if (command2 == "d--") outtextendl("dec dword " + command);
        else if (command2 == "q--") outtextendl("dec qword " + command);

        else if (command2 == "++") outtextendl("inc " + command);
        else if (command2 == "b++") outtextendl("inc byte " + command);
        else if (command2 == "w++") outtextendl("inc word " + command);
        else if (command2 == "d++") outtextendl("inc dword " + command);
        else if (command2 == "q++") outtextendl("inc qword " + command);

        else if (command2 == "&=")  outtextendl("and " + command + ", " + command3);
        else if (command2 == "|=")  outtextendl("or " + command + ", " + command3);
        else if (command2 == "^=")  outtextendl("xor " + command + ", " + command3);
        else if (command2 == "~")   outtextendl("not " + command);

        else if (command2 == "<<")  outtextendl("shl " + command + ", " + command3);
        else if (command2 == ">>")  outtextendl("shr " + command + ", " + command3);
        else if (command2 == "sar") outtextendl("sar " + command + ", " + command3);
        else if (command2 == "rol") outtextendl("rol " + command + ", " + command3);
        else if (command2 == "ror") outtextendl("ror " + command + ", " + command3);

        else if (command2 == "??")  outtextendl("test " + command + ", " + command3);

        else if (command == "<-") outtextendl("push " + command2);
        else if (command == "->") outtextendl("pop " + command2);
        else if (command == "<-<") outtextendl("pusha");
        else if (command == ">->") outtextendl("popa");

        else if (command2 == "<=>") outtextendl("xchg " + command + ", " + command3);

        else if (command == "byte" || command == "char") outtextendl(command2 + " db " + line.substr(line.find('=') + 1));
        else if (command == "short")  outtextendl(command2 + " dw " + line.substr(line.find('=') + 1));
        else if (command == "int")    outtextendl(command2 + " dd " + line.substr(line.find('=') + 1));
        else if (command == "bigint") outtextendl(command2 + " dq " + line.substr(line.find('=') + 1));
        else if (command == "const")  outtextendl(command2 + " equ " + line.substr(line.find('=') + 1));


        else if (command == "<1=" || command == "<b=") outtextendl("db " + line.substr(3, line.length()-3));
        else if (command == "<2=" || command == "<w=") outtextendl("dw " + line.substr(3, line.length()-3));
        else if (command == "<4=" || command == "<d=") outtextendl("dd " + line.substr(3, line.length()-3));
        else if (command == "<8=" || command == "<q=") outtextendl("dq " + line.substr(3, line.length()-3));
        else if (command == "<c=") outtextendl("equ " + line.substr(3, line.length()-3));
        else cmf = true;
    }

    if (!func.empty() && cmf) {
        string regs64[] = {"rax","rcx","rdx","rbx","rsi","rdi"};
        string regs32[] = {"eax","ecx","edx","ebx","esi","edi"};
        string regs16[] = {"ax","cx","dx","bx","si","di"};
        string regs8[] = {"al","cl","dl","bl","sil","dil"};

        if(mode64) {
            int count = min((int)arg.size(), 6);
            
            for (int i = 0;i<count;i++) outtextendl("push " + regs64[i]);

            for (int i = 0;i<count;i++) {
                if(arg[i][0] == '|') outtextendl("movzx " + regs64[i]+", "+ arg[i].substr(1,arg[i].length()-1));
                else mov(regs64[i], arg[i]);
            }
            outtextendl("call " + func);
            
            for (int i = count-1;i>=0;i--) outtextendl("pop " + regs64[i]); 
        }
        else if(mode32) {
            int count = min((int)arg.size(), 6);
            
            for (int i = 0;i<count;i++) outtextendl("push " + regs32[i]);
            for (int i = 0;i<count;i++) {
                if(arg[i][0] == '|') outtextendl("movzx " + regs32[i]+", "+ arg[i].substr(1,arg[i].length()-1));
                else mov(regs32[i], arg[i]);
            }
            
            outtextendl("call " + func);
            
            for (int i = count-1;i>=0;i--) outtextendl("pop " + regs32[i]);
        }
        else if(mode16) {
            int count = min((int)arg.size(), 6);
            
            for (int i = 0;i<count;i++) outtextendl("push " + regs16[i]);
            for (int i = 0;i<count;i++) {
                if(arg[i][0] == '|') outtextendl("movzx " + regs16[i]+", "+ arg[i].substr(1,arg[i].length()-1));
                else mov(regs16[i], arg[i]);
            }
            
            outtextendl("call " + func);
            
            for (int i = count-1;i>=0;i--) outtextendl("pop " + regs16[i]);
        }
        else if(mode8) {
            int count = min((int)arg.size(), 6);
            
            for (int i = 0;i<count;i++) outtextendl("push " + regs16[i]);
            for (int i = 0;i<count;i++) {
                if(arg[i][0] == '|') outtextendl("movzx " + regs8[i]+", "+ arg[i].substr(1,arg[i].length()-1));
                else mov(regs8[i], arg[i]);
            }
            
            outtextendl("call " + func);
            
            for (int i = count-1;i>=0;i--) outtextendl("pop " + regs16[i]);
        }
    }
}