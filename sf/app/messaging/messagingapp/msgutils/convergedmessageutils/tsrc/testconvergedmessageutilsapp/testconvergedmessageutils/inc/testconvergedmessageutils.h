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
 * Description: Main test class declaration for ConvergedMessageUtils
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

//Main Tets Class Declaration
class TEST_EXPORT TestConvergedMessageUtils: public QObject
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
	 * Creates a MMS messsage
	 **/  
	void createMMS();
	
	/**
	 * Tests the MMS message
	 **/  
	void testMMS();
	
	/**
	 * Test Methods of ConvergedMessageId which were not part of the tests above
	 **/ 
	void testConvergedMessageIdUnusedMethods();
	

	/**
	 * Test Methods of ConvergedMessageAttachment which were not part of the tests above
	 **/
	void testConvergedMessageAttachmentUnusedMethods();
	
	/**
	 * Cleans up each Test Case
	 **/ 
	void cleanup();//called after every testfunction.
	
	/**
	 * Cleans up the whole Test Case
	 **/  
    void cleanupTestCase();//called after the last testfunction was executed.

private: //Data
    
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
	 * mmsMsgId - The MMS message Id
	 **/  	
	long int mmsMsgId;
	};
#endif //TEST_MMS_PLUGIN_H
