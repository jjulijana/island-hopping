#include "mainwindow.h"

#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QTextStream>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QDir>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Island Hopping");
    setMinimumSize(900, 620);

    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(8);

    auto* topRow = new QHBoxLayout;
    auto* prevBtn = new QPushButton("Previous");
    auto* nextBtn = new QPushButton("Next");
    m_runButton = new QPushButton("Run algorithm");
    m_playButton = new QPushButton("Play step-by-step");
    m_pauseButton = new QPushButton("Pause");
    m_delaunayButton = new QPushButton("Show Delaunay");
    m_mstAllButton = new QPushButton("Show MST");
    m_mstStepButton = new QPushButton("MST step mode");
    m_nextEdgeButton = new QPushButton("Next MST edge");
    m_showAllEdgesButton = new QPushButton("Show all MST edges");
    m_intervalSpin = new QDoubleSpinBox;
    m_intervalSpin->setRange(0.1, 5.0);
    m_intervalSpin->setSingleStep(0.1);
    m_intervalSpin->setValue(0.5);
    m_intervalSpin->setSuffix(" s");
    m_title = new QLabel("10cases.txt");
    m_title->setAlignment(Qt::AlignCenter);

    topRow->addWidget(prevBtn);
    topRow->addWidget(m_title, 1);
    topRow->addWidget(nextBtn);
    topRow->addWidget(m_runButton);
    root->addLayout(topRow);

    auto* controlRow = new QHBoxLayout;
    controlRow->addWidget(m_playButton);
    controlRow->addWidget(m_pauseButton);
    controlRow->addWidget(m_delaunayButton);
    controlRow->addWidget(m_mstAllButton);
    controlRow->addWidget(m_mstStepButton);
    controlRow->addWidget(m_nextEdgeButton);
    controlRow->addWidget(m_showAllEdgesButton);
    controlRow->addWidget(new QLabel("Interval"));
    controlRow->addWidget(m_intervalSpin);
    root->addLayout(controlRow);

    m_status = new QLabel("Load a case, then run the algorithm.");
    root->addWidget(m_status);

    m_canvas = new Canvas;
    root->addWidget(m_canvas, 1);

    connect(prevBtn, &QPushButton::clicked, this, [this] {
        if (m_cases.isEmpty()) {
            return;
        }
        m_caseIndex = (m_caseIndex - 1 + m_cases.size()) % m_cases.size();
        showCase(m_caseIndex);
    });

    connect(nextBtn, &QPushButton::clicked, this, [this] {
        if (m_cases.isEmpty()) {
            return;
        }
        m_caseIndex = (m_caseIndex + 1) % m_cases.size();
        showCase(m_caseIndex);
    });

    connect(m_runButton, &QPushButton::clicked, this, &MainWindow::runAlgorithm);
    connect(m_playButton, &QPushButton::clicked, this, &MainWindow::playStepByStep);
    connect(m_pauseButton, &QPushButton::clicked, this, &MainWindow::pausePlayback);
    connect(m_delaunayButton, &QPushButton::clicked, this, &MainWindow::showDelaunay);
    connect(m_mstAllButton, &QPushButton::clicked, this, &MainWindow::showMstAll);
    connect(m_mstStepButton, &QPushButton::clicked, this, &MainWindow::startMstStepByStep);
    connect(m_nextEdgeButton, &QPushButton::clicked, this, &MainWindow::nextMstEdge);
    connect(m_showAllEdgesButton, &QPushButton::clicked, this, &MainWindow::showAllMstEdges);
    connect(m_intervalSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::intervalChanged);

    loadExampleCases();
    if (!m_cases.isEmpty()) {
        showCase(0);
    } else {
        m_title->setText("10cases.txt - no cases found");
    }
}

QString dataFilePath(const QString& filename)
{
    QDir dir(QCoreApplication::applicationDirPath());
    dir.cdUp();
    return dir.filePath("tests/" + filename);
}

void MainWindow::loadExampleCases()
{
    QFile file(dataFilePath("10cases.txt"));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_title->setText("could not open 10cases.txt");
        return;
    }

    QTextStream in(&file);
    int caseCount = 0;
    in >> caseCount;

    m_cases.clear();
    m_cases.reserve(caseCount);

    for (int caseIndex = 0; caseIndex < caseCount; ++caseIndex) {
        int pointCount = 0;
        in >> pointCount;

        QVector<QPointF> points;
        points.reserve(pointCount);

        for (int i = 0; i < pointCount; ++i) {
            double x = 0.0;
            double y = 0.0;
            in >> x >> y;
            points.append(QPointF(x, y));
        }

        m_cases.append(points);
    }
}

void MainWindow::showCase(int index)
{
    if (index < 0 || index >= m_cases.size()) {
        return;
    }

    m_caseIndex = index;
    m_title->setText(QString("10cases.txt - case %1/%2").arg(index + 1).arg(m_cases.size()));
    m_canvas->setPoints(m_cases[index]);
    m_canvas->setResult(SolverResult{});
    m_status->setText(QString("Case %1 ready. Press Run algorithm.").arg(index + 1));
}

void MainWindow::runAlgorithm()
{
    if (m_cases.isEmpty()) {
        return;
    }

    const SolverResult result = Solver::solve(m_cases[m_caseIndex]);
    m_canvas->setResult(result);
    m_canvas->showDelaunay();
    m_status->setText(QString("Delaunay ready. MST length: %1, edges: %2")
                          .arg(result.mstLength, 0, 'f', 3)
                          .arg(result.mstEdges.size()));
}

void MainWindow::showDelaunay()
{
    m_canvas->showDelaunay();
    m_status->setText("Delaunay view shown. Use MST buttons to switch.");
}

void MainWindow::showMstAll()
{
    m_canvas->showMstAll();
    m_status->setText("Showing all MST edges.");
}

void MainWindow::startMstStepByStep()
{
    m_canvas->showMstStepByStep();
    m_status->setText("MST step mode enabled. Press Next MST edge.");
}

void MainWindow::nextMstEdge()
{
    m_canvas->nextMstEdge();
    m_status->setText("Advanced one MST edge.");
}

void MainWindow::showAllMstEdges()
{
    m_canvas->showAllMstEdges();
    m_status->setText("All MST edges shown.");
}

void MainWindow::playStepByStep()
{
    m_canvas->setStepIntervalMs(static_cast<int>(m_intervalSpin->value() * 1000.0));
    m_canvas->playStepByStep();
    m_status->setText(QString("Playing Delaunay then MST every %1 s.").arg(m_intervalSpin->value(), 0, 'f', 1));
}

void MainWindow::pausePlayback()
{
    m_canvas->pausePlayback();
    m_status->setText("Playback paused.");
}

void MainWindow::intervalChanged(double seconds)
{
    m_canvas->setStepIntervalMs(static_cast<int>(seconds * 1000.0));
}