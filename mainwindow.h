#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "game.h"
#include "gamegui.h"
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    void new_game();
    void saveProgress();
    void loadProgress();
    void solveSimple();
    void solveSimpleAll();
    void solveRecursive();
    void wrong();


private:
    Game* game_ = nullptr;
    GameGui* gui_ = nullptr;
    int cols_, rows_;
    QString filename_;

};

#endif // MAINWINDOW_H
