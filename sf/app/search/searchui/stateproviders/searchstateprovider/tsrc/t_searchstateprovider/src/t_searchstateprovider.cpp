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
 * Description:  Main test class for searchstateprovider library.
 *
 */

#include "t_searchstateprovider.h"
//#include "searchruntimeprovider.h"
#include "searchstateprovider.h"
#include "searchuiloader.h"
#include <qmetatype.h>
#include <qstate.h>
#include <hblistwidget.h>
#include <hbsearchpanel.h>
#include <hbinstance.h>
#include <hbmainwindow.h>
#include <hbview.h>
#include <hbgraphicsscene.h>
#include <hbapplication.h>
#include <hbnamespace.h>

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//

SearchStateProviderTest::SearchStateProviderTest()
    {

    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
SearchStateProviderTest::~SearchStateProviderTest()
    {
    delete mMainWindow;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::init()
    {

    // delete mMainWindow;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::initTestCase()
    {
    mMainWindow = mainWindow();
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::cleanupTestCase()
    {

    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testCreateAndDestructSearchStateProvider()
    {
    SearchStateProvider provider;

    /*   QVERIFY(provider.mWizardMenuStateToken.mLibrary == "searchstateprovider.dll");
     QVERIFY(provider.mWizardMenuStateToken.mUri == "search.nokia.com/state/wizardprogressivestate");

     QVERIFY(provider.mWizardActivatedStateToken.mLibrary == "searchstateprovider.dll");
     QVERIFY(provider.mWizardActivatedStateToken.mUri == "search.nokia.com/state/wizardsettingstate");

     QVERIFY(provider.mInitStateToken.mLibrary == "searchstateprovider.dll");
     QVERIFY(provider.mInitStateToken.mUri == "search.nokia.com/state/initstate");
     */}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testStates()
    {
    /*  SearchStateProvider provider;
    
     QCOMPARE(states.count(), 3);

     */}

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testCreateProgressiveState()
    {

    QVERIFY(verifyStateCreation("search.nokia.com/state/wizardprogressivestate"));
    // delete wind;
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testCreateSettingsState()
    {
    //HbMainWindow* wind = mainWindow();   
    QVERIFY(verifyStateCreation("search.nokia.com/state/wizardsettingstate"));
    // delete wind;
    }
void SearchStateProviderTest::testcreatenanddelete()
    {    
    SearchUiLoader* uiloader = SearchUiLoader::instance();    
    QVERIFY(uiloader->View());
    QVERIFY(uiloader->ListWidget());
    QVERIFY(uiloader->SearchPanel());
    uiloader->deleteLater();
    }
void SearchStateProviderTest::testslotsendtobackground()
    {
    SearchUiLoader* uiloader = SearchUiLoader::instance();
    uiloader->slotsendtobackground();
    QCOMPARE(uiloader->mListWidget->count(),0);
    QCOMPARE(uiloader->mSearchPanel->criteria(),QString());
    uiloader->deleteLater();
    }
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void SearchStateProviderTest::testCreateinitState()
    {
    //HbMainWindow* wind = mainWindow();   
    QVERIFY(verifyStateCreation("search.nokia.com/state/initstate"));
    //  delete wind;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
bool SearchStateProviderTest::verifyStateCreation(const QString& uri)
    {
    SearchStateProvider provider;
    QState* state = provider.createState(uri);
    bool result = (state != NULL);
    if (state)
        {
        delete state;
        }
    return result;
    }

// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
HbMainWindow* SearchStateProviderTest::mainWindow()
    {
    int count = HbInstance::instance()->allMainWindows().count();

    for (int i = count - 1; i > -1; i--)
        {
        delete HbInstance::instance()->allMainWindows().at(i);
        }
    return new HbMainWindow;
    }

#ifdef Q_OS_SYMBIAN

//QTEST_MAIN corrected since crashes if TRAP not in correct place.
//Will be corrected in later (estimate 4.6.0) Qt release for Symbian.
int main(int argc, char *argv[])
    {
    HbApplication app(argc, argv);
    SearchStateProviderTest tc;
       
    int ret = QTest::qExec(&tc, argc, argv);
    return ret;
    }
#else //Q_OS_SYMBIAN
QTEST_MAIN(SearchStateProviderTest)
#endif //Q_OS_SYMBIAN
