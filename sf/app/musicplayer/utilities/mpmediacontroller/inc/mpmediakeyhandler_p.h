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

#ifndef MPMEDIAKEYHANDLER_P_H
#define MPMEDIAKEYHANDLER_P_H


#include <remconcoreapitargetobserver.h>

class CRemConInterfaceSelector;
class MpMediaKeyHandler;
class MpMediaKeyRemConResponse;
class MpEngine;

class MpMediaKeyHandlerPrivate : public MRemConCoreApiTargetObserver
{
public:

    MpMediaKeyHandlerPrivate( MpMediaKeyHandler *wrapper );
    virtual ~MpMediaKeyHandlerPrivate();

    void init();

// From MRemConCoreApiTargetObserver

    /**
     * From MRemConCoreApiTargetObserver
     * A command has been received.
     *
     * @param aOperationId The operation ID of the command.
     * @param aButtonAct The button action associated with the command.
     */
    void MrccatoCommand(
        TRemConCoreApiOperationId aOperationId,
        TRemConCoreApiButtonAction aButtonAct );

    /**
     * From MRemConCoreApiTargetObserver
     * A 'play' command has been received.
     *
     * @param aSpeed The playback speed.
     * @param aButtonAct The button action associated with the command.
     */
    void MrccatoPlay(
        TRemConCoreApiPlaybackSpeed aSpeed,
        TRemConCoreApiButtonAction aButtonAct );

    /**
     * From MRemConCoreApiTargetObserver
     * A 'tune function' command has been received.
     *
     * @param aTwoPart Determine which channel to be used.
     * @param aMajorChannel The major channel number.
     * @param aMinorChannel The minor channel number.
     * @param aButtonAct The button action associated with the command.
     */
    void MrccatoTuneFunction(
        TBool aTwoPart,
        TUint aMajorChannel,
        TUint aMinorChannel,
        TRemConCoreApiButtonAction aButtonAct );

    /**
     * From MRemConCoreApiTargetObserver
     * A 'select disk function' has been received.
     *
     * @param aDisk The disk.
     * @param aButtonAct The button action associated with the command.
     */
    void MrccatoSelectDiskFunction(
        TUint aDisk,
        TRemConCoreApiButtonAction aButtonAct );

    /**
     * From MRemConCoreApiTargetObserver
     * A 'select AV input function' has been received.
     *
     * @param aAvInputSignalNumber The AV input.
     * @param aButtonAct The button action associated with the command.
     */
    void MrccatoSelectAvInputFunction(
        TUint8 aAvInputSignalNumber,
        TRemConCoreApiButtonAction aButtonAct );

    /**
     * From MRemConCoreApiTargetObserver
     * A 'select audio input function' has been received.
     *
     * @param aAudioInputSignalNumber The audio input.
     * @param aButtonAct The button action associated with the command.
     */
    void MrccatoSelectAudioInputFunction(
        TUint8 aAudioInputSignalNumber,
        TRemConCoreApiButtonAction aButtonAct );

private:

    void DoInitL();

private:

    MpMediaKeyHandler *q_ptr;

    CRemConInterfaceSelector    *iInterfaceSelector;   // owned
    MpMediaKeyRemConResponse    *iResponseHandler;     // owned
    MpEngine                    *iMpEngine;            // not owned
    bool                        iSideKeyAlreadyClicked;
};

#endif // MPMEDIAKEYHANDLER_P_H
