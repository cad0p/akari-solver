#ifndef GAME_H
#define GAME_H

#include <string>
#include <vector>

using namespace std;
class Game
{
public:
    virtual void play_at(int x, int y) = 0;
    virtual void flag_at(int x, int y) = 0;
    virtual void switchFlagMode() = 0;
    virtual int cols() = 0;
    virtual int rows() = 0;
    virtual vector<vector<char>> board() = 0;
    virtual vector<vector<int>> lights() = 0;
    virtual int bulbs_around(int x, int y) = 0;
    virtual std::string get_val(int x, int y) = 0;
    virtual bool finished() = 0;
    virtual bool wrong() = 0;
    virtual bool flagMode() = 0;
    virtual std::string message() = 0;
    virtual std::string messageFlag() = 0;

    virtual void solveSimple() = 0;
    virtual void solveSimpleAll() = 0;
    virtual bool solveRecursive() = 0;

    virtual ~Game() {}
};

#endif // GAME_H
