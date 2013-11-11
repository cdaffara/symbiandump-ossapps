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
* Description:  An action plug-in for silencing the ringing tones.
*
*/



#ifndef C_SILENCEACTIONPLUGIN_H
#define C_SILENCEACTIONPLUGIN_H

// INCLUDES
#include <cfactionplugin.h> 
#include <e32base.h>

#include "msilenceactionobserver.h"

// FORWARD DECLARATIONS
class CCFActionIndication;
class CSilenceAction;

// CLASS DECLARATION

/**
* Class CActionPlugIn.
*
* Provides control for silencing the ringing tones.
* Triggered when the conditions from the rule file are valid.
*/
class CSilenceActionPlugIn : 
    public CCFActionPlugIn,
    public MSilenceActionObserver
    {
    public: // Constructors and destructor

        static CSilenceActionPlugIn* NewL();
        static CSilenceActionPlugIn* NewLC();

        ~CSilenceActionPlugIn();

    public: // From CCFActionPlugIn

        // @see CCFActionPlugIn
        void InitializeL();

        // @see CCFActionPlugIn
        TExecutionTime ExecuteL(
            CCFActionIndication* aActionIndication );
                    
        // @see CCFActionPlugIn
        void GetActionsL( CDesCArray& aActionList ) const;
        
        // @see CCFActionPlugIn
        const TSecurityPolicy& SecurityPolicy() const;
        
    private: // From MSilenceActionObserver
        
        // @see MSilenceActionObserver
        void MuteActionCompleted();
        
    protected:

        // C++ constrcutor
        CSilenceActionPlugIn();
        
    private: // New functions
        
        void DoCancel();
        
        void RunL();
                    
    private: // Data
        
        // Silence action.
        CSilenceAction* iSilence;
    };

#endif // C_SILENCEACTIONPLUGIN_H
