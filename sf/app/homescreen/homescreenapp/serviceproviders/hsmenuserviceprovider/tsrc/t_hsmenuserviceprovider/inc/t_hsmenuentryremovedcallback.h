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
* Description:  Tests for HomeScreenStateProvider class.
*
*/

#ifndef T_HS_MENU_ENTRY_REMOVED_CALLBACK_H
#define T_HS_MENU_ENTRY_REMOVED_CALLBACK_H

#include <QObject>

class T_HsMenuEntryRemovedCallback: public QObject
{
    Q_OBJECT
public:
    T_HsMenuEntryRemovedCallback(): mCallsCount(0) {}
    int callsCount() {return mCallsCount;}
public slots:
        void callback() { ++mCallsCount;};
private:
    int mCallsCount;
};

#endif
