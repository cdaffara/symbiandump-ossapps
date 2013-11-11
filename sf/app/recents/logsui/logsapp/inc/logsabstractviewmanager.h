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
*
*/
#ifndef LOGABSTRACTVIEWMANAGER_H
#define LOGABSTRACTVIEWMANAGER_H


#include "logsdefs.h"
class HbMainWindow;

/**
 * For view changing.
 */
class LogsAbstractViewManager
{
public:
    
    /**
     * Activate view. Previously active view is deactivated.
     * @param viewId, id for activated view
     * @return true if activated succesfully, otherwise false
     */
    virtual bool activateView(LogsAppViewId viewId) = 0;
    
    /**
     * Activate view by giving activation arguments. Previously active
     * view is deactivated.
     * @param viewId, id for activated view
     * @param showDialer, true if view should show dialer when activated
     * @param args, custom arguments for view
     * @return true if activated succesfully, otherwise false
     */
    virtual bool activateView(LogsAppViewId viewId, bool showDialer, QVariant args) = 0;
    
    /**
     * Activate previous view.
     * @return true if activated succesfully, otherwise false
     */
    virtual bool activatePreviousView() = 0;
    
    /**
     * Returns application main window
     */
    virtual HbMainWindow& mainWindow() = 0;
    
    /**
     * Exit the application.
     */
    virtual void exitApplication() = 0;
    
};

#endif //LOGABSTRACTVIEWMANAGER_H
