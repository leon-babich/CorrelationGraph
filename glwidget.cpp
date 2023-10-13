#include "glwidget.h"

GlWidget::GlWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    m_wScreen = 600;
    m_hScreen = 600;
    m_wIndent = 50;
    m_hIndent = 50;
    m_wChart = m_wScreen - m_wIndent * 2;
    m_hChart = m_hScreen - m_hIndent * 2;
    m_isManual = false;

    float x;

    m_wScChart = m_wChart / N_X;

    for(int i=0; i<=N_X; i+=10) {
        x = m_wScChart * i;
        i != N_X ? m_xTitles.push_back(QPair<float, QString>(x, QString::number(i))) : m_xTitles.push_back(QPair<float, QString>(x, "X"));
    }

    float y;
    m_hScChart = m_hChart / N_Y;
    for(int i=10; i<=N_Y; i+=10) {
        y = m_hScChart * i;
        i != N_Y ? m_yTitles.push_back(QPair<float, QString>(y, QString::number(i))) : m_yTitles.push_back(QPair<float, QString>(y, "Y"));
    }

    float wScChart = m_wChart / N_XS;
    QString str;
    for(int i=0; i<=N_XS; i++) {
        x = wScChart * i;
//        i!=N_XS ? m_xTitlesSin.push_back(QPair<float, QString>(x, QString::number((float)i/4))) : m_xTitlesSin.push_back(QPair<float, QString>(x, "X"));
//        str = i==4 ? "P" : "";
//        str = i==8 ? "2P" : str;

        switch (i) {
        case 2:
            str = "1/2"; break;
        case 4:
            str = "P"; break;
        case 6:
            str = "3/2"; break;
        case 8:
            str = "2P"; break;
        default:
            str = ""; break;
        }
        m_xTitlesSin.push_back(QPair<float, QString>(x, str));
//        m_xTitlesSin.push_back(QPair<float, QString>(x, QString::number((float)i/4) + str));
    }

    float hScChart = m_hChart / N_YS;
    for(int i=0; i<=N_YS; i++) {
        y = hScChart * i;
//        i!=N_YS ? m_yTitlesSin.push_back(QPair<float, QString>(y, QString::number((float)i/4 - 1))) : m_yTitlesSin.push_back(QPair<float, QString>(y, "Y"));
        m_yTitlesSin.push_back(QPair<float, QString>(y, QString::number((float)i/2 - 1)));
    }

    m_line.xLine[0] = 0;
    m_line.xLine[1] = 0;
    m_line.yLine[0] = 0;
    m_line.yLine[1] = 0;

    m_line.timerAnim = new QTimer;
    connect(m_line.timerAnim, &QTimer::timeout, this, &GlWidget::slotTimerAnimPainting);

    m_line.timerAnim2 = new QTimer;
    connect(m_line.timerAnim2, &QTimer::timeout, this, &GlWidget::slotTimerAnimShine);
}

void GlWidget::mousePressEvent(QMouseEvent *ev)
{
    float x = ev->pos().x() * m_wSc;
    float y = ev->pos().y() * m_hSc;

    if(ev->button() == Qt::LeftButton) {
        if(x < m_wIndent || x > (m_wIndent+m_wChart) || y < (m_hScreen-m_hChart-m_hIndent) || y > m_hScreen-m_hIndent) return;

        float xCh = (x - m_wIndent)/m_wScChart;
        float yCh = (m_hScreen - y - m_hIndent)/m_hScChart;

        m_vecPoints.push_back(QPair<float,float>(xCh,yCh));
    }
    else if(ev->button() == Qt::RightButton) {
        m_modeRegression = (m_modeRegression == LINE) ? SINUSA : LINE;
    }

    update();
}

void GlWidget::initializedGL()
{
    glClearColor(0.0f, 0.5f, 0.0f, 1.0f);
}

void GlWidget::resizeGL(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, m_hScreen, m_wScreen, 0, -1, 1);

    glViewport(0, 0, (GLint)w, (GLint)h);
    makeRasterFont();
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    glEnable(GL_BLEND);
    m_wSc = m_wScreen/(qreal)w;
    m_hSc = m_hScreen/(qreal)h;
}

void GlWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(0.0, 0.0, 0.0);
    paintFrame(1, 0, m_wScreen, m_hScreen-1);

    drawGrid();
    drawTitles();
    drawPoints();
    drawFormula();

    if(m_line.isAnim) drawAnimationLinePainting();
    else if(m_line.isAnim2) drawAnimationLineShine();
    else drawLine();
}

void GlWidget::paintFrame(float x, float y, float w, float h)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(x, y, w, h);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void GlWidget::makeRasterFont()
{
    GLubyte space[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    GLubyte letters[][13] = {
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x36, 0x36, 0x36},
        {0x00, 0x00, 0x00, 0x66, 0x66, 0xff, 0x66, 0x66, 0xff, 0x66, 0x66, 0x00, 0x00},
        {0x00, 0x00, 0x18, 0x7e, 0xff, 0x1b, 0x1f, 0x7e, 0xf8, 0xd8, 0xff, 0x7e, 0x18},
        {0x00, 0x00, 0x0e, 0x1b, 0xdb, 0x6e, 0x30, 0x18, 0x0c, 0x76, 0xdb, 0xd8, 0x70},
        {0x00, 0x00, 0x7f, 0xc6, 0xcf, 0xd8, 0x70, 0x70, 0xd8, 0xcc, 0xcc, 0x6c, 0x38},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x1c, 0x0c, 0x0e},
        {0x00, 0x00, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0c},
        {0x00, 0x00, 0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x18, 0x30},
        {0x00, 0x00, 0x00, 0x00, 0x99, 0x5a, 0x3c, 0xff, 0x3c, 0x5a, 0x99, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18, 0x00, 0x00},
        {0x00, 0x00, 0x30, 0x18, 0x1c, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x06, 0x06, 0x03, 0x03},
        {0x00, 0x00, 0x3c, 0x66, 0xc3, 0xe3, 0xf3, 0xdb, 0xcf, 0xc7, 0xc3, 0x66, 0x3c},
        {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x38, 0x18},
        {0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0xe7, 0x7e},
        {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0x07, 0x03, 0x03, 0xe7, 0x7e},
        {0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0xff, 0xcc, 0x6c, 0x3c, 0x1c, 0x0c},
        {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0, 0xff},
        {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
        {0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x03, 0x03, 0xff},
        {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e},
        {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x03, 0x7f, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e},
        {0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x30, 0x18, 0x1c, 0x1c, 0x00, 0x00, 0x1c, 0x1c, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06},
        {0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60},
        {0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x18, 0x0c, 0x06, 0x03, 0xc3, 0xc3, 0x7e},
        {0x00, 0x00, 0x3f, 0x60, 0xcf, 0xdb, 0xd3, 0xdd, 0xc3, 0x7e, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0x66, 0x3c, 0x18},
        {0x00, 0x00, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
        {0x00, 0x00, 0x7e, 0xe7, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
        {0x00, 0x00, 0xfc, 0xce, 0xc7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc7, 0xce, 0xfc},
        {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0, 0xff},
        {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xff},
        {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xcf, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
        {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
        {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e},
        {0x00, 0x00, 0x7c, 0xee, 0xc6, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06},
        {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xe0, 0xf0, 0xd8, 0xcc, 0xc6, 0xc3},
        {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0},
        {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb, 0xff, 0xff, 0xe7, 0xc3},
        {0x00, 0x00, 0xc7, 0xc7, 0xcf, 0xcf, 0xdf, 0xdb, 0xfb, 0xf3, 0xf3, 0xe3, 0xe3},
        {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7, 0x7e},
        {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
        {0x00, 0x00, 0x3f, 0x6e, 0xdf, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c},
        {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
        {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0xe0, 0xc0, 0xc0, 0xe7, 0x7e},
        {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff},
        {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
        {0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
        {0x00, 0x00, 0xc3, 0xe7, 0xff, 0xff, 0xdb, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
        {0x00, 0x00, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
        {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
        {0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x7e, 0x0c, 0x06, 0x03, 0x03, 0xff},
        {0x00, 0x00, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c},
        {0x00, 0x03, 0x03, 0x06, 0x06, 0x0c, 0x0c, 0x18, 0x18, 0x30, 0x30, 0x60, 0x60},
        {0x00, 0x00, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x66, 0x3c, 0x18},
        {0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0x30, 0x70},
        {0x00, 0x00, 0x7f, 0xc3, 0xc3, 0x7f, 0x03, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0},
        {0x00, 0x00, 0x7e, 0xc3, 0xc0, 0xc0, 0xc0, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0x7f, 0x03, 0x03, 0x03, 0x03, 0x03},
        {0x00, 0x00, 0x7f, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x30, 0x33, 0x1e},
        {0x7e, 0xc3, 0x03, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0},
        {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x00},
        {0x38, 0x6c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x0c, 0x00},
        {0x00, 0x00, 0xc6, 0xcc, 0xf8, 0xf0, 0xd8, 0xcc, 0xc6, 0xc0, 0xc0, 0xc0, 0xc0},
        {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78},
        {0x00, 0x00, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xfe, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xfc, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00},
        {0xc0, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0x00, 0x00, 0x00, 0x00},
        {0x03, 0x03, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0x7f, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe0, 0xfe, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xfe, 0x03, 0x03, 0x7e, 0xc0, 0xc0, 0x7f, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x1c, 0x36, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x30, 0x00},
        {0x00, 0x00, 0x7e, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xc3, 0xe7, 0xff, 0xdb, 0xc3, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xc3, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0xc3, 0x00, 0x00, 0x00, 0x00},
        {0xc0, 0x60, 0x60, 0x30, 0x18, 0x3c, 0x66, 0x66, 0xc3, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xff, 0x60, 0x30, 0x18, 0x0c, 0x06, 0xff, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x0f, 0x18, 0x18, 0x18, 0x38, 0xf0, 0x38, 0x18, 0x18, 0x18, 0x0f},
        {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},
        {0x00, 0x00, 0xf0, 0x18, 0x18, 0x18, 0x1c, 0x0f, 0x1c, 0x18, 0x18, 0x18, 0xf0},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x8f, 0xf1, 0x60, 0x00, 0x00, 0x00}
    };

    GLuint i, j;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    fontOffset = glGenLists(128);

    for(i=32; i<127; i++) {
        glNewList(fontOffset + i, GL_COMPILE);
        glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, letters[i - 32]);
        glEndList();
    }

    glNewList(fontOffset + ' ', GL_COMPILE);
    glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, space);
    glEndList();
}

void GlWidget::printString(float x, float y, const char *s)
{
    glRasterPos2f(x, y);

    glPushAttrib(GL_LIST_BIT);
    glListBase(fontOffset);
    glCallLists(strlen(s), GL_UNSIGNED_BYTE, (GLubyte*) s);
    glPopAttrib();
}

void GlWidget::drawGrid()
{
    glBegin(GL_LINES);
       glVertex2f(m_wIndent, m_hScreen - m_hIndent);
       glVertex2f(m_wIndent, m_hScreen - (m_hChart + m_hIndent));
       glVertex2f(m_wIndent, m_hScreen - m_hIndent);
       glVertex2f(m_wIndent + m_wChart, m_hScreen - m_hIndent);
    glEnd();

    float fW = m_wChart / N_X;
    float fH = m_hChart / N_Y;

    glLineStipple(3, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
//    glColor3f(0.5, 0.5, 0.5);
    glBegin(GL_LINES);
        if(m_modeRegression == LINE) {
            for(int i=10; i<=N_X; i+=10) {
                glVertex2f(m_wIndent + i * fW, m_hScreen - m_hIndent);
                glVertex2f(m_wIndent + i * fW, m_hScreen - (m_hChart + m_hIndent));
            }
            for(int i=10; i<=N_Y; i+=10) {
                glVertex2f(m_wIndent, m_hScreen - m_hIndent - i * fH);
                glVertex2f(m_wIndent + m_wChart, m_hScreen - m_hIndent - i * fH);
            }
        }
        else if(m_modeRegression == SINUSA) {
            float fCor = 1.005;
            fW = m_wChart / N_XS * fCor;
            fH = m_hChart / N_YS;

            for(int i=1; i<=N_XS; i++) {
                glVertex2f(m_wIndent + i * fW, m_hScreen - m_hIndent);
                glVertex2f(m_wIndent + i * fW, m_hScreen - (m_hChart + m_hIndent));
            }
            for(int i=1; i<=N_YS; i++) {
                glVertex2f(m_wIndent, m_hScreen - m_hIndent - i * fH);
                glVertex2f(m_wIndent + m_wChart, m_hScreen - m_hIndent - i * fH);
            }
        }
    glEnd();

    glDisable(GL_LINE_STIPPLE);
}

void GlWidget::drawTitles()
{
    float xPos = 0;
    float yPos = m_hScreen - m_hIndent + 15 * m_hSc;

    if(m_modeRegression == LINE) {
        for(int i=0, s=m_xTitles.size(); i<s; i++) {
            xPos = m_wIndent + m_xTitles[i].first - 5;
            printString(xPos, yPos, m_xTitles[i].second.toStdString().c_str());
        }

        xPos = m_wIndent - 20 * m_wSc;
        for(int i=0, s=m_yTitles.size(); i<s; i++) {
            yPos = m_hScreen - m_hIndent - m_yTitles[i].first + 5;
            printString(xPos, yPos, m_yTitles[i].second.toStdString().c_str());
        }
    }
    else if(m_modeRegression == SINUSA) {
        for(int i=0, s=m_xTitlesSin.size(); i<s; i++) {
            xPos = m_wIndent + m_xTitlesSin[i].first - 10;
            printString(xPos, yPos, m_xTitlesSin[i].second.toStdString().c_str());
        }

        xPos = m_wIndent - 30 * m_wSc;
        for(int i=0, s=m_yTitlesSin.size(); i<s; i++) {
            yPos = m_hScreen - m_hIndent - m_yTitlesSin[i].first + 5;
            xPos = i==1 ? m_wIndent - 40 * m_wSc : xPos;
            xPos = i==4 ? m_wIndent - 25 * m_wSc : xPos;
            printString(xPos, yPos, m_yTitlesSin[i].second.toStdString().c_str());
        }
    }
}

void GlWidget::drawPoints()
{
    glColor3f(1.0, 0.0, 0.0);
    glPointSize(5);
    glBegin(GL_POINTS);
        QPair<float,float> point;
        foreach (point, m_vecPoints) {
            glVertex2f(m_wIndent + point.first * m_wScChart, m_hScreen - (point.second * m_hScChart + m_hIndent));
        }

    glEnd();
    glPointSize(1);
}

void GlWidget::drawLine()
{
    glColor3f(1.0, 0.0, 0.0);
    glLineWidth(2);

    if(m_modeRegression == LINE) {
        float x1 = m_wIndent + m_line.xLine[0] * m_wScChart;
        float x2 = m_wIndent + m_line.xLine[1] * m_wScChart;
        float y1 = m_hScreen - (m_line.yLine[0] * m_hScChart + m_hIndent);
        float y2 = m_hScreen - (m_line.yLine[1] * m_hScChart + m_hIndent);

        glBegin(GL_LINES);
           glVertex2f(x1, y1);
           glVertex2f(x2, y2);
        glEnd();
        glLineWidth(1);
    }
    else if(m_modeRegression == SINUSA) {
        float x, y;
        QPair<float,float> point;

        glBegin(GL_LINE_STRIP);
            foreach (point, m_line.vecLine) {
                x = m_wIndent + point.first * m_wScChart;
                y = m_hScreen - (point.second * m_hScChart + m_hIndent);
                glVertex2f(x, y);
            }
        glEnd();
        glLineWidth(1);

        if(m_line.vecPoints.isEmpty() || m_line.vecPoints.size() <= m_sin.memNumVer[1]) return;
        glColor3f(1.0, 1.0, 0.0);
        glPointSize(10);
        glBegin(GL_POINTS);
            glVertex2f(m_wIndent + m_line.vecPoints[m_sin.memNumVer[0]].first * m_wScChart,
                    m_hScreen - (m_line.vecPoints[m_sin.memNumVer[0]].second * m_hScChart + m_hIndent));
            glVertex2f(m_wIndent + m_line.vecPoints[m_sin.memNumVer[1]].first * m_wScChart,
                    m_hScreen - (m_line.vecPoints[m_sin.memNumVer[1]].second * m_hScChart + m_hIndent));
        glEnd();
        glPointSize(1);
    }
}

void GlWidget::drawAnimationLinePainting()
{
    int &n = m_line.nSegment;
    if(n >= m_line.vecLine.size()) return;

    glColor3f(1.0, 0.0, 0.0);
    glLineWidth(2);

    if(m_modeRegression == LINE) {
        float x1 = m_wIndent + m_line.xLine[0] * m_wScChart;
        float x2 = m_wIndent + m_line.vecLine[n].first * m_wScChart;
        float y1 = m_hScreen - (m_line.yLine[0] * m_hScChart + m_hIndent);
        float y2 = m_hScreen - (m_line.vecLine[n].second * m_hScChart + m_hIndent);

        glBegin(GL_LINES);
           glVertex2f(x1, y1);
           glVertex2f(x2, y2);
        glEnd();
    }
    else if(m_modeRegression == SINUSA) {
        float x, y;

        glBegin(GL_LINE_STRIP);
            for(int i=0; i<=n; i++) {
                x = m_wIndent + m_line.vecLine[i].first * m_wScChart;
                y = m_hScreen - (m_line.vecLine[i].second * m_hScChart + m_hIndent);
                glVertex2f(x, y);
            }
        glEnd();
    }

    glLineWidth(1);
}

void GlWidget::drawAnimationLineShine()
{
    glLineWidth(m_line.widLine);
    glColor3f(1.0, 0.0, 0.0);
    if(m_modeRegression == LINE) {
        float x1 = m_wIndent + m_line.xLine[0] * m_wScChart;
        float x2 = m_wIndent + m_line.xLine[1] * m_wScChart;
        float y1 = m_hScreen - (m_line.yLine[0] * m_hScChart + m_hIndent);
        float y2 = m_hScreen - (m_line.yLine[1] * m_hScChart + m_hIndent);
        glBegin(GL_LINES);
           glVertex2f(x1, y1);
           glVertex2f(x2, y2);
        glEnd();
    }
    else if(m_modeRegression == SINUSA) {
        float x, y;
        QPair<float,float> point;

        glBegin(GL_LINE_STRIP);
            foreach (point, m_line.vecLine) {
                x = m_wIndent + point.first * m_wScChart;
                y = m_hScreen - (point.second * m_hScChart + m_hIndent);
                glVertex2f(x, y);
            }
        glEnd();
    }
    glLineWidth(1);
}

void GlWidget::drawFormula()
{
    QString strAll;
    QString strFormula;
    QString strParam;
    strFormula = m_modeRegression == LINE ? "y = a + b*x" : "y = a + b * sin(c * x + d)";
    strParam = m_modeRegression == LINE ? "   a = " + QString::number(m_line.x0) + "   b = " + QString::number(m_line.x1) :
             "   a = " + QString::number(m_line.a) + "  b = " + QString::number(m_line.b);
    QString strParam2 = m_modeRegression==LINE ? "" : "  c = " + QString::number(m_line.c) + "   d = " + QString::number(m_line.d);
    strAll.append(strFormula);
    strAll.append(strParam);
    glColor3f(0.0, 0.5, 0.0);
    float xStr = m_modeRegression == LINE ? m_wIndent * 2 : m_wIndent;
    printString(xStr, 20 * m_hSc, strAll.toStdString().c_str());
    printString(6.6*m_wIndent, 35 * m_hSc, strParam2.toStdString().c_str());
}

void GlWidget::calculateLine()
{
    if(m_modeRegression == LINE) {
        float xSum = 0, ySum = 0, x2Sum = 0, xySum = 0;
        int n = m_vecPoints.size();

        if(!m_vecPoints.isEmpty()) {
            m_line.xLine[0] = 100;
            m_line.xLine[1] = 0;
        }
        else {
            m_line.xLine[0] = 0;
            m_line.xLine[1] = 100;
        }

        QPair<float,float> point;
        foreach (point, m_vecPoints) {
            xSum += point.first;
            ySum += point.second;
            x2Sum += point.first * point.first;
            xySum += point.first * point.second;

            if(m_line.xLine[0] > point.first) m_line.xLine[0] = point.first;
            if(m_line.xLine[1] < point.first) m_line.xLine[1] = point.first;
        }
        if(m_line.xLine[0] > 3) m_line.xLine[0] -= 3;
        if(m_line.xLine[1] < 97) m_line.xLine[1] += 3;

        if(!m_isManual) {
            m_line.x1 = (n * xySum - xSum * ySum)/(n * x2Sum - xSum * xSum);
            m_line.x0 = ySum/n - m_line.x1 * xSum/n;
        }

        m_line.yLine[0] = m_line.x0 + m_line.x1 * m_line.xLine[0];
        m_line.yLine[1] = m_line.x0 + m_line.x1 * m_line.xLine[1];

        float len = std::sqrt(powf(m_line.xLine[1] - m_line.xLine[0], 2) + powf(m_line.yLine[1] - m_line.yLine[0], 2));
        int nSegment = len/m_line.step;
        float dist = (m_line.xLine[1] - m_line.xLine[0]) / nSegment;

        if(!m_line.vecLine.isEmpty()) m_line.vecLine.clear();

        for(int i=1; i<nSegment; i++) {
            float x = m_line.xLine[0] + dist * i;
            float y = m_line.x0 + m_line.x1 * x;
            m_line.vecLine.push_back(QPair<float,float>(x,y));
        }

    }
    else if(m_modeRegression == SINUSA) {
//        int n = m_vecPoints.size();

        if(!m_vecPoints.isEmpty()) {
            m_line.xLine[0] = 100;
            m_line.xLine[1] = 0;
        }
        else {
            m_line.xLine[0] = 0;
            m_line.xLine[1] = 100;
        }

        if(!m_line.vecPoints.isEmpty()) m_line.vecLine.clear();

        QPair<float,float> point;

        foreach (point, m_vecPoints) {
            m_line.vecPoints.push_back(point);
            if(m_line.xLine[0] > point.first) m_line.xLine[0] = point.first;
            if(m_line.xLine[1] < point.first) m_line.xLine[1] = point.first;
        }

        m_line.xLine[0] -= 2;
        m_line.xLine[1] += 2;
        if(m_line.xLine[0] < 0) m_line.xLine[0] = 0;
        if(m_line.xLine[1] > N_X) m_line.xLine[1] = N_X;

        //Sort
        QPair<float,float> insert;
        for(int next=1, s=m_line.vecPoints.size(); next<s; next++) {
            insert = m_line.vecPoints[next];

            int moveIt = next;

            while(moveIt>0 && m_line.vecPoints[moveIt-1].first>insert.first) {
                m_line.vecPoints[moveIt] = m_line.vecPoints[moveIt-1];
                moveIt--;
            }

            m_line.vecPoints[moveIt] = insert;
        }
        /////////

        //Finding of 'c'
        int count = 0;
        int sCount = m_line.vecPoints.size() > 5 ? 5 : m_line.vecPoints.size();
        float vertex = m_line.vecPoints.isEmpty() ? 0 : m_line.vecPoints[0].second;
        for(int i=0; i<sCount; i++) {
            if(m_line.vecPoints[i].second > vertex) count++;
        }

        m_sin.moving = count > 2 ? UP : DOWN;
        Direct finDir = m_sin.moving == UP ? DOWN : UP;
        m_sin.vertex = 0;
        m_sin.vertexN = 0;
        m_sin.count = 0;
        m_sin.memNumVer[0] = 0;
        m_sin.memNumVer[1] = 0;
        m_sin.halfPeriod = N_X / 2;
        int n = 0;

        foreach (point, m_line.vecPoints) {
            if(m_sin.moving == UP) {
                if(m_sin.vertex < point.second) {
                    m_sin.vertex = point.second;
                    m_sin.vertexN = n;
                }
                else {
                    if(++m_sin.count > m_sin.Ncount) {
                        m_sin.count = 0;
                        m_sin.moving = DOWN;

                        if(finDir == UP) {
                            m_sin.memNumVer[1] = m_sin.vertexN;
                            m_sin.halfPeriod = m_line.vecPoints[m_sin.memNumVer[1]].first - m_line.vecPoints[m_sin.memNumVer[0]].first;
                            break;
                        }
                        else m_sin.memNumVer[0] = m_sin.vertexN;
                    }
                }
            }
            else if(m_sin.moving == DOWN) {
                if(m_sin.vertex > point.second) {
                    m_sin.vertex = point.second;
                    m_sin.vertexN = n;
                }
                else {
                    if(++m_sin.count > m_sin.Ncount || n >= (m_line.vecPoints.size()-1)) {
                        m_sin.count = 0;
                        m_sin.moving = UP;

                        if(finDir == DOWN) {
                            m_sin.memNumVer[1] = m_sin.vertexN;
                            m_sin.halfPeriod = m_line.vecPoints[m_sin.memNumVer[1]].first - m_line.vecPoints[m_sin.memNumVer[0]].first;
                            break;
                        }
                        else m_sin.memNumVer[0] = m_sin.vertexN;
                    }
                }
            }

            n++;
        }

        if(!m_isManual && !m_vecPoints.isEmpty()) {
            m_line.a = 0;
            m_line.b = 1;
//            float per = m_sin.halfPeriod * 2;
//            m_line.c = N_X / per;
            m_line.c = 1.0;
            m_line.d = 0;
        }
        /////////

        //Finding of 'd'
        float z;
        QVector<float> vecZ;

        for(float d=0; d<360; d++) {
            z = 0;

            foreach (point, m_line.vecPoints) {
                float xGr = 360 / (float)N_X * point.first;
                float y = m_line.a * m_line.hSin + m_line.hSin + m_line.b * sin((m_line.c * xGr + d) * P / 180) * m_line.hSin;
                z += (point.second - y) * (point.second - y);
            }

            vecZ.push_back(z);
        }

        z = 1000000;
        count = 0;
        int deg = 0;

        for (float deviation : vecZ) {
            if(deviation < z) {
                z = deviation;
                deg = count;
            }

            count++;
        }

        m_line.d = deg;
        ////////


        m_line.yLine[0] = m_line.a + m_line.b * sin(m_line.xLine[0] + m_line.d);
        m_line.yLine[1] = m_line.a + m_line.b * sin(m_line.xLine[1] + m_line.d);

        if(!m_line.vecLine.isEmpty()) m_line.vecLine.clear();

        for(int i=0; i<=N_X; i++) {
            float x = m_line.xLine[0] + i;
            float xGr = 360 / (float)N_X * x;
            float yGr = m_line.a + m_line.b * sin((m_line.c * xGr + m_line.d) * P / 180);
            float y = m_line.hSin + yGr * m_line.hSin;

            if(x>=0 && x<=N_X && y>=0 && y<=N_Y) m_line.vecLine.push_back(QPair<float,float>(x, y));
        }
    }

    if(m_line.isAnim) {
        m_line.nSegment = 0;
    }
    else if(m_line.isAnim2) {
        m_line.timerAnim2->stop();
        m_line.isAnim2 = false;
        m_line.nAnim2 = 0;
    }
    m_line.timerAnim->start(20);
    m_line.isAnim = true;

    update();
}

void GlWidget::clearPoints()
{
    m_vecPoints.clear();
    m_line.xLine[0] = 0;
    m_line.xLine[1] = 0;
    m_line.yLine[0] = 0;
    m_line.yLine[1] = 0;
    m_line.vecLine.clear();
    m_line.vecPoints.clear();
    m_line.nAnim2 = 0;
    m_line.widLine = 1;
    update();
}

void GlWidget::setRegression(Regression mode)
{
    m_modeRegression = mode;
    update();
}

void GlWidget::setManual(bool is)
{
    m_isManual = is;
}

void GlWidget::setParametrs(float a, float b, float c, float d)
{
    if(m_modeRegression == LINE) {
        m_line.x0 = a;
        m_line.x1 = b;
    }
    else if(m_modeRegression == SINUSA) {
        m_line.a = a;
        m_line.b = b;
        m_line.c = c;
        m_line.d = d;
    }
    update();
}

float *GlWidget::getParametrs()
{
    float arg[4];
    if(m_modeRegression == LINE) {
        arg[0] = m_line.x0;
        arg[1] = m_line.x1;
        arg[2] = 0;
        arg[3] = 0;
    }
    if(m_modeRegression == SINUSA) {
        arg[0] = m_line.a;
        arg[1] = m_line.b;
        arg[2] = m_line.c;
        arg[3] = m_line.d;
    }

    return arg;
}

void GlWidget::slotTimerAnimPainting()
{
    m_line.nSegment++;

    if(m_line.nSegment >= m_line.vecLine.size()) {
        m_line.nSegment = 0;
        m_line.timerAnim->stop();
        m_line.isAnim = false;
        m_line.timerAnim2->start(100);
        m_line.isAnim2 = true;
        drawLine();
    }

    update();
}

void GlWidget::slotTimerAnimShine()
{
    m_line.nAnim2++;
    m_line.isRise ? m_line.widLine++ : m_line.widLine--;

    if(m_line.nAnim2 == 5) {
        m_line.isRise = false;
    }
    else if(m_line.nAnim2 >= m_line.N_ANIM) {
        m_line.nAnim2 = 0;
        m_line.widLine = 1;
        m_line.isRise = true;
        m_line.timerAnim2->stop();
        m_line.isAnim2 = false;
    }

    update();
}
