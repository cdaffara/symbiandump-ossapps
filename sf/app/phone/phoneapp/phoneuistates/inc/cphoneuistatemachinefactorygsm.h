/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Definition of CPhoneUIStateMachineFactoryGSM class.
*
*/


#ifndef __CPHONEUISTATEMACHINEFACTORYGSM_H
#define __CPHONEUISTATEMACHINEFACTORYGSM_H

// INCLUDES
#include "cphoneuistatemachinefactorybase.h"

/**
* This class implements GSM-specific variant of the PhoneUIStateMachine factory.
*/
class CPhoneUIStateMachineFactoryGSM :   
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
        
        CPhoneErrorMessagesHandler* CreatePhoneErrorMessagesHandlerL(
            MPhoneViewCommandHandle* aViewCommandHandle,
            MPhoneStateMachine* aStateMachine );
    };

#endif      // __CPHONEUISTATEMACHINEFACTORYGSM_H

// End of File
