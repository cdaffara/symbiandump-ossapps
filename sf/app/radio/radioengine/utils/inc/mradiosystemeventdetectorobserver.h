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

#ifndef MRADIOSYSTEMEVENTDETECTOROBSERVER_H
#define MRADIOSYSTEMEVENTDETECTOROBSERVER_H

// Class declaration
/**
*
*  Observer for the call and network state. Implementer will be notified each time the state has changed
*/
NONSHARABLE_CLASS( MRadioSystemEventDetectorObserver )
    {
public:

    /**
    * This callback notifies when networks comes up.
    */
    virtual void NetworkUpCallbackL() = 0;

    /**
    * This callback notifies when network goes down.
    */
    virtual void NetworkDownCallbackL() = 0;

    /**
    * This callback notifies when call becomes active.
    */
    virtual void CallActivatedCallbackL() = 0;

    /**
    * This callback notifies when call becomes deactive.
    */
    virtual void CallDeactivatedCallbackL() = 0;

    /**
    * This callback notifies when audio resources become available.
    */
    virtual void AudioResourcesAvailableL() = 0;

    /**
    * This callback notifies when audio auto resuming is forbidden.
    */
    virtual void AudioAutoResumeForbiddenL() = 0;

    /**
    * This callback notifies when an error occured
    * @param aError an error code
    */
    virtual void ErrorCallbackL( TInt aError ) = 0;

    };

#endif // MRADIOSYSTEMEVENTDETECTOROBSERVER_H


