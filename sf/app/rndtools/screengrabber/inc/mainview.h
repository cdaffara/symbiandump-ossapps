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

#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <hbview.h>
#include <hbmainwindow.h>

class EngineWrapper;
class HbMainWindow;
class HbApplication;
class SettingsView;
class HbAction;



/**
 * Class that implements the main view of Screen Grabber
 */
class MainView : public HbView
{
    Q_OBJECT
    
public:
    
    /**
     * Constructor
     */
    MainView(HbMainWindow &mainWindow, EngineWrapper &engineWrapper);
    
    /**
     * Destructor
     */
	~MainView();

	/**
     * Initializes Engine Wrapper
     * @param app application class of Screen Grabber
     */
    void init(HbApplication &app);
    
    inline void setSettingsView(SettingsView *aSettingsView){ mSettingsView = aSettingsView; }

    
private:
    
    /**
     * Creates menu
     */
    void createMenu(HbApplication &app);
    
    /**
     * Created info text ui components
     */
    void createInfoText();
    
    /**
     * Creates toolbar components
     */
    void createToolbar();

public slots:

    
    /**
     * Shows about pop-up
     */
    void showAboutPopup();
    
    /**
     * Sends UI to background
     */
    void sendToBackground();
    
    /**
     * opens settings view
     */
    void showSettings();

    void my_quit();
    
    void quitYesNoQuestionClosed(HbAction*);
    

public:
    
    /* Main windo of Screen Grabber */
    HbMainWindow &mMainWindow;
    
    /* Settings view */
    SettingsView *mSettingsView;
    
    /* Engine Wrapper */
    EngineWrapper &mEngineWrapper;
    
    /* Actions that are used in toolbars and menus */
    HbAction* actionSendToBg;
    HbAction* actionSettings;
    HbAction* actionAbout;
    HbAction* actionExit;

};

#endif // MAINVIEW_H
