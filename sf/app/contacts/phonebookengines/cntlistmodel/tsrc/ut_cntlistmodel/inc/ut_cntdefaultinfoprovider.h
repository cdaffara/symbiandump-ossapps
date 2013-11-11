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

#include <QtTest/QtTest>
#include <QObject>
#include <qtcontacts.h>

class CntDefaultInfoProvider;

QTM_USE_NAMESPACE

class TestCntDefaultInfoProvider : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void create();
    
    void testSupportedFields();
    void testRequestInfo();
    
    void cleanupTestCase();
    
private:
    CntDefaultInfoProvider    *mCntDefaultInfoProvider;

};
