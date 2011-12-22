#ifndef GRABBERCONSOLEWIDGET_H
#define GRABBERCONSOLEWIDGET_H

#include <QWidget>
#include <QtGui>
#include "grabberinterface.h"

class GrabberConsoleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GrabberConsoleWidget(QWidget *parent = 0);
    void setInterface(GrabberInterface *grabberinterface);
    
signals:
    
public slots:
    void messageOutput(const QString& message);

private:
    QPlainTextEdit *_logTextEdit;
    GrabberInterface *_grabberinterface;
    
};

#endif // GRABBERCONSOLEWIDGET_H
