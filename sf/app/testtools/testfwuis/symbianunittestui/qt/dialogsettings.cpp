/*
 * Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 *
 * Description: Dialog class to set options when running tests.
 *
 */

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "dialogsettings.h"
#include "settings.h"

static const QString FORMAT_HTML = "html";
static const QString FORMAT_XML = "xml";
static const QString FORMAT_TEXT = "txt";

static const QString WIN_TITLE = "Settings";

static const QString TXT_TIMEOUT = "Timeout";
static const QString TXT_TIMEOUTUNIT = "second(s)";
static const int DEFAULT_TIMEOUT = 30;

static const QString TXT_FORMAT = "Output Format";

static const QString TXT_MEMSIM = "Simulate Mem Allocation Failure";

static const QString TXT_SAVE = "Save";
static const QString TXT_CANCEL = "Cancel";

DialogSettings::DialogSettings(QWidget* parent) :
    QDialog(parent)
    {
    setContextMenuPolicy(Qt::NoContextMenu);
    setWindowTitle(WIN_TITLE);
    savedSettings = new Settings(DEFAULT_TIMEOUT, false, FORMAT_HTML);
    load();
    }

DialogSettings::~DialogSettings()
    {
    }

void DialogSettings::restoreSettings()
    {
    bool timoutChanged = savedSettings->timeout != DEFAULT_TIMEOUT;
    chkBoxTimeout->setChecked(timoutChanged);
    lineEditTimeout->setEnabled(timoutChanged);
    lineEditTimeout->setText(tr("%1").arg(savedSettings->timeout));
    bool outputFormatChanged =
            savedSettings->outputFormat.compare(FORMAT_HTML) != 0;
    chkBoxOutputFormat->setChecked(outputFormatChanged);
    comboBoxOutputFormat->setEnabled(outputFormatChanged);
    comboBoxOutputFormat->setCurrentIndex(listFormats.indexOf(
            savedSettings->outputFormat));
    chkBoxMemAllocFailureSim->setChecked(savedSettings->memAllocFailureSim);
    btnSave->setEnabled(false);
    }

void DialogSettings::createTimeoutSettings()
    {
    chkBoxTimeout = new QCheckBox(TXT_TIMEOUT, this);
    lineEditTimeout = new QLineEdit(tr("%1").arg(DEFAULT_TIMEOUT), this);
    lineEditTimeout->setEnabled(false);
    labelTimeoutUnit = new QLabel(TXT_TIMEOUTUNIT, this);
    }

void DialogSettings::setupTimeoutSettingsEventHandlers()
    {
    connect(chkBoxTimeout, SIGNAL(stateChanged(int)), this,
            SLOT(setLineEditTimeoutEnabled(int)));
    connect(lineEditTimeout, SIGNAL(textChanged(const QString&)), this,
            SLOT(changeText(const QString&)));
    }

void DialogSettings::createOutputFormatSettings()
    {
    chkBoxOutputFormat = new QCheckBox(TXT_FORMAT, this);
    comboBoxOutputFormat = new QComboBox(this);
    listFormats << FORMAT_HTML << FORMAT_XML << FORMAT_TEXT;
    comboBoxOutputFormat->addItems(listFormats);
    comboBoxOutputFormat->setEnabled(false);
    }

void DialogSettings::setupOutputFormatSettingsEventHandlers()
    {
    connect(chkBoxOutputFormat, SIGNAL(stateChanged(int)), this,
            SLOT(setComboxFormatEnabled(int)));
    connect(comboBoxOutputFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(changeIndex(int)));
    }

void DialogSettings::createMemSimSettings()
    {
    chkBoxMemAllocFailureSim = new QCheckBox(TXT_MEMSIM, this);
    }

void DialogSettings::setupMemSimSettingsEventHandlers()
    {
    connect(chkBoxMemAllocFailureSim, SIGNAL(stateChanged(int)), this,
            SLOT(setMemAllocFailureSim(int)));
    }

void DialogSettings::createButtons()
    {
    btnSave = new QPushButton(TXT_SAVE, this);
    btnCancel = new QPushButton(TXT_CANCEL, this);
    btnSave->setEnabled(false);
    }

void DialogSettings::setupButtonsEventHandlers()
    {
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(btnSave, SIGNAL(clicked()), this, SLOT(saveSettings()));
    }

void DialogSettings::load()
    {
    createTimeoutSettings();
    setupTimeoutSettingsEventHandlers();
    createOutputFormatSettings();
    setupOutputFormatSettingsEventHandlers();
    createMemSimSettings();
    setupMemSimSettingsEventHandlers();
    createButtons();
    setupButtonsEventHandlers();

    setLayout();
    }

void DialogSettings::setLayout()
    {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    // :( should check NULL pointer.
    mainLayout->addWidget(setTimeoutCtlsLayout());
    mainLayout->addWidget(setOutputCtlsLayout());
    mainLayout->addWidget(setMemAllocFailureSimCtrlLayout());
    mainLayout->addWidget(setButtonsLayout());
    }

QWidget* DialogSettings::setTimeoutCtlsLayout()
    {
    QWidget* timeoutLayoutWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(timeoutLayoutWidget);
    layout->addWidget(chkBoxTimeout);
    layout->addWidget(lineEditTimeout);
    layout->addWidget(labelTimeoutUnit);
    timeoutLayoutWidget->setLayout(layout);
    return timeoutLayoutWidget;
    }

QWidget* DialogSettings::setOutputCtlsLayout()
    {
    QWidget* formatLayoutWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(formatLayoutWidget);
    layout->addWidget(chkBoxOutputFormat);
    layout->addWidget(comboBoxOutputFormat);
    formatLayoutWidget->setLayout(layout);
    return formatLayoutWidget;
    }
QWidget* DialogSettings::setMemAllocFailureSimCtrlLayout()
    {
    QWidget* memSimLayoutWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(memSimLayoutWidget);
    layout->addWidget(chkBoxMemAllocFailureSim);
    memSimLayoutWidget->setLayout(layout);
    return memSimLayoutWidget;
    }

QWidget* DialogSettings::setButtonsLayout()
    {
    QWidget* btnLayoutWidget = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(btnLayoutWidget);
    layout->addWidget(btnSave);
    layout->addWidget(btnCancel);
    btnLayoutWidget->setLayout(layout);
    return btnLayoutWidget;
    }

void DialogSettings::setBtnSaveEnabled()
    {
    int timeout;
    if (getTimeout(&timeout))
        {
        Settings curSettings(timeout,
                chkBoxMemAllocFailureSim->checkState() == Qt::Checked,
                comboBoxOutputFormat->currentText());

        btnSave->setEnabled(!savedSettings->isEqual(curSettings));
        }
    }

void DialogSettings::setLineEditTimeoutEnabled(int chkState)
    {
    lineEditTimeout->setEnabled(chkState == Qt::Checked);
    if (!lineEditTimeout->isEnabled())
        {
        lineEditTimeout->setText(tr("%1").arg(DEFAULT_TIMEOUT));
        }
    setBtnSaveEnabled();
    }

void DialogSettings::setComboxFormatEnabled(int chkState)
    {
    comboBoxOutputFormat->setEnabled(chkState == Qt::Checked);
    if (!comboBoxOutputFormat->isEnabled())
        {
        comboBoxOutputFormat->setCurrentIndex(
                listFormats.indexOf(FORMAT_HTML));
        }
    setBtnSaveEnabled();
    }

void DialogSettings::setMemAllocFailureSim(int /*chkState*/)
    {
    setBtnSaveEnabled();
    }

void DialogSettings::changeIndex(int /*index*/)
    {
    setBtnSaveEnabled();
    }

void DialogSettings::changeText(const QString& /*text*/)
    {
    setBtnSaveEnabled();
    }

void DialogSettings::saveSettings()
    {
    int timeout;
    if (getTimeout(&timeout))
        {
        savedSettings->timeout = timeout;
        savedSettings->memAllocFailureSim = chkBoxMemAllocFailureSim->checkState() == Qt::Checked;
        savedSettings->outputFormat = comboBoxOutputFormat->currentText();
        emit this->settingsSaved(savedSettings);
        }
    this->close();
    }

bool DialogSettings::getTimeout(int* timeout)
    {
    bool ok;
    *timeout = lineEditTimeout->text().toInt(&ok, 0);
    return ok;
    }
