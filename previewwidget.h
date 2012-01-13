#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QGLWidget>

class PreviewWidget : public QGLWidget
{
    Q_OBJECT
public:
    PreviewWidget(QWidget *parent = 0);
    
signals:
    
public slots:

protected:
    void paintEvent(QPaintEvent *event);

};

#endif // PREVIEWWIDGET_H
