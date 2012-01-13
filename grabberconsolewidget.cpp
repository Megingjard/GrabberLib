#include "grabberconsolewidget.h"
#include "grabberinterface.h"
#include "grabberthread.h"

GrabberConsoleWidget::GrabberConsoleWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle("Grabber Console");

    startStopButton = new QPushButton("Start", this);

    useImageCounterCheckbox = new QCheckBox("Use Image Counter", this);
    imageCounterLabel = new QLabel("Image Count: 0", this);

    logTextEdit = new QPlainTextEdit();
    logTextEdit->setReadOnly(true);

    previewDisplay = new PreviewWidget();

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(startStopButton, 0, 0);
    layout->addWidget(useImageCounterCheckbox, 1, 0);
    layout->addWidget(imageCounterLabel, 2, 0);
    layout->addWidget(logTextEdit, 3, 0);
    layout->addWidget(previewDisplay, 0, 1, 0, 4);

    setMinimumSize(250, 400);
    setLayout(layout);

    connect(startStopButton, SIGNAL(clicked()), this, SLOT(startStopButton_Clicked()));
    connect(useImageCounterCheckbox, SIGNAL(stateChanged(int)), this, SLOT(useImageCounterCheckbox_stateChanged()));
}

void GrabberConsoleWidget::setInterface(GrabberInterface *grabberinterface, GrabberThread *grabberThread)
{
    _grabberinterface = grabberinterface;
    _grabberThread = grabberThread;

    connect(_grabberinterface, SIGNAL(messageOutput(QString)), this, SLOT(messageOutput(QString)), Qt::QueuedConnection);
    connect(_grabberinterface, SIGNAL(updateImageCounter(uint)), this, SLOT(updateImageCounter(uint)), Qt::QueuedConnection);
    connect(_grabberinterface, SIGNAL(grabberStatusChanged()), this, SLOT(grabberStatusChanged()), Qt::QueuedConnection);
    connect(_grabberinterface, SIGNAL(updatePreviewDisplay()), this, SLOT(updatePreviewDisplay()), Qt::QueuedConnection);

    useImageCounterCheckbox->setChecked(true);
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

void GrabberConsoleWidget::useImageCounterCheckbox_stateChanged()
{
    _grabberinterface->setImageCounterUsage(useImageCounterCheckbox->checkState());
}

void GrabberConsoleWidget::updateImageCounter(const uint count)
{
    imageCounterLabel->setText("Image Count: " + QString::number(count , 5));
}

void GrabberConsoleWidget::updatePreviewDisplay()
{
    messageOutput("Update Display");

    // Show the grabbed image in the OpenGL Widget
}











