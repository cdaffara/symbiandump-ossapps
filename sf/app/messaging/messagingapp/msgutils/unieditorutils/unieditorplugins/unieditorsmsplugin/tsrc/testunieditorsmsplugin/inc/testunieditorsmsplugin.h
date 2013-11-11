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
 * Description: Main test class declaration for UnieditorSMSPlugin
 */

#ifndef TEST_UNIEDITORSMS_PLUGIN_H
#define TEST_UNIEDITORSMS_PLUGIN_H

#ifdef BUILD_TESTUNIEDITORSMSPLUGIN_DLL
#define TEST_EXPORT Q_DECL_EXPORT
#else
#define TEST_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <e32const.h>
#include <e32svr.h>
#include <SMSCLNT.H>
#include <csmsaccount.h> 
#include <mtclreg.h>
#include <mtclbase.h>
#include <txtrich.h>           	// CRichText
#include <msvids.h>            	// for Message type IDs
#include <mmsclient.h>       	// for CMmsClientMtm
#include <CMsvMimeHeaders.h>        //Attachemt mimeheader
#include <MMsvAttachmentManager.h>  //Attachment manager
#include <MMsvAttachmentManagerSync.h>

class CMsvSession;
class CClientMtmRegistry; 
class CSmsClientMtm;
class UniEditorSmsPlugin;
class ConvergedMessage;

//Main Tets Class Declaration
class TEST_EXPORT TestUnieditorSMSPlugin: public QObject, public MMsvSessionObserver
    {
    Q_OBJECT

public: //MMsvSessionObserver
    /**
	 * Handles the various events while sending SMS
	 **/  
    virtual void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
    
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
	 * Create a SMS
	 **/ 
    void CreateSMS();
    
    /**
	 * Send and Test the created SMS
	 **/ 
	void SendAndTestSMS();
	
	/**
	 * Create a SMS with Text Attachment
	 **/ 
    void CreateSMSWithTextAttachment();
    
    /**
	 * Test the SMS with Text Attachment
	 **/ 
    void TestSMSWithTextAttachment();
    
    /**
	 * Create a SMS with VCard Attachment
	 **/
    void CreateSMSWithVCardAttachment();
    
    /**
	 * Test the SMS with VCard Attachment
	 **/
    void TestSMSWithVCardAttachment();
    
    /**
	 * Create a SMS with VCal Attachment
	 **/
    void CreateSMSWithVCalAttachment();
    
    /**
	 * Test the SMS with VCal Attachment
	 **/
    void TestSMSWithVCalAttachment();
    
    /**
	 * Create an Email SMS
	 **/
    void CreateEmailSMS();
    
    /**
	 * Test the Email SMS
	 **/
	void TestEmailSMS();
	
	/**
	 * Create a SMS
	 **/
	void CreateSMSForExtraOptions();
	
	/**
	 * Test the SMS
	 **/
	void TestSMSForExtraOptions();
    
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
	 * Creates the Service Center Settings
	 **/  
    void createSCSettingsL();
    
    /**
	 * Initialise the SMS MTM
	 **/
    void initL();
    
    /**
	 * Create the SMS Service
	 **/
    TMsvId CreateSmsServiceL();
    
    /**
	 * Returns SMS Service Id
	 **/
    TMsvId ServiceIdL();
    
    /**
	 * Creates default SIMTSY Settings
	 **/
    void CreateDefaultSettingsL(TMsvId aServiceId);
    
    /**
	 * Reads the default SIMTSY Settings
	 **/
    void ReadDefaultSettingsFromSharedDataL(CSmsSettings* aServiceSettings);
    
private:
    /**
	 * iCenRepSession
	 **/  
    CRepository* iCenRepSession;
    
    /**
	 * iSession - SMS MTM Session
	 **/  
    CMsvSession* iSession;
    
    /**
	 * iMtmRegistry - SMS MTM Registry
	 **/  
	CClientMtmRegistry* iMtmRegistry;
	
	/**
	 * iSmsClientMtm - SMS MTM
	 **/  
	CSmsClientMtm* iSmsClientMtm;
    
	/**
	 * msgPlugin - SMS Unieditor Plugin
	 **/  
	UniEditorSmsPlugin* msgPlugin;
    
	/**
	 * smsId - Id of the SMS
	 **/  
	TMsvId smsId;
    };
#endif //TEST_UNIEDITORSMS_PLUGIN_H
