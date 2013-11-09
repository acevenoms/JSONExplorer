/*
*   JSONExplorer, a simple JSON File Viewer
*   Copyright (C) 2013  Evan Comes (yggdrasil343@gmail.com)

*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.

*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.

*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QJsonValue;
class QTreeWidgetItem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_JSON_File_triggered();

    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    QJsonDocument *doc;

    QWidget *editWidgets[5];
    int activeEditor;
    void activateEditor(int index);

    void LoadJSON(QString fileName);

    void buildJsonTree(QTreeWidgetItem *parent, QJsonValue &obj, QString key);
    QTreeWidgetItem *createJsonTreeLeaf(QTreeWidgetItem *parent, QJsonValue &obj, QString key);
};

#endif // MAINWINDOW_H
