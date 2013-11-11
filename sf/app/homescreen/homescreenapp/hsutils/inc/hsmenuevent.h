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
* Description:  Menu Event.
*
*/

#ifndef HSMENUEVENT_H
#define HSMENUEVENT_H

#include <QEvent>
#include <QObject>
#include <qeventtransition.h>
#include <QVariant>

#include "hstest_global.h"
#include "hsutils_global.h"
HOMESCREEN_TEST_CLASS(t_hsUtils)



class HSUTILS_EXPORT HsMenuEvent : public QEvent
{
public:
    enum OperationType {
        AddToHomeScreen,
        OpenCollection,
        OpenCollectionFromAppLibrary,
        RenameCollection,
        DeleteCollection,
        CollectionDeleted,
        AddAppsToCollection,
        RemoveAppFromCollection,
        UninstallApplication,
        OpenApplicationLibrary,
        OpenInstalledView,
        BackFromInstalledView,
        OpenHomeScreen,
        ArrangeCollection,
        ArrangeAllCollections,
        CreateCollection,
        PreviewHSWidget,
        ShowAppSettings,
        ShowAppDetails,
        ShowInstallationLog,
        UninstallationFailed,
        Unknown
    };

    static QEvent::Type eventType();

    HsMenuEvent(OperationType aOperation, const QVariantMap &aData = QVariantMap());

    OperationType operation() const;

    QVariantMap data() const;

private:

    OperationType mOperation;

    QVariantMap mData;
};


#endif //HSMENUEVENT_H
