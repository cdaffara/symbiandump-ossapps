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
 * Description:  test searchonlinehandler.
 *
 */
#include "t_onlinehandler.h"
#include "onlinehandler.h"
#include <qsignalspy.h>
#include "serviceprovider.h"
#include "isengine.h"
#include "isproviderdbmanager.h"

SearchOnlineHandlerTest::SearchOnlineHandlerTest()
    {
    }
SearchOnlineHandlerTest::~SearchOnlineHandlerTest()
    {
    }
void SearchOnlineHandlerTest::init()
    {
    }
void SearchOnlineHandlerTest::initTestCase()
    {
    }
void SearchOnlineHandlerTest::cleanupTestCase()
    {
    }
void SearchOnlineHandlerTest::testsetDefaultProvider()
    {
    OnlineHandler* onlinetestobj = new OnlineHandler();
    onlinetestobj->readSettings();
    if (onlinetestobj->mAvailableProviders.count())
        {
        onlinetestobj->mAvailableProviders.at(0);
        int i = onlinetestobj->mAvailableProviders.at(0).Id();
        onlinetestobj->setDefaultProvider(i);
        onlinetestobj->mEng->SelectedProvider();
        QCOMPARE(i,onlinetestobj->mEng->SelectedProvider()->Id());
        }
    delete onlinetestobj;
    }
void SearchOnlineHandlerTest::testinitializePlugin()
    {
    OnlineHandler* onlinetestobj = new OnlineHandler();
    onlinetestobj->readSettings();
  //  QSignalSpy spy(onlinetestobj, SIGNAL(pluginIntialized(bool)));
    onlinetestobj->initializePlugin();
   // QCOMPARE(spy.count(), 1);
    delete onlinetestobj;
    }
void SearchOnlineHandlerTest::testactivatePlugin()
    {
    OnlineHandler* onlinetestobj = new OnlineHandler();
    onlinetestobj->readSettings();
    onlinetestobj->initializePlugin();
    onlinetestobj->activatePlugin();
   /* delete onlinetestobj->mOnlinePlugin;
    onlinetestobj->mOnlinePlugin = NULL;
    onlinetestobj->activatePlugin();*/
    delete onlinetestobj;
    }
void SearchOnlineHandlerTest::testactivatePlugin1()
    {
    OnlineHandler* onlinetestobj = new OnlineHandler();
    onlinetestobj->readSettings();
    onlinetestobj->initializePlugin();
    onlinetestobj->activatePlugin("ab");
    /*delete onlinetestobj->mOnlinePlugin;
    onlinetestobj->mOnlinePlugin = NULL;
    onlinetestobj->activatePlugin("ab");
*/    delete onlinetestobj;
    }
void SearchOnlineHandlerTest::testwriteSettings()
    {
    OnlineHandler* onlinetestobj = new OnlineHandler();
    onlinetestobj->readSettings();
    if (onlinetestobj->mAvailableProviders.count())
        {
        onlinetestobj->writeSettings(
                onlinetestobj->mAvailableProviders.at(0).Id());
        }
    delete onlinetestobj;
    }
void SearchOnlineHandlerTest::testloadISPlugin()
    {
    OnlineHandler* onlinetestobj = new OnlineHandler();
    onlinetestobj->readSettings();
    if (onlinetestobj->mAvailableProviders.count())
        {
        QSignalSpy spy(onlinetestobj, SIGNAL(pluginIntialized(bool)));
        onlinetestobj->loadISPlugin(
                onlinetestobj->mAvailableProviders.at(0).Id(), "ab");
        QCOMPARE(spy.count(), 0);
        }
    delete onlinetestobj;
    }
void SearchOnlineHandlerTest::testunloadPlugin()
    {
    OnlineHandler* onlinetestobj = new OnlineHandler();
    onlinetestobj->readSettings();
    onlinetestobj->initializePlugin();
    onlinetestobj->unloadPlugin();
    delete onlinetestobj;
    }
void SearchOnlineHandlerTest::testunloadPluginandExit()
    {
    OnlineHandler* onlinetestobj = new OnlineHandler();
    onlinetestobj->readSettings();
    onlinetestobj->initializePlugin();
 //   QSignalSpy spy(onlinetestobj, SIGNAL(pluginShutdown(bool)));
    onlinetestobj->unloadPluginandExit();
    //QCOMPARE(spy.count(), 1);
    delete onlinetestobj;
    }

void SearchOnlineHandlerTest::testbackEvent()
    {
    OnlineHandler* onlinetestobj = new OnlineHandler();
    QSignalSpy spy(onlinetestobj, SIGNAL(backEventTriggered()));
    onlinetestobj->backEvent();
    QCOMPARE(spy.count(), 1);
    delete onlinetestobj;
    }

void SearchOnlineHandlerTest::testPerformWebSearch()
    {
   /* OnlineHandler* onlinetestobj = new OnlineHandler();
    onlinetestobj->readSettings();
    if (onlinetestobj->mEng)
        {
        onlinetestobj->mEng->PerformWebSearch("string");
        }
    delete onlinetestobj;*/
    }
void SearchOnlineHandlerTest::testPerformInDeviceSearch()
    {
    /*OnlineHandler* onlinetestobj = new OnlineHandler();
    onlinetestobj->readSettings();
    if (onlinetestobj->mEng)
        {
        onlinetestobj->mEng->PerformInDeviceSearch("string");
        }
    delete onlinetestobj;*/
    }

#ifdef Q_OS_SYMBIAN

//QTEST_MAIN corrected since crashes if TRAP not in correct place.
//Will be corrected in later (estimate 4.6.0) Qt release for Symbian.
int main(int argc, char *argv[])
    {
    QCoreApplication app(argc, argv);
#ifdef __WINSCW__
    char *new_argv[3];
    QString str = "C:\\data\\" + QFileInfo(
            QCoreApplication::applicationFilePath()).baseName() + ".log";
    QByteArray bytes = str.toAscii();
    char arg1[] = "-o";
    new_argv[0] = argv[0];
    new_argv[1] = arg1;
    new_argv[2] = bytes.data();
    SearchOnlineHandlerTest tc;
    return QTest::qExec(&tc, 3, new_argv);
#else
    SearchOnlineHandlerTest tc;
    return QTest::qExec(&tc, argc, argv);
#endif

    }
#else //Q_OS_SYMBIAN
QTEST_MAIN(SearchOnlineHandlerTest)
#endif //Q_OS_SYMBIAN    
