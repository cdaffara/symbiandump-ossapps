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
* Description:  Engine class of the SimpleVideoPlayback MPX view plugin.
*
*/

#include <mpxplaybackutility.h> // MMPXPlaybackUtility
#include <mpxmessagegeneraldefs.h>
#include <mpxplaybackmessage.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxvideoplaybackdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediageneralextdefs.h>
#include <mpxmediavideodefs.h>

#include "svpbengine.h"
#include "trace.h"

const TUid KVideoHelixPlaybackPluginUid = { 0x10282551 };

SvpbEngine::SvpbEngine(QObject *parent) :
    QObject(parent),
    mSurfaceContainer(0),
    mState(Disconnected),
    mPlaybackUtility(0),
    mMpxPbState(EPbStateNotInitialised)
{
    FUNC_LOG;
}

SvpbEngine::~SvpbEngine()
{
    FUNC_LOG;

    disconnectMPX();
}

void SvpbEngine::connectMPX()
{
    FUNC_LOG;

    if (mState == Disconnected && !mPlaybackUtility) {
        QT_TRAP_THROWING(mPlaybackUtility =
            MMPXPlaybackUtility::UtilityL(EMPXCategoryVideo, KPbModeDefault));
        MMPXPlayerManager &manager = mPlaybackUtility->PlayerManager();
        QT_TRAP_THROWING(manager.SelectPlayerL(KVideoHelixPlaybackPluginUid));
        QT_TRAP_THROWING(mPlaybackUtility->AddObserverL(*this));
        mState = Connected;
        INFO("Connected");
    }
}

void SvpbEngine::disconnectMPX()
{
    FUNC_LOG;

    if (mState != Disconnected && mPlaybackUtility)
    {
        mPlaybackUtility->RemoveObserverL(*this);
        mPlaybackUtility->Close();
        mPlaybackUtility = 0;
        mState = Disconnected;
        INFO("Disconnected");
    }
}

/** Ownership is not transferred. */
void SvpbEngine::setSurfaceContainer(SvpbSurfaceContainer *surfaceContainer)
{
    FUNC_LOG;

    mSurfaceContainer = surfaceContainer;
}

void SvpbEngine::togglePause()
{
    FUNC_LOG;

    if (mState == PlayRequested || mState == Playing) {
        QT_TRAP_THROWING(mPlaybackUtility->CommandL(EPbCmdPause, 0));
        mState = PauseRequested;
        INFO("Pause requested");
    }
    else if (mState == PauseRequested || mState == Paused) {
        QT_TRAP_THROWING(mPlaybackUtility->CommandL(EPbCmdPlay, 0));
        mState = PlayRequested;
        INFO("Play requested");
    }
}

void SvpbEngine::stop()
{
    FUNC_LOG;

    if (mState == PlayRequested || mState == Playing) {
        QT_TRAP_THROWING(mPlaybackUtility->CommandL(EPbCmdStop, 0));
        mState = StopRequested;
        INFO("Stop requested");
    }
}

void SvpbEngine::HandlePlaybackMessage(CMPXMessage *aMessage, TInt aErr)
{
    FUNC_LOG;

    if (aMessage) {
        TMPXMessageId id(aMessage->ValueTObjectL<TMPXMessageId>(KMPXMessageGeneralId));

        if (id == KMPXMessageGeneral) {
            TInt msg(0), type(0);
            QT_TRAP_THROWING(msg = aMessage->ValueTObjectL<TInt>(KMPXMessageGeneralEvent));
            QT_TRAP_THROWING(type = aMessage->ValueTObjectL<TInt>(KMPXMessageGeneralType));
            INFO("KMPXMessageGeneral" << msg << type);

            switch(msg)
            {
            case TMPXPlaybackMessage::EInitializeComplete:
                INFO("Initialize complete");
                break;
            case TMPXPlaybackMessage::EStateChanged:
                mpxPbStateChanged(static_cast<TMPXPlaybackState>(type), aErr);
                break;
            default:
                break;
            }
        }
        else if (id == KMPXMediaIdVideoDisplayMessage) {
            TMPXVideoDisplayCommand msg =
                ( *(aMessage->Value<TMPXVideoDisplayCommand>(KMPXMediaVideoDisplayCommand)));
            INFO("KMPXMediaIdVideoDisplayMessage" << msg);

            switch (msg)
            {
            case EPbMsgVideoSurfaceCreated:
            case EPbMsgVideoSurfaceChanged:
                if (mSurfaceContainer) {
                    TSurfaceId surfaceId;
                    TRect cropRect;
                    TVideoAspectRatio aspectRatio;
                    QT_TRAP_THROWING(surfaceId = aMessage->ValueTObjectL<TSurfaceId>(KMPXMediaVideoDisplayTSurfaceId));
                    QT_TRAP_THROWING(cropRect = aMessage->ValueTObjectL<TRect>(KMPXMediaVideoDisplayCropRect));
                    QT_TRAP_THROWING(aspectRatio = aMessage->ValueTObjectL<TVideoAspectRatio>(KMPXMediaVideoDisplayAspectRatio));
                    mSurfaceContainer->attachSurface(surfaceId, cropRect, aspectRatio);
                }
                break;
            case EPbMsgVideoSurfaceRemoved:
                if (mSurfaceContainer) {
                    mSurfaceContainer->detachSurface();
                }
                break;
            }
        }
    }
}

void SvpbEngine::HandlePropertyL(TMPXPlaybackProperty aProperty, TInt aValue, TInt aError)
{
    Q_UNUSED(aProperty);
    Q_UNUSED(aValue);
    Q_UNUSED(aError);
    FUNC_LOG;
}

void SvpbEngine::HandleSubPlayerNamesL(TUid /*aPlayer*/,
                                                 const MDesCArray* /*aSubPlayers*/,
                                                 TBool /*aComplete*/,
                                                 TInt /*aError*/)
{
    FUNC_LOG;
}

void SvpbEngine::HandleMediaL(const CMPXMedia& /*aProperties*/, TInt /*aError*/)
{
    FUNC_LOG;

    QT_TRAP_THROWING(mPlaybackUtility->CommandL(EPbCmdPlay, 0));
    mState = PlayRequested;
    INFO("Play requested");
}

void SvpbEngine::mpxPbStateChanged(TMPXPlaybackState type, int err)
{
    FUNC_LOG;
    INFO("MPX PB state:" << type << "error" << err);

    if (err == 0) {
        switch(type) {
            case EPbStateInitialised:
                requestMedia(); // VideoHelixPlugin uses media request to signal view activation
                mState = MediaRequested;
                INFO("Media requested");
                break;
            case EPbStatePlaying:
                mState = Playing;
                INFO("Playing");
                break;
            case EPbStatePaused:
                mState = Paused;
                INFO("Paused");
                break;
            case EPbStateStopped:
                mState = Connected;
                INFO("Connected");
                emit finished();
                break;
            default:
                break;
        };
    }
}

void SvpbEngine::requestMedia()
{
    FUNC_LOG;

    MMPXSource *source = mPlaybackUtility->Source();
    HANDLE_ERROR_NULL(source);
    if (source) {
        RArray<TMPXAttribute> attrs;
        attrs.Append(KMPXMediaGeneralUri|
                     KMPXMediaGeneralDuration|
                     KMPXMediaGeneralTitle|
                     KMPXMediaGeneralMimeType);
        attrs.Append(KMPXMediaVideoAll);

        CMPXAttributeSpecs *specs = 0;
        TRAPD(err, specs = CMPXAttributeSpecs::NewL());
        HANDLE_ERROR_NEG(err);
        if (specs) {
            // Set the attribute to always route the media call to playback plugin
            TRAP(err, specs->SetTObjectValueL<TBool>(KMPXMediaGeneralExtMediaRedirect, ETrue));
            HANDLE_ERROR_NEG(err);
            if (err == KErrNone) {
                TRAP(err, source->MediaL(attrs.Array(), *this, specs));
                HANDLE_ERROR_NEG(err);
            }
        }

        delete specs;
        attrs.Close();
    }
}
