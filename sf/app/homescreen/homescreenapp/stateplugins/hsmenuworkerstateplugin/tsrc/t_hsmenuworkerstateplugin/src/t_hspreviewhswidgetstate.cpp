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

#include "t_hsmenuworkerstateplugin.h"

#include <QStateMachine>
#include <HbMainwindow>
#include <HbDialog>
#include <HbAction>
#include <HbLabel>
#include <HbWidget>
#include <HbScrollarea>
#include <HbMessageBox>

#include <hsmenueventfactory.h>
#include <hsconfiguration.h>
#include "hscontentservice.h"
#include "hsshortcutservice.h"
#include "hsmenuworkerstate.h"
#include "hspreviewhswidgetstate.h"
#include "hsapp_defs.h"
#include "hspage.h"
#include "hsscene.h"
#include "HsWidgetHost.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MenuStatesTest::HsPreviewHSWidgetState_construction()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState *parent = new QState();

        HsPreviewHSWidgetState *as1 = new HsPreviewHSWidgetState(parent);

        HsPreviewHSWidgetState *as2 = new HsPreviewHSWidgetState(parent);

        QString on1 = as1->objectName();
        QString on2 = as2->objectName();
        QVERIFY(on1 == on2);
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
void MenuStatesTest::HsPreviewHSWidgetState_showMessageWidgetCorrupted()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState *parent = new QState();
        HsPreviewHSWidgetState *as1 = new HsPreviewHSWidgetState(parent);
        as1->showMessageWidgetCorrupted();
        checkDialogController();

        delete parent;
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
void MenuStatesTest::HsPreviewHSWidgetState_onEntry()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState *parent = new QState();
        HsPreviewHSWidgetState *as1 = new HsPreviewHSWidgetState(parent);

        QVariantMap params;
        params.insert(Hs::itemIdKey, 2);
        params.insert(
            Hs::widgetUriAttributeName,
            QString("test_uri"));
        params.insert(
            Hs::widgetLibraryAttributeName,
            QString("test_library"));
        params.insert(Hs::entryTypeNameKey, Hs::widgetTypeName);

        QScopedPointer<QEvent> event (new HsMenuEvent(
                    HsMenuEvent::PreviewHSWidget, params));

        mFinishedEmited = false; //test slot

        as1->onEntry(event.data());

        QCOMPARE(params.value(Hs::itemIdKey).toInt(), as1->mEntryId);

        checkDialogController();

        delete as1;
        delete parent;
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
void MenuStatesTest::HsPreviewHSWidgetState_buildPreviewDialog()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        HsPreviewHSWidgetState previewWidgetState;

        CaEntry entry;
        entry.setText("entry text");

        QScopedPointer<HbDialog> previewDialog(
            previewWidgetState.buildPreviewDialog(entry));

        QVERIFY(previewDialog != NULL);
        QVERIFY(previewDialog->actions()[0]->parent() == previewDialog.data());
        QVERIFY(previewDialog->actions()[1]->parent() == previewDialog.data());

        QVERIFY(previewDialog->testAttribute(Qt::WA_DeleteOnClose));
        HbLabel *const headingLabel(qobject_cast<HbLabel*>(
            previewDialog->headingWidget()));
        QCOMPARE(headingLabel->plainText(), entry.text());
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
void MenuStatesTest::HsPreviewHSWidgetState_addToHomeScreen()
{
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARK;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
    {
        QState *parent = new QState();
        HsConfiguration::setInstance(new HsConfiguration);
        HsPreviewHSWidgetState *as1 = new HsPreviewHSWidgetState(parent);

        QScopedPointer<HsPage> page(new HsPage);
        HsScene::instance()->addPage(page.data());
        page.take();
        HsScene::instance()->load();

        as1->addToHomeScreen();

        QCOMPARE(HsContentService::instance()->mParams.size(), 2);
        QCOMPARE(HsContentService::instance()->mParams[Hs::uri], QVariant(QString()));
        QVERIFY(HsContentService::instance()->mParams[Hs::homescreenData].isNull());
        delete as1;
        delete parent;
    }
#ifdef Q_OS_SYMBIAN
#ifdef UT_MEMORY_CHECK
    __UHEAP_MARKEND;
#endif//UT_MEMORY_CHECK
#endif//Q_OS_SYMBIAN
}
