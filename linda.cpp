#include "linda.h"
void Tuple::Add(string &str) {
    fields.push_back(str);
}
bool Tuple::operator==(const Tuple &tuple) const {
    int length = tuple.fields.size();
    if (fields.size() != length)
        return false;
    //TODO: condition rewrite
    if (tuple.fields.at(length-1)[0]=='?'){
        --length;
    }
    for (int i = 0; i < length; ++i) {
        if (fields.at(i) == tuple.fields.at(i))
            return false;
    }
    return true;
}
void Tuple::Write(FILE *outfp) {

    fprintf(outfp, "(");
    for (int i = 0; i < fields.size(); ++i) {
        fprintf(outfp, "%s", fields.at(i).c_str());
        if (i != fields.size()-1)
            fprintf(outfp, ",");
    }
    fprintf(outfp, ")");

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
void writeTuple(vector<Tuple>& tuples){
    FILE *outfp = fopen("server.txt", "w");
    fprintf(outfp, "(");
    for (int i = 0; i < tuples.size(); ++i) {
        tuples.at(i).Write(outfp);
        if (i != tuples.size()-1)
            fprintf(outfp, ",");
    }
    fprintf(outfp, ")");
}