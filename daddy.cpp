#include"daddy.h"
#include<QTime>
#include<QDebug>

Daddy::Daddy(QWidget *parent) : QGLWidget(QGLFormat(QGL::SampleBuffers),parent)
{
    qsrand(QTime(0u,0u,0u).secsTo(QTime::currentTime()));
    Color=QColor(236u,182u,67u);
    //Переведём все используемые градусы в радианы
    GenerationRadians();
    circle.clear();
    Points p;
    for(Points*i=radians,*end=radians+ROUND_DEGREE;i<end;i+=CIRCLE_CLEARANCE) //Получаем координаты для отрисовки фона индикатора
    {
        p.x=i->x;
        p.y=i->y;
        p.angle=i->angle;
        circle.append(p);
    }
    ChangeFPS(.0f);
}

Daddy::~Daddy()
{
    if(IsActive())
        killTimer(timer.timerId());
}

void Daddy::timerEvent(QTimerEvent *event)
{
    if(timer.timerId()==event->timerId())
        ContinueSearch();
    QWidget::timerEvent(event);
}

void Daddy::initializeGL()
{
    qglClearColor(palette().background().color()); //Фон OpenGl-виджета
    glMatrixMode(GL_PROJECTION); //Устанавливаем матрицу
    glShadeModel(GL_SMOOTH);//GL_FLAT
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
    //glEnable(GL_DEPTH_TEST);
}

void Daddy::resizeGL(int w,int h)
{
    glEnable(GL_MULTISAMPLE);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(.0f,.0f,.0f,1.0,1.0,-1.0f);
    if(w>h)
        glViewport(static_cast<GLint>(0u),static_cast<GLint>(0u),static_cast<GLint>(h),static_cast<GLint>(h));
    else
        glViewport(static_cast<GLint>(0u),static_cast<GLint>(0u),static_cast<GLint>(w),static_cast<GLint>(w));
    glMatrixMode(GL_MODELVIEW);
    width=w;
    height=h;
}

void Daddy::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // чистим буфер изображения и буфер глубины
    glLoadIdentity(); // загружаем матрицу
    glPushMatrix();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    LocatorArea();
    qreal h=settings["offset"]["horizontal"].isNull() ? .0f : settings["offset"]["horizontal"].toDouble(),
          v=settings["offset"]["vertical"].isNull() ? .0f : settings["offset"]["vertical"].toDouble(),
          a=settings["scan"]["amplitude"].isNull() ? 1.0f : settings["scan"]["amplitude"].toDouble(),
          e=settings["scan"]["equality"].isNull() ? 1.0f : settings["scan"]["equality"].toDouble();
    glTranslatef(h,v,.0f);
    glScalef(a,a,1.0f);
    glScalef(1.0f,e,1.0f);
    DrawStation();
    InitLocatorGrid();
    DrawRange();
    DrawAzimuth();
    DrawRay();
    glPopMatrix();
    PostDraw();
}

bool Daddy::IsActive(void)const
{
    return timer.isActive();
}

void Daddy::GenerationRadians(void)
{
    radians=new Points[ROUND_DEGREE];
    for(quint16 i=0u;i<ROUND_DEGREE;i++)
    {
        radians[i].angle=GetRadianValue(i);
        radians[i].x=qFastCos(radians[i].angle);
        radians[i].y=qFastSin(radians[i].angle);
    }
}

void Daddy::LocatorArea(void)const
{
    qglColor(Qt::black);
    glLineWidth(2.0f);
    glBegin(GL_TRIANGLE_FAN);
        for(QVector<Points>::const_iterator it=circle.begin();it<circle.end();it++)
            glVertex2d(it->x,it->y);
    glEnd();
}

void Daddy::PostDraw(void)const
{
    qglColor(palette().background().color());
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(-1,1u);
    for(QVector<Points>::const_iterator it=circle.begin()+30;it<circle.begin()+60;it++)
        glVertex2d(it->x,it->y);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(1u,1u);
    for(QVector<Points>::const_iterator it=circle.begin();it<circle.begin()+30;it++)
        glVertex2d(it->x,it->y);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(-1,-1);
    for(QVector<Points>::const_iterator it=circle.begin()+60;it<circle.begin()+90;it++)
        glVertex2d(it->x,it->y);
    glEnd();
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(1u,-1);
    for(QVector<Points>::const_iterator it=circle.begin()+90;it<circle.end();it++)
        glVertex2d(it->x,it->y);
    glEnd();
}

void Daddy::GenerationRay(void)
{
    GenerationRay(ROUND_DEGREE);
}

void Daddy::GenerationRay(quint16 angle)
{
    ray.clear();
    Points*i=radians,*end=radians+angle;
    //while(i<end)ray.append(clockwise ? end-- : i++);
    while(i<end)clockwise ? ray.prepend(i++) : ray.append(i++);
    ray_position=ray.begin(); //Устанавливаем стартовую позицию луча
}

void Daddy::DrawRay(void)const
{
    QColor color=Color;
    //color.setAlphaF(settings["system"]["brightness"].toDouble());
    qglColor(color);
    //glLineWidth(3.0f*settings["system"]["focus"].toDouble());
    glBegin(GL_LINES);
        glVertex2d(static_cast<GLdouble>(.0f),static_cast<GLdouble>(.0f));
        glVertex2d((*ray_position)->x,(*ray_position)->y);
    glEnd();
}

void Daddy::ChangeFPS(qreal fps)
{
    if(fps<=.0f && IsActive())
        timer.stop();
    if(fps>.0f)
    {
        if(IsActive())
            timer.stop();
        timer.start(fps,this);
    }
}

QPixmap Daddy::RotateResourceImage(const QString resource_path,const qint16 degree)
{
    QPixmap original=QPixmap(resource_path),
            pixmap(original.size());
    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);
    p.translate(pixmap.size().width()/2,pixmap.size().height()/2);
    p.rotate(degree);
    p.translate(-pixmap.size().width()/2,-pixmap.size().height()/2);
    p.drawPixmap(0u,0u,original);
    p.end();
    return original=pixmap;
}

qreal Daddy::CalcAlpha(qreal angle)const
{
    qreal alpha;
    if(IsAllVisible())
        alpha=1.0f;
    else
    {
        alpha=(clockwise ? -1 : 1)*((*ray_position)->angle-angle)-.01;
        if(alpha<.0f)
            alpha+=2u*M_PI;
        //if(not_clean && alpha<.0f)
            //alpha+=2u*M_PI;
    }
    return alpha;
}

bool Daddy::IsAllVisible(void)const
{
    return show;
}

void Daddy::SetAllVisible(bool state)
{
    show=state;
}
