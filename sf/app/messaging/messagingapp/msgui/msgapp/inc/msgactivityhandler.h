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
 * Handles saving and opening activites. 
 *
 */

#ifndef MSGACTIVITYHANDLER_H_
#define MSGACTIVITYHANDLER_H_

#include <QObject>
#include <QVariant>

class MsgMainWindow;
class MsgViewManager;
class AfActivityStorage;

class MsgActivityHandler: public QObject
{
Q_OBJECT

public:
    /**
     * Constructor
     */
    MsgActivityHandler( QObject* parent = 0);
                  
    /**
     * Destructor.
     */
   ~MsgActivityHandler();

public slots:

     /**
      * Saves the current running activity.
      */
    void saveActivity();
    
public:   
   /** 
    * Set the message main window pointer.
    * @param mainWindow message main window.
    */
    void setMainWindow(MsgMainWindow* mainWindow);
   
    /**
     * Clears all saved messaging activities. 
     */
    void clearActivities();
    
    /**
     * Parses the activity data.
     * @param activitry data.
     * @return valid message Id if its editor activity 
     * else Invalid message id( i.e. -1) for list view activity 
     */
    int parseActivityData(const QVariant &activityData);
    
    /**
     * This return the Af activity storage instance 
     */
    AfActivityStorage*  activitiyStorage();
    
private:
    /**
     * main window reference not owned.
     */
    MsgMainWindow *mMainWindow;

    /**
     * App Framework activity service.
     * Own.
     */
    AfActivityStorage *mActivityStorage;
};

#endif /* MSGACTIVITYHANDLER_H_ */
