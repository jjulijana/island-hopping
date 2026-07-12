#pragma once

#include <QMainWindow>
#include <QPointF>
#include <QVector>

#include "canvas.h"

class QLabel;
class QPushButton;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void loadExampleCases();
    void showCase(int index);
    void runAlgorithm();

    Canvas* m_canvas = nullptr;
    QLabel* m_title = nullptr;
    QLabel* m_status = nullptr;
    QPushButton* m_runButton = nullptr;
    QVector<QVector<QPointF>> m_cases;
    int m_caseIndex = 0;
};