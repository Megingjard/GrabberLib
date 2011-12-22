#include "grabberconsolewidget.h"
#include "grabberinterface.h"

GrabberConsoleWidget::GrabberConsoleWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle("Grabber Console");
    _logTextEdit = new QPlainTextEdit();
    _logTextEdit->setReadOnly(true);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(_logTextEdit);

    setMinimumSize(500, 800);

    setLayout(layout);
}

void GrabberConsoleWidget::setInterface(GrabberInterface *grabberinterface)
{
    _grabberinterface = grabberinterface;
    connect(_grabberinterface, SIGNAL(messageOutput(QString)), this, SLOT(messageOutput(QString)));
}

void GrabberConsoleWidget::messageOutput(const QString& message)
{
    _logTextEdit->appendPlainText(message);
}
