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

#ifndef T_HSMCSSTATES_H
#define T_HSMCSSTATES_H
#include <QtTest/QtTest>
#ifdef Q_OS_SYMBIAN
#include <e32std.h>
#include <e32const.h>
#endif


class HsMenuItemModel;
/**
* @test Test class for menuservice modules.
*/
class MenuServiceTest : public QObject
{
    Q_OBJECT

private slots:


    /**
     * initialize
     */
    void initTestCase();
    /*
    * Tests
    */
    void MenuService_getAllApplicationsModel();
    void MenuService_getAllCollectionsModel();
    void MenuService_getInstalledModel();
    void MenuService_getCollectionModel();
    void MenuService_getCollectionNames();
    void MenuService_getName();
    void MenuService_executeAction();
    void MenuService_executeActionEntry();
#ifdef Q_OS_SYMBIAN
    void MenuService_launchTS();
#endif//Q_OS_SYMBIAN
    void MenuService_createCollection();
    void MenuService_renameCollection();
    void MenuService_removeCollection();
    void MenuService_addApplicationsToCollection();
    void MenuService_removeApplicationFromCollection();
    void MenuService_organizeCollection();
    void MenuService_touchAction();
    void MenuService_touchAction_ByEntry();
    void MenuService_touchListOfIdsAction();
    void MenuService_launchSoftwareUpdate();
    void MenuService_collectionIdByType();


    void HsMenuItemModel_construction();
    void HsMenuItemModel_setSort();
    void HsMenuItemModel_getData();
    void HsMenuItemModel_newIconNeeded();
    void HsMenuItemModel_uninstallChange();
    void HsMenuItemModel_preloadIcons();

    void HsIconsIdleLoader_construction();

    void HsMenuCollectionsItemModel_construction();
    void HsMenuCollectionsItemModel_getData();
    void HsMenuCollectionsItemModel_getSecondLine();

    void HsMenuEntryRemovedHandlerTest_data();
    void HsMenuEntryRemovedHandlerTest();


    /**
     * cleanup
     */
    void cleanup();
    void cleanupTestCase();

private:

    int getId(HsMenuItemModel *model , int index);

private:
#ifdef Q_OS_SYMBIAN
    void startThread();
    void stopThread();
    RThread iThread;
#endif //Q_OS_SYMBIAN
};

#endif
