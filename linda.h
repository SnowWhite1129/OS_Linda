#ifndef OS_LINDA_LINDA_H
#define OS_LINDA_LINDA_H

#include <vector>
#include <string>

#define server 0

using namespace std;

enum Operation {in, out, read};

struct Tuple{
    vector <string> fields;
    void Add(string &str);
    void Write(FILE *outfp);
    //TODO =
    bool operator == (const Tuple &tuple) const;
};
struct Instruction{
    int clientID;
    Operation operation;
    Tuple tuple;
};

int findPos(const Tuple &tuple, vector<Tuple> &tuples);
void removeTuple(vector<Tuple>& tuples, int pos);
void writeTuple(vector<Tuple>& tuples);
#endif //OS_LINDA_LINDA_H
