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
* Description:
*
*/

#include <QStateMachine>

#include "hsshortcutservice.h"
#include "hsshortcutservice_p.h"
#include "t_hsdomainmodel.h"


// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//
void TestHsDomainModel::testShortcutService()
{
    QStateMachine *machine = new QStateMachine;
    
    HsShortcutService *scService = HsShortcutService::instance(machine);

    QCOMPARE(scService->isItemShortcutWidget(0), false);

    QVERIFY(HsShortcutService::instance());

    scService->executeCollectionAction(0, QString(""));//how to test this?  
    delete machine;
}
