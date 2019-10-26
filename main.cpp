#include <iostream>
#include <sstream>
#include <omp.h>
#include "linda.h"
#include <queue>
#include <map>

vector <Tuple> tuples;
queue <int> priority;
map<string, string> table;

Tuple lookUp(Tuple &tuple){
    Tuple ret;
    for (auto & field : tuple.fields) {
        if (field[0] == '?')
            ret.fields.push_back(table[field.substr(1)]);
    }
    return ret;
}

void execClient(Instruction &instruction, int clientID){
    string filename = to_string(clientID)+".txt";
    FILE *outfp = fopen(filename.c_str(), "w");
    lookUp(instruction.tuple).Write(outfp);
}

bool execReadIn(const Instruction &instruction, bool signal[]){
    int pos = findPos(instruction.tuple, tuples);
    if (pos!=-1){
        for (int i = 0; i < tuples.at(pos).fields.size(); ++i) {
            if (instruction.tuple.fields.at(i)[0] == '?')
                table[instruction.tuple.fields.at(i).substr(1)] = tuples.at(pos).fields.at(i);
        }
        signal[instruction.clientID] = true;
        if (instruction.operation==in)
            removeTuple(tuples, pos);
        return true;
    }
    return false;
}
void execCommand(const Instruction &instruction, bool wait[], Instruction result[], bool signal[]){
    switch (instruction.operation){
        case out:
            tuples.push_back(instruction.tuple);
            writeTuple(tuples);
            break;
        case read:
        case in:
            if (!execReadIn(instruction, signal)){
                wait[instruction.clientID] = true;
                priority.push(instruction.clientID);
                result[instruction.clientID].tuple = instruction.tuple;
            }
            break;
    }
}

void execRegular(Instruction result[], bool signal[]){
    queue <int> tmp = priority;
    while (!tmp.empty()){
        if (execReadIn(result[tmp.front()], signal)){
            result[tmp.front()].tuple.fields.clear();
            priority.pop();
            break;
        }
        tmp.pop();
    }
}

int takeInput(const string &line, Instruction &instruction){

    if (line == "exit")
        return false;

    istringstream iss(line);

    iss >> instruction.clientID;
    string str;
    iss >> str;

    if(str == "in")
        instruction.operation = in;
    else if(str == "out")
        instruction.operation = out;
    else if(str == "read")
        instruction.operation = read;
    else
        cout << "Error" << endl;

    while (iss >> str){
        auto it = table.find(str);
        if (it != table.end())
            str = table[str];
        instruction.tuple.Add(str);
    }

    return true;
}

int main() {
    int threatNum;

    cin >> threatNum;
    omp_set_num_threads(threatNum+1);

    bool exit = false;
    bool wait[threatNum+1];
    bool signal[threatNum+1];

    Instruction result[threatNum+1];

    for (int i = 0; i < threatNum+1; ++i) {
        wait[i] = false;
        signal[i] = false;
    }

    Instruction instruction;

#pragma omp parallel
    {
        while (!exit){
            int threadID = omp_get_thread_num();
            if (threadID == server){
                string line;
                while (!getline(cin, line)) {
                    printf("%% ");
                    if (!takeInput(line, instruction))
                        exit = true;
                    else{
                        execCommand(instruction, wait, result, signal);
                        execRegular(result, signal);
                    }
                }
            } else {
                if (signal[threadID]){
                    execClient(result[threadID], threadID);
                    signal[threadID] = false;
                }
            }
        }
    }
}
