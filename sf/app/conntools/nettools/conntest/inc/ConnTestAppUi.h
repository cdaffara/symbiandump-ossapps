/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: The application UI class for ConnTest
*
*/

#ifndef CONNTESTAPPUI_H
#define CONNTESTAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknViewAppUi.h>
#include <akntabgrp.h>
#include <aknnavide.h>

#include "conntest.hrh"

// FORWARD DECLARATIONS
class CConnTestContainer;
class CConnTestView;
class CAknWarningNote;
class CIpdumpEngine;
class CSettingData;
class CCustomPrefsData;

// CONSTANTS
//const ?type ?constant_var = ?constant;

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* - view architecture
* - status pane
* 
*/
class CConnTestAppUi : public CAknViewAppUi
{
public: // // Constructors and destructor
    
    /**
    * Symbian default constructor.
    */      
    void ConstructL();
    
    /**
    * Constructor.
    */      
    CConnTestAppUi(
        TFixedArray<CSettingData*, KConnTestViews>& aSettings,
        TFixedArray<CCustomPrefsData*, KConnTestViews>& aCustomPrefs);
    
    /**
    * Destructor.
    */      
    ~CConnTestAppUi();
    
public: // New functions
    
    /**
    * Is the IP logging on
    * @return the logging state, ETrue = logging, EFalse = not logging
    */
    TBool IsLogging();
    
public: // Functions from base classes
    
protected: //From CAknViewAppUi

    /**
    * Handles the status pane size changes
    */
    void HandleStatusPaneSizeChange(); 
    
private:
    // From MEikMenuObserver
    void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);
    
private:
    
    /**
    * From CEikAppUi, takes care of command handling.
    * @param aCommand command to be handled
    */
    void HandleCommandL(TInt aCommand);
    
    /**
    * From CEikAppUi, handles key events.
    * @param aKeyEvent Event to handled.
    * @param aType Type of the key event. 
    * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
    */
    virtual TKeyResponse HandleKeyEventL(
        const TKeyEvent& aKeyEvent,TEventCode aType);

private: // new methods
	/**
	* ParallelGet initiates HTTP Get over socket for every view
	* @param amount of gets if possible
	*/
    void ParallelGetL(TInt aCount);
    
    /**
    * Launches a CSendUi for the log file
    */
    void SendLogFileL();    
    
    /**
    * Starts the ip packet logging
    */
    void StartIpLoggingL();
    
    /**
    * Stops the ip packet logging
    */
    void StopIpLoggingL();
    
private: //Data
	RPointerArray<CConnTestView>	iConnTestViews; // just for parallel get
    CAknNavigationControlContainer* iNaviPane;
    CAknTabGroup*                   iTabGroup;
    CAknNavigationDecorator*        iDecoratedTabGroup;
    TFixedArray<CSettingData*, KConnTestViews>&  iSettingDataArray;
    TFixedArray<CCustomPrefsData*, KConnTestViews>&  iCustomPrefsDataArray;
    CAknWarningNote* iWarningDialog; // notice, currently used: ->ExecuteLD, so deleted automatically
    CIpdumpEngine* iIpdumpEngine; // Only 1 dump engine/app, thus appui owns it
};

#endif

// End of File
