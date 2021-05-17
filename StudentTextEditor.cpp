#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>
using namespace std;

TextEditor* createTextEditor(Undo* un)
{
    return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo), rowPos(0), colPos(0){
     line.clear();
}

StudentTextEditor::~StudentTextEditor()
{
    line.clear();
}

bool StudentTextEditor::load(std::string file) {
    ifstream infile(file);
    //if not success
    if (!infile){
        return false;
    }
    //reset file and cursor
    reset();
    string s;
    //to the end of file
    while(getline(infile, s)){
        //strip the carriage
        if (s.size() != 0 && s[s.size() - 1] == '\r'){
            s.erase(s.size() - 1);
        }
        line.push_back(s);
    }
    return true;
}

bool StudentTextEditor::save(std::string file) {
    ofstream outfile(file);
    //if not success
    if(!outfile){
        return false;
    }
    //loop through the list
    list<string>::iterator it;
    it = line.begin();
    while(it != line.end()){
        //write lines into the file
        outfile << *it + '\n';
        it++;
    }
    return true;
}

void StudentTextEditor::reset() {
    //clear; reset
    line.clear();
    rowPos = 0;
    colPos = 0;
    getUndo()->clear();
}

void StudentTextEditor::move(Dir dir) {

    //get to the current line
    list<string>::iterator it = line.begin();
    for (int i = 0; i < rowPos; i++){
        it++;
    }
    
    int sizePre;
    int sizeNext;
    
    if (line.size() != 0){
        //change cursor based on dir input
        switch (dir){
            case UP:
                if (rowPos != 0){
                    rowPos--;
                    //iterator update to the line before
                    it--;
                    sizePre = int((*it).size());
                    if (colPos > sizePre){
                        colPos = sizePre;
                    }
                }
                break;
            case DOWN:
                if (rowPos != line.size() - 1){
                    rowPos ++;
                    //iterator update to the next line
                    it++;
                    sizeNext = int((*it).size());
                    if (colPos > sizeNext){
                        colPos = sizeNext;
                    }
                }
                break;
            case LEFT:
                if (colPos != 0){
                    colPos--;
                }
                //if at the beginning of the line but not the first line
                else if (colPos == 0 && rowPos != 0){
                    //go to previous line
                    it--;
                    rowPos--;
                    colPos = int((*it).size());
                }
                break;
            case RIGHT:
                if (colPos != int((*it).size())){
                    colPos++;
                }
                //if at the end of the line but not the end of the file
                else if (colPos == int((*it).size()) && rowPos != line.size() - 1){
                    rowPos++;
                    colPos = 0;
                }
                break;
            //go to the first and last pos of the current line
            case HOME:
                colPos = 0;
                break;
            case END:
                colPos = int((*it).size());
                break;
        }
    }
}

void StudentTextEditor::del() {
    list<string>::iterator it;
    
    //get the current line
    if(line.size() != 0){
        it = line.begin();
        for (int i = 0; i < rowPos; i++){
            it++;
        }
    }
    
    //if the current file is empty
    else{
        return;
    }
    
    //if the cursor at the beginning of the line and current line is empty or if the cursor is at the end of current line but not the end of the file
    if ((colPos == 0 && (*it).size() == 0) || (colPos == (*it).size() && rowPos != line.size() - 1)){
        if (rowPos != line.size() - 1){
            //get to the next line
            it++;
            string tempNext = (*it);
            it--;
            //append
            (*it) += tempNext;
            line.erase(++it);
            char c;
            getUndo()->submit(Undo::Action::JOIN, rowPos, colPos, c);
        }
    }
    //if the cursor is at the rest of the positions
    else{
        char c = (*it)[colPos];
        (*it).erase(colPos, 1);
        //submit the info
        getUndo()->submit(Undo::Action::DELETE, rowPos, colPos, c);
    }
}

void StudentTextEditor::backspace() {
    //get to the current line
    list<string>::iterator it = line.begin();
    for (int i = 0; i < rowPos; i++){
        it++;
    }
    
    //if at the beginning of the line but not the first line of the file
    if (colPos == 0 && rowPos != 0){
        string tempCur = (*it);
        //go to the previous line
        it--;
        colPos = int((*it).size());
        (*it) += tempCur;
        rowPos--;
        line.erase(++it);
        char c;
        getUndo()->submit(Undo::Action::JOIN, rowPos, colPos, c);
    }
    
    else if (colPos != 0){
        char c = (*it)[colPos - 1];
        (*it).erase(colPos - 1, 1);
        //update cursor
        colPos--;
        //submit the info
        getUndo()->submit(Undo::Action::DELETE, rowPos, colPos, c);
    }
}

void StudentTextEditor::insert(char ch) {

    list<string>::iterator it;
    
    //get the current line
    if(line.size() != 0){
        it = line.begin();
        for (int i = 0; i < rowPos; i++){
            it++;
        }
    }
    
    //if the current file is empty
    else{
        string s = "";
        line.push_back(s);
        it = line.begin();
    }
    
    //if it is a tab
    if (ch == '\t'){
        //insert space
        for (int i = 0; i < 4; i++){
            (*it).insert(colPos, " ");
        }
        //update cursor
        colPos += 4;
    }
    
    //if it's a character
    else{
        string s = "";
        s += ch;
        (*it).insert(colPos, s);
        colPos++;
    }
    
    //call undo class to submit info
    getUndo()->submit(Undo::Action::INSERT, rowPos, colPos, ch);
}

void StudentTextEditor::enter() {
    
    //get to the current line
    list<string>::iterator it = line.begin();
    for (int i = 0; i < rowPos; i++){
        it++;
    }
    
    string firstString;
    string secondString;
    
    //split the string
    firstString = (*it).substr(0, colPos);
    secondString = (*it).substr(colPos);
    
    //insert the new string element, update the current
    (*it) = firstString;
    
    //if end of the file
    if (it == line.end()){
        line.push_back("");
        it++;
    }
    
    else{
        it++;
    }
    
    line.insert(it, secondString);
    
    //update info
    char c;
    getUndo()->submit(Undo::Action::SPLIT, rowPos, colPos, c);
    
    //update the cursor
    rowPos += 1;
    colPos = 0;
}

void StudentTextEditor::getPos(int& row, int& col) const {
    row = rowPos;
    col = colPos;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
    //if error case
    if (startRow < 0 || numRows < 0 || startRow > line.size()){
        return -1;
    }
    
    //clear lines
    lines.clear();
    
    int count = 0;
    //loop through the line
    list<string>::const_iterator it = line.begin();
    
    //move to the start row
    for (int i = 0; i < startRow; i++){
        if (it != line.end()){
            it++;
        }
    }
    
    //push the correspoding lines to lines
    for (int i = 0; i < numRows; i++){
        //not exceed the end of the file
        if(it != line.end()){
            lines.push_back(*it);
            count++;
            it++;
        }
        else{
            break;
        }
    }
    return count;
}

void StudentTextEditor::undo() {
    //if empty file
    if (line.size() == 0){
        return;
    }
    list<string>::iterator it = line.begin();
    int count = 0;
    string txt = "";
    Undo::Action act;
    
    act = getUndo()->get(rowPos, colPos, count, txt);
    
    //if no undos
    if (act == Undo::Action::ERROR){
        return;
    }
    //if just inserted
    else if (act == Undo::Action::DELETE){
        for (int i = 0 ; i < rowPos; i++){
            it++;
        }
        for (int i = 0; i < count; i++){
            (*it).erase(colPos - 1, 1);
            if (colPos >= 1){
                colPos--;
            }
        }
    }
    //if just deleted
    else if (act == Undo::Action::INSERT){
        for (int i = 0; i < rowPos; i++){
            it++;
        }
        (*it).insert(colPos, txt);
    }
    //if just split
    else if (act == Undo::Action::JOIN){
        string temp;
        //go to the next line and get the value
        for (int i = 0; i <= rowPos; i++){
            it++;
        }
        temp = (*it);
        it--;
        (*it).insert(colPos, temp);
        //delete the entered line
        line.erase(++it);
    }
    //if just joined
    else if (act == Undo::Action::SPLIT){
        string temp;
        //go to the line
        for (int i = 0; i < rowPos; i++){
            it++;
        }
        temp = (*it).substr(colPos);
        (*it).erase(colPos);
        //insert a new next line and put value in
        //if end of file
        if (rowPos == line.size() - 1){
            line.push_back("" + temp);
        }
        else{
            line.insert(++it, temp);
        }
    }
}

