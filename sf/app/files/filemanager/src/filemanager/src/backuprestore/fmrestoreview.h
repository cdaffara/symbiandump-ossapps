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
 *     The header file of the restore view of file manager
 */

#ifndef FMRESTOREVIEW_H
#define FMRESTOREVIEW_H

#include "fmoperationservice.h"
#include "fmviewbase.h"

class FmRestoreWigdet;
class FmOperationService;


class FmRestoreView : public FmViewBase
{
	Q_OBJECT

public:
	FmRestoreView();
	~FmRestoreView();

private slots:
    void on_leftAction_triggered();
	void on_restoreAction_triggered();
	void onCheckBoxStateChange();
	void refreshRestoreView();
#ifdef FM_CHANGE_ORIENT_ENABLE
    void on_rotateAction_triggered();
#endif

private:
    void initMenu();
    void initMainWidget();
    void initToolBar();    
    void removeToolBarAction();
    
private:

	FmRestoreWigdet *mRestoreWigdet;
	FmOperationService *mOperationService;
    HbAction *mRestoreAction;
    HbAction *mLeftAction;
};

#endif

