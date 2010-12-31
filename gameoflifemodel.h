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

#ifndef GAMEOFLIFEMODEL_H
#define GAMEOFLIFEMODEL_H

#include <QAbstractListModel>

#include <QVector>
#include <QTimer>

struct Cell {
    bool needsUpdating() {
        return m_isAlive != m_futureAlive;
    }
    bool evolve() {
        return m_isAlive = m_futureAlive;
    }
    bool m_isAlive;
    bool m_futureAlive;
    QVector<int> m_neighbours;
};


class GameOfLifeModel : public QAbstractListModel
{
    Q_OBJECT
public:
    GameOfLifeModel(QObject* aParent = 0);
    ~GameOfLifeModel();

    void setUpGameOfLife(int aRows, int aColumns, int aAnimFrequency);

    //properties (these can be accessed in QML)
    //READ function: will be called automatically from the QML side to retrieve the value
    //NOTIFY function: signals a change in that property from the c++ to the QML side
    Q_PROPERTY(int rows READ getRows NOTIFY onRowsChanged );
    Q_PROPERTY(int columns READ getColumns NOTIFY onColumnsChanged );
    Q_PROPERTY(bool isRunning READ getIsRunning NOTIFY onIsRunningChanged );
signals: //NOTIFY functions for Q_PROPERTYes declared above
    void onRowsChanged();
    void onColumnsChanged();
    void onIsRunningChanged();
public: //READ functions for Q_PROPERTYes declared above
    int getRows();
    int getColumns();
    bool getIsRunning();
protected: // from QAbstractItemModel
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent ) const;
public: //function which are accessible from the QML side
    /**
     * Set the state of a cell
     * @aIndex cell's index
     * @aIsAlive state to be set
     */
    Q_INVOKABLE void modifyCellAt(int aIndex, bool aIsAlive);
    /**
     * Get the frequency at which each step is calculated
     */
    Q_INVOKABLE int getAnimationFrequency();
    /**
     * Assign to each cell a random state
     */
    Q_INVOKABLE void generateRandom();
    /**
     * Set all the cell to dead state
     */
    Q_INVOKABLE void clear();

    /**
     * Pauses the evolution
     */
    Q_INVOKABLE void pause();

    /**
     * Resumes the evolution
     */
    Q_INVOKABLE void resume();
public slots:
    /**
     * Calculate the state of each cell for the next iteration
     */
    void calculateNextStep();
    /**
     * Per each cell swaps the future and the current value.
     * The functions who set the future value are
     * @calculateNextStep() @clear() @generateRandom()
     */
    void applyNextStep();
private:
    /**
     * Getting the neighbours item of a cell.
     * @aIndex index of the cell we want to find the neighbours of.
     */
    QVector<int> getNeighbours(int aIndex);
private:
    int m_rows;
    int m_columns;
    int m_total;
    int m_animFrequency;
    bool m_isRunning;
    QTimer m_timer;
    QVector<Cell*> m_cells;
};

#endif // GAMEOFLIFEMODEL_H
