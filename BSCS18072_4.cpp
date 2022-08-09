// BSCS18072_4.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
/*Assumptions:
1. All productions with same non-terminal on right hand side will appear consecutively
2. for left factoring, size of alpha is not more than 1 element and writing generic code according to that (works for given grammar)
3. for left factoring, no set of rules with same left hand side will have 2 sets of rules with alphas (works for this grammar)
4. for left recurssion, at most 1 rule with from he set of rules with same L.H.S will have left recursion  (works for this grammar) 
5. input file (string) will be space separated
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <sstream>
#include <stack>

using namespace std;


//function to remove left factoring
void removeLF(vector<vector<string>> rules, vector<vector<string>> &cfg) {
    vector<vector<string>> newrules;
    string leftside = rules[0][0];
    bool alphafound = false;
    string alpha;
    for (int i = 0; i < rules.size(); i++) {
        for (int j = i + 1; j < rules.size(); j++) {
            if (rules[i][1] == rules[j][1]) {
                alphafound = true;
                alpha = rules[i][1];
                break;
            }
        }
        if (alphafound)
            break;
    }

    //if alpha not found, simply add the grammar to newCFGs
    if (!alphafound) {
        for (int i = 0; i < rules.size(); i++) {
            cfg.push_back(rules[i]);
        }
    }

    //alpha found
    else if (alphafound) {
        bool epsexists = false;
        bool epsadded = false;

        string newNT = leftside+"'";

        //adding rules that did not contain alpha
        for (int i = 0; i < rules.size(); i++) {
            if (rules[i][1] != alpha) {
                cfg.push_back(rules[i]);
            }
        }

        vector<string> rule;
        rule.push_back(leftside);
        rule.push_back(alpha);
        rule.push_back(newNT);
        cfg.push_back(rule);
        rule.clear();


        //adding rules that contained alpha
        for (int i = 0; i < rules.size(); i++) {
            if (rules[i][1] == alpha) {
                rule.push_back(newNT);
                if (rules[i].size() == 2) {
                    rule.push_back("@");
                }
                else if(rules[i].size() > 2){
                    for (int k = 2; k < rules[i].size(); k++) {
                        rule.push_back(rules[i][k]);
                    }
                }
                cfg.push_back(rule);
                rule.clear();
            }
        }
    }

}

//funcion to remove left factoring
void removeLR(vector<vector<string>> rules, vector<vector<string>>& cfg) {
    string leftside = rules[0][0];
    bool LRfound = false;
    int index = 0; // to note the index of the production with left recursion
    
    for (int i = 0; i < rules.size(); i++) {
        if (rules[i][1] == leftside) {
            LRfound = true;
            index = i;
            break;

        }
    }
    //if LR not found, simply add the grammar to newCFGs
    if (!LRfound) {
        for (int i = 0; i < rules.size(); i++) {
            cfg.push_back(rules[i]);
        }
    }
    else if (LRfound) {
        vector<string> alpha;
        vector<string> rule;
        string newNT = leftside + "'";
        for (int i = 2; i < rules[index].size(); i++) {
            alpha.push_back(rules[index][i]);
        }

        //adding new rule with newNT that removes LF 
        rule.push_back(newNT);
        for (int i = 0; i < alpha.size(); i++){
            rule.push_back(alpha[i]);
        }
        rule.push_back(newNT);
        cfg.push_back(rule);
        rule.clear();

        //adding epsilon rule
        rule.push_back(newNT);
        rule.push_back("@");

        cfg.push_back(rule);
        rule.clear();


        //adding new rules with new introduced NT
        for (int i = 0; i < rules.size(); i++) {
            if (i != index) {
                //if we have epsilon in rule
                if (rules[i][1] == "@") {
                    rule.push_back(leftside);
                    rule.push_back(newNT);
                    cfg.push_back(rule);
                    rule.clear();
                }

                else {
                    rule.push_back(leftside);
                    for (int j = 1; j < rules[i].size(); j++) {
                        rule.push_back(rules[i][j]);
                    }
                    rule.push_back(newNT);
                    cfg.push_back(rule);
                    rule.clear();
                }
                
            }
        }
        
    }
}

//to remove duplicates in FOLLOW or FIRST sets
void remove_duplicate(vector<vector<string>>& SET) {
    for (int i = 0; i < SET.size(); i++) {
        for (int j = 2; j < SET[i].size(); j++) {
            for (int k = j - 1; k > 0; k--) {
                if (SET[i][j] == SET[i][k]) {
                    SET[i].erase(SET[i].begin() + j);
                    j--;
                }
            }
        }
    }
}

//to resolve non terminals in FOLLOW or FIRST sets to terminals
void resolving_non_terminals(vector<vector<string>>& SET) {
    for (int i = 0; i < SET.size(); i++) {
        int index = 0;//to find the row index of the first set of non_terminal
        for (int j = 1; j < SET[i].size(); j++) {

            if (SET[i][j][0] > 64 && SET[i][j][0] <= 90) { // if its a non_terminal
                for (int k = 0; k < SET.size(); k++) {
                    if (SET[i][j] == SET[k][0]) {
                        index = k;
                        break;
                    }
                }

                SET[i].erase(SET[i].begin() + j); // removing the non_terminal
                for (int m = 1; m < SET[index].size(); m++) {
                    if (SET[index][m] != SET[i][0]) {
                        SET[i].push_back(SET[index][m]);
                    }
                }
                j--;
            }
        }
    }
}


void clearStack(stack<string> &stck) {
    while (!stck.empty()) {
        stck.pop();
    }
}

string printStack(stack<string> stck) {
    string s;
    while (!stck.empty()) {
        s += stck.top();
        s += " ";
        stck.pop();
    }
    return s;
}


string vecToStr(vector<string> v) {
    string s;
    s += v[0];
    for (int i = 1; i < v.size(); i++) {
        s += " ";
        s += v[i];
    }
    return s;
}
int main()
{
    string fname;
    cout << "Enter the name of the CFG file: ";
    cin >> fname;
    fstream file;
    file.open(fname);
    vector<stringstream> lines;
    string l; // to read file line by line


    //=====================following to read from file and store productions rules=====================
    //-----------------reading line by line-----------------
    while (getline(file, l)) {
        lines.push_back(stringstream(l));
        //cout << l << endl;
    }
    string word; //to read file word by word
    vector<vector<string>> productions(lines.size()); // 2D vector to store productions


    //storing production rules in a 2d vector
    for (int i = 0; i < lines.size(); i++) {
        while (getline(lines[i], word, '\t')) {
            productions[i].push_back(word);
        }
    }


    //=====================following part is to process the grammar=====================
    vector<vector<string>> newCFG; // to store grammar after removing left factoring
    //indexes to note start and end of productions with same element on left side
    int si = 0;
    int ei = 0;

    vector<vector<string>> cfgPart;
    vector<vector<string>> finalCFG; // to store grammar after removing both left factoring and left recursion
    
    
    for (int i = 1; i < productions.size(); i++) {
        if (productions[i][0] != productions[i - 1][0] || i == productions.size()-1) { // if a new non terminal is encountered, you first remove left factoring and left recursion then move on
            ei = i - 1;
            if (i == productions.size() - 1) {
                ei = i;
            }
            for (int j = si; j <= ei; j++) {
                cfgPart.push_back(productions[j]);
            }

            removeLF(cfgPart, newCFG);
            si = i;
        }
        cfgPart.clear();
    }



    //we have newCFG that is with elimination of left factoring
    //passing this grammar for elimination of left recursion now
    si = 0;
    ei = 0;
    cfgPart.clear();

    for (int i = 1; i < newCFG.size(); i++) {
        if (newCFG[i][0] != newCFG[i - 1][0] || i == newCFG.size() - 1) { // if a new non terminal is encountered, you first remove left factoring and left recursion then move on
            ei = i - 1;
            if (i == newCFG.size() - 1) {
                ei = i;
            }
            for (int j = si; j <= ei; j++) {
                cfgPart.push_back(newCFG[j]);
            }

            removeLR(cfgPart, finalCFG);
            si = i;
        }
        cfgPart.clear();
    }


    //printing new grammar just to check
    //for (int i = 0; i < finalCFG.size(); i++) {
    //    for (int k = 0; k < finalCFG[i].size(); k++) {
    //        cout << finalCFG[i][k] << "\t";
    //    }
    //    cout << endl;
    //}

    //=====================making 2 vectors to stores terminals and non terminals separately===========
    vector<string> NT;
    vector<string> T;

    for (int i = 0; i < finalCFG.size(); i++) {

        //adding to non terminals vector
        bool duplicateNT = false;
        for (int k = 0; k < NT.size(); k++) {
            if (NT[k] == finalCFG[i][0]) {
                duplicateNT = true;
                break;
            }
        }
        if (!duplicateNT)
            NT.push_back(finalCFG[i][0]);


        //adding to terminals vector
        for (int j = 0; j < finalCFG[i].size(); j++) {
            if ((finalCFG[i][j][0] < 65 || finalCFG[i][j][0] > 90) && finalCFG[i][j] != "@") {
                bool duplicateT = false;
                for (int l = 0; l < T.size(); l++) {
                    if (T[l] == finalCFG[i][j]) {
                        duplicateT = true;
                        break;
                    }
                }
                if (!duplicateT)
                    T.push_back(finalCFG[i][j]);
            }
        }
    }

    //cout << "===============NT================";
    //for (int i = 0; i < NT.size(); i++) {
    //    cout << "\n" << NT[i];
    //}

    //cout << "\n===============T================";
    //for (int i = 0; i < T.size(); i++) {
    //    cout << "\n" << T[i];
    //}


    //======================COMPUTING FIRST AND FOLLOW=====================
 //-------------vectors to store first and follow sets--------------
    vector<vector<string>> FIRST(NT.size());
    vector<vector<string>> FOLLOW(NT.size());

    for (int i = 0; i < NT.size(); i++) {
        FIRST[i].push_back(NT[i]);
        FOLLOW[i].push_back(NT[i]);
    }

    //----------------------computing first sets-----------------------

    for (int i = 0; i < finalCFG.size(); i++) {
        int index = 0; // index to check which non-terminal are we dealing with
        for (int k = 0; k < NT.size(); k++) {
            if (finalCFG[i][0] == FIRST[k][0]) {
                index = k;
                break;
            }
        }
        if (finalCFG[i][1][0] < 65 || finalCFG[i][1][0] > 90) { // this means its a terminal
            FIRST[index].push_back(finalCFG[i][1]); // we add that to first set
        }
    }

    // we iterate throught finalCFG again to deal with non terminals this time
    for (int i = 0; i < finalCFG.size(); i++) {
        int index = 0; // index to check which non-terminal are we dealing with
        for (int k = 0; k < NT.size(); k++) {
            if (finalCFG[i][0] == FIRST[k][0]) {
                index = k;
                break;
            }
        }
        int x = 1; // this is the index for word count in a production, starting from 1 because element at index 0 is the left hand side
        while (x < finalCFG[i].size()) {
            if (finalCFG[i][x][0] > 64 && finalCFG[i][x][0] <= 90) { // this means its a non-terminal
                /*we add the non_terminal to the first set and
                then check if the First set of the non_terminal contains an epsilon*/
                FIRST[index].push_back(finalCFG[i][x]);
                int in = 0; // to find the index of finalCFG[i][x] in the first set to access its first set and check for epsilon
                for (int m = 0; m < NT.size(); m++) {
                    if (finalCFG[i][x] == FIRST[m][0]) {
                        in = m;
                        break;
                    }
                }
                bool epsilon = false;
                for (int n = 1; n < FIRST[in].size(); n++) {
                    if (FIRST[in][n] == "@") {
                        epsilon = true;
                        break;
                    }
                }
                if (epsilon == false) {
                    break;
                }
                //if next symbol is a termial after the non_terminal containing an epsilon in its FIRST set
                if (epsilon == true && (finalCFG[i][x + 1][0] < 65 || finalCFG[i][x + 1][0] > 90)) {
                    FIRST[index].push_back(finalCFG[i][x + 1]);
                    break;
                }
                x++;
            }
            else break;
        }
    }
    // removing duplicates from first sets
    remove_duplicate(FIRST);

    //to resolve non_terminals to terminals
    resolving_non_terminals(FIRST);

    // removing duplicates from first sets
    remove_duplicate(FIRST);

    //for (int i = 0; i < FIRST.size(); i++) {
    //    cout << "===NEW LINEE====\n";
    //    for (int j = 0; j < FIRST[i].size(); j++) {
    //        cout << FIRST[i][j] << "\n";
    //    }
    //    cout << endl;
    //}

    /*-------------------computing the follow set-------------------*/
    FOLLOW[0].push_back("$"); // adding $ in follow set of starting symbol

    for (int i = 0; i < NT.size(); i++) {
        for (int j = 0; j < finalCFG.size(); j++) {
            if (finalCFG[j].size() > 2) {
                for (int k = 1; k < finalCFG[j].size() - 1; k++) {
                    if (finalCFG[j][k] == NT[i]) {
                        // if the follow is a terminal
                        if (finalCFG[j][k + 1][0] < 65 || finalCFG[j][k + 1][0] >90) {
                            FOLLOW[i].push_back(finalCFG[j][k + 1]);
                        }
                        //if the follow is a non-terminal
                        else if (finalCFG[j][k + 1][0] > 64 && finalCFG[j][k + 1][0] <= 90) {
                            //find out which non_terminal follows the non_terminal we are working on
                            int index = 0;
                            for (int m = 0; m < NT.size(); m++) {
                                if (finalCFG[j][k + 1] == NT[m]) {
                                    index = m;
                                    break;
                                }
                            }
                            //adding the first set of the non-terminal that follows into the follow set
                            for (int n = 1; n < FIRST[index].size(); n++) {
                                if (FIRST[index][n] != "@")
                                    FOLLOW[i].push_back(FIRST[index][n]);
                                else if (FIRST[index][n] == "@" && finalCFG[j][0] != NT[i]) {
                                    int indx = 0;
                                    for (int f = 0; f < NT.size(); f++) {
                                        if (finalCFG[j][0] == NT[f]) {
                                            indx = f;
                                            break;
                                        }
                                    }
                                    bool dont = false;
                                    for (int h = 0; h < FOLLOW[indx].size(); h++) {
                                        if (FOLLOW[indx][h] == NT[i])
                                            dont = true;
                                        else if (FOLLOW[indx][h][0] < 65 || FOLLOW[indx][h][0] >90) {
                                            FOLLOW[i].push_back(FOLLOW[indx][h]);
                                        }


                                    }
                                    if (!dont) { FOLLOW[i].push_back(finalCFG[j][0]); }

                                }
                            }
                        }
                    }
                    //If it is the last element  in production
                    if (k == finalCFG[j].size() - 2) {
                        if (finalCFG[j][k + 1] == NT[i]) {
                            if (finalCFG[j][0] != NT[i]) {
                                int indx = 0;
                                for (int f = 0; f < NT.size(); f++) {
                                    if (finalCFG[j][0] == NT[f]) {
                                        indx = f;
                                        break;
                                    }
                                }
                                bool dont = false;
                                for (int h = 0; h < FOLLOW[indx].size(); h++) {
                                    if (FOLLOW[indx][h] == NT[i])
                                        dont = true;
                                    else if (FOLLOW[indx][h][0] < 65 || FOLLOW[indx][h][0] >90) {
                                        FOLLOW[i].push_back(FOLLOW[indx][h]);
                                    }
                                }
                                if (!dont) { FOLLOW[i].push_back(finalCFG[j][0]); }
                            }
                        }
                    }
                }
            }
            if (finalCFG[j].size() == 2) {
                if (finalCFG[j][1] == NT[i]) {
                    if (finalCFG[j][0] != NT[i])
                        FOLLOW[i].push_back(finalCFG[j][0]);
                }
            }
        }
    }


    // removing duplicates from first sets
    remove_duplicate(FOLLOW);

    //resolving non_terminals to terminals
    resolving_non_terminals(FOLLOW);

    // removing duplicates from first sets
    remove_duplicate(FOLLOW);

    //for (int s = 0; s < FOLLOW.size(); s++) {
    //    cout << "===NEW FOLLOW LINEE====\n";
    //    for (int r = 0; r < FOLLOW[s].size(); r++) {
    //        cout << FOLLOW[s][r] << "\n";
    //    }
    //    cout << endl;
    //}




    //================================PARSING TABLE===============================================
    //initializing the 2d vector with space character that will indicate error state
    vector<vector<string>> M(NT.size() + 1, vector<string>((T.size() + 2), " ")); // Parsing table M
    vector<string> rule;

    for (int i = 0; i < finalCFG.size(); i++) {
        rule.push_back(finalCFG[i][0]);
        for (int j = 1; j < finalCFG[i].size(); j++) {
            rule[i] += " ";
            rule[i] += finalCFG[i][j];
        }
    }

    //leaving M[0][0] blank
    //giving column headings with terminals and $ sign in the end
    for (int i = 1; i < M[0].size() - 1; i++) {
        M[0][i] = T[i - 1];
    }
    M[0][M[0].size() - 1] = "$";

    for (int i = 1; i < M.size(); i++) {
        M[i][0] = NT[i - 1];
    }



    for (int i = 0; i < finalCFG.size(); i++) { //for each production
        for (int j = 0; j < NT.size(); j++) { //checking the right side
            if (NT[j] == finalCFG[i][0]) { //if right side is NT[j] then
                //if first of alpha is @
                if (finalCFG[i][1] == "@") {
                    for (int m = 0; m < FOLLOW[j].size(); m++) {
                        for (int n = 0; n < M[0].size(); n++) {
                            if (M[0][n] == FOLLOW[j][m]) { // for each terminal a in FIRST of A
                                M[j + 1][n] = rule[i];
                            }
                        }
                    }
                }
                //if first of alpha is terminal
                else if ((finalCFG[i][1][0] < 65 || finalCFG[i][1][0] > 90)) {
                    for (int l = 0; l < M[0].size() - 1; l++) {
                        if (M[0][l] == finalCFG[i][1]) { // for each terminal a in FIRST of A
                            M[j + 1][l] = rule[i];
                        }
                    }
                }
                //if first of alpha is non-terminal
                else {
                    for (int s = 0; s < NT.size(); s++) {
                        if (finalCFG[i][1] == NT[s]) {
                            for (int k = 0; k < FIRST[s].size(); k++) { // index j k non-terminal ka first
                                for (int l = 0; l < M[0].size() - 1; l++) {
                                    if (M[0][l] == FIRST[s][k]) { // for each terminal a in FIRST of A
                                        M[j + 1][l] = rule[i];
                                    }
                                }
                            }
                        }
                    }


                }
            }
        }
    }

    //writing parse table to file
    ofstream out("BSCS18072_ParseTable.txt");
    for (int i = 1; i < M.size(); i++) {
        for (int j = 1; j < M[i].size(); j++) {
            //cout << "M[" << M[i][0] << "," << M[0][j] << "] = " << M[i][j] << endl;
            out << "M[" << M[i][0] << "," << M[0][j] << "] = " << M[i][j] << endl;
        }
    }

    /*
    //=====================WRITING DRIVER PROGRAM===================
    
    string stringFile;
    cout << "Enter the name of the input file: ";
    cin >> stringFile;
    fstream fil;
    fil.open(stringFile);
    vector<stringstream> li;
    string _line; // to read file line by line

    //assumption, input file will be space separated

//-----------------reading line by line-----------------
    while (getline(fil, _line)) {
        li.push_back(stringstream(_line));
        //cout << l << endl;
    }
    string _word; //to read file word by word

    vector<vector<string>> codelines(li.size()); // 2D vector to store productions

    for (int i = 0; i < li.size(); i++) {
        int j = 0;  //j indicates the word number in each line
        while (getline(li[i], _word, ' ')) {
            codelines[i].push_back(_word);
        }
    }


    //for (int i = 0; i < codelines.size(); i++) {
    //    cout << endl;
    //    for (int j = 0; j < codelines[i].size(); j++) {
    //        cout << codelines[i][j] << "\t";

    //    }
    //}

/*
    stack<string> stck;
    string matched;
    string stack;
    string input;
    string action;
    string tos; //top of stack
    int ptr = 0;
    string atString;
    


    for (int i = 0; i < codelines.size(); i++) {
        clearStack(stck); // clearing stack to start with new string(new line)
        codelines[i].push_back("$"); // adding dollar at end of string
        stck.push("$"); //push dollar to stack
        stck.push(finalCFG[0][0]); // push starting symbol on stack

        //printing output
        cout << "\nMATCHED: " << matched;
        cout << "\nSTACK: " << printStack(stck);
        cout << "\nINPUT: " << vecToStr(codelines[i]);
        cout << "\nACTION: " << action;

        while (ptr < codelines[i].size()) { // while we have not reached the end of the string
            //checking in parser table
            for (int i = 1; i < M.size(); i++) {
                if (M[i][0] == stck.top()) {
                    for (int j = 1; j < M[i].size(); j++) {
                        if (M[0][j] == codelines[i][ptr]) {
                            if (M[i][j] == " ") { 
                                cout << "\nERROR\n"; // stop here
                            }
                            else {
                                for(int k = M[i][j].size(); k <)
                            }


                           
                        }

                    }
                }

            }
        }
    }

    /*
    for (int i = 1; i < M.size(); i++) {
        if (M[i][0] == stck.top()) {
            for (int j = 1; j < M[i].size(); j++) {
                if (M[0][j] == atString) {

                }
                
            }
        }

    }
    */
    

    

    return 0;
}

