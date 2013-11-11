/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Definition of CPhoneUIStateMachineFactoryVoIP class.
*
*/


#ifndef CPHONEUISTATEMACHINEFACTORYVOIP_H
#define CPHONEUISTATEMACHINEFACTORYVOIP_H

// INCLUDES
#include "cphoneuistatemachinefactorybase.h"

/**
* This class implements VoIP-specific variant of the PhoneUIStateMachine factory.
*/
NONSHARABLE_CLASS( CPhoneUIStateMachineFactoryVoIP ):   
    public CPhoneUIStateMachineFactoryBase
    {
    public: // From MPhoneUIStateMachineFactory

        /**
        * See MPhoneUIStateMachineFactory for details
        */
        MPhoneStateMachine* CreatePhoneStateMachineL(
            MPhoneViewCommandHandle* aViewCommandHandle );

        /**
        * See MPhoneUIStateMachineFactory for details
        */
        CPhoneResourceResolverBase* CreatePhoneResourceResolverL();
        
        /**
        * See MPhoneUIStateMachineFactory for details
        */
        CPhoneErrorMessagesHandler* CreatePhoneErrorMessagesHandlerL(
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneStateMachine* aStateMachine );
    };

#endif      // CPHONEUISTATEMACHINEFACTORYVOIP_H

// End of File
