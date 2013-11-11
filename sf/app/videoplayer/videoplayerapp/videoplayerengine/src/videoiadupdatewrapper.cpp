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
* Description:  Implementation of VideoIadUpdateWrapper
*
*/

// Version : %version: 

#include <iaupdate.h>
#include <iaupdateparameters.h>
#include <iaupdateresult.h>
#include <featmgr.h>

#include "videoiadupdatewrapper.h"
#include "mpxvideo_debug.h"

const TUid KIadParamUid = { 0x200211FE }; // Uid from videoplayer.pkg
_LIT( KIadParamExec, "videoplayer.exe" ); // Executable of videoplayer

// -------------------------------------------------------------------------------------------------
// VideoIadUpdateWrapper::VideoIadUpdateWrapper()
// -------------------------------------------------------------------------------------------------
//
VideoIadUpdateWrapper::VideoIadUpdateWrapper() : 
 mUpdate(0),
 mParameters(0)
{
    MPX_DEBUG(_L("VideoIadUpdateWrapper::VideoIadUpdateWrapper()"));
}

// -------------------------------------------------------------------------------------------------
// VideoIadUpdateWrapper::~VideoIadUpdateWrapper()
// -------------------------------------------------------------------------------------------------
//
VideoIadUpdateWrapper::~VideoIadUpdateWrapper()
{
    MPX_ENTER_EXIT(_L("VideoIadUpdateWrapper::~VideoIadUpdateWrapper()"));
    cleanup();
}

// -------------------------------------------------------------------------------------------------
// VideoIadUpdateWrapper::checkForUpdates()
// -------------------------------------------------------------------------------------------------
//
void VideoIadUpdateWrapper::checkForUpdates()
{
    MPX_ENTER_EXIT(_L("VideoIadUpdateWrapper::checkForUpdates()"));
    
    TRAP_IGNORE( doCheckForUpdatesL() );
}

// -------------------------------------------------------------------------------------------------
// VideoIadUpdateWrapper::doCheckForUpdatesL()
// -------------------------------------------------------------------------------------------------
//
void VideoIadUpdateWrapper::doCheckForUpdatesL()
{
    MPX_ENTER_EXIT(_L("VideoIadUpdateWrapper::doCheckForUpdatesL()"));
    
    FeatureManager::InitializeLibL();
    
    TBool isSupported = FeatureManager::FeatureSupported(KFeatureIdIAUpdate);
    
    FeatureManager::UnInitializeLib();
    
    if(isSupported)
    {
        if(!mUpdate)
        {
            mUpdate = CIAUpdate::NewL(*this);
        }
        if(!mParameters)
        {
            mParameters = CIAUpdateParameters::NewL();
        }
        
        mParameters->SetUid(KIadParamUid);
        // We want Videos to be started after update is finished
        mParameters->SetCommandLineExecutableL(KIadParamExec);
        mParameters->SetShowProgress(EFalse);
    
        // Check the updates
        mUpdate->CheckUpdates(*mParameters);
    }
    else
    {
        MPX_DEBUG(_L("VideoIadUpdateWrapper::doCheckForUpdatesL() Feature not supported."));
    }
}

// -------------------------------------------------------------------------------------------------
// VideoIadUpdateWrapper::CheckUpdatesComplete()
// -------------------------------------------------------------------------------------------------
//
void VideoIadUpdateWrapper::CheckUpdatesComplete(TInt errorCode, TInt availableUpdates)
{
    MPX_ENTER_EXIT( _L("VideoIadUpdateWrapper::CheckUpdatesComplete()"), 
        _L("aErrorCode: %d, availableUpdates: %d"), errorCode, availableUpdates );

    if(errorCode == KErrNone)
    {
        if(availableUpdates > 0 && mUpdate)
        {
            // There were some updates available and video list is active.
            mUpdate->UpdateQuery();
        }
        else
        {
            // No updates available or playback ongoing.
            cleanup();
        }
    }    
}

// -------------------------------------------------------------------------------------------------
// VideoIadUpdateWrapper::UpdateComplete()
// -------------------------------------------------------------------------------------------------
//
void VideoIadUpdateWrapper::UpdateComplete(TInt errorCode, CIAUpdateResult* resultDetails)
{
    MPX_ENTER_EXIT(_L("VideoIadUpdateWrapper::UpdateComplete()"), _L("aErrorCode: %d, SuccessCount: %d"), 
            errorCode, resultDetails->SuccessCount());
    
    delete resultDetails; // Ownership was transferred, so this must be deleted by the client
    
    // We do not need the client-server session anymore
    cleanup();
}

// -------------------------------------------------------------------------------------------------
// VideoIadUpdateWrapper::UpdateQueryComplete()
// -------------------------------------------------------------------------------------------------
//
void VideoIadUpdateWrapper::UpdateQueryComplete(TInt errorCode, TBool updateNow)
{
    MPX_ENTER_EXIT(_L("VideoIadUpdateWrapper::UpdateQueryComplete()"),
            _L("aErrorCode: %d, updateNow: %d"), errorCode, updateNow);
    
    if(errorCode == KErrNone)
    {
        if(updateNow && mUpdate)
        {
            // User choosed to update now, so let's launch the IAUpdate UI.
            mUpdate->ShowUpdates(*mParameters);
        }
        else
        {
            // The answer was 'Later'. 
            cleanup();
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoIadUpdateWrapper::cleanup()
// -------------------------------------------------------------------------------------------------
//
void VideoIadUpdateWrapper::cleanup()
{
    MPX_ENTER_EXIT(_L("VideoIadUpdateWrapper::cleanup()"));
    
    delete mUpdate;
    mUpdate = 0;
    
    delete mParameters;
    mParameters = 0;  
}

// End of file.
