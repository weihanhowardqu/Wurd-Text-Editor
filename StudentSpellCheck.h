#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck();
    virtual ~StudentSpellCheck();
    bool load(std::string dict_file);
    bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
    void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
    struct Node{
        //alphabet size with a place for '.' and '''
        Node* child[28];
    };
    
    Node* root;
    int charIndex(char c);
    void insertNode(std::string word);
    bool searchNode(std::string word);
    void cleanUp(Node* p);
    
};

#endif  // STUDENTSPELLCHECK_H_
