#include <iostream>
#include <sstream>
#include <omp.h>
#include "linda.h"
#include <queue>
#include <map>

vector <Tuple> tuples;
map<string, string> table;

Tuple lookUp(Tuple &tuple){
    Tuple ret;
    for (auto & field : tuple.fields) {
        if (field[0] == '?')
            ret.fields.push_back(table[field.substr(1)]);
        else
            ret.fields.push_back(field);
    }
    return ret;
}

void execClient(Instruction &instruction, int clientID){
    string filename = to_string(clientID)+".txt";
    FILE *outfp = fopen(filename.c_str(), "w");
    lookUp(instruction.tuple).Write(outfp);
    fclose(outfp);
}

bool execReadIn(const Instruction &instruction){
    int pos = findPos(instruction.tuple, tuples);
    if (pos!=-1){
        for (int i = 0; i < tuples.at(pos).fields.size(); ++i) {
            if (instruction.tuple.fields.at(i)[0] == '?')
                table[instruction.tuple.fields.at(i).substr(1)] = tuples.at(pos).fields.at(i);
        }
        if (instruction.operation==in){
            removeTuple(tuples, pos);
            writeTuple(tuples);
        }

        return true;
    }
    return false;
}
void execCommand(const Instruction &instruction, bool wait[], Instruction result[], bool signal[], queue <int> &priority){
    if (wait[instruction.clientID])
        return;
    switch (instruction.operation){
        case out:
            tuples.push_back(instruction.tuple);
            writeTuple(tuples);
            break;
        case read:
        case in:
            if (!execReadIn(instruction)){
                wait[instruction.clientID] = true;
                priority.push(instruction.clientID);
                //result[instruction.clientID].tuple = instruction.tuple;
            }
            result[instruction.clientID].tuple = instruction.tuple;
            result[instruction.clientID].operation = instruction.operation;
            signal[instruction.clientID] = true;
#ifdef dbg
            cout << "==================" << instruction.clientID << endl;
            for (int i=0; i<result[instruction.clientID].tuple.fields.size();++i) {
                cout << result[instruction.clientID].tuple.fields.at(i) << " ";
            }
            cout << endl;
            cout << "==================" << endl;
#endif

            break;
    }
}

void execRegular(Instruction result[], bool signal[], queue <int> &priority){
    queue <int> tmp = priority;
    while (!tmp.empty()){
        if (execReadIn(result[tmp.front()])){
//            cout << "reallly" << endl;
//            result[tmp.front()].tuple.fields.clear();
            priority.pop();
            signal[tmp.front()] = true;
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
    /*
#ifdef dbg
    cout << "==================" << endl;
    for (const auto & field : instruction.tuple.fields) {
        cout << field << " ";
    }
    cout << endl;
    cout << "==================" << endl;
#endif
     */
    return true;
}

int main() {
    int threatNum;
    char nullchar;

    cin >> threatNum;
    scanf("%c", &nullchar);

    bool exit = false;
    bool wait[threatNum+1];
    bool signal[threatNum+1];

    Instruction result[threatNum+1];

    for (int i = 0; i < threatNum+1; ++i) {
        wait[i] = false;
        signal[i] = false;
    }
    queue <int> priority;

    omp_set_num_threads(threatNum+1);
#pragma omp parallel
    {
        while (!exit){
            int threadID = omp_get_thread_num();
            if (threadID == server){
                string line;
                if (getline(cin, line)) {
                    Instruction instruction;
                    if (!takeInput(line, instruction))
                        exit = true;
                    else{
                        execCommand(instruction, wait, result, signal, priority);
                        execRegular(result, signal, priority);
                    }
                }
            } else {
                if (signal[threadID]){
#ifdef dbg
                    cout << threadID << endl;
                    for (int i = 0; i < result[threadID].tuple.fields.size(); ++i) {
                        cout << result[threadID].tuple.fields.at(i) << " ";
                    }
                    cout << endl;
#endif
                    execClient(result[threadID], threadID);
                    signal[threadID] = false;
                }
            }
        }
    }
}
