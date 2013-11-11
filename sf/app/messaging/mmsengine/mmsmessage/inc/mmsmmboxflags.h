/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Structure to hold MMS encapsulation 1.1 version
*                previoulsy-sent-by and previously-sent-date information  
*
*/




#ifndef MMSMMBOXFLAGS_H
#define MMSMMBOXFLAGS_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class RMsvReadStream;
class RMsvWriteStream;

// CLASS DECLARATION

/**
*  Object for storing previously-sent-by objects.
*
*  @lib mmsutil.lib
*  @since 2.6
*/
class CMmsMMBoxFlags : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMmsMMBoxFlags();

        /**
        * Destructor.
        */
        virtual ~CMmsMMBoxFlags();

    public: // New functions
        
        /**
        * Internalize.
        * @since 2.6
        * @param aStream opened read stream
        */
        void InternalizeL( RMsvReadStream& aStream );

        /**
        * Externalize.
        * @since 2.6
        * @param aStream opened write stream
        */
        void ExternalizeL( RMsvWriteStream& aStream ) const;

        /**
        * Operation token accessor.
        * @since 2.6
        * @return operation token for this keyword
        */
        inline TInt32 Token() const;

        /**
        * Order mutator.
        * @since 2.6
        * @param sets token to be Add/Delete/Filter
        */
        inline void SetToken( TInt32 aToken );

        /**
        * Keyword accessor.
        * @since 2.6
        * @return Keyword.
        */
        inline TPtrC Keyword() const;

        /**
        * Keyword mutator.
        * @since 2.6
        * @param aKeyword
        */
        void SetKeywordL( const TDesC& aKeyword );

        /*
        * How much room this needs on disk
        * @since 2.1
        * @return amount of bytes the data in this item needs
        */
        TInt Size() const;

    public: // Functions from base classes

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

    public:     // Data
    
    protected:  // Data

    private:    // Data
        
        // Operation for the flag 
        TInt32  iOperationToken; // Add/Remove/Filter
        // the address of the sender
        HBufC*  iKeyword;

    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#include "mmsmmboxflags.inl"

#endif      // MMSMMBOXFLAGS_H   
            
// End of File
