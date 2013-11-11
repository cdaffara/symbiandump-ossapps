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

#include <QString>
#include <QScopedPointer>

#include <HbGlobal>
#include <HbAction>
#include <HbMessageBox>
#include "hsmenudialogfactory.h"
#include "t_hsutils.h"


#ifdef ONLY_MENU_TESTCASES

void t_hsUtils::testMenuDialogFactoryCreate()
{
    HsMenuDialogFactory hsMenuDialogFactory;
    const QString message("testMessage");
    QScopedPointer<HbMessageBox> box(
            hsMenuDialogFactory.create(message, HsMenuDialogFactory::OkCancel));

    QVERIFY(box);
    QCOMPARE(box->text(), message);
    QCOMPARE(box->actions().count(), 2);
    QCOMPARE(box->actions().at(HsMenuDialogFactory::acceptActionIndex())->text(),
             hbTrId("txt_common_button_ok"));
    QCOMPARE(box->actions().at(HsMenuDialogFactory::rejectActionIndex())->text(),
             hbTrId("txt_common_button_cancel"));

    box.reset(
            hsMenuDialogFactory.create(message, HsMenuDialogFactory::Close));

    QVERIFY(box);
    QCOMPARE(box->text(), message);
    QCOMPARE(box->actions().count(), 1);
    QCOMPARE(box->actions().at(HsMenuDialogFactory::acceptActionIndex())->text(),
             hbTrId("txt_common_button_close"));
}
#endif
