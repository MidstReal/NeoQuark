#include "out_utils.h"
#include "parser.h"
#include "global.h"

using namespace std;

void funcCall(const string* regs, int count, const vector<string>& arg, const string& func) {
    if (line[line.find('(') - 1] == '&') {
        for(int i = 0; i < arg.size(); i++) outtextendl("push " + arg[i]);

        outtextendl("call " + func.substr(0, func.length() - 1));

        int wordSize = mode64 ? 8 : (mode32 ? 4 : 2);
        string offset = to_string(arg.size() * wordSize);

        if(mode64) outtextendl("add rsp, " + offset);
        else if(mode32) outtextendl("add esp, " + offset);
        else if(mode16 || mode8) outtextendl("add sp, " + offset);
    }
    else{

        if (!mode8) {
            for (int i = 0; i < count; i++) if(arg[i][0] != '$') outtextendl("push " + regs[i]);
        }

        for (int i = 0; i < count; i++) {
            string a = (arg[i][0] == '$') ? arg[i].substr(1) : arg[i];
            if(a[0] == '|') outtextendl("movzx " + regs[i] + ", " + a.substr(1));
            else if(a[0] == '/') outtextendl("lea " + regs[i] + ", " + a.substr(1));
            else mov(regs[i], a);
        }
        outtextendl("call " + func);
        
        if (!mode8) {
            for (int i = count - 1; i >= 0; i--) if(arg[i][0] != '$') outtextendl("pop " + regs[i]);
        }

    }
}

void chkcom() {
    size_t first = line.find_first_not_of(" \t");
    if (first == string::npos) return;
    line = line.substr(first);
    if(line.find(';') == string::npos && dbg) {outtextendl("");outtextendl("");outtextendl(";-------" + line + "-------");}

    string command = "", command2 = "", command3 = "", command4 = "", func = "", asmstr = "";
    vector<string> arg;
    bool inbkt = false, inqt = false, inasm = false, insqbkt = false;
    int cmdp = 1;
    int curarg = 0;
    string md = "";


    for (int i = 0; i < line.length(); i++) {
        if (line[i] == ';' && !inqt && !insqbkt) break;
        if (line[i] == '`' && !inqt) { inasm = !inasm; continue; }
        if (inasm) { asmstr += line[i]; continue; }
        if (line[i] == '"') {
            inqt = !inqt;
            if (inbkt) {
                if (curarg >= arg.size()) arg.push_back("");
                arg[curarg] += line[i];
            } else if (insqbkt) {
                if (cmdp == 1) command += line[i];
                else if(cmdp == 2) command2 += line[i];
                else if(cmdp == 3) command3 += line[i];
                else if(cmdp == 4) command4 += line[i];
            }
            continue;
        }
        if (inqt) {
            if (inbkt) {
                if (curarg >= arg.size()) arg.push_back("");
                arg[curarg] += line[i];
            } else if (insqbkt) {
                if (cmdp == 1) command += line[i];
                else if(cmdp == 2) command2 += line[i];
                else if(cmdp == 3) command3 += line[i];
                else if(cmdp == 4) command4 += line[i];
            }
            continue;
        }
        if (line[i] == '[' && !inbkt) { insqbkt = true; }
        if (line[i] == ']' && !inbkt) { insqbkt = false; }
        if (line[i] == '(') { inbkt = true; cmdp = 0; continue; }
        if (line[i] == ')') { inbkt = false; continue; }
        if (line[i] == ',') { 
            if (inbkt) curarg++; 
            continue; 
        }
        if ((line[i] == ' ' || line[i] == '\t') && !insqbkt) {
            if (!inqt && !inbkt) cmdp++;
            if (!inbkt) continue;
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

    for (auto& a : arg) {
        size_t start = a.find_first_not_of(" \t");
        if (start != string::npos) {
            a = a.substr(start);
        } else {
            a = "";
        }
    }

    if (!asmstr.empty()) outtextendl(asmstr);
    string aft = line.substr(line.find('=') + 1);
    int aftpos = line.find('=');

    if(line[aftpos + 1] == 'b' || line[aftpos - 1] == 'b') md = "byte ";
    else if(line[aftpos + 1] == 'w' || line[aftpos - 1] == 'w') md = "word ";
    else if(line[aftpos + 1] == 'd' || line[aftpos - 1] == 'd') md = "dword ";
    else if(line[aftpos + 1] == 'q' || line[aftpos - 1] == 'q') md = "qword ";

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
        
        else if (command == "cmp") outtextendl("cmp " + arg[0] + ", " + arg[1]);
        else if (command == "if"){
            string type;

            if (arg.size() >= 5){
                if (arg[4] == "b"){
                    type = "byte ";
                } else if(arg[4] == "w"){
                    type = "word ";
                } else if(arg[4] == "d"){
                    type = "dword ";
                } else if(arg[4] == "q"){
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
        else if (command == "hsf") {
            if(mode64) {outtextendl("push rbp");outtextendl("mov rbp, rsp");}
            else if(mode32) {outtextendl("push ebp");outtextendl("mov ebp, esp");}
            else if(mode16 || mode8) {outtextendl("push bp");outtextendl("mov bp, sp");}
        }
        else if (command == "endsf") {
            if(mode64) {outtextendl("pop rbp");}
            else if(mode32) {outtextendl("pop ebp");}
            else if(mode16 || mode8) {outtextendl("pop bp");}
            outtextendl("ret");
        }
        else if (command == "end") outtextendl("ret");
        else if (command == "syscall") {if(arg.size() <= 0) outtextendl("syscall"); else {mov("rax", arg[0]);outtextendl("syscall");}}
        else if (command == "$share") outtextendl("global " + arg[0]);
        else if (command == "$include") outtextendl("%include " + arg[0]);
        else if (command == "$define") outtextendl("%define " + arg[0]);
        else if (command == "$macro") outtextendl("%macro " + command2 + " " + arg[0]);
        else if (command == "$endmacro") outtextendl("%endmacro");
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
        else if (command == "in") outtextendl("in " + arg[0] + ", " + arg[1]);
        else if (command == "out") outtextendl("out " + arg[0] + ", " + arg[1]);
        else if (command == "int") outtextendl("int " + arg[0]);
        else if (command == "rep") outtextendl("rep " + arg[0]);
        else if (command == "repe" || command == "repz") outtextendl("repe " + command2);
        else if (command == "repne" || command == "repnz") outtextendl("repne " + command2);
        else if (command == "lock") outtextendl("lock " + command2);
        else if (command == "nop") outtextendl("nop");
        else if (command == "hlt") outtextendl("hlt");
        else if (command == "cli") outtextendl("cli");
        else if (command == "sti") outtextendl("sti");
        else if (command == "leave") outtextendl("leave");
        else if (command == "reserve") {
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

        else if (command2 == "&=")  outtextendl("and " + command + ", " + command3);
        else if (command2 == "|=")  outtextendl("or " + command + ", " + command3);
        else if (command2 == "^=")  outtextendl("xor " + command + ", " + command3);
        else if (command2 == "~")   outtextendl("not " + command);

        else if (command == "<1=" || command == "<b=") outtextendl("db " + line.substr(3, line.length()-3));
        else if (command == "<2=" || command == "<w=") outtextendl("dw " + line.substr(3, line.length()-3));
        else if (command == "<4=" || command == "<d=") outtextendl("dd " + line.substr(3, line.length()-3));
        else if (command == "<8=" || command == "<q=") outtextendl("dq " + line.substr(3, line.length()-3));
        else if (command == "<c=") outtextendl("equ " + line.substr(3, line.length()-3));

        else if (command2 == "<=>") outtextendl("xchg " + command + ", " + command3);

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

        else if (command2 == "<<")  outtextendl("shl " + command + ", " + command3);
        else if (command2 == ">>")  outtextendl("shr " + command + ", " + command3);
        else if (command2 == "sar") outtextendl("sar " + command + ", " + command3);
        else if (command2 == "rol") outtextendl("rol " + command + ", " + command3);
        else if (command2 == "ror") outtextendl("ror " + command + ", " + command3);

        else if (command2 == "??")  outtextendl("test " + command + ", " + command3);

        else if (command == "<-") outtextendl("push " + command2);
        else if (command == "<-b") outtextendl("push byte " + command2);
        else if (command == "<-w") outtextendl("push word " + command2);
        else if (command == "<-d") outtextendl("push dword " + command2);
        else if (command == "<-q") outtextendl("push qword " + command2);

        else if (command == "->") outtextendl("pop " + command2);
        else if (command == "b->") outtextendl("pop byte " + command2);
        else if (command == "w->") outtextendl("pop word " + command2);
        else if (command == "d->") outtextendl("pop dword " + command2);
        else if (command == "q->") outtextendl("pop qword " + command2);

        else if (command == "<-<") outtextendl("pusha");
        else if (command == ">->") outtextendl("popa");

        else if (command == "byte" || command == "char") outtextendl(command2 + " db " + aft);
        else if (command == "short")  outtextendl(command2 + " dw " + aft);
        else if (command == "int")    outtextendl(command2 + " dd " + aft);
        else if (command == "bigint") outtextendl(command2 + " dq " + aft);
        else if (command == "const")  outtextendl(command2 + " equ " + aft);

        else if (command == "res"){
            if(command2 == "byte") outtextendl(command3 + " resb " + aft);
            else if(command2 == "short") outtextendl(command3 + " resw " + aft);
            else if(command2 == "int") outtextendl(command3 + " resd " + aft);
            else if(command2 == "bigint") outtextendl(command3 + " resq " + aft);
        }
        
        else if (line.find('=') != string::npos){
            if (line[aftpos-1] == 'r') {for(int i = 1;i<aft.length(); i++) {
                if (aft[i] == '"') continue;
                outtextendl("mov byte [" + command + "+" + to_string(i-2) + "]" ", '" + aft[i] + "'");
            }; outtextendl("mov byte [" + command + "+" + to_string(aft.length()-2) + "]" + ", 0");}
            else if (line[aftpos+1] == '0') outtextendl("movzx "+ md + command + ", " + command3);
            
            else if (line[aftpos-1] == '+') outtextendl("add "+ md + command + ", " + command3);
            else if (line[aftpos-1] == '-') outtextendl("sub "+ md + command + ", " + command3);
            else if (line[aftpos-1] == '*' &&  line[aftpos+1] == '|') outtextendl("mul "+ md + command + ", " + command3);
            else if (line[aftpos-1] == '/' &&  line[aftpos+1] == '|') outtextendl("div "+ md + command);
            else if (line[aftpos-1] == '*') outtextendl("imul "+ md + command + ", " + command3);
            else if (line[aftpos-1] == '/') {
                outtextendl("xor rdx, rdx");
                outtextendl("idiv "+ md + command3);
            }
            else if (line[aftpos-1] == '%') {
                outtextendl("xor rdx, rdx");
                outtextendl("idiv "+ md + command3);
                outtextendl("mov " + command + ", rdx");
            }

            else mov(md+command, command3);
        }


        else cmf = true;
    }

    if (!func.empty() && cmf) {
        string regs64[] = {"rax","rcx","rdx","rbx","rsi","rdi","r8","r9","r10","r11","r12","r13","r14","r15"};
        string regs32[] = {"eax","ecx","edx","ebx","esi","edi"};
        string regs16[] = {"ax","cx","dx","bx","si","di","bp","sp"};
        string regs8[]  = {"al","cl","dl","bl","ah","ch","dh","bh"};

        int count = min((int)arg.size(), 14);
        
        if(mode64) funcCall(regs64, count, arg, func);
        else if(mode32) funcCall(regs32, count, arg, func);
        else if(mode16) funcCall(regs16, count, arg, func);
        else if(mode8) funcCall(regs8, count, arg, func);
    }

    if(line.find(';') == string::npos && dbg){outtextendl(";----------------------------");}
}
