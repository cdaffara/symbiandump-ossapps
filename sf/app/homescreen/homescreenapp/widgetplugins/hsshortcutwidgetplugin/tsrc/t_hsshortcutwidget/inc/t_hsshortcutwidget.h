/*
* Copyright (c) 2008, 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtTest/QtTest>
#include "hsshortcutservice.h"

class QStateMachine;

class TestShortcutWidget : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testConstruction();
    void testProperties();
    void testSignals();
    void testShowHide();
    void testCaNotifier();
    void testUserInteraction();

private:
    HsShortcutService *mShortcutService;
    QStateMachine *mMachine;
};
