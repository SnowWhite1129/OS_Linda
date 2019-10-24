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
            //TODO mutiple key value
            string key = instruction.tuple.fields.at(instruction.tuple.fields.size()-1);

            int pos = findPos(instruction.tuple, tuples);
            if (pos!=-1){
                if (key[0] == '?'){
                    string value = tuples.at(pos).fields.at(tuples.at(pos).fields.size()-1);
                    table[key] = value;
                }
                //TODO: Send tuple to client i
                if (instruction.operation==in)
                    removeTuple(tuples, pos);
            } else{
                if (key[0]!= '?') {
                    wait[instruction.clientID] = true;
                    priority.push(instruction.clientID);
                    //TODO =
                    result[instruction.clientID] = instruction.tuple;
                }
            }
            break;
    }
}
void execRegular(Tuple result[], int threatNum){
    //TODO: Priority go through
    for (int i = 0; i < threatNum+1; ++i) {
        int pos = findPos(result[i], tuples);
        if (pos!=-1){
            //TODO: Send tuple to client i
        }
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
        //TODO lookup table and replace
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
                        execRegular(result, threatNum);
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
