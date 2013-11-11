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
* Description:
*
*/

#include "fbeditorview.h"

#include <HbView>
#include <HbApplication>
#include <HbAction>
#include <HbMenu>
#include <HbTextEdit>
#include <hbpushbutton.h>

#include <QGraphicsLinearLayout>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QByteArray>
#include <QByteRef>

FbEditorView::FbEditorView() :
    mTextEdit(0),
    mFileHex(false),
    mFlagReadOnly(),
    mFileName(),
    mFileContent(),
    mToTextAction(0),
    mToHexAction(0),
    mExitAction(0)
{
    // Override back navigation action
    HbAction *backNaviAction = new HbAction(Hb::BackNaviAction, this);
    connect(backNaviAction, SIGNAL(triggered()), this, SLOT(backButtonClicked()));
    setNavigationAction(backNaviAction);

    createMenu();

    // text editor
//    mTextEdit = new QTextEdit();
//    QGraphicsLinearLayout *editorLayout = new QGraphicsLinearLayout(Qt::Vertical);
//    QGraphicsProxyWidget *editorWidget = new QGraphicsProxyWidget();
//    editorWidget->setWidget(mTextEdit);
//    editorLayout->addItem(editorWidget);
//    setLayout(editorLayout);

    mTextEdit = new HbTextEdit();
    mTextEdit->setAlignment(Qt::AlignTop);
    QGraphicsLinearLayout *editorLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    editorLayout->addItem(mTextEdit);
    setLayout(editorLayout);
}

FbEditorView::~FbEditorView()
{
//    delete mTextEdit;
//    delete mToTextAction;
//    delete mToHexAction;
//    delete mExitAction;
}

void FbEditorView::open(const QString& fileName, bool flagReadOnly)
{
    mFlagReadOnly = flagReadOnly;

    if (!fileName.isEmpty()) {
        mTextEdit->setReadOnly(mFlagReadOnly);
        loadFile(fileName);
    }
    else {
        // not valid file name
    }
}

void FbEditorView::loadFile(const QString &fileName)
{
    mFileName = fileName;
    QFile file(fileName);
    QFileInfo baseName(fileName);
    setTitle(baseName.fileName());

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        // note: cannot be opened. readonly or not text
        return;
    }
    QTextStream in(&file);
    in.setAutoDetectUnicode(false);

    mFileContent = in.readAll();
    file.close();
    displayInText();
}

void FbEditorView::displayInText()
{
    mFileHex = false;
    QByteArray asciiContent = mFileContent.toAscii();
    mTextEdit->setPlainText(QString(asciiContent));
}

void FbEditorView::displayInHex()
{
    QByteArray asciiContent = mFileContent.toAscii().toHex();
    QString textInHex = QString(asciiContent);
    mTextEdit->setPlainText(textInHex);
    mFileHex = true;
}

void FbEditorView::createMenu()
{
    mToTextAction = menu()->addAction("View as text");
    connect(mToTextAction, SIGNAL(triggered()), this, SLOT(displayInText()));
    mToHexAction = menu()->addAction("View as hex");
    connect(mToHexAction, SIGNAL(triggered()), this, SLOT(displayInHex()));
//    mExitAction = menu()->addAction("Exit");
//    connect(mExitAction, SIGNAL(triggered()), this, SLOT(backButtonClicked()));

    // update the menus before showing it:
    connect(menu(), SIGNAL(aboutToShow()), this, SLOT(updateMenu()));
}

void FbEditorView::updateMenu()
{
    mToTextAction->setVisible(mFileHex);
    mToHexAction->setVisible(!mFileHex);
}	

void FbEditorView::backButtonClicked()
{
    emit finished(false);
}

