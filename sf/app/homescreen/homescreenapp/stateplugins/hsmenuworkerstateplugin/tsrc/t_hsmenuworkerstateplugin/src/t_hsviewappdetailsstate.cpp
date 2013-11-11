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
* Description:  Main test class for hsHomeScreenStatePlugin library.
*
*/

#include <QStateMachine>
#include <QSet>
#include <HbDialog>
#include <HbLabel>
#include <HbScrollArea>
#include "hsmenuworkerstate.h"
#include "hsviewappdetailsstate.h"
#include <casoftwareregistry.h>
#include "t_hsmenuworkerstateplugin.h"


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::ViewAppDetails_construction()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState parentState1;

        HsViewAppDetailsState vas1(&parentState1);
        QString s1 = vas1.objectName();
        QVERIFY(s1 == tr("homescreen.nokia.com/state/MenuWorkerState/ViewAppDetailsState"));

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
void MenuStatesTest::ViewAppDetails_onEntry()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsMenuWorkerState parentState1;
        HsViewAppDetailsState viewAppDetailsState(&parentState1);

        QString testNewName(QString("TestNewName") +
                            QDateTime::currentDateTime().toString("ddmmyyyy_hh_mm_ss_zzz"));

        CaQuery query;
        query.setFlagsOn(RemovableEntryFlag);
        QStringList typeNames;
        typeNames.append(Hs::applicationTypeName);
        query.setEntryTypeNames(typeNames);
        QList< QSharedPointer<CaEntry> > entries = CaService::instance()->getEntries(query);

        int entryId = entries.size() < 1 ? 555 : entries[0]->id();

        QVariantMap eventAttributes;
        eventAttributes.insert(Hs::itemIdKey, entryId);
        QScopedPointer<HsMenuEvent> viewAppDetailsEvent;
        viewAppDetailsEvent.reset(new HsMenuEvent(
                HsMenuEvent::ShowAppDetails, eventAttributes));


        viewAppDetailsState.onEntry(viewAppDetailsEvent.data());

        if (entries.size() < 1) {
            QWARN("No removable application or widget present.");
            return;
        }

        checkDialogController();
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}

