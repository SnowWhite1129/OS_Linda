#ifndef OS_LINDA_LINDA_H
#define OS_LINDA_LINDA_H

#include <vector>
#include <string>

using namespace std;

enum Operation {in, out, read};

struct Tuple{
    vector <string> fields;
    void Add(string &str);
    bool operator == (const Tuple &tuple) const;
};
struct Instruction{
    int clientID;
    Operation operation;
    Tuple tuple;
};
void Tuple::Add(string &str) {
    fields.push_back(str);
}
bool Tuple::operator==(const Tuple &tuple) const {
    int length = tuple.fields.size();
    if (fields.size() != length)
        return false;

    if (tuple.fields.at(length-1)[0]=='?'){
        --length;
    }
    for (int i = 0; i < length; ++i) {
        if (fields.at(i) == tuple.fields.at(i))
            return false;
    }
    return true;
}
int findPos(const Tuple &tuple, vector<Tuple> &tuples){
    for (int i = 0; i < tuples.size(); ++i) {
        if (tuples.at(i) == tuple)
            return i;
    }
    return -1;
}

void removeTuple(vector<Tuple>& tuples, int pos){
    tuples.erase(tuples.begin()+pos-1);
}
#endif //OS_LINDA_LINDA_H
