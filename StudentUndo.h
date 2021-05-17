#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"
#include <iostream>
#include <stack>

class StudentUndo : public Undo {
public:

    void submit(Action action, int row, int col, char ch = 0);
    Action get(int& row, int& col, int& count, std::string& text);
    void clear();

private:
    struct Node{
        Action act;
        char value;
        int colPos;
        int rowPos;
        int mCount;
        bool delTogether;
        bool isDelete;
    };
    std::stack<Node> undoStack;
};

#endif // STUDENTUNDO_H_
