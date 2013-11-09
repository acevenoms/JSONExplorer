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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    editWidgets[0] = ui->boolButton;
    editWidgets[1] = ui->doubleValue;
    editWidgets[2] = ui->stringEdit;
    editWidgets[3] = ui->arrayList;
    editWidgets[4] = ui->objectTable;

    editWidgets[0]->show();
    editWidgets[1]->hide();
    editWidgets[2]->hide();
    editWidgets[3]->hide();
    editWidgets[4]->hide();

    activeEditor = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_JSON_File_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this, tr("Open JSON File"), "", tr("JSON Files (*.json);;All Files (*.*)"));

    LoadJSON(filePath);
}

void MainWindow::LoadJSON(QString fileName)
{
    QFile jsonFile(fileName);

    if(!jsonFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->statusBar->showMessage("Could not open file: " + fileName);
        return;
    }

    QByteArray rawJson = jsonFile.readAll();

    ui->textEdit->setText(QString(rawJson));

    QJsonParseError error;

    doc = new QJsonDocument(QJsonDocument::fromJson(rawJson, &error));

    if(error.error != QJsonParseError::NoError)
    {
        ui->statusBar->showMessage("Failed to parse file: " + error.errorString());
        delete doc;
        doc = NULL;
        return;
    }

    /*
    QJsonObject root = doc->object();
    for(QJsonObject::ConstIterator i = root.begin(); i != root.end(); ++i)
    {
        QTreeWidgetItem *toAdd = new QTreeWidgetItem();
        toAdd->setText(0, i.key());
        ui->treeWidget->addTopLevelItem(toAdd);
    }
    */

    QTreeWidgetItem *root = new QTreeWidgetItem();
    QString rootId = "root";

    if(doc->isArray())
    {
        QJsonArray array = doc->array();
        int count = array.count();
        rootId += "["+QString::number(count)+"]";

        for(int i = 0; i < count; ++i)
        {
            QJsonValue val = array.at(i);
            buildJsonTree(root, val, QString::number(i)+" : ");
        }
    }
    else if(doc->isObject())
    {
        QJsonObject object = doc->object();
        rootId += "{"+QString::number(object.count())+"}";

        for(QJsonObject::ConstIterator i = object.begin(); i != object.end(); ++i)
        {
            QJsonValue val = i.value();
            buildJsonTree(root, val, i.key()+" : ");
        }
    }
    else
    {
        rootId = "Null or Undefined root object";
        return;
    }

    root->setText(0, rootId);
    ui->treeWidget->addTopLevelItem(root);
    ui->treeWidget->setCurrentItem(root);
}

void MainWindow::buildJsonTree(QTreeWidgetItem *parent, QJsonValue &obj, QString key = "")
{
    QTreeWidgetItem *toAdd = NULL;
    switch(obj.type())
    {
    case QJsonValue::Bool:
    case QJsonValue::Double:
    case QJsonValue::String:
    case QJsonValue::Undefined:
    case QJsonValue::Null:
        toAdd = createJsonTreeLeaf(parent, obj, key);
        break;
    case QJsonValue::Array:
    {
        toAdd = new QTreeWidgetItem(parent);
        QJsonArray array = obj.toArray();
        int count = array.count();
        toAdd->setText(0, key+"["+QString::number(count)+"]");

        for(int i = 0; i < count; ++i)
        {
            QJsonValue val = array.at(i);
            buildJsonTree(toAdd, val, QString::number(i)+" : ");
        }
        break;
    }
    case QJsonValue::Object:
    {
        toAdd = new QTreeWidgetItem(parent);
        QJsonObject object = obj.toObject();
        int count = object.count();
        toAdd->setText(0, key+"{"+QString::number(count)+"}");

        for(QJsonObject::ConstIterator i = object.begin(); i != object.end(); ++i)
        {
            QJsonValue val = i.value();
            buildJsonTree(toAdd, val, i.key()+" : ");
        }
        break;
    }
    default:
        break;
    }

    toAdd->setData(0, Qt::UserRole, QVariant(obj));
    parent->addChild(toAdd);
}

QTreeWidgetItem *MainWindow::createJsonTreeLeaf(QTreeWidgetItem *parent, QJsonValue &obj, QString key)
{
    QTreeWidgetItem *leaf = new QTreeWidgetItem(parent);
    QString type = "unknown";
    QString value = "unknown";

    switch(obj.type())
    {
    case QJsonValue::Bool:
        type = "Bool";
        value = obj.toBool() ? "True" : "False";
        break;
    case QJsonValue::Double:
        type = "Double";
        value = QString::number(obj.toDouble());
        break;
    case QJsonValue::String:
        type = "String";
        value = "\""+obj.toString()+"\"";
        break;
    case QJsonValue::Undefined:
        type = "Undefined";
        value = "Undefined";
        break;
    case QJsonValue::Null:
        type = "Null";
        value = "Null";
        break;
    default:
        break;
    }

    leaf->setText(0, key+"("+type+")");
    leaf->setText(1, value);

    return leaf;
}

void MainWindow::activateEditor(int index)
{
    editWidgets[activeEditor]->hide();

    activeEditor = index;
    editWidgets[activeEditor]->show();
}

void MainWindow::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    QJsonValue jsonObj = current->data(0, Qt::UserRole).toJsonValue();

    ui->typeSelector->setCurrentIndex(jsonObj.type() - 1);
}

void MainWindow::on_typeSelector_currentIndexChanged(int index)
{
    activateEditor(index);
}
