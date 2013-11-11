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
 * Main window of the messaging application. Activates the default view.
 *
 */

#include "msgmainwindow.h"

#include "msgviewmanager.h"
#include "msgserviceinterface.h"
#include "msgsendserviceinterface.h"
#include "conversationsengine.h"
#include "msgactivityhandler.h"

#include <hbapplication.h>
#include <QKeyEvent>
#include <afactivation.h>
#include <afactivitystorage.h>

const int INVALID_MSGID = -1;

//---------------------------------------------------------------
// MsgMainWindow::MsgMainWindow
// Constructor
//---------------------------------------------------------------
MsgMainWindow::MsgMainWindow(bool serviceRequest, QWidget *parent) :
HbMainWindow(parent), mMsgSI(0), mMsgSendSI(0)
{
    initialize(serviceRequest);    
}

//---------------------------------------------------------------
// MsgMainWindow::MsgMainWindow
// Constructor
//---------------------------------------------------------------
void MsgMainWindow::initialize(bool serviceRequest)
{
    int activityMsgId = INVALID_MSGID;
    
    MsgActivityHandler* activityHandler = 
            new MsgActivityHandler(this);
        
    AfActivation *activation = new AfActivation( this );
    
    Af::ActivationReason reason = activation->reason();      
    
    if (reason == Af::ActivationReasonActivity) 
        {
        // set service request to false , since its a activity launch
        serviceRequest = false;
        
        QString actName = activation->name();
                
        QVariant data = activityHandler->activitiyStorage()->activityData(actName);        
        
        activityMsgId = activityHandler->parseActivityData(data);
        }
    
    mViewManager = new MsgViewManager(serviceRequest,this,this,activityMsgId);
    mMsgSI = new MsgServiceInterface(NULL,mViewManager);
    mMsgSendSI = new MsgSendServiceInterface(NULL,mViewManager);
            
    //Model creation
    ConversationsEngine::instance();
    
    // clear the old activities
    activityHandler->clearActivities();
    
    // Set the main window pointer to activity handler.
    activityHandler->setMainWindow(this); 
    
    // connect to aboutToQuit signal to save drafts content
    QObject::connect(qApp, SIGNAL(aboutToQuit()), 
            activityHandler, SLOT(saveActivity()));
}

//---------------------------------------------------------------
// MsgMainWindow::~MsgMainWindow
// Constructor
//---------------------------------------------------------------
MsgMainWindow::~MsgMainWindow()
{   
    if (mMsgSI)
    {
        delete mMsgSI;
        mMsgSI = NULL;
    }
    if (mMsgSendSI)
    {
        delete mMsgSendSI;
        mMsgSendSI = NULL;
    }
    
    delete ConversationsEngine::instance();
}

//---------------------------------------------------------------
// MsgMainWindow::viewManager
// Constructor
//---------------------------------------------------------------
MsgViewManager* MsgMainWindow::viewManager()
{   
     return mViewManager; 
}

//---------------------------------------------------------------
// MsgMainWindow::keyPressEvent
// @see header
//---------------------------------------------------------------
void MsgMainWindow::keyPressEvent(QKeyEvent *event)
{
    bool eventHandled = false;
    if (Qt::Key_Yes == event->key()) {
        eventHandled = mViewManager->handleKeyEvent(event->key());
    }

    if (eventHandled) {
        event->accept();
    }
    else {
        HbMainWindow::keyPressEvent(event);
    }

}

// End of file
