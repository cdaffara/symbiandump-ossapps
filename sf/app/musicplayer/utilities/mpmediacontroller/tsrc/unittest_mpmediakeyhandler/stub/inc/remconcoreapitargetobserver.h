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
* Description: remconcoreapitargetobserver stub for testing mpmediakeyhandler
*
*/


#ifndef REMCONCOREAPITARGETOBSERVER_H
#define REMCONCOREAPITARGETOBSERVER_H

#include <e32base.h>
#include <remconcoreapi.h>


class MRemConCoreApiTargetObserver
    {
public:

    virtual void MrccatoCommand(TRemConCoreApiOperationId aOperationId, 
        TRemConCoreApiButtonAction aButtonAct) = 0;
    
    virtual void MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed, 
        TRemConCoreApiButtonAction aButtonAct) = 0;

    virtual void MrccatoTuneFunction(TBool aTwoPart, 
        TUint aMajorChannel, 
        TUint aMinorChannel,
        TRemConCoreApiButtonAction aButtonAct) = 0;

   virtual void MrccatoSelectDiskFunction(TUint aDisk,
        TRemConCoreApiButtonAction aButtonAct) = 0;

    virtual void MrccatoSelectAvInputFunction(TUint8 aAvInputSignalNumber,
        TRemConCoreApiButtonAction aButtonAct) = 0;

    virtual void MrccatoSelectAudioInputFunction(TUint8 aAudioInputSignalNumber,
        TRemConCoreApiButtonAction aButtonAct) = 0;
    };

#endif // REMCONCOREAPITARGETOBSERVER_H
