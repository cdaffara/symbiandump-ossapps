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
 *     The header file of the back up view of file manager
 */

#ifndef FMBACKUPVIEW_H
#define FMBACKUPVIEW_H

#include "fmoperationservice.h"
#include "fmviewbase.h"

class FmBackupWidget;
class FmOperationService;

class HbAction;

class FmBackupView : public FmViewBase
{
    Q_OBJECT

public:
    FmBackupView();
    ~FmBackupView();
    
signals:
    void activateRestoreView();

private:
    void initMenu();
    void initMainWidget();
    void initToolBar();
    
    void removeToolBarAction();

public slots:
    void refreshBackupDate();
    void refreshModel();
    
private slots:
    void on_leftAction_triggered();

    void on_backupAction_triggered();
    void on_deleteBackupAction_triggered();

#ifdef FM_CHANGE_ORIENT_ENABLE
    void on_rotateAction_triggered();
#endif

private:
    FmBackupWidget *mMainWidget;
    FmOperationService *mOperationService;
    
    HbAction *mToolBarAction;
};

#endif

