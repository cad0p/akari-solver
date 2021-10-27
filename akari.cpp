#include "akari.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <QString>
#include <fstream>
#include <QKeyEvent>
#include <QFrame>
#include <mutex>

using namespace std;

vector<vector<int>> dirs4 = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
vector<vector<int>> diag4 = {{1, -1}, {1, 1}, {-1, 1}, {-1, -1}};

Akari::Akari(QString filename)
{
//    if(board == 0) board_ = board_1;
//    else board_ = board_;
    filename_ = filename;
    load_file();
    rows_ = board_.size(); cols_ = board_[0].size();
    // create the lights, assigning -1 for walls
    lights_.assign(rows_, vector<int>(cols_, 0));
    for (auto y = 0; y < rows_; ++y) {
        for (auto x = 0; x < cols_; ++x) {
            if('0' <= board_[y][x] && board_[y][x] <= '5') lights_[y][x] = -1;
        }
    }
    for(auto y = 0; y < rows_; ++y){
        for(auto x = 0; x < cols_; ++x){
           if(board_[y][x] == BULB){
               int count = 1;
               for (auto d : dirs4) {
                   auto mx = x + d[0], my = y + d[1];
                   while (0 <= mx && mx < cols_ && 0 <= my && my < rows_
                          && (board_[my][mx] == LIGHT || board_[my][mx] == EMPTY)) {
                       lights_[my][mx] += count;
                       mx += d[0]; my += d[1];
                   }
               }
           }
        }
    }
}

void Akari::load_file()
{
    ifstream in{filename_.toStdString()};
    if (in.good()) {
        string line;
        while(getline(in, line)){
            vector<char> row;
            for(char c: line){
                if(c == '.' || c == '-') c = EMPTY;
                row.push_back(c);
            }
            board_.push_back(row);
        }
    }
}

void Akari::play_at(int x, int y)
{
    /* ADD YOUR CODE HERE */
    if (0 <= x && x < cols_ && 0 <= y && y < rows_) {
        char val = 'c'; //inizializzato con valore che non crea problemi
        int count = 1;
        if (board_[y][x] == EMPTY ) val = BULB;
        else if (board_[y][x] == BULB) val = EMPTY;
        //else if (board_[y][x] == LIGHT) val = LIGHT;
        if(!(('0' <= board_[y][x] && board_[y][x] <= '5') ||
             board_[y][x] == LIGHT || board_[y][x] == FLAG)) board_[y][x] = val;    // if not wall or light or flag..

        if ('0' <= board_[y][x] && board_[y][x] <= '4') auto_wall(x, y);

        if(val == BULB){ // place lights when placing bulb
            val = LIGHT;
            for (auto d : dirs4) {
                auto mx = x + d[0], my = y + d[1];
                while (0 <= mx && mx < cols_ && 0 <= my && my < rows_
                       && (board_[my][mx] == EMPTY || board_[my][mx] == LIGHT ||
                           board_[my][mx] == FLAG)) {
                    board_[my][mx] = val;
                    lights_[my][mx] += count;
                    mx += d[0]; my += d[1];
                }
            }
        }
        else if(val == EMPTY){ //when deleting bulb
            val = EMPTY;
            for (auto d : dirs4) {
                auto mx = x + d[0], my = y + d[1];
                while (0 <= mx && mx < cols_ && 0 <= my && my < rows_
                       && (board_[my][mx] == LIGHT || board_[my][mx] == EMPTY)) {
                    lights_[my][mx] -= count;
                    if(lights_[my][mx] == 0) board_[my][mx] = val;
                    mx += d[0]; my += d[1];
                }
            }
        }
    }
}

void Akari::flag_at(int x, int y)
{
    if (0 <= x && x < cols_ && 0 <= y && y < rows_) {
        if(board_[y][x] == EMPTY) board_[y][x] = FLAG;
        else if(board_[y][x] == FLAG) board_[y][x] = EMPTY;
    }
}

std::string Akari::get_val(int x, int y)
{
    // conversion from char to string
    // char val = '5';  /* just for example ... */
    // string txt = string(1, val);
    
    /* ADD YOUR CODE HERE */
    if (0 <= x && x < cols_ && 0 <= y && y < rows_) {
        char val = board_[y][x]; string txt = string(1, val);

        if (board_[y][x] != EMPTY) return txt;

    }
    return "";
}

bool Akari::finished()
{
    // conversion from char to int
    // char val = '4';  /* just for example ... */
    // int num = val - '0';
    
    /* ADD YOUR CODE HERE */
    for (auto y = 0; y < rows_; ++y) {
        for (auto x = 0; x < cols_; ++x) {
            auto val = this->get_val(x, y);

            if (!(
                    ("0" == val && this->bulbs_around(x, y) == 0) ||
                    ("1" == val && this->bulbs_around(x, y) == 1) ||
                    ("2" == val && this->bulbs_around(x, y) == 2) ||
                    ("3" == val && this->bulbs_around(x, y) == 3) ||
                    ("4" == val && this->bulbs_around(x, y) == 4) ||
                    ("@" == val) || ("+" == val) || ("5" == val)
                 )) return false;
        }
    }
    return true;
}

bool Akari::wrong()
{
    if(finished()) return false;
    for (auto y = 0; y < rows_; ++y) {
        for (auto x = 0; x < cols_; ++x) {
        char cell = board_[y][x];
        if('0' <= cell && cell <= '4' && wrongWall(x, y)) return true;
        else if(cell == FLAG && wrongFlag(x, y)) return true;
        }
    }
    return false;
}

bool Akari::wrongWall(int x, int y)
{
    char cell = board_[y][x];
    int wall = cell - '0';
    if(bulbs_around(x, y) > wall ||
            possible_cells_around(x, y) < wall - bulbs_around(x, y)) return true;
    else return false;
}

bool Akari::wrongFlag(int x, int y)
{
    // returns true when a flag has no empty places to be lit around
    for (auto d : dirs4) {
        auto mx = x + d[0], my = y + d[1];
        while (0 <= mx && mx < cols_ && 0 <= my && my < rows_
               && (board_[my][mx] == EMPTY || board_[my][mx] == LIGHT ||
                   board_[my][mx] == FLAG)) { // go until wall..
            if(board_[my][mx] == EMPTY) return false;
            mx += d[0]; my += d[1];
        }
    }
    return true;
}

string Akari::to_string()
{
    /* ADD YOUR CODE HERE */
    return "";
}

void Akari::switchFlagMode()
{
    if(flag_click_ == true) flag_click_ = false;
    else flag_click_ = true;
}

int Akari::bulbs_around(int x, int y)
{
    if (0 <= x && x < cols_ && 0 <= y && y < rows_){
        int count = 0;
        for(auto d: dirs4){
            int mx = x + d[0], my = y + d[1];
            if(0 <= mx && mx < cols_ && 0 <= my && my < rows_){
                if(board_[my][mx] == BULB) count += 1;
            }
        }
        return count;
    }
    return 0;
}

int Akari::possible_cells_around(int x, int y)
{
    if (0 <= x && x < cols_ && 0 <= y && y < rows_){
        int count = 0;
        for(auto d: dirs4){
            int mx = x + d[0], my = y + d[1];
            if(0 <= mx && mx < cols_ && 0 <= my && my < rows_){
                if(board_[my][mx] == EMPTY) count += 1;
            }
        }
        return count;
    }
    return 0;
}

void Akari::auto_wall(int x, int y) // for clicks on walls
{
    if (board_[y][x] == '4') {
        for(auto d: dirs4){
            int mx = x + d[0], my = y + d[1];
            if(0 <= mx && mx < cols_ && 0 <= my && my < rows_){
                if (board_[my][mx] == EMPTY) play_at(mx, my);
            }

        }
    }

    else if (board_[y][x] == '0') {
        for(auto d: dirs4){
            int mx = x + d[0], my = y + d[1];
            if(0 <= mx && mx < cols_ && 0 <= my && my < rows_){
                //if (board_[my][mx] == BULB) {play_at(mx, my); flag_at(mx, my); }
                if(board_[my][mx] == EMPTY) flag_at(mx, my);
            }

        }
    }

    else if (board_[y][x] == '1') {
        if (bulbs_around(x, y) == 1){
            for(auto d: dirs4){
                int mx = x + d[0], my = y + d[1];
                if(0 <= mx && mx < cols_ && 0 <= my && my < rows_&&
                        board_[my][mx] == EMPTY) flag_at(mx, my);
            }
        }
//        else if(bulbs_around(x, y) > 1){
//            for(auto d: dirs4){
//                int mx = x + d[0], my = y + d[1];
//                if (0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
//                        board_[my][mx] == BULB) play_at(mx, my); // remove bulbs

//            }
//        }
        else if(bulbs_around(x, y) == 0 && possible_cells_around(x, y) == 1){
            for(auto d: dirs4){
                int mx = x + d[0], my = y + d[1];
                if(0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
                        board_[my][mx] == EMPTY) play_at(mx, my);
            }
        }
    }
    else if (board_[y][x] == '2') {
        if (bulbs_around(x, y) == 2){
            for(auto d: dirs4){
                int mx = x + d[0], my = y + d[1];
                if(0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
                        board_[my][mx] == EMPTY) flag_at(mx, my);
            }
        }
//        else if(bulbs_around(x, y) > 2){
//            for(auto d: dirs4){
//                int mx = x + d[0], my = y + d[1];
//                if (0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
//                        board_[my][mx] == BULB) play_at(mx, my); // remove bulbs

//            }
//        }
        else if(bulbs_around(x, y) == 1 && possible_cells_around(x, y) == 1){
            for(auto d: dirs4){
                int mx = x + d[0], my = y + d[1];
                if (0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
                        board_[my][mx] == EMPTY) play_at(mx, my);
            }
        }
        else if(bulbs_around(x, y) == 0 && possible_cells_around(x, y) == 2){
            for(auto d: dirs4){
                int mx = x + d[0], my = y + d[1];
                if(0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
                        board_[my][mx] == EMPTY) play_at(mx, my);
            }
        }
    }
    else if (board_[y][x] == '3') {
        if (bulbs_around(x, y) == 3){
            for(auto d: dirs4){
                int mx = x + d[0], my = y + d[1];
                if(0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
                        board_[my][mx] == EMPTY) flag_at(mx, my);
            }
        }
//        else if(bulbs_around(x, y) > 3){
//            for(auto d: dirs4){
//                int mx = x + d[0], my = y + d[1];
//                if (0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
//                        board_[my][mx] == BULB) play_at(mx, my); // remove bulbs

//            }
//        }
        else if(bulbs_around(x, y) == 2 && possible_cells_around(x, y) == 1){
            for(auto d: dirs4){
                int mx = x + d[0], my = y + d[1];
                if (0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
                        board_[my][mx] == EMPTY) play_at(mx, my);
            }
        }
        else if(bulbs_around(x, y) == 1 && possible_cells_around(x, y) == 2){
                for(auto d: dirs4){
                    int mx = x + d[0], my = y + d[1];
                    if (0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
                            board_[my][mx] == EMPTY) play_at(mx, my);
                }
        }
        else if(bulbs_around(x, y) == 0 && possible_cells_around(x, y) == 3){
            for(auto d: dirs4){
                int mx = x + d[0], my = y + d[1];
                if(0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
                        board_[my][mx] == EMPTY) play_at(mx, my);
            }
        }
    }
}

bool Akari::bulb_alone(int x, int y)
{
    for (auto d : dirs4) {
        auto mx = x + d[0], my = y + d[1];
        while (0 <= mx && mx < cols_ && 0 <= my && my < rows_
               && (board_[my][mx] == EMPTY || board_[my][mx] == LIGHT ||
                   board_[my][mx] == FLAG)) { // go until wall..
            if(board_[my][mx] == EMPTY) return false;
            mx += d[0]; my += d[1];
        }
    }
    return true;
}

void Akari::check_if_flag_alone(int x, int y)
{
    int count = 0;
    int bulb_x, bulb_y, bulb2_x, bulb2_y;
    for (auto d : dirs4) {
        auto mx = x + d[0], my = y + d[1];
        while (0 <= mx && mx < cols_ && 0 <= my && my < rows_
               && (board_[my][mx] == EMPTY || board_[my][mx] == LIGHT ||
                   board_[my][mx] == FLAG)) { // go until wall..
            if(board_[my][mx] == EMPTY) {
                ++count;
                if(count == 1) {bulb_y = my; bulb_x = mx;}
                else if(count == 2) {bulb2_y = my; bulb2_x = mx;}
                if(count > 2) return;
            }
            mx += d[0]; my += d[1];
        }
    }
    if(count == 1) { // it means that there's only a place to put a bulb
        play_at(bulb_x, bulb_y);
    }
    else if(count == 2){
        int delta_bulb_x = (bulb_x - x), delta_bulb_y = (bulb_y - y),
                delta_bulb2_x = (bulb2_x - x), delta_bulb2_y = (bulb2_y - y),
                delta_x, delta_y;
        if(delta_bulb_x != 0) delta_x = delta_bulb_x;
        else if(delta_bulb2_x != 0) delta_x = delta_bulb2_x;
        else return;

        if(delta_bulb_y != 0) delta_y = delta_bulb_y;
        else if(delta_bulb2_y != 0) delta_y = delta_bulb2_y;
        else return;

        if(delta_x > 0){
            for(int i = 0; i < delta_x; ++i){
                if('0' <= board_[y + delta_y][x + i] &&
                        board_[y + delta_y][x + i] <= '5') return;
            }
        }
        else{
            for(int i = 0; i > delta_x; --i){
                if('0' <= board_[y + delta_y][x + i] &&
                        board_[y + delta_y][x + i] <= '5') return;
            }
        }

        if(delta_y > 0){
            for(int i = 0; i < delta_y; ++i){
                if('0' <= board_[y + i][x + delta_x] &&
                        board_[y + i][x + delta_x] <= '5') return;
            }
        }
        else{
            for(int i = 0; i > delta_y; --i){
                if('0' <= board_[y + i][x + delta_x] &&
                        board_[y + i][x + delta_x] <= '5') return;
            }
        }
        if(board_[y + delta_y][x + delta_x] == EMPTY) flag_at(x + delta_x, y + delta_y);
    }
}

void Akari::board_combos()
{
    for(auto y = 0; y < rows_; ++y){
        for(auto x = 0; x < cols_; ++x){
            char cell = board_[y][x];
            if(cell == '3'){
                for(auto d: diag4){
                    int mx = x + d[0], my = y + d[1];
                    if(0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
                            board_[my][mx] == EMPTY) flag_at(mx, my);
                    if(0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
                            board_[my][mx] == '1'){
                        //put flags near the 1
                        int flag_x1 = mx + d[0], flag_y2 = my + d[1];
                        if(0 <= flag_x1 && flag_x1 < cols_ && 0 <= my && my < rows_ &&
                                board_[my][flag_x1] == EMPTY)
                            flag_at(flag_x1, my);
                        if(0 <= mx && mx < cols_ && 0 <= flag_y2 && flag_y2 < rows_ &&
                                board_[flag_y2][mx] == EMPTY)
                            flag_at(mx, flag_y2);
                        // put bulbs opposite side of 1
                        int bulb_x1 = x - d[0], bulb_y2 = y - d[1];
                        if(0 <= bulb_x1 && bulb_x1 < cols_ && 0 <= y && y < rows_ &&
                                board_[y][bulb_x1] == EMPTY)
                            play_at(bulb_x1, y);
                        if(0 <= x && x < cols_ && 0 <= bulb_y2 && bulb_y2 < rows_ &&
                                board_[bulb_y2][x] == EMPTY)
                            play_at(x, bulb_y2);
                    }
                }
            }
        }
    }
}

vector<int> Akari::find_first_playable()
{
    for(auto y = 0; y < rows_; ++y){
        for(auto x = 0; x < cols_; ++x){
            char cell = board_[y][x];
            if(cell == EMPTY) return {x, y};
        }
    }
    return{-1, -1};
}


void Akari::solveSimple()
{
    //call_once(onceFlag_, &Akari::board_combos, this);
    board_combos();
    for(auto y = 0; y < rows_; ++y){
        for(auto x = 0; x < cols_; ++x){
            char cell = board_[y][x];
            if('0' <= cell && cell <= '4') auto_wall(x, y);
            else if(cell == EMPTY && bulb_alone(x, y)) play_at(x, y);
            else if(cell == FLAG) check_if_flag_alone(x,y);

            if('1' <= cell && cell <= '3' && (cell - '0' - bulbs_around(x, y) == 1) && possible_cells_around(x, y) == 2){ // 1-0, 2-1, 3-2
                for(auto d: diag4){
                    int mx = x + d[0], my = y + d[1];
                    if(0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
                            (board_[my][mx] == '1' || board_[my][mx] == '2' || board_[my][mx] == '3') &&
                            possible_cells_around(mx, my) >= 2){
                        int empty_x1 = x +  d[0], empty_y2 = y + d[1];
                        if(board_[y][empty_x1] == EMPTY && board_[empty_y2][x] == EMPTY){

                            if(board_[my][mx] == '1' || (board_[my][mx] == '2' && bulbs_around(mx, my) == 1)){
                                if(0 <= mx + d[0] && mx + d[0] < cols_ && 0 <= my && my < rows_ &&
                                        board_[my][mx + d[0]] == EMPTY) flag_at(mx + d[0], my);
                                if(0 <= mx && mx < cols_ && 0 <= my + d[1] && my + d[1] < rows_ &&
                                        board_[my + d[1]][mx] == EMPTY) flag_at(mx, my + d[1]);
                            }

                            else if(board_[my][mx] == '3' ||
                                    (board_[my][mx] == '2' && bulbs_around(mx, my) == 0 && possible_cells_around(mx, my) == 3)){
                                if(0 <= mx + d[0] && mx + d[0] < cols_ && 0 <= my && my < rows_ &&
                                        board_[my][mx + d[0]] == EMPTY) play_at(mx + d[0], my);
                                if(0 <= mx && mx < cols_ && 0 <= my + d[1] && my + d[1] < rows_ &&
                                        board_[my + d[1]][mx] == EMPTY) play_at(mx, my + d[1]);
                            }

                        }
                    }
                }
            }

            else if(cell == '1' && possible_cells_around(x, y) >= 3){
                for(auto d: diag4){
                    int mx = x + d[0], my = y + d[1];
                    if(0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
                            board_[my][mx] == '2' && possible_cells_around(mx, my) == 3 && bulbs_around(mx, my) == 0){
                        // if it finds a 2 with a FLAG or a LIGHT
                        int empty_x1 = x +  d[0], empty_y2 = y + d[1];
                        if(board_[y][empty_x1] == EMPTY && board_[empty_y2][x] == EMPTY){
                            if(0 <= mx + d[0] && mx + d[0] < cols_ && 0 <= my && my < rows_ &&
                                    board_[my][mx + d[0]] == EMPTY) play_at(mx + d[0], my);
                            if(0 <= mx && mx < cols_ && 0 <= my + d[1] && my + d[1] < rows_ &&
                                    board_[my + d[1]][mx] == EMPTY) play_at(mx, my + d[1]);
                        }
                    }
                }
            }
            // only one case needed, using the one with no bulbs, then becomes a problem of possible_cells_around == 2
            else if(cell == '2' && possible_cells_around(x, y) == 3 && bulbs_around(x, y) == 0){
                for(auto d: diag4){
                    int mx = x + d[0], my = y + d[1];
                    if(0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
                            board_[my][mx] == '2' && possible_cells_around(mx, my) == 3 && bulbs_around(mx, my) == 1){
                        int empty_x1 = x +  d[0], empty_y2 = y + d[1];
                        if(board_[y][empty_x1] == EMPTY && board_[empty_y2][x] == EMPTY){
                            if(0 <= mx + d[0] && mx + d[0] < cols_ && 0 <= my && my < rows_ &&
                                    board_[my][mx + d[0]] == EMPTY) flag_at(mx + d[0], my);
                            if(0 <= mx && mx < cols_ && 0 <= my + d[1] && my + d[1] < rows_ &&
                                    board_[my + d[1]][mx] == EMPTY) flag_at(mx, my + d[1]);
                        }
                    }
                }
            }

            else if(cell == '2' && possible_cells_around(x, y) == 3 && bulbs_around(x, y) == 1){
                for(auto d: diag4){
                    int mx = x + d[0], my = y + d[1];
                    if(0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
                            board_[my][mx] == '3'){
                        int empty_x1 = x +  d[0], empty_y2 = y + d[1];
                        if(board_[y][empty_x1] == EMPTY && board_[empty_y2][x] == EMPTY){
                            if(0 <= mx + d[0] && mx + d[0] < cols_ && 0 <= my && my < rows_ &&
                                    board_[my][mx + d[0]] == EMPTY) play_at(mx + d[0], my);
                            if(0 <= mx && mx < cols_ && 0 <= my + d[1] && my + d[1] < rows_ &&
                                    board_[my + d[1]][mx] == EMPTY) play_at(mx, my + d[1]);
                        }
                    }
                }
            }

            if('1' <= cell && cell <= '2' && cell - '0' > bulbs_around(x, y) && possible_cells_around(x, y) == cell - '0' - bulbs_around(x, y) + 1){
                // flag diagonal cells that impede the cell to be completed
                for(auto d: diag4){
                    int mx = x + d[0], my = y + d[1];
                    if(0 <= mx && mx < cols_ && 0 <= my && my < rows_ &&
                            board_[my][x] == EMPTY && board_[y][mx] == EMPTY &&
                            board_[my][mx] == EMPTY) flag_at(mx, my);
                }
            }
        }
    }
}

void Akari::solveSimpleAll()
{
    auto old_board = board_;
    do{
        old_board = board_;
        solveSimple();
    } while(board_ != old_board);
}

bool Akari::solveRecursive()
{
    solveSimpleAll();
    auto pos = find_first_playable();  // vector of 2 ints
    auto x = pos[0], y = pos[1];
    if (x >= 0 && y >= 0 && !wrong()) {
        auto copy = *this;  // save current status
        for (auto sign : {BULB, FLAG}) {
            if(sign == FLAG) flag_at(x, y);
            else if (sign == BULB) play_at(x, y);
            if (solveRecursive()) return true;
            *this = copy;  // backtracking
        }
    }
    return finished();
}

/*void Akari::keyPressEvent(QKeyEvent* e) {
    switch (e->key()) {
    case Qt::Key_F:
        if(flag_click_ == true) flag_click_ = false;
        else flag_click_ = true;
        break;

    case Qt::Key_Down:
    break;
        default:
        QWidget::keyPressEvent(e);
    }
}*/
