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
 *     The header file of the delete backup view of file manager
 */

#ifndef FMDELETEBACKUPVIEW_H
#define FMDELETEBACKUPVIEW_H

#include "fmoperationservice.h"
#include "fmviewbase.h"

class FmDeleteBackupWidget;
class FmOperationService;
class HbAction;


class FmDeleteBackupView : public FmViewBase
{
	Q_OBJECT

public:
	FmDeleteBackupView();
	~FmDeleteBackupView();
    
private slots:
    void on_leftAction_triggered();
	void on_deleteAction_triggered();
	void on_deleteBackupWidget_stateChanged(int state);
	void refreshDeleteBackupView();
#ifdef FM_CHANGE_ORIENT_ENABLE
    void on_rotateAction_triggered();
#endif

private:
    void initMenu();
    void initMainWidget();
    void initToolBar();    
    void removeToolBarAction();

private:

	FmDeleteBackupWidget *mDeleteBackupWidget;
	FmOperationService *mOperationService;

    HbAction *mDeleteAction;
    HbAction *mLeftAction;
};

#endif

