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
* Description:  Main test class for test library.
*
*/

#include <hbinstance.h>
#include <hbview.h>
#include <QVariant>
#include <QList> 

#include <caentry.h>
#include <caquery.h>
#include <caicondescription.h>
#include <caservice.h>

#include <hsapp_defs.h>
#include "t_hsmenuclientplugin.h"
#include "hsmenuclient.h"

const QString hsItemId("id");

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsMenuClientPluginTest::initTestCase()
{
    mCaServicePrivateMock = &CaServicePrivate::instance();
//    mainWindow();
}    

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsMenuClientPluginTest::init()
{
    mCaServicePrivateMock->clearAll();    
}    


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void HsMenuClientPluginTest::cleanupTestCase()
{
    
//    delete mainWindow();

}

void HsMenuClientPluginTest::cleanup()
{
    mCaServicePrivateMock->clearAll();
    qApp->processEvents();
}

void HsMenuClientPluginTest::testCreateEntry()
{
    #ifdef Q_OS_SYMBIAN
    #ifdef UT_MEMORY_CHECK
        __UHEAP_MARK;
    #endif//UT_MEMORY_CHECK
    #endif//Q_OS_SYMBIAN
        {    
            QList<QVariantMap> list;
            QMap<QString, QVariant> map;
            map[hsItemName] = "ModuleTestTApp";
            map[hsItemDescription] = "Unit test app for HsClientPlugin";

            map[hsitemLaunchUri] = "appto://2002DCEC?";
            map[hsitemPublisherId] = "tappModuleTest";
            map[hsItemWidgetUri] = "hsclockwidgetplugin";
            
            map[hsIconFileName] = "Z:\\private\\20022F35\\import\\widgetregistry\\20022F6C\\tclock.png";
            map[hsIconName] = "qtg_large_applications_user";
            map[hsIconApplicationId] = "268458321";
           
            
            
            HsMenuClient *publisher = new HsMenuClient();
            QVERIFY(publisher->add(map));
                        
            QSharedPointer<CaEntry> res_entry = mCaServicePrivateMock->mCreatedEntries[0];
            
            
            QCOMPARE(res_entry->entryTypeName(),
                QString(Hs::templatedApplicationTypeName));
            QCOMPARE(res_entry->role(), ItemEntryRole);
            QCOMPARE(res_entry->text(), map[hsItemName].toString());
            QCOMPARE(res_entry->description(), map[hsItemDescription].toString());
            
            QCOMPARE(res_entry->attributes().count(), 3);
            QCOMPARE(res_entry->attribute(hsitemLaunchUri), map[hsitemLaunchUri].toString());
            QCOMPARE(res_entry->attribute(hsitemPublisherId), map[hsitemPublisherId].toString());
            QCOMPARE(res_entry->attribute(hsItemWidgetUri), map[hsItemWidgetUri].toString());
            
            // icon
            QCOMPARE(res_entry->iconDescription().filename(), map[hsIconFileName].toString());
            QCOMPARE(res_entry->iconDescription().skinId(),map[hsIconName].toString());
            QCOMPARE(res_entry->iconDescription().applicationId(), map[hsIconApplicationId].toString());
            
            QCOMPARE(res_entry->flags(), VisibleEntryFlag); // Visible by default, entry not removable
            
            delete publisher; 

        }
    #ifdef Q_OS_SYMBIAN
    #ifdef UT_MEMORY_CHECK
        __UHEAP_MARKEND;
    #endif//UT_MEMORY_CHECK
    #endif//Q_OS_SYMBIAN
}

void HsMenuClientPluginTest::testCreateLocalizedEntry()
{
    #ifdef Q_OS_SYMBIAN
    #ifdef UT_MEMORY_CHECK
        __UHEAP_MARK;
    #endif//UT_MEMORY_CHECK
    #endif//Q_OS_SYMBIAN
        {    
            QList<QVariantMap> list;
            QMap<QString, QVariant> map;
            QString testName("Games");
            QString testDesc("");
            map[hsItemName] = "ModuleTestTApp";
            map[hsItemLocName] = "loc://emailTest/txt_applib_dblist_games";
            map[hsItemDescription] = "Unit test app for HsClientPlugin";
            map[hsItemLocDescription] = "loc://emailTest/txt_applib_subtitle_office";

            map[hsitemLaunchUri] = "appto://2002DCEC?";
            map[hsitemPublisherId] = "tappModuleTest";
            map[hsItemWidgetUri] = "hsclockwidgetplugin";
            
            map[hsIconFileName] = "Z:\\private\\20022F35\\import\\widgetregistry\\20022F6C\\tclock.png";
            map[hsIconName] = "qtg_large_applications_user";
            map[hsIconApplicationId] = "268458321";
           
            
            
            HsMenuClient *publisher = new HsMenuClient();
            QVERIFY(publisher->add(map));
                        
            QSharedPointer<CaEntry> res_entry = mCaServicePrivateMock->mCreatedEntries[0];
            
            
            QCOMPARE(res_entry->entryTypeName(),
                QString(Hs::templatedApplicationTypeName));
            QCOMPARE(res_entry->role(), ItemEntryRole);
            QCOMPARE(res_entry->text(), map[hsItemLocName].toString());
            QCOMPARE(res_entry->description(), map[hsItemLocDescription].toString());
            
            QCOMPARE(res_entry->attribute(hsitemLaunchUri), map[hsitemLaunchUri].toString());
            QCOMPARE(res_entry->attribute(hsitemPublisherId), map[hsitemPublisherId].toString());
            QCOMPARE(res_entry->attribute(hsItemWidgetUri), map[hsItemWidgetUri].toString());
            
            // icon
            QCOMPARE(res_entry->iconDescription().filename(), map[hsIconFileName].toString());
            QCOMPARE(res_entry->iconDescription().skinId(),map[hsIconName].toString());
            QCOMPARE(res_entry->iconDescription().applicationId(), map[hsIconApplicationId].toString());
            
            QCOMPARE(res_entry->flags(), VisibleEntryFlag); // Visible by default, entry not removable
            
            delete publisher; 

        }
    #ifdef Q_OS_SYMBIAN
    #ifdef UT_MEMORY_CHECK
        __UHEAP_MARKEND;
    #endif//UT_MEMORY_CHECK
    #endif//Q_OS_SYMBIAN
}

void HsMenuClientPluginTest::testUpdateEntry()
{
    #ifdef Q_OS_SYMBIAN
    #ifdef UT_MEMORY_CHECK
        __UHEAP_MARK;
    #endif//UT_MEMORY_CHECK
    #endif//Q_OS_SYMBIAN
        {  
            QMap<QString, QVariant> map;
            map[hsItemId] = 1; // entry id == 1, entry to update
            
            map[hsItemName] = "UpdatedName";
            map[hsItemDescription] = "UpdatedDescription";

            map[hsitemLaunchUri] = "appto://102828D6?";
            map[hsitemPublisherId] = "tappModuleTestUpdated";
            map[hsItemWidgetUri] = "hsclockwidgetpluginUpdated";
            
            map[hsIconFileName] = "Z:\\private\\20022F35\\import\\widgetregistry\\20022F6C\\UpdatedClockIcon.png";
            map[hsIconName] = "qtg_large_applications_user_updated";
            map[hsIconApplicationId] = "123458321";
            
            // create original entry to be updated
            mCaServicePrivateMock->AddEntriesForUpdate();
            
            QScopedPointer<HsMenuClient> publisher (new HsMenuClient());
            QVERIFY(publisher->add(map));
                        
            // from mock
            QSharedPointer<CaEntry> res_entry = mCaServicePrivateMock->mCreatedEntries[1];
            
            QCOMPARE(res_entry->entryTypeName(),
                QString(Hs::templatedApplicationTypeName));
            QCOMPARE(res_entry->role(), ItemEntryRole);
            QCOMPARE(res_entry->flags(), VisibleEntryFlag); // Visible by default, entry not removable

            QCOMPARE(res_entry->text(), map[hsItemName].toString());
            QCOMPARE(res_entry->description(), map[hsItemDescription].toString());
            QCOMPARE(res_entry->attributes().count(), 3);
            QCOMPARE(res_entry->attribute(hsitemLaunchUri), map[hsitemLaunchUri].toString());
            QCOMPARE(res_entry->attribute(hsitemPublisherId), map[hsitemPublisherId].toString());
            QCOMPARE(res_entry->attribute(hsItemWidgetUri), map[hsItemWidgetUri].toString());
            
            // icon
            QCOMPARE(res_entry->iconDescription().filename(), map[hsIconFileName].toString());
            QCOMPARE(res_entry->iconDescription().skinId(),map[hsIconName].toString());
            QCOMPARE(res_entry->iconDescription().applicationId(), map[hsIconApplicationId].toString());
             }
    #ifdef Q_OS_SYMBIAN
    #ifdef UT_MEMORY_CHECK
        __UHEAP_MARKEND;
    #endif//UT_MEMORY_CHECK
    #endif//Q_OS_SYMBIAN    
}

void HsMenuClientPluginTest::testRemoveEntry()
{
    #ifdef Q_OS_SYMBIAN
    #ifdef UT_MEMORY_CHECK
        __UHEAP_MARK;
    #endif//UT_MEMORY_CHECK
    #endif//Q_OS_SYMBIAN
        { 
            QMap<QString, QVariant> map;
            map[hsItemName] = "ModuleTestTApp";
            map[hsitemLaunchUri] = "appto://2002DCEC?";
            map[hsitemPublisherId] = "tappModuleTest";
            map[hsItemDescription] = "Unit test app for HsClientPlugin";
            map[hsIconApplicationId] = "268458321";

            HsMenuClient *publisher = new HsMenuClient();            
            bool result = publisher->remove(map);
            QVERIFY(!result);

            mCaServicePrivateMock->mEntryToRemove = -1;
            map[hsItemId] = 1; // entry id == 1
            result = publisher->remove(map);
            QCOMPARE(mCaServicePrivateMock->mEntryToRemove, map[hsItemId].toInt());
            delete publisher;
        }
    #ifdef Q_OS_SYMBIAN
    #ifdef UT_MEMORY_CHECK
        __UHEAP_MARKEND;
    #endif//UT_MEMORY_CHECK
    #endif//Q_OS_SYMBIAN       
}

void HsMenuClientPluginTest::testListEntry()
{
    #ifdef Q_OS_SYMBIAN
    #ifdef UT_MEMORY_CHECK
        __UHEAP_MARK;
    #endif//UT_MEMORY_CHECK
    #endif//Q_OS_SYMBIAN
        { 
            QMap<QString, QVariant> map;            
            map[hsItemName] = "Name";
            map[hsItemDescription] = "Description";

            map[hsitemLaunchUri] = "appto://102828D6?";
            map[hsitemPublisherId] = "tappModuleTest";
            map[hsItemWidgetUri] = "hsclockwidgetplugin";
            map[hsIconFileName] = "Z:\\private\\20022F35\\import\\widgetregistry\\20022F6C\\clockIcon.png";
            map[hsIconName] = "qtg_large_applications_user";
            map[hsIconApplicationId] = "223458321";
            
            HsMenuClient *publisher = new HsMenuClient();
            QList<QVariantMap> list;
            list = publisher->getList(map);
            
            // verify query passed to GetEntries
            QCOMPARE(mCaServicePrivateMock->mQueryPassedToGetList->flagsOn(), VisibleEntryFlag);
            QCOMPARE(mCaServicePrivateMock->mQueryPassedToGetList->entryRoles(), ItemEntryRole);
            QCOMPARE(mCaServicePrivateMock->mQueryPassedToGetList->entryTypeNames(), 
                QStringList(Hs::templatedApplicationTypeName));
            
            QCOMPARE(mCaServicePrivateMock->mQueryPassedToGetList->attribute(hsItemName), QString()); // not used
            QCOMPARE(mCaServicePrivateMock->mQueryPassedToGetList->attribute(hsItemDescription), QString()); // not used
            
            QCOMPARE(mCaServicePrivateMock->mQueryPassedToGetList->attribute(hsitemLaunchUri), 
                map.value(hsitemLaunchUri).toString());
            QCOMPARE(mCaServicePrivateMock->mQueryPassedToGetList->attribute(hsitemPublisherId), 
                map.value(hsitemPublisherId).toString());
            QCOMPARE(mCaServicePrivateMock->mQueryPassedToGetList->attribute(hsItemWidgetUri), 
                map.value(hsItemWidgetUri).toString());
            QCOMPARE(mCaServicePrivateMock->mQueryPassedToGetList->attribute(hsIconFileName), 
                map.value(hsIconFileName).toString());
            QCOMPARE(mCaServicePrivateMock->mQueryPassedToGetList->attribute(hsIconName), 
                map.value(hsIconName).toString());
            QCOMPARE(mCaServicePrivateMock->mQueryPassedToGetList->attribute(hsIconApplicationId), 
                map.value(hsIconApplicationId).toString());

            // Verify if all values are added to the map
            QVERIFY(list.count() == 2);            
            QSharedPointer<CaEntry> res_entry = mCaServicePrivateMock->mReturnedEntries[0];
            QCOMPARE(res_entry->id(), list.at(0).value(hsItemId).toInt());
            QCOMPARE(res_entry->text(), list.at(0).value(hsItemName).toString());
            QCOMPARE(res_entry->description(), list.at(0).value(hsItemDescription).toString());
            QCOMPARE(res_entry->attribute(hsitemLaunchUri), list.at(0).value(hsitemLaunchUri).toString());
            QCOMPARE(res_entry->attribute(hsitemPublisherId), list.at(0).value(hsitemPublisherId).toString());
            QCOMPARE(res_entry->attribute(hsItemWidgetUri), list.at(0).value(hsItemWidgetUri).toString());
            QCOMPARE(res_entry->iconDescription().filename(), list.at(0).value(hsIconFileName).toString());
            QCOMPARE(res_entry->iconDescription().skinId(),list.at(0).value(hsIconName).toString());
            QCOMPARE(res_entry->iconDescription().applicationId(), list.at(0).value(hsIconApplicationId).toString());
            
            res_entry = mCaServicePrivateMock->mReturnedEntries[1];            
            QCOMPARE(res_entry->id(), list.at(1).value(hsItemId).toInt());
            QCOMPARE(res_entry->text(), list.at(1).value(hsItemName).toString());
            QCOMPARE(res_entry->description(), list.at(1).value(hsItemDescription).toString());
            QCOMPARE(res_entry->attribute(hsitemLaunchUri), list.at(1).value(hsitemLaunchUri).toString());
            QCOMPARE(res_entry->attribute(hsitemPublisherId), list.at(1).value(hsitemPublisherId).toString());
            QCOMPARE(res_entry->attribute(hsItemWidgetUri), list.at(1).value(hsItemWidgetUri).toString());
            QCOMPARE(res_entry->iconDescription().filename(), list.at(1).value(hsIconFileName).toString());
            QCOMPARE(res_entry->iconDescription().skinId(),list.at(1).value(hsIconName).toString());
            QCOMPARE(res_entry->iconDescription().applicationId(), list.at(1).value(hsIconApplicationId).toString());
            delete publisher;
        }
    #ifdef Q_OS_SYMBIAN
    #ifdef UT_MEMORY_CHECK
        __UHEAP_MARKEND;
    #endif//UT_MEMORY_CHECK
    #endif//Q_OS_SYMBIAN      
}

QTEST_MAIN(HsMenuClientPluginTest)
