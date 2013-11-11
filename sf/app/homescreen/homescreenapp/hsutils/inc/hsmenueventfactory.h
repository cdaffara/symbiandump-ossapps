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
* Description:  Menu Event Factory.
*
*/


#ifndef HSMENUEVENTFACTORY_H
#define HSMENUEVENTFACTORY_H

#include <QEvent>
#include <QObject>

#include <qeventtransition.h>

#include "hstest_global.h"
#include "hsutils_global.h"
#include "hsapp_defs.h"
HOMESCREEN_TEST_CLASS(t_hsUtils)


class HSUTILS_EXPORT HsMenuEventFactory: public QObject
{

private:
    HsMenuEventFactory();

public:

    static QEvent *createAddToHomeScreenEvent(int entryId,
        Hs::HsMenuMode menuMode,
        QVariant homescreenData = NULL);

    static QEvent *createOpenCollectionEvent(int itemId,
            const QString &collectionType);
    
    static QEvent *createOpenCollectionFromAppLibraryEvent(int itemId,
            const QString &collectionType);    

    static QEvent *createRenameCollectionEvent(int aItemId);

    static QEvent *createNewCollectionEvent();

    static QEvent *createDeleteCollectionEvent(int aItemId);

    static QEvent *createOpenAppLibraryEvent(
        Hs::HsMenuMode menuMode = Hs::NormalHsMenuMode,
        QVariant homescreenData = NULL);
    
    static QEvent *createBackFromInstalledViewEvent(
        int id,
        QString collectionType);

    static QEvent *createOpenInstalledViewEvent(
        int id = 0,
        QString collectionType = "");
    
    static QEvent *createOpenHomeScreenEvent();

    static QEvent *createCollectionDeletedEvent();

    static QEvent *createAddAppsFromApplicationsViewEvent(
        Hs::HsSortAttribute aApplicationsSortOder,
        int aItemId = 0);

    static QEvent *createAddAppsFromCollectionViewEvent(
        int aCollectionId,
        int aApplicationId = 0);

    static QEvent *createRemoveAppFromCollectionEvent(int aItemId, int aCollectionId);

    static QEvent *createUninstallApplicationEvent(int aItemId);

    static QEvent *createArrangeCollectionEvent(int aTopItemId,
        int aCollectionId);

    static QEvent *createArrangeAllCollectionsEvent(int aTopItemId);

    static QEvent *createPreviewHSWidgetEvent(
        int entryId,
        const QString &entryTypeName = QString(),
        const QString &uri = QString(),
        const QString &library = QString()
    );
    
    static QEvent *createAppSettingsViewEvent(int entryId);
    
    static QEvent *createAppDetailsViewEvent(int entryId);
    
    static QEvent *createInstallationLogEvent();
    
    static QEvent *createUninstallFailedEvent(int error);

    static QEvent *createUnknownEvent();

};


#endif //HSMENUEVENTFACTORY_H
