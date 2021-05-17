#include "StudentUndo.h"
#include <algorithm>
using namespace std;

Undo* createUndo()
{
    return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
    //create a new node and put values in
    Node d;
    d.act = action;
    d.value = ch;
    d.rowPos = row;
    d.colPos = col;
    d.mCount = 1;
    d.delTogether = false;
    d.isDelete = false;
    //if not empty and the act equals to the previous act
    if (!undoStack.empty()){
        //if same row and insert right to the previous or it's a tab
        if (d.act == INSERT && undoStack.top().act == INSERT && (d.colPos == undoStack.top().colPos + 1 || d.value == '\t')){
            d.mCount = undoStack.top().mCount + 1;
        }
        //if delete together, not separately
        else if (d.act == DELETE && undoStack.top().act == DELETE && (d.colPos == undoStack.top().colPos - 1 || d.colPos == undoStack.top().colPos) && d.rowPos == undoStack.top().rowPos){
            //if it's deleted not backspaced
            if (d.colPos == undoStack.top().colPos){
                d.isDelete = true;
                undoStack.top().isDelete = true;
            }
            d.delTogether = true;
            undoStack.top().delTogether = true;
        }
    }
    //if it's a tab
    if (d.value == '\t'){
        d.mCount += 3;
    }
    //save in our stack
    undoStack.push(d);
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text) {
    //if there's nothing to undo
    if (undoStack.empty()){
        return ERROR;
    }
    
    //if there's something to undo
    Node d = undoStack.top();
    count = d.mCount;
    //check to see the first undo object
    switch (d.act){
        //if just inserted
        case INSERT: {
            row = d.rowPos;
            col = d.colPos;
            while (undoStack.top().act == INSERT){
                undoStack.pop();
                if (undoStack.empty()){
                    break;
                }
            }
            text = "";
            return DELETE;
            break;
        }
        //if just deleted
        case DELETE: {
            text = "";
            string textTemp = "";
            row = undoStack.top().rowPos;
            col = undoStack.top().colPos;
            //if just deleted together
            if (undoStack.top().delTogether){
                while (undoStack.top().delTogether){
                    //reverse the string if it's deleted not backspaced
                    if (undoStack.top().isDelete){
                        while (undoStack.top().isDelete){
                            textTemp += undoStack.top().value;
                            undoStack.pop();
                        }
                        reverse(textTemp.begin(), textTemp.end());
                        text += textTemp;
                    }
                    //if still delete but from backspace
                    if(undoStack.top().delTogether){
                        text += undoStack.top().value;
                        undoStack.pop();
                    }
                }
            }
            else{
                text += undoStack.top().value;
                undoStack.pop();
            }
            count = 1;
            return INSERT;
            break;
        }
        //if just splitted
        case SPLIT: {
            row = d.rowPos;
            col = d.colPos;
            count = 1;
            text = "";
            undoStack.pop();
            return JOIN;
            break;
        }
        //if just joined
        case JOIN: {
            row = d.rowPos;
            col = d.colPos;
            count = 1;
            text = "";
            undoStack.pop();
            return SPLIT;
            break;
        }
    }
    return ERROR;
}

void StudentUndo::clear() {
    //delete all items
    while(!undoStack.empty()){
        undoStack.pop();
    }
}
