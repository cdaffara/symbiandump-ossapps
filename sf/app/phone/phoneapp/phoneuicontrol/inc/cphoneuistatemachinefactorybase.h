/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*     See class description.
*
*/


#ifndef CPHONEUISTATEMACHINEFACTORYBASE_H
#define CPHONEUISTATEMACHINEFACTORYBASE_H

//  INCLUDES
#include    <e32base.h>
#include    "mphoneuistatemachinefactory.h"

/**
*  This class implements the PhoneUIStateMachine factory base. This class
*  contains common code used for Phone UI State Machine Factories. Also derived
*  from CBase, thus providing the cleanupstack support etc.
*/
class CPhoneUIStateMachineFactoryBase :
    public CBase,
    public MPhoneUIStateMachineFactory
    {
    public:
    };

#endif // CPHONEUISTATEMACHINEFACTORYBASE_H

// End of File