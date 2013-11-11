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
 * Description: hsmenuservice.h
 *
 */

#ifndef HSMENUSERVICE_H
#define HSMENUSERVICE_H

// System includes
#include <QObject>
#include <QStringList>

// User includes
#include "hsapp_defs.h"
#include "hsmenuservice_global.h"
#include "cadefs.h"
#include "caservice.h"

// Forward declarations
class HsMenuItemModel;

// Class declaration
class MENUSERVICE_EXPORT HsMenuService
{

public:

    // Function declarations
    static HsMenuItemModel *getAllApplicationsModel(
        Hs::HsSortAttribute sortAttribute = Hs::DescendingNameHsSortAttribute);
    static HsMenuItemModel *getAllCollectionsModel();
    static HsMenuItemModel *getInstalledModel(
        Hs::HsSortAttribute sortAttribute = Hs::DescendingNameHsSortAttribute);
    static HsMenuItemModel *getCollectionModel(int collectionId,
            Hs::HsSortAttribute sortAttribute = Hs::LatestOnTopHsSortAttribute,
            const QString &collectionType = Hs::collectionTypeName);
    static QStringList getCollectionNames();
    static QString getName(int entryId);
    static int executeAction(int entryId,
            const QString &actionName = Hs::openActionIdentifier,
            QObject* receiver = NULL,
            const char* member = NULL);
    static int executeAction(const CaEntry &entry,
            const QString &actionName = Hs::openActionIdentifier,
            QObject* receiver = NULL,
            const char* member = NULL);
    static bool launchTaskSwitcher();

    static int createCollection(const QString &name);
    static bool renameCollection(int collectionId,
                                 const QString &newCollectionName);
    static bool removeCollection(int collectionId);
    static bool addApplicationsToCollection(
        const QList<int> &applicationList, int collectionId);
    static bool removeApplicationFromCollection(int applicationId,
            int collectionId);

    static bool organizeCollection(int groupId, QList<int> &entryIdList);

    static int allCollectionsId();

    static int collectionIdByType(const QString& collectionType);

    static void touch(int entryId);
    
    static void touch(const CaEntry &entry);

    static void touch(const QList<int> &entryIdList);

    static int launchSoftwareUpdate();

private:
    // data

    static int mAllCollectionsId;

};

#endif // HSMENUSERVICE_H
