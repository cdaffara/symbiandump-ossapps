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
 * Description: Main test class declaration for UnieditorPluginLoader
 */

#ifndef TEST_UNIEDITORPLUGINLOADER_H
#define TEST_UNIEDITORPLUGINLOADER_H

#ifdef BUILD_TEST_DLL
#define TEST_EXPORT Q_DECL_EXPORT
#else
#define TEST_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <e32const.h>
#include "convergedmessage.h"

class UniEditorPluginLoader;

//Main Tets Class Declaration
class TEST_EXPORT TestUnieditorPluginLoader: public QObject
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
	 * Creates a SMS messsage
	 **/  
	void createSMS();
	
	/**
	 * Tests the SMS message
	 **/  
	void testSMS();
	
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
	 * msgPlugin - SMS/MMS Message Plug-in Loader
	 **/  	
    UniEditorPluginLoader* unieditorPluginLoader;
	
	/**
	 * mmsMsgId - The SMS/MMS message Id
	 **/  	
	long int msgId;
	
	/**
	 * messageType - type of Message - SMS or MMS
	 **/  	
	ConvergedMessage::MessageType messageType;
	};
#endif //TEST_UNIEDITORPLUGINLOADER_H
