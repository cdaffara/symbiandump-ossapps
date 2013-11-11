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
 * Description: Main test class declaration for MMS Plugin
 */

#ifndef TEST_MMS_PLUGIN_H
#define TEST_MMS_PLUGIN_H

#ifdef BUILD_TEST_DLL
#define TEST_EXPORT Q_DECL_EXPORT
#else
#define TEST_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <QSignalSpy>
#include <e32const.h>

//Forward Declarations
class UniEditorMmsPlugin;
class ConvergedMessage;
class MmsTestBed;

//Class Declaration: TestMmsPlugin
class TEST_EXPORT TestMmsPlugin: public QObject
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
	 * Creates a MMS messsage with a text attachment
	 **/  
	void createMMSWithTextAttachment();
	
	/**
	 * Tests the MMS message with a text attachment
	 **/  
	void testMMSWithTextAttachment();
	
	/**
	 * Sends the MMS message with a text attachment
	 **/  
	void sendMMSWithTextAttachment();
	
	/**
	 * Tests that the MMS message is sent successfully
	 **/  
	void testSentMMSWithTextAttachment();
	
	/**
	 * Creates a MMS messsage with a text and an Image attachments
	 **/  
	void createMMSWithTextAndImageAttachment();
	
	/**
	 * Tests the MMS message with a text and an Image attachments
	 **/  
	void testMMSWithTextAndImageAttachment();
	
	/**
	 * Sends the MMS message with a text and an Image attachments
	 **/  
	void sendMMSWithTextAndImageAttachment();
	
	/**
	 * Tests that the MMS message is sent successfully
	 **/  
	void testSentMMSWithTextAndImageAttachment();
	
	/**
	 * Cleans up each Test Case
	 **/  	
	void cleanup();
	
	/**
	 * Cleans up the whole Test Case
	 **/  	
    void cleanupTestCase();

private: // Data
    
    /**
	 * msgPlugin - MMS Message Plug-in
	 **/  	
    UniEditorMmsPlugin* msgPlugin;
    
    /**
	 * mmstestbed - MMS Message Simulator
	 **/  	
    MmsTestBed* mmstestbed;
    
    /**
	 * spy_draft - Signal Spy for Draft Folder
	 **/  	
	QSignalSpy* spy_draft;
	
	/**
	 * spy_outbox - Signal Spy for Outbox Folder
	 **/  	
	QSignalSpy* spy_outbox;
	
	/**
	 * spy_sent - Signal Spy for Sent Folder
	 **/  	
	QSignalSpy* spy_sent;
	
	/**
	 * spy_inbox - Signal Spy for Inbox Folder
	 **/  	
	QSignalSpy* spy_inbox;
	
	/**
	 * mmsMsgId - The MMS message Id
	 **/  	
	long int mmsMsgId;
    };
#endif //TEST_MMS_PLUGIN_H
