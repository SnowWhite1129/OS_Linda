#include <iostream>
#include <sstream>
#include <omp.h>
#include "linda.h"
#include <queue>
#include <map>

vector <Tuple> tuples;
queue <int> priority;
map<string, string> table;

void execCommand(const Instruction &instruction, bool wait[], Tuple result[]){
    switch (instruction.operation){
        case out:
            tuples.push_back(instruction.tuple);
            writeTuple(tuples);
            break;
        case read:
        case in:
            int pos = findPos(instruction.tuple, tuples);
            if (pos!=-1){
                for (int i = 0; i < tuples.at(pos).fields.size(); ++i) {
                    if (instruction.tuple.fields.at(i)[0] == '?')
                        table[instruction.tuple.fields.at(i).substr(1)] = tuples.at(pos).fields.at(i);
                }
                //TODO: Send tuple to client i
                if (instruction.operation==in)
                    removeTuple(tuples, pos);
            } else{
                wait[instruction.clientID] = true;
                priority.push(instruction.clientID);
                result[instruction.clientID] = instruction.tuple;
            }
            break;
    }
}
//TODO: The structure is as same as above
//TODO: Arguements should be Instruction
void execRegular(Tuple result[]){
    queue <int> tmp = priority;
    while (!tmp.empty()){
        int pos = findPos(result[tmp.front()], tuples);
        if (pos!=-1){
            //TODO: Send tuple to client i
            result[tmp.front()].fields.clear();
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
    Tuple result[threatNum+1];

    for (int i = 0; i < threatNum+1; ++i) {
        wait[i] = false;
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
                        execCommand(instruction, wait, result);
                        execRegular(result);
                    }
                }
            } else {
                /*
                if (!wait[threadID] && instruction.clientID == threadID && instruction.operation!=out){

                }
                */
            }
        }
    }
}
