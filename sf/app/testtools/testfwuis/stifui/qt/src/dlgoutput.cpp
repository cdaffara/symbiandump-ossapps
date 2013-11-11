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
#include "dlgoutput.h"
#include <QtGui>

DlgOutput::DlgOutput(IStfQtUIController* ctl, QWidget *parent) :
    QDialog(parent), controller(ctl)
    {
    QGridLayout *mainLayout = new QGridLayout(this);
    this->setLayout(mainLayout);
    this->setContextMenuPolicy(Qt::NoContextMenu);

    tabMain = new QTabWidget(this);
    tabMain->setContextMenuPolicy(Qt::NoContextMenu);

    QWidget *toolWidget = new QWidget(this);
    toolWidget->setContextMenuPolicy(Qt::NoContextMenu);
    QGridLayout *toolLayout = new QGridLayout(this);
    toolWidget->setLayout(toolLayout);
    btnPause = new QPushButton(tr("Pause"), toolWidget);
    btnPause->setContextMenuPolicy(Qt::NoContextMenu);
    QObject::connect(btnPause, SIGNAL(clicked()), this,
            SLOT(on_btnPause_clicked()));
    btnAbort = new QPushButton(tr("Abort"), toolWidget);
    btnAbort->setContextMenuPolicy(Qt::NoContextMenu);
    QObject::connect(btnAbort, SIGNAL(clicked()), this,
            SLOT(on_btnAbort_clicked()));
    
    btnClose = new QPushButton(tr("Hide"), toolWidget);
    btnClose->setContextMenuPolicy(Qt::NoContextMenu);
    QObject::connect(btnClose, SIGNAL(clicked()), this,
            SLOT(on_btnClose_clicked()));
    toolLayout->addWidget(btnPause, 0, 0);
    toolLayout->addWidget(btnAbort, 0, 1);
    toolLayout->addWidget(btnClose, 0, 2);
        

    mainLayout->addWidget(toolWidget, 0, 0);
    mainLayout->addWidget(tabMain, 1, 0);
    controller->AddStfEventListener(this);
    }

DlgOutput::~DlgOutput()
    {
    controller->RemoveStfEventListener(this);
    }

void DlgOutput::CreateItem(QString index, QString item)
    {
    QPlainTextEdit* edit = new QPlainTextEdit(this);
    edit->setContextMenuPolicy(Qt::NoContextMenu);
    tabMain->addTab(edit, item);
    tabList.insert(index, edit);
    }

void DlgOutput::CloseItem(QString index)
    {
    delete tabList.value(index);
    tabList.remove(index);
    if (tabMain->count() == 0)
        {
        this->close();
        }
    }

void DlgOutput::ShowMessage(QString index, QString msg)
    {
    if(tabList.contains(index))
        {
        tabList.value(index)->setPlainText(msg);    
        }
    else
        {
//        bool ok;
//        CSTFCase acase = controller->GetRunningCase(index.toInt(&ok, 10));
//        CreateItem(index, acase.Name());
//        ShowMessage(index, msg);
        }
    }

void DlgOutput::on_btnPause_clicked()
    {
    if (btnPause->text() == "Pause")
        {
        controller->PauseCase();
        btnPause->setText(tr("Resume"));
        }
    else
        {
        controller->ResumeCase();
        btnPause->setText(tr("Pause"));
        }
    }

void DlgOutput::on_btnAbort_clicked()
    {
    controller->AbortCase();
    }

void DlgOutput::on_btnClose_clicked()
    {
    controller->SetShowOutput(false);
    this->close();
    }

void DlgOutput::OnCaseOutputChanged(const IStfEventListener::CaseOutputCommand& cmd,
        const QString& index, const QString& msg)
    {
    if(controller->ShowOutput() && this->isVisible() == false)
        {
        this->showMaximized();
        }
    switch (cmd)
        {
        case IStfEventListener::ECreate:
            CreateItem(index, msg);
            break;
        case IStfEventListener::EClose:
            CloseItem(index);
            break;
        default:
            ShowMessage(index, msg);
            break;
        }

    }

// End of File
