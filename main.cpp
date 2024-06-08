#include <bits/stdc++.h>
using namespace std;

string DecodingString(string s){
    int val = BinToDic(s);
    string a = "";
    switch(val){
        case 0 : a = "ADD"; break;
        case 1 : a = "SUB"; break;
        case 2 : a = "MUL"; break;
        case 3 : a = "INC"; break;
        case 4 : a = "AND"; break;
        case 5 : a = "OR"; break;
        case 6 : a = "XOR"; break;
        case 7 : a = "NOT"; break;
        case 8 : a = "SLLI"; break;
        case 9 : a = "SRLI"; break;
        case 10 : a = "LI"; break;
        case 11 : a = "LD"; break;
        case 12 : a = "ST"; break;
        case 13 : a = "JMP"; break;
        case 14 : a = "BEQZ"; break;
        case 15 : a = "HLT"; break;
    }
    return a;
}


int immediate(int a){
    if (a/(1<<7) == 1){
        return ((a-1)^255)*-1;
    }
    else return a;
}


int BinToDic (string s){
    int a = 0;
    for(int i=3;i>=0;i--){
        if (s[3-i] == '1') a += (1 << i); 
    }
    return a;
}

string DicToHex(int a){
    string s= "";
    for(int i = 1;i<=2;i++){
        int a_ = a%16;
        switch (a_){
            case 0 : s = '0' + s; break;
            case 1 : s = '1' + s; break;
            case 2 : s = '2' + s; break;
            case 3 : s = '3' + s; break;
            case 4 : s = '4' + s; break;
            case 5 : s = '5' + s; break;
            case 6 : s = '6' + s; break;
            case 7 : s = '7' + s; break;
            case 8 : s = '8' + s; break;
            case 9 : s = '9' + s; break;
            case 10 : s = 'a' + s; break;
            case 11 : s = 'b' + s; break;
            case 12 : s = 'c' + s; break;
            case 13 : s = 'd' + s; break;
            case 14 : s = 'e' + s; break;
            case 15 : s = 'f' + s; break;
        }
        a = a/16;
    }
    return s;
}

string StringToBinary(string s){
    string a = "";
    for(int i=0;i<2;i++){
        switch (s[i]){
            case '0': a += "0000"; break;
            case '1': a += "0001"; break;
            case '2': a += "0010"; break;
            case '3': a += "0011"; break;
            case '4': a += "0100"; break;
            case '5': a += "0101"; break;
            case '6': a += "0110"; break;
            case '7': a += "0111"; break;
            case '8': a += "1000"; break;
            case '9': a += "1001"; break;
            case 'a': a += "1010"; break;
            case 'b': a += "1011"; break;
            case 'c': a += "1100"; break;
            case 'd': a += "1101"; break;
            case 'e': a += "1110"; break;
            case 'f': a += "1111"; break;
        }
    }
    return a;
}



int main(){
    ifstream ifile ("./input/ICache.txt");
    ifstream dfile ("./input/DCache.txt");
    ifstream rfile ("./input/RF.txt");
    ofstream d_out_file("./output/DCache.txt",ios::out);
    ofstream ofile ("./output/Output.txt",ios::out);

    string inst;
    string instructions = "";
    while(ifile >> inst){
        instructions += StringToBinary(inst); 
    }
    
    string data;
    vector<int> data_values;
    while(dfile >> data){
        data_values.push_back(BinToDic(StringToBinary(data)));
    }

    string RegisterValues;
    vector<int> register_values;
    while(rfile >> RegisterValues){
        register_values.push_back(BinToDic(StringToBinary(RegisterValues)));
    }

    int pc = 0;

    string IR;

    int NoofInstruction = 0;
    int arithmatic = 0;
    int log = 0;
    int shift = 0;
    int mem = 0;
    int cont = 0;
    int hal = 0;
    int Hazard = 0;
    int hazard_control= 0;
    int load_imm = 0;

    unordered_set<int> register_mark;

    vector<int> CountOfRegister(16,0);

    while(pc <= 510 && instructions.substr(8*pc,16) != "0000000000000000" ){
        NoofInstruction++;
        // Instruction Fetch cycle (IF)
        IR = instructions.substr(8*pc,16);
        pc += 2;


        string opcode = DecodingString(IR.substr(0,4));

        int operator_value = BinToDic(IR.substr(0,4)); 
        int dest_reg = BinToDic(IR.substr(4,4));
        int source_reg_1 = BinToDic(IR.substr(8,4));
        int source_reg_2 = BinToDic(IR.substr(12,4));

       
        bool check = true;
        if(operator_value != 3 && operator_value != 10 && operator_value != 13 && operator_value != 15){
        
            for(auto i : register_mark){
            
                if(operator_value != 7 && operator_value != 11 && operator_value != 12 && operator_value != 14){
                    if (i == source_reg_1 || i == source_reg_2) {
                        check = false;
                        
                        Hazard++;
                    }
                }
                else{
                    if(i == source_reg_1){
                      
                        pc = pc-2;
                        Hazard++;
                    }
                }
            }
        }

        vector<bool> bool_set(16,false);
        for(auto i : register_mark){
           
            CountOfRegister[i]++;
            if(CountOfRegister[i] == 3) {
                bool_set[i] = true;
                CountOfRegister[i] = 0;
            }
        }

        for(int i = 0;i<16;i++){
            if(bool_set[i]) {   
                register_mark.erase(i);
                bool_set[i] = false;
            }
        }

        int A = register_values[source_reg_1];
        int B = register_values[source_reg_2];

        if(check){
            
            register_mark.insert(dest_reg);
            CountOfRegister[dest_reg]++;
            if(CountOfRegister[dest_reg]==3) CountOfRegister[dest_reg] = 1;
        }


        // Execution cycle

        int ALU_0utput;

        //Register - Register operation
        if (operator_value < 10){
            switch (operator_value){
                case 0 :{
                     ALU_0utput = (A + B) % (1 << 8); 
                     break;
                }   
                case 1 :{
                     ALU_0utput = (A - B) % (1 << 8); 
                     break;
                }
                case 2 :{
                     ALU_0utput = (A * B) % (1 << 8);
                     break;
                }
                case 3 : {
                    ALU_0utput = register_values[dest_reg] + 1; 
                    break;
                }
                case 4 :{ 
                    ALU_0utput = A & B; 
                    break;
                }
                case 5 : {
                    ALU_0utput = A | B;
                    break;
                }
                case 6 :{ 
                    ALU_0utput = A ^ B; 
                    break;
                }
                case 7 :{ 
                    ALU_0utput = A^255; 
                    break;
                }
                case 8 :{ 
                    ALU_0utput = (A << source_reg_2) % (1 << 8); 
                    break;
                }
                case 9 :{ 
                    ALU_0utput = (A >> source_reg_2); 
                    break;
                }
            }
            if(operator_value <= 3) arithmatic++;
            else if(operator_value <= 7) log++;
            else shift++;
        }

        // Memory Reference
        else if(operator_value < 13){
            switch (operator_value){
                case 10:{
                    ALU_0utput = immediate(((source_reg_1 << 4) + source_reg_2)); 
                    break;
                }
                case 11: //ignore
                case 12:{ 
                    ALU_0utput = A + immediate(source_reg_2); 
                    break;
                }
            }
            if(operator_value != 10){
                mem++;
            }
            else {
                load_imm++;
            }
        }

        else if (operator_value != 15){
            switch (operator_value){
                case 13:{ 
                    ALU_0utput = pc + ((immediate(((dest_reg << 4) + source_reg_1)))<<1) ; 
                    break;
                }
                case 14:{ 
                    ALU_0utput = pc + ((register_values[dest_reg] == 0) ? (immediate(((source_reg_1 << 4) + source_reg_2))<<1) : 0); 
                    break;
                } 
            }
            haxard_control++;
            cont++;
        }

        else{
            pc = 512;hal++;
        }

        // Memory Access Cycle
        int lmd;
        if(operator_value == 11 || operator_value == 12){
            if(operator_value == 11){
                lmd = data_values[ALU_0utput];
            }
            else{
                data_values[ALU_0utput] = register_values[dest_reg];
            }
        }


        if(operator_value <= 10){
            register_values[dest_reg] = ALU_0utput;
        }
        else if (operator_value == 11){
            register_values[dest_reg] = lmd;
        }

        else if(operator_value == 13 || operator_value == 14){
            pc = ALU_0utput;
        }

        
    }

    for(int i = 0;i<256;i++){
        d_out_file << DicToHex(data_values[i]) << endl;
    }

    int total_hazard = Hazard + (haxard_control<<1);

    ofile << "Total number of instructions executed        : " << NoofInstruction << endl;
    ofile << "Number of instructions in each class\n";
    ofile << "Arithmetic instructions                      : " << arithmatic << endl;
    ofile << "Logical instructions                         : " << log << endl;
    ofile << "Shift instructions                           : " << shift << endl;
    ofile << "Memory instructions       int Hazard = 0;                       : " << mem << endl;
    ofile << "Load immediate instructions                  : " << load_imm << endl;
    ofile << "Control instructions                         : " << cont << endl;
    ofile << "Halt instructions                            : " << hal <<endl;
    ofile << "Cycles Per Instruction                       : " << (double)(NoofInstruction + 4 + total_hazard)/NoofInstruction <<  endl;
    ofile << "Total number of stalls                       : " << total_hazard <<endl;
    ofile << "Data stalls (RAW)                            : " << Hazard << endl;
    ofile << "Control Stalls                               : " << (hazard_control<< 1) << endl;
}
