/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: QT C++ based Class.
 * 
 */

#include "dlgrepeatrun.h"
#include <QtGui>

DlgRepeatRun::DlgRepeatRun(QWidget *parent)
    : QDialog(parent),
      loopTimes(0),
      isInfinite(true)
    {
    SetupUI();
    }

void DlgRepeatRun::SetupUI()
    {
    this->setContextMenuPolicy(Qt::NoContextMenu);
    QGridLayout *mainLayout = new QGridLayout(this);
    this->setLayout(mainLayout);
    
    //nullWidget is just place holder for better looking.
    QWidget *nullWidget = new QWidget(this);
    nullWidget->setMinimumHeight(30);
    mainLayout->addWidget(nullWidget, 0, 0);
    
    // check box LoopInfinitely
    rdoLoopInfinitely = new QRadioButton(this);
    rdoLoopInfinitely->setMinimumHeight(40);
    rdoLoopInfinitely->setText(tr("Repeat infinitely."));
    rdoLoopInfinitely->setChecked(Qt::Checked);
    QObject::connect(rdoLoopInfinitely, SIGNAL(clicked(bool)), this,
                SLOT(on_rdoLoopInfinitely_stateChanged(bool)));
    

    // check box LoopTime
    rdoGiveLoopTimes = new QRadioButton(this);
    rdoGiveLoopTimes->setMinimumHeight(40);
    rdoGiveLoopTimes->setText(tr("Give loop times:"));
    rdoGiveLoopTimes->setChecked(Qt::Unchecked);
    QObject::connect(rdoGiveLoopTimes, SIGNAL(clicked(bool)), this,
                SLOT(on_rdoGiveLoopTimes_stateChanged(bool)));
    
    // lineEdit, 
    QValidator *validator = new QIntValidator(1, 999999, this); 
    lineEdit = new QLineEdit(this);
    lineEdit->setValidator(validator);
    lineEdit->setMinimumHeight(40);
    lineEdit->setMaxLength(6);
    lineEdit->setMaximumWidth(60);
    lineEdit->setEchoMode(QLineEdit::NoEcho);
    lineEdit->setReadOnly(true);
    
    // add check box and lineEdit to layout. 
    QWidget *toolWidgetForLoopTimes = new QWidget(this);
    QGridLayout *toolLayoutForLoopTimes = new QGridLayout(this);
    toolWidgetForLoopTimes->setLayout(toolLayoutForLoopTimes);
    toolLayoutForLoopTimes->addWidget(rdoLoopInfinitely, 0, 0);
    toolLayoutForLoopTimes->addWidget(rdoGiveLoopTimes, 1, 0);
    toolLayoutForLoopTimes->addWidget(lineEdit, 1, 1);
    mainLayout->addWidget(toolWidgetForLoopTimes, 1, 0);
    
    // add button
    QWidget *toolWidgetForButtons = new QWidget(this);
    QGridLayout *toolLayoutForButtons = new QGridLayout(this);
    toolWidgetForButtons->setLayout(toolLayoutForButtons);
    btnOk = new QPushButton(tr("Ok"), toolWidgetForButtons);
    QObject::connect(btnOk, SIGNAL(clicked()), this,
            SLOT(on_btnOk_clicked()));
    btnCancel = new QPushButton(tr("Cancel"), toolWidgetForButtons);
    QObject::connect(btnCancel, SIGNAL(clicked()), this,
            SLOT(on_btnCancel_clicked()));
    toolLayoutForButtons->addWidget(btnOk, 0, 0);
    toolLayoutForButtons->addWidget(btnCancel, 0, 1);
    mainLayout->addWidget(toolWidgetForButtons, 2, 0);
    }

void DlgRepeatRun::on_btnOk_clicked()
    {
    if(!isRepeatInfinitely())
        {
        loopTimes = lineEdit->text().toInt();
        if (loopTimes < 0)
            {
            loopTimes = 0;
            }
        }
    else
        {
        loopTimes = 0;
        }
    this->accept();
    }

void DlgRepeatRun::on_btnCancel_clicked()
    {
    this->reject();    
    }

void DlgRepeatRun::on_rdoLoopInfinitely_stateChanged(bool checked)
    {
    if(checked)
        {
        isInfinite = true;
        lineEdit->setEchoMode(QLineEdit::NoEcho);
        lineEdit->setReadOnly(true);
        }
    }

void DlgRepeatRun::on_rdoGiveLoopTimes_stateChanged(bool checked)
    {
    if(checked)
        {
        isInfinite = false;
        lineEdit->setEchoMode(QLineEdit::Normal); 
        lineEdit->setReadOnly(false);
        }
    }

// End of File
