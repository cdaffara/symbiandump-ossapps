/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A plug-in for silencing the rigning tones.
*
*/



#ifndef C_SILENCEPLUGIN_H
#define C_SILENCEPLUGIN_H

#include <e32base.h>
#include <rphcltserver.h>
#include <cphcltcommandhandler.h>

#include "msilenceactionobserver.h"

// FORWARD DECLARATIONS
class CPhCltCommandHandler;

/**
* Mutes ringing tone by commanding phoneclient.
* 
* @lib silenceactionplugin.lib
* @since S60 3.2
*/
NONSHARABLE_CLASS( CSilenceAction ): public CActive
    {
    public:
    
        // Two phased constructors
        static CSilenceAction* NewL( MSilenceActionObserver& aObserver );
        static CSilenceAction* NewLC( MSilenceActionObserver& aObserver );
            
        // Destructor
        ~CSilenceAction();
        
    public: // New methods
    
        void ActionMuteRingingTone();

    private: // From base classes
    
        // @see CActive
        void RunL();
    
        // @see CActive
        void DoCancel();
    
        // @see CActive
        TInt RunError( TInt aError );
        
    private:
    
        // C++ constrcutor
        CSilenceAction( MSilenceActionObserver& aObserver );
        
        // Second phase constructor.
        void ConstructL();  
        
    private:
    
        // Silence observer.
        MSilenceActionObserver& iObserver;
        
        // phone client handles phone call operations.
        CPhCltCommandHandler* iPhoneClient;

    };
    
#endif // C_SILENCEPLUGIN_H
