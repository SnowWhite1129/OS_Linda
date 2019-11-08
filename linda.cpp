#include "linda.h"
#include <iostream>
void Tuple::Add(string &str) {
    fields.push_back(str);
}
bool Tuple::operator==(const Tuple &tuple) const {
    int length = tuple.fields.size();
    if (fields.size() != length)
        return false;

    for (int i = 0; i < length; ++i) {
        if (tuple.fields.at(i)[0]=='?'){
            continue;
        }
        if (fields.at(i) != tuple.fields.at(i))
            return false;
    }
    return true;
}
Tuple& Tuple::operator=(const Tuple &tuple) {
    fields.clear();
    for (const auto & field : tuple.fields) {
        fields.push_back(field);
    }
}
void Tuple::Write(FILE *outfp) const{
    fprintf(outfp, "(");
    for (int i = 0; i < fields.size(); ++i) {
        fprintf(outfp, "%s", fields.at(i).c_str());
        if (i != fields.size()-1)
            fprintf(outfp, ",");
    }
    fprintf(outfp, ")");
}
int findPos(const Tuple &tuple, const vector<Tuple> &tuples){
    for (int i = 0; i < tuples.size(); ++i) {
        if (tuples.at(i) == tuple)
            return i;
    }
    return -1;
}

void removeTuple(vector<Tuple>& tuples, int pos){
    tuples.erase(tuples.begin()+pos);
}
void writeTuple(const vector<Tuple>& tuples){
    FILE *outfp = fopen("server.txt", "a");
    fprintf(outfp, "(");
    for (int i = 0; i < tuples.size(); ++i) {
        tuples.at(i).Write(outfp);
        if (i != tuples.size()-1)
            fprintf(outfp, ",");
    }
    fprintf(outfp, ")\n");
    fclose(outfp);
}
