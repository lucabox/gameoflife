/**
* Licensed to the Apache Software Foundation (ASF) under one
* or more contributor license agreements.  See the NOTICE.txt
* file distributed with this work for additional information
* regarding copyright ownership.  The ASF licenses this file
* to you under the Apache License, Version 2.0 (the
* "License"); you may not use this file except in compliance
* with the License.  You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing,
* software distributed under the License is distributed on an
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
* KIND, either express or implied.  See the License for the
* specific language governing permissions and limitations
* under the License.
*/

#include "gameoflifemodel.h"

#include <QDebug>
#include <QDateTime>


GameOfLifeModel::GameOfLifeModel(QObject* aParent) : QAbstractListModel(aParent)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());
}

void GameOfLifeModel::setUpGameOfLife(int aRows, int aColumns, int aAnimFrequency){
    m_isRunning = false;
    m_rows = aRows;
    m_columns = aColumns;
    m_total = m_rows * m_columns;
    m_animFrequency= aAnimFrequency;

    m_cells.clear();
    m_cells.reserve(m_total);
    for(int i = 0; i < m_total; ++i) {
        Cell* newCell = new Cell();
        //getting all the neighbours of this cell and storing int the cell struct itself
        newCell->m_neighbours = getNeighbours(i);
        //putting this cell into the world
        m_cells.append(newCell);
    }
    generateRandom();

    m_timer.setSingleShot(true);
    //m_timer.setInterval(m_animFrequency);
    //connect(&m_timer, SIGNAL(timeout()), this, SLOT(calculateNextStep()));
}

GameOfLifeModel::~GameOfLifeModel()
{
    foreach( Cell* cell, m_cells) {
        delete cell;
    }
    m_cells.clear();
}

QVector<int> GameOfLifeModel::getNeighbours(int aIndex) {
    bool isFirstRow = aIndex/m_columns == 0;
    bool isFirstColumn = aIndex%m_columns == 0;
    bool isLastRow = aIndex/m_columns == m_rows -1;
    bool isLastColumn = aIndex%m_columns == m_columns -1;

// the 8 indexes of aIndex's neighbours
//-------------------------------------------------------------------------
//|                       |                       |                       |
//| aIndex - m_columns -1 |   aIndex - m_columns  | aIndex - m_columns +1 |
//|                       |                       |                       |
//-------------------------------------------------------------------------
//|                       |                       |                       |                       |
//|       aIndex - 1      |        aIndex         |       aIndex + 1      |
//|                       |                       |                       |
//-------------------------------------------------------------------------
//|                       |                       |                       |
//| aIndex + m_columns -1 |   aIndex + m_columns  | aIndex + m_columns +1 |
//|                       |                       |                       |
//-------------------------------------------------------------------------

    QVector<int> neighbours;
    if(!isFirstRow) {
        if(!isFirstColumn) {
            //top left item
            neighbours.append(aIndex - m_columns - 1);
        }
        //top item item
        neighbours.append(aIndex - m_columns);
        if(!isLastColumn) {
            //top right item
            neighbours.append(aIndex - m_columns + 1);
        }
    }
    if(!isFirstColumn) {
        //left item
        neighbours.append(aIndex - 1);
    }
    if(!isLastColumn) {
        //right item
        neighbours.append(aIndex + 1);
    }
    if(!isLastRow) {
        if(!isFirstColumn) {
            //bottom left
            neighbours.append(aIndex + m_columns - 1);
        }
        //bottom item
        neighbours.append(aIndex + m_columns);
        if(!isLastColumn) {
            //bottom right
            neighbours.append(aIndex + m_columns + 1);
        }
    }
    return neighbours;
}

void GameOfLifeModel::calculateNextStep() {
    if(getIsRunning()) {
        foreach(Cell* currentCell, m_cells) {
            int aliveNeighbours = 0;
            QVector<int> neighbours = currentCell->m_neighbours;
            foreach(int index, neighbours) {
                aliveNeighbours += m_cells.at(index)->m_isAlive ? 1 : 0;
            }
            // apply the rules of life
            if(aliveNeighbours <2 || aliveNeighbours > 3) {
                //death
                currentCell->m_futureAlive = false;
            } else if(aliveNeighbours == 3) {
                //birth
                currentCell->m_futureAlive = true;
            } else {
                //keep status quo
                currentCell->m_futureAlive = currentCell->m_isAlive;
            }
        }
        applyNextStep();
        m_timer.singleShot(m_animFrequency, this, SLOT(calculateNextStep()));
    }
}

void GameOfLifeModel::applyNextStep() {
    for(int i=0; i< m_total; ++i) {
        if(m_cells.at(i)->needsUpdating()) {
            m_cells[i]->evolve();
            //this item has changed, update the UI
            emit dataChanged( createIndex(i,0), createIndex(i,0));
        }
    }
    //instead of emit dataChanged() for the cell which actually changed
    //emit dataChanged( createIndex(0,0), createIndex(m_total-1,0));
    //works fine too (even though it forces to ask for all the items):

    //but doing a reset will literaly kills the performance
    //probably because forces the delegates to be reinstantiated???
    //emit reset();
}

int GameOfLifeModel::getRows() {
    return m_rows;
}

int GameOfLifeModel::getColumns() {
    return m_columns;
}

QVariant GameOfLifeModel::data ( const QModelIndex & index, int role) const {
    int row = index.row();
    if(!index.isValid() &&
       row > m_total) {
        return QVariant();
    }
    switch(role) {
    case Qt::DisplayRole:
        return QVariant( m_cells.at(row)->m_isAlive );
    default:
        return QVariant();
    }
}

int GameOfLifeModel::rowCount(const QModelIndex &parent ) const {
    if(parent.isValid()) {
        return 0;
    }
    return m_total;
}

void GameOfLifeModel::modifyCellAt(int index, bool aIsAlive) {
    if (index < 0 && index > m_total) {
        return;
    }
    m_cells[index]->m_isAlive = aIsAlive;
    emit dataChanged( createIndex(index,0), createIndex(index,0));
}

int GameOfLifeModel::getAnimationFrequency() {
    return m_animFrequency;
}

void GameOfLifeModel::generateRandom() {
    pause();
    foreach(Cell* currentCell, m_cells){
        //randomly choose the state of the cell
        currentCell->m_futureAlive = (qrand() % 10 == 0);
    }
    QTimer::singleShot(0, this, SLOT(applyNextStep()));
}

void GameOfLifeModel::clear() {
    pause();
    foreach(Cell* currentCell, m_cells){
        currentCell->m_futureAlive = false;
    }
    QTimer::singleShot(0, this, SLOT(applyNextStep()));
}

bool GameOfLifeModel::getIsRunning() {
    return m_isRunning;
}

void GameOfLifeModel::pause() {
    //m_timer.stop();
    m_isRunning = false;
    emit onIsRunningChanged();
}

void GameOfLifeModel::resume() {
    m_isRunning = true;
    emit onIsRunningChanged();
    m_timer.singleShot(0, this, SLOT(calculateNextStep()));
}
