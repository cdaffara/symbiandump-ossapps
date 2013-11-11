/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Observer interface for engine operations
*
*/


#ifndef M_VTENGOPERATIONOBSERVER_H
#define M_VTENGOPERATIONOBSERVER_H

//  INCLUDES
#include <e32def.h>
#include "vtengcommands.h"

// FORWARD DECLARATIONS
class CVtEngOperation;

// CLASS DECLARATION

/**
*  Interface for setting active operation and responding to commands
*
*  @lib videoteleng
*  @since 5.0
*/
class MVtEngOperationObserver
    {
public:

    virtual void SetOperation( CVtEngOperation* aActiveOp ) = 0;
    
    virtual void CommandCompleteL(
        const TVtEngCommandId aCommand,
        const TInt aResult ) = 0;
    };

#endif // M_VTENGOPERATIONOBSERVER_H

// End of File
