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

#include "msgactivityhandler.h"

#include "msgviewmanager.h"
#include "msgbaseview.h"
#include "msgmainwindow.h"
#include <QVariantHash>
#include <afactivitystorage.h>

const int INVALID_MSGID = -1;
// Activity Names 
const QString ListViewActivityName("MsgConversationsList");
const QString EditorActivityName("MsgCreate");

//-----------------------------------------------------------------------------
// MsgActivityHandler::MsgActivityHandler
// Constructor
//-----------------------------------------------------------------------------
MsgActivityHandler::MsgActivityHandler(QObject* parent):
    QObject(parent)
{
    mActivityStorage = new AfActivityStorage();
}

//-----------------------------------------------------------------------------
// MsgActivityHandler::MsgActivityHandler
// Destructor
//-----------------------------------------------------------------------------
MsgActivityHandler::~MsgActivityHandler()
{
   if (mActivityStorage) {
       delete mActivityStorage;
       mActivityStorage = NULL;
   }
}

//-----------------------------------------------------------------------------
// MsgActivityHandler::saveActivity
// @see header
//-----------------------------------------------------------------------------
void MsgActivityHandler::saveActivity()
{
     int currentView = mMainWindow->viewManager()->currentView();
     int msgId = INVALID_MSGID;
     if((currentView == MsgBaseView::CV) || (currentView== MsgBaseView::UNIEDITOR))
         {
         msgId = mMainWindow->viewManager()->saveContentToDraft();
         }
     
     // get a screenshot for saving to the activity manager
     QVariantHash metadata;
     metadata.insert("screenshot", 
             QPixmap::grabWidget(mMainWindow, mMainWindow->rect()));

     // save any data necessary to save the state
     QByteArray serializedActivity;
     QDataStream stream(&serializedActivity, 
             QIODevice::WriteOnly | QIODevice::Append);
  
    if( msgId != INVALID_MSGID)
        {
        stream << EditorActivityName;
        stream << msgId;

        // add the activity to the activity manager
        mActivityStorage->saveActivity(EditorActivityName, serializedActivity, metadata);
        }
    else
        {    
        stream << ListViewActivityName;

        // add the activity to the activity manager
        mActivityStorage->saveActivity(ListViewActivityName, serializedActivity, metadata);
        }
}

//-----------------------------------------------------------------------------
// MsgActivityHandler::parseActivityData
// @see header
//-----------------------------------------------------------------------------
int MsgActivityHandler::parseActivityData(const QVariant &activityData)
{
    QByteArray serializedModel = activityData.toByteArray();
    QDataStream stream(&serializedModel, QIODevice::ReadOnly);

    QString activityName;
    int msgId = INVALID_MSGID;
    stream >> activityName;
    if( activityName == EditorActivityName)
        {
        stream >> msgId;
        }
    return msgId;
}

//-----------------------------------------------------------------------------
// MsgActivityHandler::clearActivities
// @see header
//-----------------------------------------------------------------------------
void MsgActivityHandler::clearActivities()
{
    mActivityStorage->removeActivity(ListViewActivityName);
    mActivityStorage->removeActivity(EditorActivityName);
}

//-----------------------------------------------------------------------------
// MsgActivityHandler::setMainWindow
// @see header
//-----------------------------------------------------------------------------
void  MsgActivityHandler::setMainWindow(MsgMainWindow* mainWindow)
{
    mMainWindow = mainWindow;
}

//-----------------------------------------------------------------------------
// MsgActivityHandler::activitiyStorage
// @see header
//-----------------------------------------------------------------------------
AfActivityStorage*  MsgActivityHandler::activitiyStorage()
{
    return mActivityStorage;
}
