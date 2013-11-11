/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Videoteleng extension base class 
*
*/


#ifndef CVTENGEXTENSIONSBASE_H
#define CVTENGEXTENSIONSBASE_H

//  INCLUDES
#include <e32base.h>
#include "vtengcommands.h"

// FORWARD DECLARATIONS
class CVtEngOperation;

// CLASS DECLARATION

/**
*  Videoteleng extension base class.
*
*  @lib videoteleng.lib
*  @since S60 v3.2
*/
NONSHARABLE_CLASS ( CVtEngExtensionBase ): public CBase
    
    {
    public: // pure virtuals.
        
        /**
        * Returns extension API interface 
        * @return API IF as TAny pointer
        */
        virtual TAny* GetInterface() = 0;
        
        /**
        * Handles requested extension operation
        * @param aOp requested operation pointer
        * @return TBool if operation succeed or not 
        */
        virtual TBool HandleL( CVtEngOperation* aOp ) = 0;
        
        /**
        * Validates requested extension command
        * @param aCommandId requested command identifier (TInt)
        * @return TBool if command is valid or not 
        */
        virtual TBool ValidateCommand( const TVtEngCommandId aCommandId ) = 0;
        
        /**
        * Gets extension UID
        * @return extension UID
        */
        const TUid& Uid() const;
        
    protected:// Constructor.
        /*
        * C++ default constructor.
        */        
        CVtEngExtensionBase( TUid aExtensionUid );
    
    private:
        
        // Extension UID 
        const TUid iExtensionUid;
    };
        
#endif //CVTENGEXTENSIONSBASE_H    
