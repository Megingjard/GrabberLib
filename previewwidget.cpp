
#include <QtGui>
#include "previewwidget.h"

PreviewWidget::PreviewWidget(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    setFixedSize(400, 300);
    setAutoFillBackground(false);
}

void PreviewWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QBrush background = QBrush(QColor(0, 0, 0));
    painter.fillRect(event->rect(), background);

    painter.end();
}
