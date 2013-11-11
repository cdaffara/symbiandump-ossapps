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
* Description:  Videoteleng API extension configuration
*
*/


#ifndef CVTENGEXTENSIONS_H
#define CVTENGEXTENSIONS_H

// INCLUDES
#include <e32base.h>
#include "vtengcommands.h"

// CONSTANTS AND MACROS
const TInt KExtensionGranularity = 1;

// FORWARD DECLARATIONS
class CVtEngExtensionBase;
class CVtEngCameraPreferences;
class CVtEngOperation;
class CVtEngMediaHandler;

// CLASS DECLARATION

/**
*  Video telephony extension handling.
*
*  @lib videoteleng.lib
*  @since S60 v3.2
*/
NONSHARABLE_CLASS(CVtEngExtensions): public CBase
    
    {
    public: // Constructor and destructor.

        /**
        * Symbian two-phase constructor.
        * @return engine extension instance
        */
        static CVtEngExtensions* NewL();

        /**
        * Destructor.
        */
        ~CVtEngExtensions();

    public: 
        
        /**
        * Gets client interface from certain extension
        * @param aInterfacePtr a pointer to client IF pointer
        * @param aUid requested client IF extension UID
        * @param aHandler a handle to media handler 
        */
        TInt GetClientInterface( TAny** aInterfacePtr, 
        	const TUid& aUid , CVtEngMediaHandler& aHandler );
        
        /**
        * Offers operation for extensions
        * @param aOp operation pointer
        * @return TBool value indicating if any of exsisting extension
        * was able to handle operation 
        */
        TBool OfferOperationL( CVtEngOperation* aOp );
        
        /**
        * Validates extension command 
        * @param aCommandId command ID (TInt)
        * @return TBool value indicating if any of exsisting extension
        * was able to validate operation 
        */
        TBool ValidateCommand( const TVtEngCommandId aCommandId );
        
    private: 

        /**
        * C++ default constructor.
        */
        CVtEngExtensions();

        /**
        * Symbian constructor that may leave.
        */
        void ConstructL();
        
        /**
        * Creates certain extension 
        */
        CVtEngExtensionBase* CreateExtensionsL( const TUid& aUid, 
        	CVtEngMediaHandler& aHandler );
        
        /**
        * Search extensions using UID 
        */
        TInt LookupExtensions( const TUid& aUid) const;
    
    private:
        
        // Extension handle container
        RPointerArray <CVtEngExtensionBase> iExtensionArray;        
        
    };

#endif //CVTENGEXTENSIONS_H