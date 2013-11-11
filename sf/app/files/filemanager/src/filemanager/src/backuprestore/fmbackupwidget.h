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
 *     The header file of the back up widget of file manager
 */

#ifndef FMBACKUPWIDGET_H
#define FMBACKUPWIDGET_H

#include <hbwidget.h>

class FmBackupSettings;

class HbDataForm;
class HbDataFormModel;
class HbDataFormModelItem;
class FmBackupConfigLoader;

class FmBackupWidget : public HbWidget
{
    Q_OBJECT

public:
	FmBackupWidget( QGraphicsItem *parent = 0  );
	~FmBackupWidget();
	void updateBackupDate();

signals:
    void doModelRefresh();
    void changeContents();
    void changeScheduling();
    void changeWeekday();
    void changeTime();
    void changeTargetDrive();

public slots:
    // refresh whole model immediately.
    void refreshModel();
    
private slots:
	void on_list_activated( const QModelIndex &index );
    
    void on_changeContents();
    void on_changeScheduling();
    void on_changeWeekday();
    void on_changeTime();
    void on_changeTargetDrive();

private:

    void initModel();
    void init();
    void expandAllGroup();

private:

    HbDataForm *mDataForm;
    HbDataFormModel *mModel;
    HbDataFormModelItem *mContentsGroup;
    HbDataFormModelItem *mSchedulingGroup;

    HbDataFormModelItem *mContentsItem;
    HbDataFormModelItem *mTargetItem;
    HbDataFormModelItem *mSchedulingItem;
    HbDataFormModelItem *mWeekdayItem;
    HbDataFormModelItem *mTimeItem;
    HbDataFormModelItem *mLastBackupDateItem;
	FmBackupSettings        *mBackupSettings;
	FmBackupConfigLoader    *mBackupConfigLoader;
};

#endif // FMBACKUPWIDGET_H
