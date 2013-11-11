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
 *     The header file of the restore widget of file manager
 */

#ifndef FMRESTOREWIDGET_H
#define FMRESTOREWIDGET_H

#include <hbwidget.h>

class HbListView;
class QStringListModel;
class FmRestoreSettings;
class FmBackupConfigLoader;

class FmRestoreWigdet : public HbWidget
{
     Q_OBJECT

public:
    FmRestoreWigdet( QGraphicsItem *parent = 0  );
    ~FmRestoreWigdet();

    QList<int> selectionIndexes();
    int backupDataCount();
    void refresh();
private:
    void init();

signals:    
    void stateChanged(int state);

private slots:
    void on_list_activated( const QModelIndex &index );

private:
    HbListView *mListView;
    QStringListModel  *mModel;

    FmRestoreSettings *mRestoreSettings;
    FmBackupConfigLoader *mBackupConfigLoader;
};

#endif // FMRESTOREWIDGET_H
