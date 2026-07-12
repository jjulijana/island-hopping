#include "mainwindow.h"

#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextStream>
#include <QVBoxLayout>

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
    m_title = new QLabel("example.txt");
    m_title->setAlignment(Qt::AlignCenter);

    topRow->addWidget(prevBtn);
    topRow->addWidget(m_title, 1);
    topRow->addWidget(nextBtn);
    topRow->addWidget(m_runButton);
    root->addLayout(topRow);

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

    loadExampleCases();
    if (!m_cases.isEmpty()) {
        showCase(0);
    } else {
        m_title->setText("example.txt - no cases found");
    }
}

void MainWindow::loadExampleCases()
{
    QFile file("example.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_title->setText("could not open example.txt");
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
    m_title->setText(QString("example.txt - case %1/%2").arg(index + 1).arg(m_cases.size()));
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
    m_status->setText(QString("MST length: %1, edges: %2")
                          .arg(result.mstLength, 0, 'f', 3)
                          .arg(result.mstEdges.size()));
}