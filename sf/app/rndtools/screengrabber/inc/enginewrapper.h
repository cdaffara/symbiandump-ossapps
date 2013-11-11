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
* Description: 
*
*/

#ifndef ENGINEWRAPPER_H
#define ENGINEWRAPPER_H

#include <s32file.h>
#include <hbapplication.h>


class SGEngine;
class GrabSettings;
class MainView;
class TWsEvent;
class HbDeviceProgressDialog;

/**
 * class that is used for communicating between Symbian and Qt code.
 */
class EngineWrapper{
public:
    
    /**
     * Constructor
     */
    EngineWrapper();
    
    /**
     * Destructor
     */
    ~EngineWrapper();
    
    /**
     * Initializes Engine Wrapper
     * @return true if engine was started successfully
     */
    bool init();

public: 
    
    /* Functions that are called from UI */

    /**
     * Fetches settings from engine and converts Symbian data types to Qt's types.
     */
    GrabSettings getGrabSettings();
    
    /**
     * Converts given parameters into Symbian data types and saves them into ScreenGrabber engine
     * @param settings new settings
     */
    bool saveSettings(const GrabSettings& settings); 
    
       
    /*
     * Sends received s60 event to engine
     */
#if defined(HB_QT_S60_EVENT_FILTER)
	bool handleCaptureCommands(const TWsEvent* aEvent);
#else
	bool handleCaptureCommands(const QSymbianEvent *aEvent);
#endif
    
    
    /**
     * Sends UI to background
     */
    void sendUIToBackground();
	void EnableRcpOfFoc(TBool aState);
    void LoadSettings(void);
public:

    /* Functions that are called from engine: */

    /**
     * uses Notifications class to show image captured note 
     */
    void ShowImageCapturedNote();
    
    /**
     * uses Notifications class to show video captured note 
     */
    void ShowVideoCapturedNote();
    
    /** 
     * uses Notifications class to show seguantial images captured note
     */
    void ShowSequantialImagesCapturedNote(TInt amount);
    
    /** 
     * uses Notifications class to show error message 
     */
    void ShowErrorMessage(const TDesC16& aErrorMessage);

    /**
     * uses Notifications class to show progressbar
     */
    void ShowProgressBar(int max);
    
    /**
     * increments shown progressbar's value
     */
    void IncrementProgressbarValue();
    
    /**
     * closes progressbar
     */
    void CloseProgressbar();

    
private:
    
    bool convertTBoolToBool(TBool value);
    
private:
    
    
    /* Screen grabber engine */
    SGEngine *iSGEngine;
    
    /* progressnote that is shown */
	HbDeviceProgressDialog *iProgressNote;
    
};

#endif; 

