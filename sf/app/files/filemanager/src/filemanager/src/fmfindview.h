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
*     The header file of the file find view of file manager
*
*/

#ifndef FMFINDVIEW_H
#define FMFINDVIEW_H

#include "fmcommon.h"
#include "fmviewbase.h"

class HbProgressDialog;
class HbMenu;
class FmFindWidget;

class FmFindView : public FmViewBase
{
    Q_OBJECT
public:
    FmFindView();
    virtual ~FmFindView();

    void find( const QString &keyword, const QStringList &pathList );

public slots:
    void activated( const QString &pathName );
    
private slots:
    void on_sortNameAction_triggered();
    void on_sortTimeAction_triggered();
    void on_sortSizeAction_triggered();
    void on_sortTypeAction_triggered();
    
    void on_findWidget_setEmptyMenu( bool isMenuEmpty );
    
private:
    void initMenu();
    void initMainWidget();
    void initToolBar();

    // from FmViewBase
    virtual void aboutToClose();

    FmFindWidget *mFindWidget;
    HbMenu *mMenu;
};

#endif
