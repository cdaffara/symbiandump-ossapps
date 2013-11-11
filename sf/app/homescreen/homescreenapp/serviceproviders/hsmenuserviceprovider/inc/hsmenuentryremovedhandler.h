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
* Description:  Entry removal handler.
*
*/


#ifndef HS_MENU_ENTRY_REMOVED_HANDLER_H
#define HS_MENU_ENTRY_REMOVED_HANDLER_H

#include <QScopedPointer>

#include <cadefs.h>
#include <canotifier.h>

#include "hsmenuservice_global.h"

class MENUSERVICE_EXPORT  HsMenuEntryRemovedHandler: public QObject
{
    Q_OBJECT
public:
    HsMenuEntryRemovedHandler(
        int entryId, QObject *receiver, const char *callback);
    virtual ~HsMenuEntryRemovedHandler();

signals:
    void notify();
private slots:
    virtual void entryChanged(int entryId, ChangeType changeType);
    
private:
    void subscribe(int entryId);

private:
    QScopedPointer<CaNotifier> mNotifier;
};
#endif
