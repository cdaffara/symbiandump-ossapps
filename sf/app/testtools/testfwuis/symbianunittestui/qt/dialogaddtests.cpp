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
 * Description: Dialog class to add test dll files.
 *
 */

#include <QDialog>
#include <QTextEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include "dialogaddtests.h"

static const QString filter = "[\n \t\r;]";
static const QChar separator = ',';

DialogAddTests::DialogAddTests(QWidget* parent) :
    QDialog(parent)
    {
    setContextMenuPolicy(Qt::NoContextMenu);
    setWindowTitle(tr("Add Tests"));
    load();
    setLayout();
    clear();
    }

DialogAddTests::~DialogAddTests()
    {
    }

void DialogAddTests::clear()
    {
    txtTests->clear();
    }

void DialogAddTests::saveTests()
    {
    QString text = txtTests->toPlainText();
    if (!text.isNull() && !text.isEmpty())
        {
        text.remove(QRegExp(filter));
        emit this->testsSaved(text.split(separator));
        this->close();
        }
    }

void DialogAddTests::load()
    {
    txtTests = new QTextEdit(this);
    txtTests->setToolTip(tr("enter file names without dll extension, comma separated."));
    connect(txtTests, SIGNAL(textChanged()), this, SLOT(changeText()));
    
    btnOk = new QPushButton(tr("OK"), this);
    connect(btnOk, SIGNAL(clicked()), this, SLOT(saveTests()));
    
    btnCancel = new QPushButton(tr("Cancel"), this);
    connect(btnCancel, SIGNAL(clicked()), this, SLOT(close()));
    
    btnOk->setEnabled(false);
    }

void DialogAddTests::changeText()
    {
    QString text = txtTests->toPlainText();
    if (text.isNull() || text.isEmpty())
        {
        btnOk->setEnabled(false);
        }
    else
        {
        btnOk->setEnabled(true);
        }
    }

void DialogAddTests::setLayout()
    {    
    QGridLayout* layout = new QGridLayout(this);
    layout->setSpacing(2);
    layout->setMargin(2);
    layout->addWidget(txtTests, 0, 0, 1, 2);
    layout->addWidget(btnOk, 1, 0, 1, 1);
    layout->addWidget(btnCancel, 1, 1, 1, 1);
    }

