/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*/

#ifndef VCXMYVIDEOSTESTTRANSACTIONS_H
#define VCXMYVIDEOSTESTTRANSACTIONS_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  CVCXMyVideosTestTransactions
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
class CVCXMyVideosTestTransactions : public CBase
{
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CVCXMyVideosTestTransactions* NewL();

        /**
        * Destructor.
        */
        virtual ~CVCXMyVideosTestTransactions();

    public: // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CVCXMyVideosTestTransactions();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( );
        
    public: // New functions
    
        /**
         * Returns the current transaction ID.
         */
        TUint32 TransactionId();
        
        /**
        * Increments the current transaction ID.
        * @return Current transaction id.
        */
        TUint32 NextTransactionId();        
        
        /**
         * Adds transaction to queue for the current id.
         * @return Current transaction id.
         */
        void AddTransactionId( TUint32 aCommandId );
        
        /**
         * User calls this when response for transaction is got. 
         * Transaction is removed from queue if found.
         * @return ETrue if found from queue, else EFalse.      
         */
        TBool TransactionResponse( TUint32 aTransactionId );
        
        /**
         * Checks that all transactions have responses, meaning the queue is empty. 
         */
        TInt CheckTransactions();

    public: // Data
  
        TUint32 iTransactionId;
        
        RArray<TUint32> iTransactionIds;
        RArray<TUint32> iCommandIds;
};

#endif      // VCXMYVIDEOSTESTTRANSACTIONS_H

// End of File
