/**
 * @author Michele Tomaiuolo - http://www.ce.unipr.it/people/tomamic
 * @license This software is free - http://www.gnu.org/licenses/gpl.html
 */

#include "gamegui.h"

#include <QGridLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QFrame>
#include <QKeyEvent>
#include <iostream>

GameGui::GameGui(Game* game)
{
    game_ = game;
    cols_ = game->cols();
    rows_ = game->rows();

    auto grid = new QGridLayout; setLayout(grid);

    for (auto y = 0; y < rows_; ++y) {
        for (auto x = 0; x < cols_; ++x) {
            auto b = new QPushButton;
            b->setFixedSize(30, 30);
            grid->addWidget(b, y, x);
            connect(b, &QPushButton::clicked,
                    [=]{ handle_click(x, y); });
        }
    }
    update_all_buttons();

    // fix appearance
//    layout()->setMargin(11); // 11 for MacOS
    layout()->setSpacing(11); // ''
    setFixedSize(sizeHint());
}

/*void GameGui::open_file()
{
    auto filename = QFileDialog::getOpenFileName(this);
    if (filename != "") {
        ifstream in{filename.toStdString()};
        if (in.good()) {

        }
        else {
            QMessageBox::critical(this, tr("Error"),
                                  tr("Could not open file"));
        }
}*/



void GameGui::update_button(int x, int y)
{
    auto val = game_->get_val(x, y);
    auto b = layout()->itemAt(y * cols_ + x)->widget();
    dynamic_cast<QPushButton*>(b)->setText(val.c_str());

    if ("0" == val && game_->bulbs_around(x, y) == 0) b->setStyleSheet("background: black; color: gray;");
    else if ("0" == val && game_->bulbs_around(x, y) > 0) b->setStyleSheet("background: black; color: red;");

    else if ("1" == val && game_->bulbs_around(x, y) == 0) b->setStyleSheet("background: black; color: white;");
    else if("1" == val && game_->bulbs_around(x, y) == 1) b->setStyleSheet("background: black; color: gray;");
    else if ("1" == val && game_->bulbs_around(x, y) > 1) b->setStyleSheet("background: black; color: red;");

    else if("2" == val && game_->bulbs_around(x, y) == 0) b->setStyleSheet("background: black; color: white;");
    else if("2" == val && game_->bulbs_around(x, y) > 0 && game_->bulbs_around(x, y) < 2 ) b->setStyleSheet("background: black; color: aqua;");
    else if("2" == val && game_->bulbs_around(x, y) == 2) b->setStyleSheet("background: black; color: gray;");
    else if ("2" == val && game_->bulbs_around(x, y) > 2) b->setStyleSheet("background: black; color: red;");

    else if("3" == val && game_->bulbs_around(x, y) == 0) b->setStyleSheet("background: black; color: white;");
    else if("3" == val && game_->bulbs_around(x, y) > 0 && game_->bulbs_around(x, y) < 3 ) b->setStyleSheet("background: black; color: aqua;");
    else if("3" == val && game_->bulbs_around(x, y) == 3) b->setStyleSheet("background: black; color: gray;");
    else if ("3" == val && game_->bulbs_around(x, y) > 3) b->setStyleSheet("background: black; color: red;");

    else if("4" == val && game_->bulbs_around(x, y) == 0) b->setStyleSheet("background: black; color: white;");
    else if("4" == val && game_->bulbs_around(x, y) > 0 && game_->bulbs_around(x, y) < 4 ) b->setStyleSheet("background: black; color: aqua;");
    else if("4" == val && game_->bulbs_around(x, y) == 4) b->setStyleSheet("background: black; color: gray;");

    else if ("5" == val) b->setStyleSheet("background: black; color: black;");
    else if ("@" == val) b->setStyleSheet("background: red; color: red;");
    else if ("+" == val) b->setStyleSheet("background: yellow; color: yellow;");
    else b->setStyleSheet("background: white; color: black;");
}

void GameGui::update_all_buttons()
{
    for (auto y = 0; y < rows_; y++) {
        for (auto x = 0; x < cols_; x++) {
            update_button(x, y);
        }
    }
}

void GameGui::handle_click(int x, int y)
{
    if(game_->flagMode())
        game_->flag_at(x, y);
    else game_->play_at(x, y);
    update_all_buttons();

    if (game_->finished()) {
        QMessageBox::information(this,
                                 tr("Game finished"),
                                 tr(game_->message().c_str()));
        //window()->close();
    }
}

void GameGui::keyPressEvent(QKeyEvent* e) {
    std::string str = to_string(e->key());
    QMessageBox::information(this,
                             tr("Flag Mode:"),
                             tr(str.c_str()));
    switch (e->key()) {
    case Qt::Key_F:
        game_->switchFlagMode();
        QMessageBox::information(this,
                                 tr("Flag Mode:"),
                                 tr(game_->messageFlag().c_str()));
        break;

    case Qt::Key_Down:
    break;
        default:
        QWidget::keyPressEvent(e);
    }
}
