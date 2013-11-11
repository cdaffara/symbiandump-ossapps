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
#include <eikenv.h>
#include <apgtask.h>
#include <hbdeviceprogressdialog.h>
#include "enginewrapper.h"

#include "settingsview.h"
#include "sgengine.h"
#include "mainview.h"
#include "notifications.h"


// ---------------------------------------------------------------------------

EngineWrapper::EngineWrapper()
: iSGEngine(0), iProgressNote(0)
{
}

// ---------------------------------------------------------------------------

EngineWrapper::~EngineWrapper()
{
    if (iSGEngine) {
        iSGEngine->DeActivateModelL();
        delete iSGEngine;
    }
    if(iProgressNote){
        delete iProgressNote;
    }
}

// ---------------------------------------------------------------------------

bool EngineWrapper::init()
{
    TRAPD(error, iSGEngine = SGEngine::NewL(this));
	
    
    if (error == KErrNone) {
        TRAP(error, iSGEngine->ActivateModelL());
    }
    
    if (error != KErrNone) {
        return false;
    }
    else {
        return true;
    }
	
}

// ---------------------------------------------------------------------------

GrabSettings EngineWrapper::getGrabSettings()
{
    TGrabSettings tSettings = iSGEngine->GrabSettings();
    GrabSettings settings;
    
    // Convert TGrabSettings to GrabSettings
    settings.mCaptureMode = tSettings.iCaptureMode;
    
    // Single image settings
    settings.mSingleCaptureHotkey = tSettings.iSingleCaptureHotkey;
    settings.mSingleCaptureImageFormat = tSettings.iSingleCaptureImageFormat;
	settings.mSingleCaptureMemoryInUseMultiDrive = tSettings.iSingleCaptureMemoryInUseMultiDrive;
    settings.mSingleCaptureFileName = QString((QChar*)tSettings.iSingleCaptureFileName.Ptr(),tSettings.iSingleCaptureFileName.Length());
    
    //Seguantial image settings
    settings.mSequantialCaptureDelay = tSettings.iSequantialCaptureDelay;
    settings.mSequantialCaptureHotkey = tSettings.iSequantialCaptureHotkey;
    settings.mSequantialCaptureImageFormat = tSettings.iSequantialCaptureImageFormat;
	settings.mSequantialCaptureMemoryInUseMultiDrive = tSettings.iSequantialCaptureMemoryInUseMultiDrive;
    settings.mSequantialCaptureDelay = tSettings.iSequantialCaptureDelay;
    //convert TFilename to QString
    settings.mSequantialCaptureFileName = QString((QChar*)tSettings.iSequantialCaptureFileName.Ptr(),tSettings.iSequantialCaptureFileName.Length());
    
    
    // Video settings
    settings.mVideoCaptureHotkey = tSettings.iVideoCaptureHotkey;
    settings.mVideoCaptureVideoFormat = tSettings.iVideoCaptureVideoFormat;
	settings.mVideoCaptureMemoryInUseMultiDrive = tSettings.iVideoCaptureMemoryInUseMultiDrive;
    //convert TFilename to QString
    settings.mVideoCaptureFileName = QString((QChar*)tSettings.iVideoCaptureFileName.Ptr(),tSettings.iVideoCaptureFileName.Length());
    
    return settings;
}

// ---------------------------------------------------------------------------

bool EngineWrapper::saveSettings(const GrabSettings& settings)
{
    TGrabSettings tSettings;
    // Convert GrabSettings to TGrabSettings
    tSettings.iCaptureMode = settings.mCaptureMode;
    
    tSettings.iSingleCaptureHotkey = settings.mSingleCaptureHotkey;
    tSettings.iSingleCaptureImageFormat = settings.mSingleCaptureImageFormat;
    tSettings.iSingleCaptureMemoryInUseMultiDrive = settings.mSingleCaptureMemoryInUseMultiDrive;
    //convert TFilename to QString
    tSettings.iSingleCaptureFileName = TFileName(settings.mSingleCaptureFileName.utf16());
    tSettings.iSequantialCaptureDelay = settings.mSequantialCaptureDelay;
    tSettings.iSequantialCaptureHotkey = settings.mSequantialCaptureHotkey;
    tSettings.iSequantialCaptureImageFormat = settings.mSequantialCaptureImageFormat;
    tSettings.iSequantialCaptureMemoryInUseMultiDrive = settings.mSequantialCaptureMemoryInUseMultiDrive; 
    tSettings.iSequantialCaptureDelay = settings.mSequantialCaptureDelay;
    //convert TFilename to QString
    tSettings.iSequantialCaptureFileName = TFileName(settings.mSequantialCaptureFileName.utf16());
    
    tSettings.iVideoCaptureHotkey = settings.mVideoCaptureHotkey;
    tSettings.iVideoCaptureVideoFormat = settings.mVideoCaptureVideoFormat;
    tSettings.iVideoCaptureMemoryInUseMultiDrive = settings.mVideoCaptureMemoryInUseMultiDrive; 
    //convert TFilename to QString
    tSettings.iVideoCaptureFileName = TFileName(settings.mVideoCaptureFileName.utf16());
    TRAPD(error, iSGEngine->SaveSettingsL(tSettings));
    
    if (error == KErrNone) {
        TRAP(error, iSGEngine->ActivateCaptureKeysL(true));
    }
    
    if (error == KErrNone) {
        return true;
    }
    else {
        return false;
    }
    
    
}

// ---------------------------------------------------------------------------


void EngineWrapper::ShowImageCapturedNote()
{
    Notifications::imageCaptured();
}

// ---------------------------------------------------------------------------

void EngineWrapper::ShowErrorMessage(const TDesC16& aErrorMessage)
{
    QString errorMessage((QChar*)aErrorMessage.Ptr(),aErrorMessage.Length());
    Notifications::error(errorMessage);
 
}

// ---------------------------------------------------------------------------

void EngineWrapper::sendUIToBackground()
{
    CCoeEnv* coeEnv = CCoeEnv::Static();
    TApaTask selfTask(coeEnv->WsSession());
    selfTask.SetWgId(coeEnv->RootWin().Identifier());
    selfTask.SendToBackground();
}

// ---------------------------------------------------------------------------


#if defined(HB_QT_S60_EVENT_FILTER)
bool EngineWrapper::handleCaptureCommands(const TWsEvent* aEvent)
#else
bool EngineWrapper::handleCaptureCommands(const QSymbianEvent *aEvent)
#endif
{
    TBool retVal = ETrue;
    TRAPD(errorCode, retVal = iSGEngine->HandleCaptureCommandsL(aEvent));
    if (errorCode != KErrNone) {
        Notifications::error("Error occured when capturing screen.");
    }
    
    return convertTBoolToBool(retVal);
    
}

// ---------------------------------------------------------------------------

bool EngineWrapper::convertTBoolToBool(TBool value)
{
    if (value) {
        return true;
    }
    else {
        return false;
    }
}

// ---------------------------------------------------------------------------

void EngineWrapper::ShowSequantialImagesCapturedNote(TInt amount)
{
    Notifications::sequantialImagesCaptured(amount);
}

// ---------------------------------------------------------------------------

void EngineWrapper::ShowVideoCapturedNote()
{
    Notifications::videoCaptured();
}

// ---------------------------------------------------------------------------

void EngineWrapper::ShowProgressBar(int max)
{
    iProgressNote = Notifications::showProgressBar(max);
}

// ---------------------------------------------------------------------------

void EngineWrapper::IncrementProgressbarValue()
{
    iProgressNote->setProgressValue(iProgressNote->progressValue() + 1);
}

// ---------------------------------------------------------------------------

void EngineWrapper::CloseProgressbar()
{
    delete iProgressNote;
    iProgressNote = 0;
}

void EngineWrapper::EnableRcpOfFoc(TBool aState)
{
    iSGEngine->EnableRcpOfFoc(aState);
}

void EngineWrapper::LoadSettings(void)
{
	iSGEngine->LoadSettingsL();
}

// ---------------------------------------------------------------------------
