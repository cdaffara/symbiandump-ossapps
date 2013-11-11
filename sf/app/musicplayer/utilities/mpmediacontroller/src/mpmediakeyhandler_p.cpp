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
* Description: Music Player media key handler - private implementation.
*
*/


//symbian header files
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>

#include "mpmediakeyhandler_p.h"
#include "mpmediakeyhandler.h"
#include "mpmediakeyremconresponse.h"
#include "mpenginefactory.h"
#include "mpcommondefs.h"
#include "mptrace.h"


/*!
 \internal
 */
MpMediaKeyHandlerPrivate::MpMediaKeyHandlerPrivate( MpMediaKeyHandler *wrapper )
    : q_ptr( wrapper ),
      iInterfaceSelector(NULL),
      iResponseHandler(NULL),
      iMpEngine(NULL),
      iSideKeyAlreadyClicked(false)
{
    TX_LOG
}

/*!
 \internal
 */
MpMediaKeyHandlerPrivate::~MpMediaKeyHandlerPrivate()
{
    TX_ENTRY
    delete iResponseHandler;
    delete iInterfaceSelector;
    TX_EXIT
}

/*!
 \internal
 */
void MpMediaKeyHandlerPrivate::init()
{
    TX_ENTRY
    TRAPD(err, DoInitL());
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT

}

/*!
 From MRemConCoreApiTargetObserver
 A command has been received.
 */
void MpMediaKeyHandlerPrivate::MrccatoCommand(
    TRemConCoreApiOperationId aOperationId,
    TRemConCoreApiButtonAction aButtonAct )
{
    TX_ENTRY_ARGS( " aOperationId=" << aOperationId << " aButtonAct=" << aButtonAct );

    switch ( aOperationId ) {
    case ERemConCoreApiPausePlayFunction:
        if ( aButtonAct == ERemConCoreApiButtonClick ) {
            iMpEngine->playPause();
        }
        break;
    case ERemConCoreApiPlay:
        if ( aButtonAct == ERemConCoreApiButtonClick
             || aButtonAct == ERemConCoreApiButtonPress ) {
            iMpEngine->play();
        }
        break;
    case ERemConCoreApiStop:
        if ( aButtonAct == ERemConCoreApiButtonClick
             || aButtonAct == ERemConCoreApiButtonPress ) {
            iMpEngine->stop();
        }
        break;
    case ERemConCoreApiPause:
        if ( aButtonAct == ERemConCoreApiButtonClick
             || aButtonAct == ERemConCoreApiButtonPress ) {
            iMpEngine->pause();
        }
        break;
    case ERemConCoreApiRewind:
        switch ( aButtonAct ) {
        case ERemConCoreApiButtonPress:
            iMpEngine->startSeekBackward();
            break;
        case ERemConCoreApiButtonRelease:
            iMpEngine->stopSeeking();
            break;
        default:
            break;
        }
        break;
    case ERemConCoreApiFastForward:
        switch ( aButtonAct ) {
        case ERemConCoreApiButtonPress:
            iMpEngine->startSeekForward();
            break;
        case ERemConCoreApiButtonRelease:
            iMpEngine->stopSeeking();
            break;
        default:
            break;
        }
        break;
    case ERemConCoreApiBackward:
        if ( aButtonAct == ERemConCoreApiButtonClick ) {
            iMpEngine->skipBackward();
        }
        break;
    case ERemConCoreApiForward:
        if ( aButtonAct == ERemConCoreApiButtonClick ) {
            iMpEngine->skipForward();
        }
        break;
    // Pressing headset volume key only generates ButtonPress, however pressing and holding hardware side key
    // generates ButtonClick and after a while ButtonPressed, we should make sure increase/decrease only once
    case ERemConCoreApiVolumeUp:
        if ( aButtonAct == ERemConCoreApiButtonClick || 
             ( aButtonAct == ERemConCoreApiButtonPress && !iSideKeyAlreadyClicked ) ) {
            iSideKeyAlreadyClicked = true;
            iMpEngine->increaseVolume();
        }
        else if ( aButtonAct == ERemConCoreApiButtonRelease ) {
            iSideKeyAlreadyClicked = false;
        }
        break;
    case ERemConCoreApiVolumeDown:
        if ( aButtonAct == ERemConCoreApiButtonClick || 
             ( aButtonAct == ERemConCoreApiButtonPress && !iSideKeyAlreadyClicked ) ) {
            iSideKeyAlreadyClicked = true;
            iMpEngine->decreaseVolume();
        }
        else if ( aButtonAct == ERemConCoreApiButtonRelease ) {
            iSideKeyAlreadyClicked = false;
        }
        break;
    default:
        break;
    }
    iResponseHandler->CompleteAnyKey( aOperationId );
    TX_EXIT
}

/*!
 From MRemConCoreApiTargetObserver
 A 'play' command has been received.
 */
void MpMediaKeyHandlerPrivate::MrccatoPlay(
    TRemConCoreApiPlaybackSpeed aSpeed,
    TRemConCoreApiButtonAction aButtonAct )
{
    TX_ENTRY_ARGS( "aButtonAct=" << aButtonAct );
    Q_UNUSED(aSpeed);

    if ( ( aButtonAct == ERemConCoreApiButtonClick ) ||
        ( aButtonAct == ERemConCoreApiButtonPress ) ) {
            iMpEngine->play();
        }
    iResponseHandler->CompleteAnyKey( ERemConCoreApiPlay );
    TX_EXIT
}

/*!
 From MRemConCoreApiTargetObserver
 A 'tune function' command has been received.
 */
void MpMediaKeyHandlerPrivate::MrccatoTuneFunction(
    TBool aTwoPart,
    TUint aMajorChannel,
    TUint aMinorChannel,
    TRemConCoreApiButtonAction aButtonAct )
{
    Q_UNUSED(aTwoPart);
    Q_UNUSED(aMajorChannel);
    Q_UNUSED(aMinorChannel);
    Q_UNUSED(aButtonAct);

    iResponseHandler->CompleteAnyKey( ERemConCoreApiTuneFunction );
}

/*!
 From MRemConCoreApiTargetObserver
 A 'select disk function' has been received.
 */
void MpMediaKeyHandlerPrivate::MrccatoSelectDiskFunction(
    TUint aDisk,
    TRemConCoreApiButtonAction aButtonAct )
{
    Q_UNUSED(aDisk);
    Q_UNUSED(aButtonAct);

    iResponseHandler->CompleteAnyKey( ERemConCoreApiSelectDiskFunction );
}

/*!
 From MRemConCoreApiTargetObserver
 A 'select AV input function' has been received.
 */
void MpMediaKeyHandlerPrivate::MrccatoSelectAvInputFunction(
    TUint8 aAvInputSignalNumber,
    TRemConCoreApiButtonAction aButtonAct )
{
    Q_UNUSED(aAvInputSignalNumber);
    Q_UNUSED(aButtonAct);

    iResponseHandler->CompleteAnyKey( ERemConCoreApiSelectAvInputFunction );

}

/*!
 From MRemConCoreApiTargetObserver
 A 'select audio input function' has been received.
*/
void MpMediaKeyHandlerPrivate::MrccatoSelectAudioInputFunction(
    TUint8 aAudioInputSignalNumber,
    TRemConCoreApiButtonAction aButtonAct )
{
    Q_UNUSED(aAudioInputSignalNumber);
    Q_UNUSED(aButtonAct);

    iResponseHandler->CompleteAnyKey( ERemConCoreApiSelectAudioInputFunction );

}

/*!
 \internal
 */
void MpMediaKeyHandlerPrivate::DoInitL()
{
    TX_ENTRY
    // Register to remote control framework
    iInterfaceSelector = CRemConInterfaceSelector::NewL();
    CRemConCoreApiTarget *coreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this );
    CleanupStack::PushL( coreTarget );
    iInterfaceSelector->OpenTargetL();
    iResponseHandler = MpMediaKeyRemConResponse::NewL( *coreTarget );
    CleanupStack::Pop(coreTarget);

    iMpEngine = MpEngineFactory::sharedEngine();
    TX_EXIT
}

