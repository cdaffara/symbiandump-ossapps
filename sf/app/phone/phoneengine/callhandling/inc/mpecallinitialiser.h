/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Initialises incoming or external calls.
*
*/


#ifndef MPECALLINITIALISER_H
#define MPECALLINITIALISER_H

#include <e32std.h>

class MCCECall;

/**
 *  Interface for incoming and external call initialisation
 *
 *  @lib CallHandling.lib
 *  @since S60 5.1
 */
NONSHARABLE_CLASS( MPECallInitialiser )
    {
public:
    
    /**
     * Initialises incoming / external voice call.
     * 
     * @param aCall New voice call.
     */ 
    virtual void InitVoiceCall( MCCECall& aNewCall ) = 0;
    
    /**
     * Initialises incoming / external data/video call.
     * 
     * @param aCall New data/video call.
     */ 
    virtual void InitVideoCall( MCCECall& aNewCall ) = 0;
    
    /**
     * Initialises self-created / external conference call.
     * 
     * @param aCall New data/video call.
     */ 
    virtual void InitConferenceCall( MCCEConferenceCall& aConference ) = 0;
    };


#endif // MPECALLINITIALISER_H
