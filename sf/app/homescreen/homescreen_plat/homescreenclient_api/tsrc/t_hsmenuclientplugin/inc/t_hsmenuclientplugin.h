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
* Description:  Tests for hsmenuclientplugin class.
*
*/

#ifndef T_HSMMENUCLIENTPLUGINTEST_H
#define T_HSMMENUCLIENTPLUGINTEST_H

#include <QtTest/QtTest>
#include <HbMainWindow>

#ifdef Q_OS_SYMBIAN
#include <e32base.h>
#endif//Q_OS_SYMBIAN

#include "caservice_p.h" //mock
/**
* @test Test class for homescreen::hsutils modules.
*/
class HsMenuClientPluginTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase();
    void init();
    
    void testCreateEntry();
    void testCreateLocalizedEntry();
    void testUpdateEntry();
    void testRemoveEntry();
    void testListEntry();

    
    /**
     * cleanup
     */
    void cleanupTestCase();
    void cleanup();
    
private:
    CaServicePrivate *mCaServicePrivateMock;

};
    
#endif //T_HSMMENUCLIENTPLUGINTEST_H
