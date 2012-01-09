#include "grabberconsolewidget.h"
#include "grabberinterface.h"
#include "grabberthread.h"

GrabberConsoleWidget::GrabberConsoleWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle("Grabber Console");

    startStopButton = new QPushButton("Start", this);

    useEventCounterCheckbox = new QCheckBox("Use Event Counter", this);
    eventCounterLabel = new QLabel("Event Count: 0", this);

    logTextEdit = new QPlainTextEdit();
    logTextEdit->setReadOnly(true);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(startStopButton);
    layout->addWidget(useEventCounterCheckbox);
    layout->addWidget(eventCounterLabel);
    layout->addWidget(logTextEdit);

    setMinimumSize(250, 400);

    setLayout(layout);

    connect(startStopButton, SIGNAL(clicked()), this, SLOT(startStopButton_Clicked()));
    connect(useEventCounterCheckbox, SIGNAL(stateChanged(int)), this, SLOT(useEventCounterCheckbox_stateChanged()));
}

void GrabberConsoleWidget::setInterface(GrabberInterface *grabberinterface, GrabberThread *grabberThread)
{
    _grabberinterface = grabberinterface;
    _grabberThread = grabberThread;

    connect(_grabberinterface, SIGNAL(messageOutput(QString)), this, SLOT(messageOutput(QString)), Qt::QueuedConnection);
    connect(_grabberinterface, SIGNAL(updateEventCounter(int)), this, SLOT(updateEventCounter(int)), Qt::QueuedConnection);
    connect(_grabberinterface, SIGNAL(grabberStatusChanged()), this, SLOT(grabberStatusChanged()), Qt::QueuedConnection);

    useEventCounterCheckbox->setChecked(true);
}

void GrabberConsoleWidget::messageOutput(const QString& message)
{
     logTextEdit->appendPlainText(message);
}

void GrabberConsoleWidget::startStopButton_Clicked()
{
    if (_grabberinterface->isOpened())
       _grabberinterface->close();
    else
       _grabberThread->start();
}

void GrabberConsoleWidget::grabberStatusChanged()
{
    if (_grabberinterface->isOpened())
        startStopButton->setText("Stop");
    else
        startStopButton->setText("Start");
}

void GrabberConsoleWidget::useEventCounterCheckbox_stateChanged()
{
    _grabberinterface->setEventCounterUsage(useEventCounterCheckbox->checkState());
}

void GrabberConsoleWidget::updateEventCounter(const int count)
{
    eventCounterLabel->setText("Event Count: " + QString::number(count , 5));
}

