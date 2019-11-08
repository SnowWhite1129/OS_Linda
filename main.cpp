#include <iostream>
#include <sstream>
#include <omp.h>
#include "linda.h"
#include <queue>
#include <map>
#include <unistd.h>

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
    FILE *outfp = fopen(filename.c_str(), "a");
    lookUp(instruction.tuple).Write(outfp);
    fprintf(outfp, "\n");
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
void execCommand(const Instruction &instruction, bool wait[], Instruction result[], bool signal[], queue <int> &priority, omp_lock_t *writelock){
    if (wait[instruction.clientID])
        return;
    switch (instruction.operation){
        case out:
            tuples.push_back(instruction.tuple);
            break;
        case read_:
        case in:
	    bool success = false;
            if (!execReadIn(instruction)){
                wait[instruction.clientID] = true;
                priority.push(instruction.clientID);
            } else {
		        success = true;
	        }
            if (success){
		        omp_set_lock(writelock);
                while(signal[instruction.clientID]){
                    omp_unset_lock(writelock);
                    usleep(1000);
                    omp_set_lock(writelock);
                }
                signal[instruction.clientID] = true;
                omp_unset_lock(writelock);
            }
            result[instruction.clientID].tuple = instruction.tuple;
            result[instruction.clientID].operation = instruction.operation;
            omp_set_lock(writelock);
            while(signal[instruction.clientID]){
                omp_unset_lock(writelock);
                usleep(1000);
                omp_set_lock(writelock);
            }
            omp_unset_lock(writelock);
            break;
    }
}

bool execRegular(Instruction result[], bool signal[], queue <int> &priority, bool wait[], omp_lock_t *writelock){
    queue <int> tmp = priority;
    while (!tmp.empty()){
        if (execReadIn(result[tmp.front()])){
            priority.pop();
            wait[tmp.front()] = false;
            omp_set_lock(writelock);
	        while(signal[tmp.front()]){
                omp_unset_lock(writelock);
                usleep(1000);
                omp_set_lock(writelock);
	        }
            signal[tmp.front()] = true;
            omp_unset_lock(writelock);
            return true;
        }
        tmp.pop();
    }
    return false;
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
        instruction.operation = read_;
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
    char nullchar;

    cin >> threatNum;
    scanf("%c", &nullchar);

    bool exit = false;
    bool wait[threatNum+1];
    bool signal[threatNum+1];

    Instruction result[threatNum+1];

    for (int i = 1; i < threatNum+1; ++i) {
        string filename = to_string(i)+".txt";
        FILE *fp = fopen(filename.c_str(), "a");
        fclose(fp);
        wait[i] = false;
        signal[i] = false;
    }
    queue <int> priority;
    omp_lock_t writelock;

    omp_init_lock(&writelock);
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
                        execCommand(instruction, wait, result, signal, priority, &writelock);
                        if(!execRegular(result, signal, priority, wait, &writelock)&&instruction.operation==out)
                            writeTuple(tuples);
                    }
                }
            } else {
                omp_set_lock(&writelock);
                if (signal[threadID]){
                    execClient(result[threadID], threadID);
                    signal[threadID] = false;
                }
                omp_unset_lock(&writelock);
            }
        }
    }
}
