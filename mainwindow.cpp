#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidget>
#include <QMessageBox>
#include <iostream>
#include <vector>
#include <cmath>
#include <QString>
#include <algorithm>


int step = 0;

std::vector <std::vector <QString>> table; // таблица
int index;
int variebles_num;  //
QString result;

std::vector <QString> letters; // названия колонок
std::vector <std::vector <int>> state;

std::vector <QString> left_letters;

std::vector <QString> MDNF;
int MDNF_size = 1e9;

int table_sizeY;
int table_sizeX;

void Generate_letters(QString prefix, int n) {
    if (n == 0) {
        letters.push_back(prefix);
    } else {
        Generate_letters(prefix + "1", n - 1);
        Generate_letters(prefix + "0", n - 1);
    }
}

void Generate_table(QString prefix, int n) {
    if (n == 0) {
        for (int i = 0; i < variebles_num; ++i) {
            table[index][i] = prefix[i];
        }
        index++;
    } else {
        Generate_table(prefix + "0", n - 1);
        Generate_table(prefix + "1", n - 1);
    }
}

bool f(QString lhs, QString rhs) {
    if (lhs.size() < rhs.size()) return true;
    else if (lhs.size() > rhs.size()) return false;
    return lhs < rhs;
}

void MakeTable() {

    Generate_letters("", variebles_num);

    letters.resize(letters.size() - 1);
    for (unsigned int i = 0; i < letters.size(); ++i) {
        QString elem = letters[i];
        QString tmp = "";
        for (int j = 0; j < elem.size(); ++j) {
            if (elem[j] == '1') {
                tmp = tmp + (char)('a' + j);
            }
        }
        letters[i] = tmp;
    }

    std::sort(letters.begin(), letters.end(), f);

    table_sizeY = pow(2, variebles_num);
    table_sizeX = letters.size();
    table.resize(table_sizeY, std::vector<QString>(table_sizeX, ""));
    state.resize(table_sizeY, std::vector<int>(table_sizeX, 0));
    Generate_table("", variebles_num);

    for (int i = variebles_num; i < table_sizeX; ++i) {
        for (int j = 0; j < table_sizeY; ++j) {
            for (int symb = 0; symb < letters[i].size(); ++symb) {
                int num_column = letters[i][symb].unicode() - (int)'a';
                table[j][i] += table[j][num_column];
            }
        }
    }

}

void DrawTable(Ui::MainWindow* ui) {

    ui->tableWidget->setColumnCount(table_sizeX);
    ui->tableWidget->setRowCount(table_sizeY);
    for (unsigned int i = 0; i < letters.size(); ++i) {
        QString elem = letters[i];
        QTableWidgetItem* itm = new QTableWidgetItem(elem);
        ui->tableWidget->setHorizontalHeaderItem(i, itm);
    }

    for (int i = 0; i < table_sizeY; ++i) {
        QString elem = "" + result[i];
        QTableWidgetItem* itm = new QTableWidgetItem(elem);
        ui->tableWidget->setVerticalHeaderItem(i, itm);
    }

    for (int i = 0; i < table_sizeY; ++i) {
        for (int j = 0; j < table_sizeX; ++j) {
            QString s = table[i][j];
            QTableWidgetItem* itm = new QTableWidgetItem(s);
            ui->tableWidget->setItem(i, j, itm);
        }
    }
}

void StepOne(Ui::MainWindow* ui) {
    QString string = ui->textEdit->toPlainText();

    for (int i = 0; i < result.size(); ++i) {
        if (result[i] == '0') {
            for (int j = 0; j < table_sizeX; ++j) {
                state[i][j] = 1;
                ui->tableWidget->item(i, j)->setBackground(Qt::darkCyan);
                QString el = table[i][j];
                for (int k = 0; k < table_sizeY; ++k) {
                    if (table[k][j] == el) {
                        state[k][j] = 1;
                        ui->tableWidget->item(k, j)->setBackground(QColor(Qt::blue).lighter(150));
                    }
                }
            }
        }
    }
}

bool CheckFound(int coordY, int coordX1, int coordX2) {
    if (state[coordY][coordX2] != 0)
        return false;
    QString short_string = letters[coordX1];
    QString long_string = letters[coordX2];
    if (short_string.size() >= long_string.size()) {
        return false;
    }
    QString combination_short = table[coordY][coordX1];
    QString combination_long = table[coordY][coordX2];

    for (int i = 0; i < short_string.size(); ++i) {
        bool flag = false;
        for (int j = 0; j < long_string.size(); ++j) {
            if (short_string[i] == long_string[j]) {
                flag = true;
                if (combination_short[i] != combination_long[j])
                    return false;
            }
        }
        if (!flag)
            return false;
    }
    return true;
}

void StepTwo(Ui::MainWindow* ui) {
    for (int i = 0; i < table_sizeY; ++i) {
        for (int j = 0; j < table_sizeX; ++j) {
            if (state[i][j] == 0) {
                QString short_string = letters[j];
                for (int k = j + 1; k < table_sizeX; ++k) {
                    QString long_string = letters[k];
                    if (CheckFound(i, j, k)) {
                        state[i][k] = 2;
                        ui->tableWidget->item(i, k)->setBackground(QColor(Qt::magenta).lighter(150));
                    }
                }
            }
        }
    }
}

bool GetResultOfDNF(QString prefix, int row) {
    bool res = false;
    int prefix_size = prefix.size();
    for (int i = 0; i < prefix_size; ++i) {
        if (prefix[i] == '1') {
            bool res_tmp = true;
            QChar symb = ' ';
            int left_letter_size = left_letters[i].size();
            for (int j = 0; j < left_letter_size; ++j) {
                if (left_letters[i][j] == '!') {
                    symb = '!';
                    continue;
                } else {
                    int num_column = left_letters[i][j].unicode() - (int)'a';
                    bool x;
                    if (table[row][num_column] == '0') x = false;
                    else x = true;

                    if (symb == '!') {
                        symb = ' ';
                        res_tmp = res_tmp && !x;
                    } else {
                        res_tmp = res_tmp && x;
                    }
                }
            }
            res = res || res_tmp;
        }
    }
    return res;
}

bool CheckDNF(QString prefix) {
    QString current_result = "";
    for (unsigned int i = 0; i < table.size(); ++i) {
        if (GetResultOfDNF(prefix, i)) current_result += '1';
        else current_result += '0';
        if (current_result[i] != result[i])
            return false;
    }
    return true;
}

void Generate_DNF(QString prefix, int n) {
    if (n == 0) {
        int current_size = 0;
        for (int i = 0; i < prefix.size(); ++i) {
            if (prefix[i] == '1') {
                current_size++;
            }
        }
        if (CheckDNF(prefix)) {
            if (current_size < MDNF_size) {
                MDNF.clear();
                MDNF.push_back(prefix);
                MDNF_size = current_size;
            } else if (current_size == MDNF_size) {
                MDNF.push_back(prefix);
            }
        }
    } else {
        Generate_DNF(prefix + "0", n - 1);
        Generate_DNF(prefix + "1", n - 1);
    }
}

void GetLeftItems() {
    for (unsigned int i = 0; i < state.size(); ++i) {
        for (unsigned int j = 0; j < state[i].size(); ++j) {
            if (state[i][j] == 0) {
                QString el = letters[j];
                QString conjunction = "";
                QString current_combination = table[i][j];
                for (int k = 0; k < current_combination.size(); ++k) {
                    if (current_combination[k] == '1') conjunction += el[k];
                    else conjunction += "!" + el[k];
                }
                if (std::find(left_letters.begin(), left_letters.end(), conjunction) == left_letters.end()) {
                    left_letters.push_back(conjunction);
                }
            }
        }
    }
}

void StepThree(Ui::MainWindow* ui) {
    GetLeftItems();
    Generate_DNF("", left_letters.size());
    for (unsigned int i = 0; i < MDNF.size(); ++i) {
        QString s = "";
        for (int j = 0; j < MDNF[i].size(); ++j) {
            if (MDNF[i][j] == '1') {
                s += left_letters[j];
                s += '+';
            }
        }
        s.remove(s.size() - 1, 1);
        ui->textEdit_3->insertPlainText(s);
        ui->textEdit_3->insertPlainText("\n");
    }
}

void ClearAll(Ui::MainWindow* ui) {
    letters.clear();
    state.clear();
    table.clear();
    result.clear();
    left_letters.clear();
    MDNF_size = 1e9;
    index = 0;
    ui->textEdit_3->clear();
    ui->tableWidget->clear();
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if (step == 0) {
        variebles_num = ui->textEdit->toPlainText().toInt();
        result = ui->textEdit_2->toPlainText();

        QString s(pow(2, variebles_num) - result.size(), '0');
        result = s + result;
        ui->textEdit_2->clear();
        ui->textEdit_2->insertPlainText(result);

        MakeTable();
        DrawTable(ui);
        step++;
    } else if (step == 1) {
        StepOne(ui);
        ++step;
    } else if (step == 2) {
        StepTwo(ui);
        ++step;
    } else if (step == 3) {
        StepThree(ui);
        ++step;
    } else {
        step = 0;
        ClearAll(ui);
    }
}
