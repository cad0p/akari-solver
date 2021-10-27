#include "mainwindow.h"
#include "gamegui.h"
#include "akari.h"

#include <iostream>
#include <fstream>
#include <QMenuBar>
#include <QMenu>
#include <QString>
#include <QAction>
#include <QInputDialog>
#include <QGridLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <sstream>
#include <string>


using namespace std;

MainWindow::MainWindow()
{
    //cols_ = 3; rows_ = 4;
    setWindowTitle(tr("Akari"));
    auto menu_game = menuBar()->addMenu(tr("Game"));
    auto game_act = menu_game->addAction(tr("New game"));
    auto load_progress_act = menu_game->addAction(tr("Load Progress"));
    auto menu_save = menuBar()->addMenu(tr("Save"));
    auto save_act = menu_save->addAction(tr("Save progress"));
    auto menu_solve = menuBar()->addMenu(tr("Solve"));
    auto solvesimple_act = menu_solve->addAction(tr("Solve Simple"));
    auto solvesimpleall_act = menu_solve->addAction(tr("Solve Simple All"));
    auto solverecursive_act = menu_solve->addAction(tr("Solve Recursive"));
    auto menu_help = menuBar()->addMenu(tr("?"));
    auto wrong_act = menu_help->addAction(tr("Wrong?"));

    connect(game_act, &QAction::triggered,
            this, &MainWindow::new_game);
//    connect(load_progress_act, &QAction::triggered,
//            this, &MainWindow::load_progress)
    connect(save_act, &QAction::triggered,
            this, &MainWindow::saveProgress);
    connect(load_progress_act, &QAction::triggered,
            this, &MainWindow::loadProgress);
    connect(solvesimple_act, &QAction::triggered,
            this, &MainWindow::solveSimple);
    connect(solvesimpleall_act, &QAction::triggered,
            this, &MainWindow::solveSimpleAll);
    connect(solverecursive_act, &QAction::triggered,
            this, &MainWindow::solveRecursive);
    connect(wrong_act, &QAction::triggered,
            this, &MainWindow::wrong);
    new_game();
}

void MainWindow::new_game()
{
    if (centralWidget() != nullptr) delete centralWidget();
    if (game_ != nullptr) delete game_;

    // choose the input file
    filename_ = QFileDialog::getOpenFileName(this, tr("Select a level to open"),
                                                    "../akari/games/",
                                                    tr("Text Files (*.txt)"));


    if (filename_ != ""){
        string filename = filename_.toStdString();
        size_t path = filename.find_last_of("/");
        size_t lastindex = filename.find_last_of(".");
        string title = "Akari - " + filename.substr(path + 1, lastindex - (path + 1));
        setWindowTitle(tr(title.c_str()));
        game_ = new Akari{filename_}; //new Queens{8};
        gui_ = new GameGui{game_};
        setCentralWidget(gui_);
    }
    else {
        QMessageBox::critical(this, tr("Error"),
                               tr("Could not open file"));
        if(game_ == nullptr) new_game();
        else {game_ = 0; new_game();}
    }
    // fix appearance
    adjustSize();
    setFixedSize(sizeHint());
}

void MainWindow::saveProgress()
{
    string filename = filename_.toStdString();
    size_t path = filename.find_last_of("/");
    size_t lastindex = filename.find_last_of(".");
    string filesave;
    //8 is len("Progress")
    if(filename.substr(path - 8, 8) == "Progress") filesave = filename; //rw file
    else filesave = filename.substr(0, path) + "/Progress" + filename.substr(path, lastindex - path) + "_progress.txt";
    ofstream myfile;
      myfile.open (filesave);
      auto board = game_->board();
      auto lights = game_->lights();
      string sboard, slights;
      for(auto row: board){
          for(auto c: row) {
              if(c != ' ')sboard += string(1, c);
              else sboard += " ";
          }
          sboard += "\n";
      }

      myfile << sboard;
      myfile.close();

      //games/Progress  "../akari/games/Progress"
}

void MainWindow::loadProgress()
{
    string filename = filename_.toStdString();
    size_t path = filename.find_last_of("/");
    size_t lastindex = filename.find_last_of(".");
    if(filename.substr(path - 8, 8) == "Progress") filename = filename; // load same file
    else filename = filename.substr(0, path) + "/Progress" + filename.substr(path, lastindex - path) + "_progress.txt";
    ifstream in{filename};
    if(in.good()){
        string title = "Akari - " + filename.substr(path + 1, lastindex - (path + 1));
        setWindowTitle(tr(title.c_str()));
        filename_ = QString::fromStdString(filename);
        game_ = new Akari{filename_};
        gui_ = new GameGui{game_};
        setCentralWidget(gui_);
    }
    else {
        QMessageBox::critical(this, tr("Error"),
                               tr("Could not find progress for this level"));
    }
}

void MainWindow::solveSimple()
{
    game_->solveSimple();
    gui_->update_all_buttons();
    if (game_->finished()) {
        QMessageBox::information(this,
                                 tr("Game finished"),
                                 tr(game_->message().c_str()));
    }
}

void MainWindow::solveSimpleAll()
{
    game_->solveSimpleAll();
    gui_->update_all_buttons();
    if (game_->finished()) {
        QMessageBox::information(this,
                                 tr("Game finished"),
                                 tr(game_->message().c_str()));
    }
}

void MainWindow::solveRecursive()
{
    game_->solveSimpleAll();
    gui_->update_all_buttons();
    if (game_->finished()) {
        QMessageBox::information(this,
                                 tr("Game finished"),
                                 tr("This game was 'Solve Simplable'! :)"));
        return;
    }
    if(game_->solveRecursive()){
        gui_->update_all_buttons();
        QMessageBox::information(this,
                                 tr("Game finished"),
                                 tr(game_->message().c_str()));
    }
    else {
        gui_->update_all_buttons();
        QMessageBox::critical(this, tr("Error"),
                               tr("Could not complete this level recursively"));
    }
}

void MainWindow::wrong()
{
    if(game_->wrong()){
        QMessageBox::critical(this, tr("Wrong"),
                               tr("You have made an error"));
    }
    else {
        QMessageBox::information(this,
                                 tr("Not wrong!"),
                                 tr("You did not do critical errors!"));
    }
}

