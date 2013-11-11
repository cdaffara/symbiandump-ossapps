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
* Description:  Operation class for commands not originated by UI but
*                engine itself.
*
*/


#ifndef C_VTENGINTERNALOPERATION_H
#define C_VTENGINTERNALOPERATION_H

//  INCLUDES
#include <e32base.h>
#include "CVtEngOperation.h"

// FORWARD DECLARATIONS
class CVtEngCommandHandler;
class CVtEngHandlerContainer;

/**
*  Class for handling operation (command) originated engine itself.
*  Deviation from base class behavior: does not set itself as "active
*  operation" in command handler (observer of the operation). Also
*  response is given in synchronous manner.
*
*  @lib videoteleng
*  @since S60 5.0
*/
NONSHARABLE_CLASS( CVtEngInternalOperation ) : public CVtEngOperation
    {
public: // constructor and destructors

    /**
    * two-phase constructor
    * @param aCommandId command id
    * @param aHandlers handler container
    * @param aCommandHandler command handler
    * @param aDelete if ETrue command deletes itself after completion
    * @return internal operation
    */
    static CVtEngInternalOperation* NewL(
        TVtEngCommandId aCommandId,
        CVtEngHandlerContainer& aHandlers,
        MVtEngOperationObserver& aObserver,
        TBool aDelete = ETrue );

    /**
    * Destructor
    */
    ~CVtEngInternalOperation( );

public: // from CVtEngOperation

    /**
    * @see CVtEngOperation
    */
    virtual TBool ExecuteL();

    /**
    * @see CVtEngOperation
    */
    virtual void HandleOpComplete( const TInt aResult );

private:
    /**
    * c++ constructor
    */
    CVtEngInternalOperation(
        TVtEngCommandId aCommandId,
        CVtEngHandlerContainer& aHandlers,
        MVtEngOperationObserver& aObserver,
        TBool aDelete );
    };

#endif  // C_VTENGINTERNALOPERATION_H

// End of File
