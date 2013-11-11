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
* Description: Dialog class to display some useful message.
*
*/

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "dialogmsg.h"

DialogMsg::DialogMsg(QWidget* parent): QDialog(parent)
    {
    createControls();
    setLayout();
    setupEventHandler();
    }

void DialogMsg::createControls()
    {
    btnOk = new QPushButton(tr("OK"), this);
    lableMsg = new QLabel(this);
    }

void DialogMsg::setLayout()
    {
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->addWidget(lableMsg);
    QWidget* wdgt = new QWidget(this);
    QHBoxLayout* hLayout = new QHBoxLayout(this);
    hLayout->addWidget(btnOk);
    wdgt->setLayout(hLayout);
    vLayout->addWidget(wdgt);
    }

void DialogMsg::setupEventHandler()
    {
    connect(btnOk, SIGNAL(clicked()), this, SLOT(close()));
    }

DialogMsg::~DialogMsg()
    {
    }

void DialogMsg::showMsg(const QString msg)
    {
    lableMsg->setText(msg);
    this->show();
    int res = this->exec();
    }
