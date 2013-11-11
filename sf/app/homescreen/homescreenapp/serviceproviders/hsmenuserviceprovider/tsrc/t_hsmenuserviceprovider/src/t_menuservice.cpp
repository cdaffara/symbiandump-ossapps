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
 * Description:  Main test class for hsHomeScreenStateProvider library.
 *
 */

#include <qcoreapplication.h>
#include <qstandarditemmodel.h>
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hbgridview.h>
#include <hbgridviewitem.h>
#include <hstest_global.h>
#include <qvaluespacepublisher.h>

#include "caitemmodel.h"
#include "hsmenuitemmodel.h"
#include "caservice.h"
#include "hsmenuservice.h"
#include "hsmenuserviceutils.h"
#include "t_hsmenuserviceprovider.h"

QTM_USE_NAMESPACE

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_getAllApplicationsModel()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        const Hs::HsSortAttribute
            defaultSortAttribute(Hs::DescendingNameHsSortAttribute);

        QScopedPointer<HsMenuItemModel>
            model(HsMenuService::getAllApplicationsModel());

        CaQuery actualQuery = model->mCaItemModelQuery;
        CaQuery expectedQuery;
        expectedQuery.setEntryRoles(ItemEntryRole);
        expectedQuery.addEntryTypeName(Hs::applicationTypeName);
        expectedQuery.addEntryTypeName(Hs::urlTypeName);
        expectedQuery.addEntryTypeName(Hs::widgetTypeName);
        expectedQuery.addEntryTypeName(Hs::templatedApplicationTypeName);
        expectedQuery.setFlagsOn(VisibleEntryFlag);
        expectedQuery.setFlagsOff(MissingEntryFlag);
        expectedQuery.setSort(HsMenuServiceUtils::sortBy(defaultSortAttribute),
                      HsMenuServiceUtils::sortOrder(defaultSortAttribute));
        QCOMPARE(actualQuery.mSetEntryRoles, expectedQuery.mSetEntryRoles);
        QCOMPARE(actualQuery.mAddEntryTypeNames,
            expectedQuery.mAddEntryTypeNames);
        QCOMPARE(actualQuery.mSetFlagsOn, expectedQuery.mSetFlagsOn);
        QCOMPARE(actualQuery.mSetFlagsOff, expectedQuery.mSetFlagsOff);
        QCOMPARE(actualQuery.mSetSortAttribute,
            expectedQuery.mSetSortAttribute);
        QCOMPARE(actualQuery.mSetSortOrder, expectedQuery.mSetSortOrder);

        // check non-default sort attribute
        const Hs::HsSortAttribute
            ascendingSortAttribute(Hs::AscendingNameHsSortAttribute);

        model.reset(
            HsMenuService::getAllApplicationsModel(ascendingSortAttribute));

        actualQuery = model->mCaItemModelQuery;

        expectedQuery.setSort(
            HsMenuServiceUtils::sortBy(ascendingSortAttribute),
            HsMenuServiceUtils::sortOrder(ascendingSortAttribute));

        QCOMPARE(actualQuery.mSetSortAttribute,
            expectedQuery.mSetSortAttribute);
        QCOMPARE(actualQuery.mSetSortOrder, expectedQuery.mSetSortOrder);
        }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif

}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_getAllCollectionsModel()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
    const int allCollectionId = 8;
    // ensure HsMenuService::allCollectionId will return
    // non zero value, if it does not already
    CaService::instance()->mGetEntryIdsResult.clear();
    CaService::instance()->mGetEntryIdsResult << allCollectionId;

        QScopedPointer<HsMenuItemModel>
            model(HsMenuService::getAllCollectionsModel());

        CaQuery actualQuery = model->mCaItemModelQuery;

        CaQuery expectedQuery;
        expectedQuery.setParentId(HsMenuService::allCollectionsId());
        expectedQuery.setFlagsOn(VisibleEntryFlag);
        expectedQuery.setFlagsOff(MissingEntryFlag);

        QCOMPARE(actualQuery.mSetParentId, expectedQuery.mSetParentId);
        QCOMPARE(actualQuery.mSetFlagsOn, expectedQuery.mSetFlagsOn);
        QCOMPARE(actualQuery.mSetFlagsOff, expectedQuery.mSetFlagsOff);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_getInstalledModel()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        const Hs::HsSortAttribute
            defaultSortAttribute(Hs::DescendingNameHsSortAttribute);

        QScopedPointer<HsMenuItemModel>
            model(HsMenuService::getInstalledModel());

        CaQuery actualQuery = model->mCaItemModelQuery;

        CaQuery expectedQuery;
        expectedQuery.addEntryTypeName(Hs::packageTypeName);
        expectedQuery.setFlagsOn(VisibleEntryFlag | RemovableEntryFlag);
        expectedQuery.setFlagsOff(MissingEntryFlag);
        expectedQuery.setSort(HsMenuServiceUtils::sortBy(defaultSortAttribute),
                      HsMenuServiceUtils::sortOrder(defaultSortAttribute));

        QCOMPARE(actualQuery.mAddEntryTypeNames,
            expectedQuery.mAddEntryTypeNames);
        QCOMPARE(actualQuery.mSetFlagsOn, expectedQuery.mSetFlagsOn);
        QCOMPARE(actualQuery.mSetFlagsOff, expectedQuery.mSetFlagsOff);
        QCOMPARE(actualQuery.mSetSortAttribute,
            expectedQuery.mSetSortAttribute);
        QCOMPARE(actualQuery.mSetSortOrder, expectedQuery.mSetSortOrder);


        // check non-default sort attribute
        model.reset(
            HsMenuService::getInstalledModel(Hs::CustomHsSortAttribute));

        actualQuery = model->mCaItemModelQuery;

        expectedQuery.setSort(
            HsMenuServiceUtils::sortBy(Hs::CustomHsSortAttribute),
            HsMenuServiceUtils::sortOrder(Hs::CustomHsSortAttribute));

        QCOMPARE(actualQuery.mSetSortAttribute,
            expectedQuery.mSetSortAttribute);
        QCOMPARE(actualQuery.mSetSortOrder, expectedQuery.mSetSortOrder);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_getCollectionModel()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        const Hs::HsSortAttribute
            defaultSortAttribute(Hs::LatestOnTopHsSortAttribute);
        const QString defaultCollectionType(Hs::collectionTypeName);
        const int collectionId = 7;

        QScopedPointer<HsMenuItemModel>
            model(HsMenuService::getCollectionModel(collectionId));

        CaQuery actualQuery = model->mCaItemModelQuery;

        CaQuery expectedQuery;
        expectedQuery.setParentId(collectionId);
        expectedQuery.setFlagsOn(VisibleEntryFlag);
        expectedQuery.setFlagsOff(MissingEntryFlag);
        expectedQuery.setSort(
            HsMenuServiceUtils::sortBy(defaultSortAttribute),
            HsMenuServiceUtils::sortOrder(defaultSortAttribute));


        // check with non-default parameters
        model.reset(HsMenuService::getCollectionModel(collectionId,
            Hs::NoHsSortAttribute, Hs::collectionDownloadedTypeName));

        actualQuery = model->mCaItemModelQuery;

        expectedQuery.setParentId(collectionId);
        expectedQuery.setFlagsOn(RemovableEntryFlag | VisibleEntryFlag);
        expectedQuery.setFlagsOff(MissingEntryFlag);
        expectedQuery.setSort(
            HsMenuServiceUtils::sortBy(Hs::NoHsSortAttribute),
            HsMenuServiceUtils::sortOrder(Hs::NoHsSortAttribute));
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_getCollectionNames()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        QSharedPointer<CaEntry> collectionA(new CaEntry);
        collectionA->mTextResult = "A";
        QSharedPointer<CaEntry> collectionB(new CaEntry);
        collectionB->mTextResult = "B";

        CaService::instance()->mGetEntriesResult.clear();
        CaService::instance()->mGetEntriesResult << collectionA;
        CaService::instance()->mGetEntriesResult << collectionB;

        QStringList expectedNames;
        expectedNames << collectionA->text();
        expectedNames << collectionB->text();

        CaQuery expectedQuery;
        expectedQuery.setParentId(HsMenuService::allCollectionsId());
        expectedQuery.setFlagsOn(VisibleEntryFlag);
        expectedQuery.setFlagsOff(MissingEntryFlag);

        QStringList actualNames = HsMenuService::getCollectionNames();

        QCOMPARE(actualNames, expectedNames);

        CaQuery actualQuery = CaService::instance()->mGetEntriesQuery;
        QCOMPARE(actualQuery.mSetParentId, expectedQuery.mSetParentId);
        QCOMPARE(actualQuery.mSetFlagsOn, expectedQuery.mSetFlagsOn);
        QCOMPARE(actualQuery.mSetFlagsOff, expectedQuery.mSetFlagsOff);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_getName()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        const int expectedEntryId = 5989;

        QString actualName = HsMenuService::getName(expectedEntryId);
        CaService::instance()->mGetEntryResult.clear();

        QCOMPARE(CaService::instance()->mGetEntryId, expectedEntryId);
        QCOMPARE(actualName, QString());

        CaService::instance()->mGetEntryResult =
            QSharedPointer<CaEntry>(new CaEntry);
        const QString expectedName = "ABCD";
        CaService::instance()->mGetEntryResult->mTextResult = expectedName;

        actualName = HsMenuService::getName(expectedEntryId);

        QCOMPARE(actualName, expectedName);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_executeAction()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        QSharedPointer<CaService> service = CaService::instance();

        // first case: command fails for entryId=19 and
        // "invalidName" action
        service->mExecuteCommandResult = -333;
        QString commandName = "invalidName";
        int entryId = 19;

        QVERIFY(!(HsMenuService::executeAction(entryId, commandName)==0));
        QCOMPARE(service->mExecuteEntryId, entryId);
        QCOMPARE(service->mExecuteCommand, commandName);

        // now pretend CaService accepted the command above
        // so HsMenuService accepts it too
        service->mExecuteCommandResult = 0;
        QVERIFY(HsMenuService::executeAction(entryId, commandName)==0);

    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_executeActionEntry()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        QSharedPointer<CaService> service = CaService::instance();

        // first case: command fails for entryId=19 and
        // "invalidName" action
        service->mExecuteCommandResult = -333;
        QString commandName = "invalidName";
        CaEntry entry;

        QVERIFY(!(HsMenuService::executeAction(entry, commandName)==0));
        QCOMPARE(service->mExecuteCommand, commandName);

        // now pretend CaService accepted the command above
        // so HsMenuService accepts it too
        service->mExecuteCommandResult = 0;
        QVERIFY(HsMenuService::executeAction(entry, commandName)==0);

    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
#ifdef Q_OS_SYMBIAN
void MenuServiceTest::MenuService_launchTS()
{
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
    {
        QVERIFY(HsMenuService::launchTaskSwitcher());
    }
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
}
#endif//Q_OS_SYMBIAN

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_createCollection()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
    {
        QSharedPointer<CaService> service = CaService::instance();

        // case 1: create collection fails
        service->mCreateEntryResult.clear();
        const QString createdCollectionName("collectionName");
        int actualCollectionId =
            HsMenuService::createCollection(createdCollectionName);

        QCOMPARE(actualCollectionId, 0);

        CaEntry& collectionEntry = service->mCreatedEntry;
        QCOMPARE(collectionEntry.entryTypeName(),
            QString(Hs::collectionTypeName));
        QCOMPARE(collectionEntry.mCaEntryRole, GroupEntryRole);
        QCOMPARE(collectionEntry.text(), createdCollectionName);
        CaIconDescription collectionIcon =
            collectionEntry.mSetCaIconDescription;
        QCOMPARE(collectionIcon.mSetFileName,
            QString(Hs::defaultCollectionIconId));

        // case 2: collection can be created
        const int createdCollectionId = 9;
        QSharedPointer<CaEntry> createdEntry(new CaEntry);
        createdEntry->mIdResult = createdCollectionId;
        service->mCreateEntryResult = createdEntry;

        actualCollectionId =
            HsMenuService::createCollection(createdCollectionName);

        QCOMPARE(actualCollectionId, createdCollectionId);
        QCOMPARE(service->mCreatedEntry.mAttributes[Hs::groupNameAttributeName],
            createdCollectionName);
        QCOMPARE(service->mAppendGroupId, HsMenuService::allCollectionsId());
        QCOMPARE(service->mAppenEntryId, createdCollectionId);
    }
#endif
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_renameCollection()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        QSharedPointer<CaService> service = CaService::instance();

        service->mGetEntryResult.clear();

        const QString newCollectionName("DEFG");
        const int collectionId = 8;

        // case 1: no entry to rename has been found
        QVERIFY(!HsMenuService::renameCollection(collectionId,
            newCollectionName));
        QCOMPARE(service->mGetEntryId, collectionId);

        // case 2: entry to rename has been found
        QSharedPointer<CaEntry> collectionEntry(new CaEntry);
        service->mGetEntryResult = collectionEntry;
        service->mUpdateEntryResult = true;
        QVERIFY(HsMenuService::renameCollection(collectionId,
            newCollectionName));

        QCOMPARE(service->mUpdateEntry.text(), newCollectionName);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_removeCollection()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
    QSharedPointer<CaService> service = CaService::instance();
    const int collectionId = 1;

    // case 1: service cannot remove collection
    service->mRemoveEntryForIdResult = false;
    QVERIFY(!HsMenuService::removeCollection(collectionId));
    QCOMPARE(service->mRemoveEntryId, collectionId);

    // case 2: service can remove collection
    service->mRemoveEntryForIdResult = true;
    QVERIFY(HsMenuService::removeCollection(collectionId));
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_addApplicationsToCollection()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        QSharedPointer<CaService> service = CaService::instance();
        QList<int> applicationIdList;
        applicationIdList << 1 << 2 << 3;
        const int collectionId = 11;

        service->mAppendEntriesToGroupResult = true;
        QVERIFY(HsMenuService::addApplicationsToCollection(
            applicationIdList, collectionId));

        QCOMPARE(service->mAppendEntryIdList, applicationIdList);
        QCOMPARE(service->mAppendListGroupId , collectionId);

        // pretend CaService to fail on append
        service->mAppendEntriesToGroupResult = false;
        QVERIFY(!HsMenuService::addApplicationsToCollection(
            applicationIdList, collectionId));
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_removeApplicationFromCollection()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        QSharedPointer<CaService> service = CaService::instance();

        const int collectionId = 4;
        const int applicationId = 25;

        service->mRemoveEntryFromGroupResult = true;
        QVERIFY(HsMenuService::removeApplicationFromCollection(
            applicationId, collectionId));

        QCOMPARE(service->mRemoveGroupId, collectionId);
        QCOMPARE(service->mRemoveEntryFromGroupId, applicationId);

        // pretend CaService to fail on removal operation
        service->mRemoveEntryFromGroupResult = true;
        QVERIFY(HsMenuService::removeApplicationFromCollection(
            applicationId, collectionId));
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_organizeCollection()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        QSharedPointer<CaService> service = CaService::instance();
        QList<int> entryIdList;
        entryIdList << 2 << 3 << 4;
        const int groupId = 2;

        service->mCustomSortResult = true;
        QVERIFY(HsMenuService::organizeCollection(
            groupId, entryIdList));

        QCOMPARE(service->mCustomSortGroupId, groupId);
        QCOMPARE(service->mCustomSortEntryIdList, entryIdList);

        // pretend CaService fails on customSort
        service->mCustomSortResult = false;
        QVERIFY(!HsMenuService::organizeCollection(
            groupId, entryIdList));

    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif

}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_touchAction()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        QSharedPointer<CaService> service = CaService::instance();

        ////touch on unused entry
        const int touchedEntryId = 5;
        QSharedPointer<CaEntry> touchedEntry(new CaEntry);
        touchedEntry->mIdResult = touchedEntryId;
        touchedEntry->setText("GHI");
        touchedEntry->setFlags(RemovableEntryFlag | VisibleEntryFlag);
        service->mGetEntryResult = touchedEntry;
        service->mTouchResult = true;

        HsMenuService::touch(touchedEntryId);
        QCOMPARE(service->mGetEntryId, touchedEntryId);
        QCOMPARE(service->mTouchEntry.id(), touchedEntryId);
        QCOMPARE(service->mTouchEntry.text(), touchedEntry->text());

        ////touch on already used entry
        const int unTouchedEntryId = 5;
        QSharedPointer<CaEntry> unTouchedEntry(new CaEntry);
        unTouchedEntry->mIdResult = unTouchedEntryId;
        unTouchedEntry->setText("GHI_UT");
        unTouchedEntry->setFlags(0);
        service->mGetEntryResult = unTouchedEntry;
        service->mTouchResult = true;

        HsMenuService::touch(touchedEntryId);
        QCOMPARE(service->mGetEntryId, unTouchedEntryId);
        QCOMPARE(service->mTouchEntry.id(), touchedEntryId);
        QCOMPARE(service->mTouchEntry.text(), touchedEntry->text());
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif

}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_touchAction_ByEntry()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        QSharedPointer<CaService> service = CaService::instance();

        ////touch on unused entry
        const int touchedEntryId = 5;
        QSharedPointer<CaEntry> touchedEntry(new CaEntry);
        touchedEntry->mIdResult = touchedEntryId;
        touchedEntry->setText("GHI");
        touchedEntry->setFlags(RemovableEntryFlag | VisibleEntryFlag);
        service->mGetEntryResult = touchedEntry;
        service->mTouchResult = true;

        HsMenuService::touch(*touchedEntry);
        QCOMPARE(service->mTouchEntry.id(), touchedEntryId);
        QCOMPARE(service->mTouchEntry.text(), touchedEntry->text());

        ////touch on already used entry
        const int unTouchedEntryId = 5;
        QSharedPointer<CaEntry> unTouchedEntry(new CaEntry);
        unTouchedEntry->mIdResult = unTouchedEntryId;
        unTouchedEntry->setText("GHI_UT");
        unTouchedEntry->setFlags(0);
        service->mGetEntryResult = unTouchedEntry;
        service->mTouchResult = true;

        HsMenuService::touch(*touchedEntry);
        QCOMPARE(service->mTouchEntry.id(), touchedEntryId);
        QCOMPARE(service->mTouchEntry.text(), touchedEntry->text());
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif

}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_touchListOfIdsAction()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        QSharedPointer<CaService> service = CaService::instance();

        const int touchedEntryId = 5;
        QSharedPointer<CaEntry> touchedEntry(new CaEntry);
        touchedEntry->mIdResult = 9999;
        touchedEntry->setText("GHI");
        touchedEntry->setFlags(RemovableEntryFlag | VisibleEntryFlag);
        service->mGetEntryResult = touchedEntry;
        service->mTouchResult = true;
        service->mGetEntryIdsResult.append(touchedEntryId);

        const int alreadyTouchedEntryId = 6;
        //alreadyTouchedEntryId is not return by mGetEntryIds
        //service->mGetEntryIdsResult.append(alreadyTouchedEntryId);

        QList<int> entryIdList;
        entryIdList.append(touchedEntryId);
        entryIdList.append(alreadyTouchedEntryId);

        HsMenuService::touch(entryIdList);

        QCOMPARE((bool)service->mTouchEntriesId.contains(touchedEntryId),true);
        QCOMPARE((bool)service->mTouchEntriesId.contains(alreadyTouchedEntryId),false);

        QCOMPARE(service->mGetEntryIdQuery.mSetFlagsOn,RemovableEntryFlag);
        QCOMPARE(service->mGetEntryIdQuery.mSetFlagsOff,UsedEntryFlag);
        QCOMPARE(service->mGetEntryIdQuery.mSetEntryRoles,ItemEntryRole);

        QCOMPARE(service->mGetEntryId, touchedEntryId);
        QCOMPARE(service->mTouchEntry.id(), touchedEntryId);
        QCOMPARE(service->mTouchEntry.text(), touchedEntry->text());
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_launchSoftwareUpdate()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        CaService::instance()->mExecuteCommandResult = 0;
        QCOMPARE(HsMenuService::launchSoftwareUpdate(),0);
        QCOMPARE(CaService::instance()->mExecuteCommand,
            QString(Hs::openActionIdentifier));
        QCOMPARE(CaService::instance()->mExecuteEntry.mEntryTypeNameResult,
            QString(Hs::applicationTypeName));
        QCOMPARE(CaService::instance()->mExecuteEntry.mAttributes.value(
            Hs::applicationUidEntryKey),
            QString::number(Hs::softwareUpdateApplicationUid));

        CaService::instance()->mExecuteCommandResult = -333;
        QCOMPARE(HsMenuService::launchSoftwareUpdate(),-333);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::MenuService_collectionIdByType()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif
#endif
    {
        const int ourCollectionId = 123;
        QSharedPointer<CaService> service = CaService::instance();
        CaService::instance()->mGetEntryIdsResult.clear();
        CaService::instance()->mGetEntryIdsResult << ourCollectionId;

        int result = HsMenuService::collectionIdByType("collection");

        QCOMPARE(result, ourCollectionId);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif
#endif
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::initTestCase()
{
    const int allCollectionId = 8;
    // ensure HsMenuService::allCollectionId will return
    // non zero value, if it does not already
    CaService::instance()->mGetEntryIdsResult.clear();
    CaService::instance()->mGetEntryIdsResult << allCollectionId;
}

void MenuServiceTest::cleanup()
{
    CaService::instance()->clear();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::cleanupTestCase()
{
#ifdef Q_OS_SYMBIAN
//    stopThread();
#endif //Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
int threadFunction(void *params)
{
    Q_UNUSED(params)
#ifdef Q_OS_SYMBIAN
    while (ETrue) {
        User::ResetInactivityTime();//it should help for Viewserver11 panic
        User::After(5000000);
    }
#endif //Q_OS_SYMBIAN
    return 0;
}

#ifdef Q_OS_SYMBIAN

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::startThread()
{
    User::LeaveIfError(iThread.Create(
                           _L("thread_kill_viewsrv11"),
                           threadFunction,
                           16384,
                           4000,
                           4000,
                           NULL));
    iThread.Resume();
}
#endif //Q_OS_SYMBIAN


#ifdef Q_OS_SYMBIAN


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::stopThread()
{
    iThread.Close();
}
#endif //Q_OS_SYMBIAN

QTEST_MAIN(MenuServiceTest)
