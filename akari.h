#ifndef AKARI_H
#define AKARI_H
#include "game.h"
#include <vector>
#include <complex>
#include <QString>
#include <QKeyEvent>
#include <mutex>

using namespace std;

class Akari: public Game
{
public:
    Akari(QString filename);
    void load_file();
    void play_at(int x, int y);
    void flag_at(int x, int y);
    string get_val(int x, int y);
    int bulbs_around(int x, int y);
    int possible_cells_around(int x, int y);
    void auto_wall(int x, int y);
    bool bulb_alone(int x, int y);
    void check_if_flag_alone(int x, int y);
    void board_combos();
    vector<int> find_first_playable();
    void solveSimple();
    void solveSimpleAll();
    bool solveRecursive();
    bool finished();
    bool wrong();
    bool wrongWall(int x, int y);
    bool wrongFlag(int x, int y);
    string to_string();
    void switchFlagMode();

    int cols() {return cols_; }
    int rows() { return rows_; }
    bool flagMode() { return flag_click_; }
    vector<vector<char>> board() { return board_; }
    vector<vector<int>> lights() { return lights_; }

    string message() { return "Puzzle solved"; }
    string messageFlag() {
        if(flagMode()) return "On"; else return "Off"; }



    static const char EMPTY = ' ';
    static const char BULB  = '@';
    static const char FLAG  = 'X';
    static const char LIGHT = '+';
//    static const char WALL0 = '0';
//    static const char WALL1 = '1';
//    static const char WALL2 = '2';
//    static const char WALL3 = '3';
//    static const char WALL4 = '4';
//    static const char WALL  = '5';

private:
    //once_flag onceFlag_;
    QString filename_;
    int cols_;
    int rows_;

    bool flag_click_ = false; // true when F is pressed, places Flag instead of Bulb
    // board_ is the one active, the others board_1 are not
    vector< vector<char> > board_;
    /*vector< vector<char> > board_ = {
        {' ', ' ', ' ', '5', '5', '1', ' '},
        {' ', '5', ' ', ' ', ' ', ' ', ' '},
        {'1', ' ', ' ', '5', ' ', '0', ' '},
        {' ', '5', ' ', ' ', ' ', '5', ' '},
        {' ', '1', ' ', '3', ' ', ' ', '2'},
        {' ', ' ', ' ', ' ', ' ', '2', ' '},
        {' ', '5', '5', '5', ' ', ' ', ' '} };
    vector< vector<char> > board_1 = {
        {'5', '5', ' ', ' ', '5', ' ', '2'},
        {' ', ' ', ' ', ' ', ' ', ' ', ' '},
        {' ', '1', ' ', ' ', ' ', '1', ' '},
        {' ', ' ', '2', ' ', '5', '5', ' '},
        {' ', '5', ' ', '1', ' ', ' ', ' '},
        {'1', ' ', ' ', ' ', ' ', '3', ' '},
        {' ', ' ', ' ', ' ', ' ', ' ', '5'} };*/


    //when -1, wall, when > 0, is number of bulbs lighting the cell

    /*vector< vector<int> > lights_ = {
        {0,  0,  0, -1, -1, -1,  0},
        {0, -1,  0,  0,  0,  0,  0},
        {-1, 0,  0, -1,  0, -1,  0},
        {0, -1,  0,  0,  0, -1,  0},
        {0, -1,  0, -1,  0,  0, -1},
        {0,  0,  0,  0,  0, -1,  0},
        {0, -1, -1, -1,  0,  0,  0} };*/
    vector< vector<int> > lights_;
    // '5' is for unnumbered walls:
    // no constraint on surrounding bulbs
};

#endif // AKARI_H
