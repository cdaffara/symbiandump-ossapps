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
 */

#ifndef TEST_UNI_DATA_MODEL_SMS_PLUGIN_H
#define TEST_UNI_DATA_MODEL_SMS_PLUGIN_H

#ifdef BUILD_TEST_DLL
#define TEST_EXPORT Q_DECL_EXPORT
#else
#define TEST_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <txtetext.h>
#include <txtrich.h>
#include <mtclreg.h>
#include <mtclbase.h>
#include <msvids.h>
#include <smsclnt.h>
#include <smuthdr.h>
#include <QDateTime>

class UniDataModelPluginInterface;
class UniDataModelLoader;
class TestObserver;

class ConvergedMessage;

class TEST_EXPORT TestUniDataModelSMSPlugin : public QObject
{
Q_OBJECT

private slots:
    //called by frame work.
    //  void initTestCase();//called before the first testfunction is executed.
    //  void cleanupTestCase();//called after the last testfunction was executed.
    void init();//called before each testfunction is executed.
    void cleanup();//called after every testfunction.

    //test cases.
    
    void testInboxMessage();
    void testOutboxMessage();
    void testDraftsMessage();
    void testDraftsMessageNoRecipient();
    void testUnusedAPIs();

private:
    void createInboxSMS();
    void createOutboxSMS();
    void createDraftsSMS();
    void createDraftsSMSNoRecipient();
    void testGetPluginWithoutLoading();
    

private:
    UniDataModelPluginInterface* pluginInterface;
    TestObserver* iObserver;
    CMsvSession* iMSession;
    CClientMtmRegistry* iMtmReg;
    CSmsClientMtm* iSmsClientMtm;
    TMsvId messageId;
    UniDataModelLoader* pluginLoader;
    QDateTime retTimeStamp ;
};

class TestObserver : public MMsvSessionObserver
{
public:
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
                             TAny* aArg3);
};

#endif //TEST_UNI_DATA_MODEL_SMS_PLUGIN_H
