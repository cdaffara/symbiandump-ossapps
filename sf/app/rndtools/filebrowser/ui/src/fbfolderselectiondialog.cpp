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

#include "fbfolderselectiondialog.h"
#include "fbfolderselectorwrapper.h"

#include <HbListWidget>
#include <HbAction>
#include <HbListWidgetItem>
#include <HbLabel>
#include <HbPushButton>

#include <QGraphicsLinearLayout>
#include <QFileInfo>
#include <QFileIconProvider>

FbFolderSelectionDialog::FbFolderSelectionDialog(QGraphicsItem *parent) :
        HbDialog(parent),
        mTitle(0),
        mFolderList(0),
        mFolderSelectorWrapper(0),
        mFileIconProvider(0)
{
    init();
}

FbFolderSelectionDialog::~FbFolderSelectionDialog()
{
    if (mFileIconProvider)
        delete mFileIconProvider;
}

QString FbFolderSelectionDialog::selectedFolder()
{
    return mFolderSelectorWrapper->currentPath();
}

void FbFolderSelectionDialog::init()
{
    setAttribute(Qt::WA_DeleteOnClose);

    mFileIconProvider = new QFileIconProvider();

    createHeading();
    createList();
    createToolBar();
}

void FbFolderSelectionDialog::createHeading()
{
    HbWidget *headingWidget = new HbWidget(this);
    QGraphicsLinearLayout *headingLayout = new QGraphicsLinearLayout(Qt::Horizontal, headingWidget);
    headingWidget->setLayout(headingLayout);

    mTitle = new HbLabel(this);
    mTitle->setElideMode(Qt::ElideRight);

    HbPushButton *upButton = new HbPushButton(this);
    upButton->setIcon(HbIcon(QString(":/qtg_indi_status_back.svg")));
    connect(upButton, SIGNAL(pressed()),
            this, SLOT(moveUpPressed()));

    headingLayout->addItem(mTitle);
    headingLayout->addItem(upButton);
    headingLayout->setAlignment(upButton, Qt::AlignRight);

    setHeadingWidget(headingWidget);
}

void FbFolderSelectionDialog::createList()
{
    mFolderList = new HbListWidget(this);
    mFolderSelectorWrapper = new FbFolderSelectorWrapper();
    mFolderSelectorWrapper->init();

    refreshView();

    setContentWidget(mFolderList);
    connect(mFolderList, SIGNAL(activated(HbListWidgetItem *)),
            this, SLOT(activated(HbListWidgetItem *)));

    connect(mFolderSelectorWrapper, SIGNAL(FolderSelectionChanged()),
            this, SLOT(refreshView()));
}

void FbFolderSelectionDialog::createToolBar()
{
    HbAction *rejectAction = new HbAction(QString("Cancel"), this);
    addAction(rejectAction);
}

void FbFolderSelectionDialog::refreshView()
{
    mFolderList->clear();
    QIcon icon;
    if (mFolderSelectorWrapper->isDriveListViewActive())
    {
        const QString KSimpleDriveEntry("%d\t%c: <%S>\t\t");
        //TODO icon = mFileIconProvider->icon(QFileIconProvider::Drive);
        icon = mFileIconProvider->icon(QFileIconProvider::File);

        for (TInt i=0; i<mFolderSelectorWrapper->itemCount(); i++) {
            FbDriveEntry driveEntry = mFolderSelectorWrapper->getDriveEntry(i);

            const QString SimpleDriveEntry("%1: <%2>");
            QString diskName = SimpleDriveEntry.arg(driveEntry.driveLetter()).arg(driveEntry.mediaTypeString());

            mFolderList->addItem(icon, diskName);
        }
    } else {
        const QString SimpleFileEntry("%1");
        icon = mFileIconProvider->icon(QFileIconProvider::Folder);

        // append current folder item
        QString currentAbsolutePath = mFolderSelectorWrapper->currentPath().left(mFolderSelectorWrapper->currentPath().length()-1);
        QFileInfo fileInfo(currentAbsolutePath);
        QString currentFolderName = fileInfo.fileName();
        if (currentFolderName.isEmpty()) {
            currentFolderName = QString("[root level]");
        }

        QString currentDirTextEntry = SimpleFileEntry.arg(currentFolderName); ///*<< fileEntry.IconId()*/
        mFolderList->addItem(icon, currentDirTextEntry);

        for (TInt i=0; i<mFolderSelectorWrapper->itemCount(); i++) {
            FbFileEntry fileEntry = mFolderSelectorWrapper->getFileEntry(i);

            QString fileName = SimpleFileEntry.arg(fileEntry.name()); ///*<< fileEntry.IconId()*/

            mFolderList->addItem(icon, fileName);
        }
    }
}

void FbFolderSelectionDialog::activated(HbListWidgetItem * item)
{
    int row = mFolderList->row(item);
    if (mFolderSelectorWrapper->isDriveListViewActive()) {
        mFolderSelectorWrapper->moveDownToDirectory(row);
    } else if (row == 0) {
        if (this->actions().count() > 0) {
            accept();
        }
    } else if (row > 0) {
        mFolderSelectorWrapper->moveDownToDirectory(row);
    }
}

void FbFolderSelectionDialog::moveUpPressed()
{
    mFolderSelectorWrapper->moveUpOneLevel();
}

// ---------------------------------------------------------------------------

FbCopyToFolderSelectionDialog::FbCopyToFolderSelectionDialog(QGraphicsItem *parent) :
        FbFolderSelectionDialog(parent)
{
    if (headingWidget()) {
        mTitle->setPlainText(QString("Copy To"));
    }
}

// ---------------------------------------------------------------------------

FbMoveToFolderSelectionDialog::FbMoveToFolderSelectionDialog(QGraphicsItem *parent) :
        FbFolderSelectionDialog(parent)
{
    if (headingWidget()) {
        mTitle->setPlainText(QString("Move To"));
    }
}

// End of file
