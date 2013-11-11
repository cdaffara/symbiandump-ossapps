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

#include <QAction>
#include <HbMainWindow>
#include<hbmessagebox.h>
#include<hbaction.h>

#include "hsdeletecollectionitemstate.h"
#include "t_hsmenuworkerstateplugin.h"
#include "hscontentservice.h"
#include "hsshortcutservice.h"
#include "hsmenuevent.h"
#include "hsmenuitemmodel.h"
#include "hsdialogcontroller.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::DeleteCollectionItemState_construction()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState parentState1;
        parentState1.setObjectName(tr("testName1"));

        HsDeleteCollectionItemState del1(&parentState1);
        QString s1 = del1.objectName();
        QVERIFY(s1 == tr("testName1/DeleteCollectionItemState"));

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
void MenuStatesTest::DeleteCollectionItemState_onEntry()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState parentState3;
        parentState3.setObjectName(tr("testName3"));
        HsDeleteCollectionItemState del3(&parentState3);

        int SomeAppId = 14; ////app to delete //esheel
        int dummyCollectionID = 123456789;
        QVariantMap params;
        params.insert(Hs::itemIdKey, SomeAppId);
        params.insert(Hs::collectionIdKey, dummyCollectionID);

        QEvent *e = new HsMenuEvent(HsMenuEvent::RemoveAppFromCollection, params);
        
        QString s3 = del3.objectName();
        QCOMPARE(s3, tr("testName3/DeleteCollectionItemState"));
        
        del3.onEntry( e );
        
        QCOMPARE(del3.mItemId, SomeAppId);
        QCOMPARE(del3.mCollectionId , dummyCollectionID);

        checkDialogController();
        
        del3.cleanUp();
        
        QCOMPARE(del3.mItemId, 0);
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}
