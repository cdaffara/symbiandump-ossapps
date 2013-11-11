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
 *     Zhiqiang Yang <zhiqiang.yang@nokia.com>
 * 
 * Description:
 *     The header file of File Manager driver view
 */

#ifndef FMDRIVERVIEW_H
#define FMDRIVERVIEW_H

#include "fmcommon.h"
#include "fmviewbase.h"

class FmDriverListWidget;
class FmOperationService;
class HbSearchPanel;
class HbAction;

class FmDriverView : public FmViewBase
{
    Q_OBJECT
public:
    FmDriverView();
    virtual ~FmDriverView();
    
private slots:
    void on_leftAction_triggered();
    void on_rightAction_triggered();
    void on_backupAction_triggered();
    void on_restoreAction_triggered();
    void on_findAction_triggered();

    void activated( const QString &pathName );
    void refreshDrive();
    void startSearch( const QString &criteria );

#ifdef FM_CHANGE_ORIENT_ENABLE
    void on_rotateAction_triggered();
#endif

private:
    void initMenu();
    void initDiskListWidget();
    void initToolBar();
    
    void removeToolBarAction();
    
private:
    FmDriverListWidget *mDriverList;
    HbSearchPanel* mSearchPanel;
    
    HbAction *mToolBarLeftAction;
    HbAction *mToolBarRightAction;
};

#endif

