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

import Qt 4.7


//Note: the model has been imported in the QML space with id: lifemodel

Rectangle {
    id: root
    property int cellWidth: 10
    property int cellHeight: 10
    // lifemodel has two properties called rows and columns,
    // they are accessible through the getters declares in the header file
    width: lifemodel.columns*cellWidth*2 + rowId.spacing
    height: lifemodel.rows*cellHeight + controls.height
    color: "gray"

    Row {
        id: controls
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40
        spacing: 10
        property int buttonWidth: (width - 2*spacing)/3

        Rectangle {
            id: startButton
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.buttonWidth
            Text {
                id: startText
                anchors.centerIn: parent
                text: lifemodel.isRunning ? "Pause" : "Resume"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if(lifemodel.isRunning) {
                        lifemodel.pause();
                    } else {
                        lifemodel.resume();
                    }
                }
            }
        }
        Rectangle {
            id: clearButton
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.buttonWidth
            Text {
                id: clearText
                anchors.centerIn: parent
                text: "Clear"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    lifemodel.clear();
                }
            }
        }
        Rectangle {
            id: generateButton
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.buttonWidth
            Text {
                id: generateText
                anchors.centerIn: parent
                text: "Generate Random"
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    lifemodel.generateRandom();
                }
            }
        }
    }

    Row {
        id: rowId
        anchors.top: controls.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        spacing: 10
        //first way of displaying the grid (Using a Grid a Repeater)
        Grid {
            id: grid
            rows: lifemodel.rows
            columns: lifemodel.columns

            Repeater {
                model: lifemodel
                Rectangle {
                    property int idx: index
                    width: root.cellWidth; height: root.cellHeight
                    property bool isAlive: display
                    color: isAlive ? "blue" : "black"
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            // - touch of life/death -
                            //a click on this cell will make the cell alive/dead
                            //note that the isAlive property is not touched at all
                            //modifyCellAt is a Q_INVOKABLE method in the c++ world
                            //which will change the corresponding cell in the model
                            //this change will be propagated back to the UI and will
                            //be visible by both "grid" and "gridview"
                            //(as expected, since they share the model)
                            lifemodel.modifyCellAt(idx , !isAlive);
                        }
                    }
                }
            }
        }
        //second way of displaying the grid (Using a GridView)
        GridView {
            id: gridview
            cellWidth: root.cellHeight
            cellHeight: root.cellWidth
            width: lifemodel.columns*cellWidth
            height: lifemodel.columns*cellHeight

            model: lifemodel
            delegate: cellDelegate

            Component{
                id: cellDelegate
                Rectangle {
                    id: index
                    width: gridview.cellWidth; height: gridview.cellHeight
                    //setting the isAlive property to the Qt::DisplayRole
                    //provided by the c++ model
                    property bool isAlive: display

                    //using States just for fun...
                    states: [
                        State {
                            name: "dead"; when: !isAlive
                            PropertyChanges { target: index; color: "black" }
                        },
                        State {
                            name: "alive"; when: isAlive
                            PropertyChanges { target: index; color: "green" }
                        }
                    ]
                    //...and for using a transition to animate the changes
                    transitions: [
                        Transition {
                            reversible: true
                            from: "dead"
                            to: "alive"
                            PropertyAnimation{ property: "color"; duration: lifemodel.getAnimationFrequency(); easing.type: Easing.Linear}
                        }
                    ]
                }
            }
        }
    }
}
