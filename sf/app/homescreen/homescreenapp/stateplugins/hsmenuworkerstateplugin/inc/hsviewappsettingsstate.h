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
* Description:  Menu Application Library state.
*
*/


#ifndef HSVIEWAPPSETTINGSSTATE_H
#define HSVIEWAPPSETTINGSSTATE_H

#include <QState>

#include "hsmenustates_global.h"
#include "hbmainwindow.h"

HS_STATES_TEST_CLASS(MenuStatesTest)

class HsMenuService;
class HbView;
class HbAction;
class CaNotifier;

class HsViewAppSettingsState : public QState
{
    Q_OBJECT

    HS_STATES_TEST_FRIEND_CLASS(MenuStatesTest)

public:

    HsViewAppSettingsState(QState *parent = 0);

    virtual ~HsViewAppSettingsState();
    
signals:
    void initialize(const QString &aUid, const QString &aAppName);      
    void exit();

protected:

    void onEntry(QEvent *event);
    void onExit(QEvent *event);

private:

    void construct();
    
    HbMainWindow *mainWindow() const; 

private:

    HbView *mView;

    HbView *mPreviousView;
    
    HbAction *mActionConfirm;
};



#endif //HSVIEWAPPSETTINGSSTATE_H
