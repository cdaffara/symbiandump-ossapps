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

#ifndef FBFOLDERSELECTIONDIALOG_H_
#define FBFOLDERSELECTIONDIALOG_H_

#include <HbDialog>

// forward declarations
class FbFolderSelectorWrapper;
class HbListWidget;
class HbListWidgetItem;
class HbLabel;
class QFileIconProvider;

class FbFolderSelectionDialog : public HbDialog
{
    Q_OBJECT

public:
    explicit FbFolderSelectionDialog(QGraphicsItem *parent = 0);
    virtual ~FbFolderSelectionDialog();

    QString selectedFolder();

private:
    void init();
    void createHeading();
    void createList();
    void createToolBar();

private slots:
    void activated(HbListWidgetItem *);
    void refreshView();
    void moveUpPressed();

protected:
    HbLabel *mTitle;

private:
    HbListWidget *mFolderList;
    FbFolderSelectorWrapper *mFolderSelectorWrapper;
    QFileIconProvider *mFileIconProvider;
};

class FbCopyToFolderSelectionDialog : public FbFolderSelectionDialog
{
    Q_OBJECT

public:
    explicit FbCopyToFolderSelectionDialog(QGraphicsItem *parent = 0);
};

class FbMoveToFolderSelectionDialog : public FbFolderSelectionDialog
{
    Q_OBJECT

public:
    explicit FbMoveToFolderSelectionDialog(QGraphicsItem *parent = 0);
};

#endif // FBFOLDERSELECTIONDIALOG_H_
