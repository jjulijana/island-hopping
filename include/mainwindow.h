#pragma once

#include <QMainWindow>
#include <QPointF>
#include <QVector>

#include "canvas.h"

class QLabel;
class QPushButton;
class QDoubleSpinBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void loadExampleCases();
    void showCase(int index);
    void runAlgorithm();
    void showDelaunay();
    void showMstAll();
    void startMstStepByStep();
    void nextMstEdge();
    void playStepByStep();
    void pausePlayback();
    void intervalChanged(double seconds);

    Canvas* m_canvas = nullptr;
    QLabel* m_title = nullptr;
    QLabel* m_status = nullptr;
    QPushButton* m_runButton = nullptr;
    QPushButton* m_playButton = nullptr;
    QPushButton* m_pauseButton = nullptr;
    QPushButton* m_delaunayButton = nullptr;
    QPushButton* m_mstAllButton = nullptr;
    QPushButton* m_mstStepButton = nullptr;
    QPushButton* m_nextEdgeButton = nullptr;
    QDoubleSpinBox* m_intervalSpin = nullptr;
    QVector<QVector<QPointF>> m_cases;
    int m_caseIndex = 0;
};