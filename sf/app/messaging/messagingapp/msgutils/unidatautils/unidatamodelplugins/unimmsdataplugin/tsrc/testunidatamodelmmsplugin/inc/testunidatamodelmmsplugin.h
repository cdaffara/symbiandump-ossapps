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
 * Description: Main test class declaration for MMS Data Plugin
 */

#ifndef TESTUNIDATAMODELMMSPLUGIN_H_
#define TESTUNIDATAMODELMMSPLUGIN_H_

#ifdef BUILD_TESTUNIDATAMODELMMSPLUGIN_DLL
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
#include <QDateTime>

//Forward Declarations
class UniDataModelPluginInterface;
class UniDataModelLoader;
class TestObserverMMS;
class CMmsClientMtm;
class ConvergedMessage;
class RFs;

//Class Declaration
class TEST_EXPORT TestUniDataModelMMSPlugin : public QObject
{
Q_OBJECT

private slots:
	
	/**
	 * Initialises the whole Test
	 **/  
    void initTestCase();//called before the first testfunction is executed.
    
    /**
	 * Initialises each Test Case
	 **/  
    void init();//called before each testfunction is executed.

    /**
	 * Tests that the MMS message is created successfully in the Inbox Folder
	 **/ 
    void testInboxMessage();
    
    /**
	 * Cleans up each Test Case
	 **/  
   	void cleanup();//called after every testfunction. 
   	
   	/**
	 * Cleans up the whole Test Case
	 **/  
   	void cleanupTestCase();//called after the last testfunction was executed.
    
private:
   	
    /**
   	 * Creates the MMS message in the Inbox Folder
   	 **/ 
    void createMMS(TMsvId pId, TBool subjectField);
    
private: //Data
    
    /**
	 * pluginInterface - UniDataMMSPlug-in
	 **/  
    UniDataModelPluginInterface* pluginInterface;
    
    /**
	 * pluginLoader - UniDataPlug-in Loader
	 **/
    UniDataModelLoader* pluginLoader;
    
    /**
	 * iObserver - Observer pointer For Test Class
	 **/
    TestObserverMMS* iObserver;
    
    /**
	 * iMSession - Session
	 **/
    CMsvSession* iMSession;
    
    /**
	 * iMmsClientMtm - MMS Client MTM
	 **/
    CMmsClientMtm* iMmsClientMtm;
    
    /**
	 * iMessageId - MMS Message ID of the MMS created in the Inbox Folder
	 **/
    TMsvId iMessageId;
    
    /**
	 * retTimeStamp - Timestamp for the MMS being Created
	 **/
    QDateTime retTimeStamp;
    
    /**
	 * iFs - File Server
	 **/
    RFs iFs;
    
    /**
	 * iServiceId - MMS Service Id
	 **/
    TMsvId iServiceId;
    
    /**
	 * msgSize - Size of the MMS message created in the Inbox Folder
	 **/
    TInt msgSize;
};

//Observer Class Declaration
class TestObserverMMS : public MMsvSessionObserver
{
public:
	
	/**
	 * Handles various outcome Events after the different services like Creation, Sending, etc. of a MMS Message
	 **/  
    void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
                             TAny* aArg3);
};

#endif /* TESTUNIDATAMODELMMSPLUGIN_H_ */
