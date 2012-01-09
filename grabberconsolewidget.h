#ifndef GRABBERCONSOLEWIDGET_H
#define GRABBERCONSOLEWIDGET_H

#include <QtGui>
#include "grabberinterface.h"
#include "grabberthread.h"

class GrabberConsoleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GrabberConsoleWidget(QWidget *parent = 0);
    void setInterface(GrabberInterface *grabberinterface, GrabberThread *grabberThread);

signals:
    
public slots:
    void messageOutput(const QString& message);
    void updateEventCounter(const int count);
    void grabberStatusChanged();

private:
    QPushButton *startStopButton;
    QCheckBox *useEventCounterCheckbox;
    QLabel *eventCounterLabel;
    QPlainTextEdit *logTextEdit;

    GrabberInterface *_grabberinterface;
    GrabberThread *_grabberThread;

private slots:
    void startStopButton_Clicked();
    void useEventCounterCheckbox_stateChanged();
    
};

#endif // GRABBERCONSOLEWIDGET_H
