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
 * Description: - Main test class for Msgnotification -indicator plugin, 
 *                                                   -notification dialog plugin
 */

#ifndef TEST_MMS_PLUGIN_H
#define TEST_MMS_PLUGIN_H

#ifdef BUILD_TEST_DLL
#define TEST_EXPORT Q_DECL_EXPORT
#else
#define TEST_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <e32const.h>

class UniEditorMmsPlugin;
class ConvergedMessage;
class MmsTestBed;
class QSignalSpy;

class TEST_EXPORT TestMmsPlugin: public QObject
    {
    Q_OBJECT

private slots:
     
    /**
        * Called by framework,its called before the 1st test function is executed.
     */  
   
    void initTestCase();

    /**
        * Called by framework,its called before each test function is executed.
     */  

    void init();

    /**
        * Test Sending MMS
     */  

    void testSendMMS(); 

    /**
        * Test receiving MMS
     */  

    void testMmsReceived();

    /**
        * Called by framework,its called after each test function is executed.
     */  

    void cleanup();

    /**
        * Called by framework,its called after the last test function is executed.
     */  

    void cleanupTestCase();

private:

    QSignalSpy *spy_draft;
    QSignalSpy *spy_outbox;
    QSignalSpy *spy_sent;

    UniEditorMmsPlugin* msgPlugin;
    int mmsId;
    MmsTestBed* mmstestbed;
   
    };
#endif //TEST_MMS_PLUGIN_H
