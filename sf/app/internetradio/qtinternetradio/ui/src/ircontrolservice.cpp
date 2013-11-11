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

// System includes
#include <xqserviceutil.h>

// User includes
#include "ircontrolservice.h"
#include "irapplication.h"
#include "irplaycontroller.h"
#include "irqlogger.h"

// Contants
static const QString IR_CONTROL_SERVICE_NAME = "internet_radio_10_1.com.nokia.symbian.IInternetRadioControl";

// Constructor
IrControlService::IrControlService(IRApplication *aIrApp, QObject *aParent) :
    XQServiceProvider(IR_CONTROL_SERVICE_NAME, aParent),
    mIrApp(aIrApp)
{
    LOG_METHOD;
    publishAll();
}

// Destructor
IrControlService::~IrControlService()
{
    LOG_METHOD;
}

// service interface, called via Qt Highway
int IrControlService::handleCmd(int aCmdId)
{
    LOG_METHOD;
    LOG_FORMAT( "command = %d",  aCmdId);
    IrServiceResult::Type result = IrServiceResult::Fail;
        
    switch (aCmdId)
    {
        case IrServiceCmd::LaunchNormally:
            result = handleLaunchNormallyCmd();
            break;
                    
        case IrServiceCmd::LaunchNowPlayingView:
            result = handleLaunchNowPlayingViewCmd();
            break;

        case IrServiceCmd::Play:
            result = handlePlayCmd();
            break;

        case IrServiceCmd::Stop:
            result = handleStopCmd();
            break;

        case IrServiceCmd::Cancel:
            result = handleCancelCmd();
            break;

        case IrServiceCmd::Foreground:
            XQServiceUtil::toBackground(false);
            result = IrServiceResult::Success;
            break;

        default:
            break;
    }
    
    if (!requestInfo().isSynchronous())
    {
        // NOTE ! we consider the Async to be success as long as the request can be sent here
        completeRequest(setCurrentRequestAsync(), (int)result);
    }
    
    return (int)result;
}

IrServiceResult::Type IrControlService::handleLaunchNowPlayingViewCmd()
{
    LOG_METHOD;
    if (!mIrApp->isAppFullyStarted())
    {
        mIrApp->launchStartingView(EIRView_PlayingView);
    }
    return IrServiceResult::Success;
}

IrServiceResult::Type IrControlService::handleLaunchNormallyCmd()
{
    LOG_METHOD;
    if (!mIrApp->isAppFullyStarted())
    {
        mIrApp->setLaunchView();
    }
    return IrServiceResult::Success;
}

IrServiceResult::Type IrControlService::handlePlayCmd()
{      
    LOG_METHOD;     
#ifdef HS_WIDGET_ENABLED    
    LOG( "IrControlService::handlePlayCmd");
    bool ret = mIrApp->startPlaying();
    LOG_FORMAT( "IrControlService::handlePlayCmd, the ret is:%d",(int)ret);
    return ret?IrServiceResult::Success : IrServiceResult::Fail;    
#else
    return IrServiceResult::Fail;
#endif       
    
}

IrServiceResult::Type IrControlService::handleStopCmd()
{
    LOG_METHOD;
    if (mIrApp->getPlayController()->isPlaying())
    {
        mIrApp->getPlayController()->stop(EIRQUserTerminated);
    }
    return IrServiceResult::Success; 
}

IrServiceResult::Type IrControlService::handleCancelCmd()
{
    LOG_METHOD;
#ifdef HS_WIDGET_ENABLED       
    mIrApp->cancelPlayerLoading();
#endif     
    return IrServiceResult::Success;
}

