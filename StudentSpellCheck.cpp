#include "StudentSpellCheck.h"
#include <string>
#include <vector>
using namespace std;

SpellCheck* createSpellCheck()
{
    return new StudentSpellCheck;
}

StudentSpellCheck::StudentSpellCheck(){
    root = new Node;
    for (int i = 0; i < 28; i++){
        root->child[i] = nullptr;
    }
}

StudentSpellCheck::~StudentSpellCheck() {
    Node* p = root;
    cleanUp(p);
}

int StudentSpellCheck::charIndex(char c){
    //if it's '\''
    if (c == 39){
        return 1;
    }
    else{
        return c - 'a' + 2;
    }
}

void StudentSpellCheck::insertNode(string word){
    Node* p = root;
    for (int i = 0; i < word.length(); i++) {
        int index = charIndex(word[i]);
        //if not alredy exist
        if (p->child[index] == nullptr)
            p->child[index] = new Node;
        p = p->child[index];
    }
    //mark the end of the word
    p->child[0] = new Node;
}

bool StudentSpellCheck::searchNode(std::string word){
    //empty word alwasy true
    if (word == ""){
        return true;
    }
    Node* p = root;
    for (int i = 0; i < word.length(); i++) {
        int index = charIndex(word.at(i));
        //if not included
        if (p->child[index] == nullptr) {
            return false;
        }
        //update p
        p = p->child[index];
    }
    //see if the word ends or it's part of other words
    return (p->child[0] != nullptr);
}

bool StudentSpellCheck::load(std::string dictionaryFile) {
    //input file
    ifstream infile(dictionaryFile);
    if(!infile){
        cout << "Unable to open dictionary file";
        return false;
    }
    string s;
    //load to the end
    while(getline(infile, s)){
        //stripping
        for (int i = 0; i < s.size(); i++){
            if (s[i] != '\'' && !isalpha(s[i])){
                s.erase(i);
                break;
            }
            //lower the word
            s[i] = tolower(s[i]);
        }
        //add word to the trie
        insertNode(s);
    }
    return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
    //lower case the word
    for (int i = 0; i < word.size(); i++){
        word[i] = tolower(word[i]);
    }
    //if word exists
    if (searchNode(word)) {
        return true;
    }
    suggestions.clear();
    //to change each character in the word one at a time and check if the newly made word is in the dict
    for (int i = 0; i < word.length(); i++) {
        string before = word.substr(0, i);
        string after = word.substr(i + 1);
        for (int j = 0; j < 27; j++) {
            char c;
            if (j == 26) {
                c = '\'';
            }
            else{
                c = 'a' + j;
            }
            string suggestion = before + c + after;
            //if exists, it's a spelling suggestion
            if (searchNode(suggestion)) {
                suggestions.push_back(suggestion);
            }
            //can't go over max suggestion numbers
            if (suggestions.size() >= max_suggestions) {
                break;
            }
        }
    }
    return false;
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
    problems.clear();
    string str = line;
    //lower
    for (int i = 0; i < str.size(); i++){
        str[i] = tolower(str[i]);
    }
    //for the case of char at the end of the line
    str += ' ';
    string word = "";
    for (int i = 0; i < str.length(); i++) {
        char c = str[i];
        //if it's char
        if (isalpha(c) || c == '\'') {
            word += c;
        }
        else {
            //if not found the word
            if (!searchNode(word)) {
                Position p;
                p.start = i - int(word.length());
                p.end = i - 1;
                problems.push_back(p);
            }
            //reset to receive other words
            word = "";
        }
    }
}

void StudentSpellCheck::cleanUp(Node* p){
    if (p == nullptr){
        return;
    }
    //recursively delete the node and its children if there's any
    for (int i = 0; i < 28; i++){
        cleanUp(p->child[i]);
    }
    delete p;
}
