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


#ifndef COMMONACTIONS_H
#define COMMONACTIONS_H

#include <qobject.h>

class EngineWrapper;
class HbAction;
class HbMenu;
class HbApplication;

class CommonActions : public QObject
{
    Q_OBJECT
public:
    
    /**
     * Constructor
     */
    CommonActions(EngineWrapper *engine, HbApplication *application);
    
    /**
     * Destructor
     */
    ~CommonActions();
    
    /**
     * Adds all actions to menu that is given as parameter
     */
    void addActionsToMenu(HbMenu *menu);
    
public slots:

    /**
     * Shows about box
     */
    void showAbout();
    
    /**
     * Stops launching applications
     */
    void stopLaunch();
    
    /**
     * Sends DLL info
     */
    void sendDllInfo();
    
    /**
     * Compares DLL infos
     */
    void compareDllInfo();
    
    /**
     * Sends log
     */
    void sendLog();
    
    /**
     * Deletes log
     */
    void deleteLog();
    
    /**
     * Exits launcher
     */
    void exit();

private:
    void connectSignalsAndSlots();
    
private:
    EngineWrapper *mEngine;
    
    HbApplication *mApplication;
    
    //Actions:

    HbAction *mActionStopLaunch;
    HbAction *mActionSendDllInfo;
    HbAction *mActionCompareDllInfo;
    HbAction *mActionSendLog;
    HbAction *mActionDeleteLog;
    HbAction *mActionAbout;
    HbAction *mActionExit;
    
};
    
#endif //COMMONACTIONS_H
