#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QVector>
#include <QPair>
#include <QMouseEvent>
#include <QTimer>
#include <math.h>
#include <QDebug>

#include "definition.h";

//using std::sin;
using namespace std;

const int N_X = 100;
const int N_Y = 100;
const int N_XS = 8;
const int N_YS = 4;

struct Line
{
    QVector<QPair<float,float>> vecPoints;
    QVector<QPair<float,float>> vecLine;
    QTimer *timerAnim;
    QTimer *timerAnim2;
    const int N_ANIM = 9;
    float step = 5;
    int nAnim2 = 0;
    int nSegment = 0;
    int widLine = 1;
    bool isRise = true;
    bool isAnim = false;
    bool isAnim2 = false;
    float xLine[2];
    float yLine[2];
    float x0 = 0;
    float x1 = 0;
    float a = 0;
    float b = 1.0;
    float c = 1.0;
    float d = 0;
    float hSin = 50;
};

enum Direct { UP, DOWN };
struct Sinusoida
{
    const int Ncount = 3;
    float vertex;
    int vertexN;
    int memNumVer[2];
    Direct moving;
    int count;
    float halfPeriod;
};

class GlWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit GlWidget(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *ev);
    void calculateLine();
    void clearPoints();
    void setRegression(Regression mode);
    void setManual(bool is);
    void setParametrs(float a, float b, float c, float d);
    float* getParametrs();

protected:
    virtual void initializedGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

private slots:
    void slotTimerAnimPainting();
    void slotTimerAnimShine();

private:
    void paintFrame(float x, float y, float w, float h);
    void makeRasterFont();
    void printString(float x, float y, const char* s);
    void drawGrid();
    void drawTitles();
    void drawPoints();
    void drawLine();
    void drawAnimationLinePainting();
    void drawAnimationLineShine();
    void drawFormula();

private:
    int m_wScreen;
    int m_hScreen;
    int m_wIndent;
    int m_hIndent;
    int m_wChart;
    int m_hChart;
    float m_wSc;
    float m_hSc;
    float m_wScChart;
    float m_hScChart;
    GLuint fontOffset;
    QVector<QPair<float,QString>> m_xTitles;
    QVector<QPair<float,QString>> m_yTitles;
    QVector<QPair<float,QString>> m_xTitlesSin;
    QVector<QPair<float,QString>> m_yTitlesSin;

    QVector<QPair<float, float>> m_vecPoints;
    Line m_line;
    Sinusoida m_sin;
    Regression m_modeRegression;
    bool m_isManual;
};

#endif // GLWIDGET_H
