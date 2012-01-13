#ifndef GRABBERCONSOLEWIDGET_H
#define GRABBERCONSOLEWIDGET_H

#include <QtGui>
#include <QGLWidget>
#include "grabberinterface.h"
#include "grabberthread.h"
#include "previewwidget.h"

class GrabberConsoleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GrabberConsoleWidget(QWidget *parent = 0);
    void setInterface(GrabberInterface *grabberinterface, GrabberThread *grabberThread);

signals:
    
public slots:
    void messageOutput(const QString& message);
    void updateImageCounter(const uint count);
    void grabberStatusChanged();

    void updatePreviewDisplay();

private:
    QPushButton *startStopButton;
    QCheckBox *useImageCounterCheckbox;
    QLabel *imageCounterLabel;

    QPlainTextEdit *logTextEdit;
    PreviewWidget *previewDisplay;

    GrabberInterface *_grabberinterface;
    GrabberThread *_grabberThread;

private slots:
    void startStopButton_Clicked();
    void useImageCounterCheckbox_stateChanged();
    
};

#endif // GRABBERCONSOLEWIDGET_H
