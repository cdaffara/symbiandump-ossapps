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
 * 
 * Description:
 *     The header file of the delete backup widget of file manager
 */

#ifndef FMDELETEBACKUPWIDGET_H
#define FMDELETEBACKUPWIDGET_H

#include <hbwidget.h>

class HbListView;
class QStringListModel;
class FmRestoreSettings;
class FmBackupConfigLoader;
class FmRestoreViewItem;

class FmDeleteBackupWidget : public HbWidget
{
     Q_OBJECT

public:

    explicit FmDeleteBackupWidget( HbWidget *parent = 0  );
    ~FmDeleteBackupWidget();
    void refresh();
    QList<int> selectionIndexes();
    int backupDataCount();
signals:
    void stateChanged(int state);
private:
    void loadData();
    void init();

private slots:
    void on_list_activated( const QModelIndex &index );


private:
    HbListView *mListView;
    QStringListModel  *mModel;

    FmRestoreSettings *mRestoreSettings;
    FmBackupConfigLoader *mBackupConfigLoader;    
};

#endif
