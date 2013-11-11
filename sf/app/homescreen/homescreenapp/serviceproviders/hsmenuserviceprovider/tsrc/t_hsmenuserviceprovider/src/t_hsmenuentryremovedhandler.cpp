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

#include <QtGlobal>
#include <QSignalSpy>
 
#include "t_hsmenuserviceprovider.h"
#include "hsmenuentryremovedhandler.h"
#include "t_hsmenuentryremovedcallback.h"
#include <cadefs.h>
#include "caservice.h"
#include "canotifier.h"
#include "canotifierfilter.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::HsMenuEntryRemovedHandlerTest_data()
{
    // define testing data table
    QTest::addColumn<int>("subscribedId");
    QTest::addColumn<int>("notifiedChangeType");
    QTest::addColumn<int>("notificationsCount");
    QTest::addColumn<int>("callbacksCount");
    // prepare concrete rows for testing
    //                                             id   changeType                  
    QTest::newRow("non_matching_id") << 0 << static_cast<int>(RemoveChangeType) << 1 << 0;
    
    QTest::newRow("non_matching_change_type") << 1 << static_cast<int>(AddChangeType) << 1 << 0;
                                                 
    QTest::newRow("many_notifications") << 1 << static_cast<int>(RemoveChangeType) << 3  << 1;
                                                 
    QTest::newRow("one_notification") << 1 << static_cast<int>(RemoveChangeType) << 1 << 1;
                                                 
    QTest::newRow("no_notification") << 1 << static_cast<int>(RemoveChangeType) << 0 << 0;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuServiceTest::HsMenuEntryRemovedHandlerTest()
{
    QFETCH(int, subscribedId);
    QFETCH(int, notifiedChangeType);
    QFETCH(int, notificationsCount);
    QFETCH(int, callbacksCount);
    
    QPointer<CaNotifier> caNotifierMock(new CaNotifier());
    CaService::instance()->mCreateNotifierResult = caNotifierMock.data();
    
    T_HsMenuEntryRemovedCallback callbackObj;
    HsMenuEntryRemovedHandler handler(subscribedId, &callbackObj, SLOT(callback()));
    
    QSignalSpy spy(&handler, SIGNAL(notify()));
    
    for (int i = 0 ; i < notificationsCount && !caNotifierMock.isNull(); ++i) {
        caNotifierMock->issueEntryChanged(subscribedId, 
            static_cast<ChangeType>(notifiedChangeType));
        qApp->processEvents();
    }
    
    QCOMPARE(callbackObj.callsCount(), callbacksCount);
    QCOMPARE(spy.count(), callbackObj.callsCount());
}
