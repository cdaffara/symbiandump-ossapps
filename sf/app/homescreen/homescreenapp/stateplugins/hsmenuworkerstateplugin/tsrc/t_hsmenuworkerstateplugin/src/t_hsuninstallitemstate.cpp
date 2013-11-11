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
* Description:  Main test class for hsHomeScreenStatePlugin library.
*
*/

#include<hbmessagebox.h>
#include<hbaction.h>

#ifdef Q_OS_SYMBIAN
#include <usif/sif/sif.h>
#include <usif/scr/scr.h>
#endif//Q_OS_SYMBIAN

#include "hsuninstallitemstate.h"
#include "t_hsmenuworkerstateplugin.h"
#include "hscontentservice.h"
#include "hsshortcutservice.h"
#include "hsmenueventfactory.h"
#include "hsmenuevent.h"
#include "hsmenuitemmodel.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsUninstallItemState_construction()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState parentState1;
        parentState1.setObjectName(tr("testName1"));

        HsUninstallItemState uni1(&parentState1);
        QString s1 = uni1.objectName();
        QVERIFY(s1 == tr("testName1/UninstallItemState"));
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsUninstallItemState_onEntry()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState parentState3;
        parentState3.setObjectName(tr("testName3"));
        HsUninstallItemState uni3(&parentState3);

        int SomeAppId = 14; ////app to uninst

        QVariantMap params;
        params.insert(Hs::itemIdKey, SomeAppId);

        QEvent *e = new HsMenuEvent(HsMenuEvent::UninstallApplication, params);

        QString s3 = uni3.objectName();
        QCOMPARE(s3, tr("testName3/UninstallItemState"));

        uni3.onEntry( e );
        checkDialogController();

        QCOMPARE(uni3.mEntry->id(), SomeAppId);

        uni3.cleanUp();

        QVERIFY(uni3.mEntry.isNull());

    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

#ifdef Q_OS_SYMBIAN

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsUninstallItemState_getApplicationsNames()
{
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
    {

        static const char javaUid[] = "270437153";   //in DS_Snow

        Usif::RSoftwareInstall sif;
        CleanupClosePushL( sif );
        User::LeaveIfError( sif.Connect() );

        // Install the component
        _LIT16( KDsSnowJadPath, "c:\\testing\\data\\t_hsmenuworkerstateplugin\\installs\\DS_Snow.jad" );
        Usif::COpaqueNamedParams* arguments = NULL;
        Usif::COpaqueNamedParams* results = NULL;
        arguments = Usif::COpaqueNamedParams::NewLC();
        results = Usif::COpaqueNamedParams::NewLC();
        arguments->AddIntL( Usif::KSifInParam_InstallSilently, 1 );
        TRequestStatus status;
        sif.Install( KDsSnowJadPath, *arguments, *results, status, EFalse );
        User::WaitForRequest( status );
        User::LeaveIfError( status.Int() );

        CleanupStack::PopAndDestroy( results );
        CleanupStack::PopAndDestroy( arguments );

        // Disconnect from the SIF server
        CleanupStack::PopAndDestroy( &sif );


        QSharedPointer<CaService> service = CaService::instance();
        CaQuery query;
        query.setAttribute(Hs::applicationUidEntryKey, javaUid);
        QList< QSharedPointer<CaEntry> > entries = service->getEntries(query);
        QCOMPARE(entries.length(), 1);
        QSharedPointer<CaEntry> entry = entries[0];

        HsUninstallItemState state;

        QVariantMap params;
        params.insert(Hs::itemIdKey, entry->id());

        QEvent *e = new HsMenuEvent(HsMenuEvent::UninstallApplication, params);
        state.onEntry( e );

        QCOMPARE(state.mEntry->id(), entry->id());


        QString componentName;
        QStringList appNames;
        QString confirmationMessage;

        bool result = state.getApplicationsNames(componentName,
            appNames, confirmationMessage);
        QVERIFY(result);
        QCOMPARE(componentName, QString("DS_Snow"));
        QCOMPARE(appNames.length(), 0); //in DS_Snow

        const QString remove("remove");
        service->executeCommand(entry->id(), remove);

        state.cleanUp();
        QVERIFY(state.mEntry.isNull());
    }
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
}
#endif // Q_OS_SYMBIAN
