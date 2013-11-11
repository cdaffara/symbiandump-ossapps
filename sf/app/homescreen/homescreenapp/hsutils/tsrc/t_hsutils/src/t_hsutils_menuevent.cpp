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
* Description:
*
*/


#ifdef ONLY_MENU_TESTCASES

#include <QState>

#include "t_hsutils.h"
#include "hsmenuevent.h"
#include "hsmenueventfactory.h"
#include "hsmenueventtransition.h"
#include "hsapp_defs.h"


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventConstruction()
{
    QVariantMap sourceMap;
    sourceMap.insert("my_key", "my_value");
    HsMenuEvent menuEvent(HsMenuEvent::AddToHomeScreen, sourceMap);

    QVariantMap resultMap = menuEvent.data();
    QString value(resultMap.value("my_key").toString());
    HsMenuEvent::OperationType opType = menuEvent.operation();

    QCOMPARE(value, QString("my_value"));
    QCOMPARE(opType, HsMenuEvent::AddToHomeScreen);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateAddToHomeScreenEvent()
{

    {
        const int expectedId = 12345;
        const QString expectedEntryTypeName("");
        const QString expectedUri("");
        const QString expectedLibrary("");

        QSharedPointer<QEvent>
        menuEvent(HsMenuEventFactory::createAddToHomeScreenEvent(
                expectedId, Hs::NormalHsMenuMode));
        QCOMPARE(static_cast<HsMenuEvent *>(menuEvent.data())->operation(),
                 HsMenuEvent::AddToHomeScreen);

        QVariantMap resultMap = static_cast<HsMenuEvent *>(
                menuEvent.data())->data();
        const int id = resultMap.value(Hs::itemIdKey).toInt();
        const QString widgetUri(
                resultMap.value(Hs::widgetUriAttributeName).toString());
        const QString widgetLibrary(
                resultMap.value(Hs::widgetLibraryAttributeName).toString());
        const QString entryTypeName(
                resultMap.value(Hs::entryTypeNameKey).toString());

        QCOMPARE(expectedId, id);
        QCOMPARE(expectedUri, widgetUri);
        QCOMPARE(expectedLibrary, widgetLibrary);
        QCOMPARE(expectedEntryTypeName, entryTypeName);
    }

    {
        int expectedEntrId(667);

        QSharedPointer<QEvent>
        menuEvent(HsMenuEventFactory::createAddToHomeScreenEvent(
                expectedEntrId, Hs::NormalHsMenuMode));

        QVariantMap resultMap = static_cast<HsMenuEvent *>(
                menuEvent.data())->data();

        int id(-1) ;
        id = resultMap.value(Hs::itemIdKey).toInt();

        QCOMPARE(expectedEntrId, id);
    }

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreatePreviewHSWidgetEvent()
{
    {
        const int expectedId = 12345;
        const QString expectedEntryTypeName("");
        const QString expectedUri("");
        const QString expectedLibrary("");

        QSharedPointer<QEvent>
        menuEvent(HsMenuEventFactory::createPreviewHSWidgetEvent(expectedId));
        QCOMPARE(static_cast<HsMenuEvent *>(menuEvent.data())->operation(),
                 HsMenuEvent::PreviewHSWidget);

        QVariantMap resultMap = static_cast<HsMenuEvent *>(
                menuEvent.data())->data();
        const int id = resultMap.value(Hs::itemIdKey).toInt();
        const QString widgetUri(
                resultMap.value(Hs::widgetUriAttributeName).toString());
        const QString widgetLibrary(
                resultMap.value(Hs::widgetLibraryAttributeName).toString());
        const QString entryTypeName(
                resultMap.value(Hs::entryTypeNameKey).toString());

        QCOMPARE(expectedId, id);
        QCOMPARE(expectedUri, widgetUri);
        QCOMPARE(expectedLibrary, widgetLibrary);
        QCOMPARE(expectedEntryTypeName, entryTypeName);
    }

    {
        const QString expectedEntryTypeName(Hs::widgetTypeName);
        const QString expectedUri("dummyUri");
        const QString expectedLibrary("dummyLibraryName");

        QSharedPointer<QEvent>
        menuEvent(HsMenuEventFactory::createPreviewHSWidgetEvent(0,
                  expectedEntryTypeName, expectedUri, expectedLibrary));

        QVariantMap resultMap = static_cast<HsMenuEvent *>(
                menuEvent.data())->data();

        const QString widgetUri(
            resultMap.value(Hs::widgetUriAttributeName).toString());
        const QString widgetLibrary(
            resultMap.value(Hs::widgetLibraryAttributeName).toString());
        const QString entryTypeName(resultMap.value(Hs::entryTypeNameKey).toString());

        QCOMPARE(expectedUri, widgetUri);
        QCOMPARE(expectedLibrary, widgetLibrary);
        QCOMPARE(expectedEntryTypeName, entryTypeName);
    }

}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateAppSettingsViewEvent()
{
    int dummyId = 12345;
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createAppSettingsViewEvent(dummyId);
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(),
             HsMenuEvent::ShowAppSettings);

    QVariantMap resultMap = static_cast<HsMenuEvent *>(menuEvent)->data();
    int id = resultMap.value(Hs::itemIdKey).toInt();
    QCOMPARE(id, dummyId);

    delete menuEvent;
}
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateAppDetailsViewEvent()
{
    int dummyId = 12345;
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createAppDetailsViewEvent(dummyId);
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(),
             HsMenuEvent::ShowAppDetails );

    QVariantMap resultMap = static_cast<HsMenuEvent *>(menuEvent)->data();
    int id = resultMap.value(Hs::itemIdKey).toInt();
    QCOMPARE(id, dummyId);

    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateOpenCollectionEvent()
{
    int dummyId = 12345;
    QString dummyType("dummy");
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createOpenCollectionEvent(dummyId, dummyType);
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(), HsMenuEvent::OpenCollection);

    QVariantMap resultMap = static_cast<HsMenuEvent *>(menuEvent)->data();
    int id = resultMap.value(Hs::itemIdKey).toInt();
    QString type = resultMap.value(Hs::entryTypeNameKey).toString();
    QCOMPARE(id, dummyId);
    QCOMPARE(type, dummyType);

    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateOpenCollectionFromAppLibraryEvent()
{
    int dummyId = 12345;
    QString dummyType("dummy");
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createOpenCollectionFromAppLibraryEvent(dummyId, dummyType);
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(), HsMenuEvent::OpenCollectionFromAppLibrary);

    QVariantMap resultMap = static_cast<HsMenuEvent *>(menuEvent)->data();
    int id = resultMap.value(Hs::itemIdKey).toInt();
    QString type = resultMap.value(Hs::entryTypeNameKey).toString();
    QCOMPARE(id, dummyId);
    QCOMPARE(type, dummyType);

    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateRenameCollectionEvent()
{
    int dummyId = 12345;
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createRenameCollectionEvent(dummyId);
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(), HsMenuEvent::RenameCollection);

    QVariantMap resultMap = static_cast<HsMenuEvent *>(menuEvent)->data();
    int id = resultMap.value(Hs::itemIdKey).toInt();
    QCOMPARE(id, dummyId);

    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateNewCollectionEvent()
{
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createNewCollectionEvent();
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(),
             HsMenuEvent::CreateCollection);

    QVariantMap resultMap = static_cast<HsMenuEvent *>(menuEvent)->data();
    int id = resultMap.value(Hs::itemIdKey).toInt();
    QCOMPARE(id, 0);

    delete menuEvent;
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateDeleteCollectionEvent()
{
    int dummyId = 12345;
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createDeleteCollectionEvent(dummyId);
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(), HsMenuEvent::DeleteCollection);

    QVariantMap resultMap = static_cast<HsMenuEvent *>(menuEvent)->data();
    int id = resultMap.value(Hs::itemIdKey).toInt();
    QCOMPARE(id, dummyId);

    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateCollectionDeletedEvent()
{
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createCollectionDeletedEvent();
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(),
        HsMenuEvent::CollectionDeleted);
    QVERIFY(static_cast<HsMenuEvent *>(menuEvent)->data().isEmpty());
    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateBackFromInstalledViewEvent()
{
    int dummyId = 12345;
    QString dummyType("dummy");
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createBackFromInstalledViewEvent(
        dummyId, dummyType);
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(),
        HsMenuEvent::BackFromInstalledView);
    QVariantMap resultMap = static_cast<HsMenuEvent *>(menuEvent)->data();
    QCOMPARE(resultMap.value(Hs::itemIdKey).toInt(), dummyId);
    QCOMPARE(resultMap.value(Hs::entryTypeNameKey).toString(), dummyType);
    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateOpenInstalledViewEvent()
{
    int dummyId = 12345;
    QString dummyType("dummy");
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createOpenInstalledViewEvent(
        dummyId, dummyType);
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(),
        HsMenuEvent::OpenInstalledView);
    QVariantMap resultMap = static_cast<HsMenuEvent *>(menuEvent)->data();
    QCOMPARE(resultMap.value(Hs::itemIdKey).toInt(), dummyId);
    QCOMPARE(resultMap.value(Hs::entryTypeNameKey).toString(), dummyType);
    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactorycCreateOpenAppLibraryEvent()
{
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createOpenAppLibraryEvent();
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(), HsMenuEvent::OpenApplicationLibrary);
    QVERIFY(!static_cast<HsMenuEvent *>(menuEvent)->data().isEmpty());
    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateAddAppsFromApplicationsViewEvent()
{
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::
                createAddAppsFromApplicationsViewEvent(
                    Hs::NoHsSortAttribute, 0);
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(),
             HsMenuEvent::AddAppsToCollection);
    QVERIFY(!static_cast<HsMenuEvent *>(menuEvent)->data().isEmpty());
    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateAddAppsFromCallectionViewEvent()
{
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::
                createAddAppsFromCollectionViewEvent(0, 0);
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(),
             HsMenuEvent::AddAppsToCollection);
    QVERIFY(!static_cast<HsMenuEvent *>(menuEvent)->data().isEmpty());
    delete menuEvent;
}



// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateRemoveAppFromCollectionEvent()
{
    int dummyId = 12345;
    int dummyColId = 6789;
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createRemoveAppFromCollectionEvent(dummyId, dummyColId);
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(), HsMenuEvent::RemoveAppFromCollection);

    QVariantMap resultMap = static_cast<HsMenuEvent *>(menuEvent)->data();
    int id = resultMap.value(Hs::itemIdKey).toInt();
    int cid = resultMap.value(Hs::collectionIdKey).toInt();
    QCOMPARE(id, dummyId);
    QCOMPARE(cid, dummyColId);

    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateUninstallApplicationEvent()
{
    int dummyId = 54321;
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createUninstallApplicationEvent(dummyId);

    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(), HsMenuEvent::UninstallApplication );

    QVariantMap resultMap = static_cast<HsMenuEvent *>(menuEvent)->data();
    int id = resultMap.value(Hs::itemIdKey).toInt();
    QCOMPARE(id, dummyId);

    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateInstLogEvent()
{
    QEvent *menuEvent = HsMenuEventFactory::createInstallationLogEvent();
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(),
        HsMenuEvent::ShowInstallationLog);
    QVERIFY(static_cast<HsMenuEvent *>(menuEvent)->data().isEmpty());
    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateUninstallFailedEvent()
{
    int dummyError = 3;
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createUninstallFailedEvent(dummyError);

    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(), HsMenuEvent::UninstallationFailed );

    QVariantMap resultMap = static_cast<HsMenuEvent *>(menuEvent)->data();
    int error = resultMap.value(Hs::uninstallError).toInt();
    QCOMPARE(error, dummyError);

    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateUnknownEvent()
{
    QEvent *menuEvent = HsMenuEventFactory::createUnknownEvent();
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(),
        HsMenuEvent::Unknown);
    QVERIFY(static_cast<HsMenuEvent *>(menuEvent)->data().isEmpty());
    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryOpenHSEvent()
{
    QEvent *menuEvent = HsMenuEventFactory::createOpenHomeScreenEvent();
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(),
             HsMenuEvent::OpenHomeScreen);
    QVERIFY(static_cast<HsMenuEvent *>(menuEvent)->data().isEmpty());
    delete menuEvent;
}


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventTransitionConstruction()
{
    HsMenuEventTransition transition(HsMenuEvent::AddToHomeScreen, NULL, NULL);
    QCOMPARE(transition.mOperation, HsMenuEvent::AddToHomeScreen);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventTransitionTestCondition()
{
    QVariantMap sourceMap;
    sourceMap.insert("my_key", "my_value");
    HsMenuEvent correctMenuEvent(HsMenuEvent::OpenCollectionFromAppLibrary, sourceMap);
    HsMenuEvent wrongMenuEvent(HsMenuEvent::Unknown, sourceMap);

    HsMenuEventTransition transition(HsMenuEvent::OpenCollectionFromAppLibrary, NULL, NULL);
    bool correctResult = transition.eventTest(&correctMenuEvent);
    bool wrongResult = transition.eventTest(&wrongMenuEvent);

    QCOMPARE(correctResult, true);
    QCOMPARE(wrongResult, false);
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactoryCreateArrangeCollectionEvent()
{
    const int topItemId = 12345;
    const int collectionId = 11;
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createArrangeCollectionEvent(topItemId, collectionId);
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(),
             HsMenuEvent::ArrangeCollection);

    QVariantMap resultMap = static_cast<HsMenuEvent *>(menuEvent)->data();
    const int actualItemId = resultMap.value(Hs::itemIdKey).toInt();
    const int actualCollectionId = resultMap.value(Hs::collectionIdKey).toInt();
    QCOMPARE(actualItemId, topItemId);
    QCOMPARE(actualCollectionId, collectionId);

    delete menuEvent;
}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void t_hsUtils::testMenuEventFactorycreateArrangeAllCollectionsEvent()
{
    const int topItemId = 12345;
    QEvent *menuEvent(NULL);
    menuEvent = HsMenuEventFactory::createArrangeAllCollectionsEvent(topItemId);
    QCOMPARE(static_cast<HsMenuEvent *>(menuEvent)->operation(),
             HsMenuEvent::ArrangeAllCollections);

    QVariantMap resultMap = static_cast<HsMenuEvent *>(menuEvent)->data();
    const int actualItemId = resultMap.value(Hs::itemIdKey).toInt();
    QCOMPARE(actualItemId, topItemId);

    delete menuEvent;
}

#endif //ONLY_MENU_TESTCASES
