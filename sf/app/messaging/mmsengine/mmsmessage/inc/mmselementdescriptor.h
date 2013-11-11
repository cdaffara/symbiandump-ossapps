/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   M-Mms-Element-Descriptor header
*
*/




#ifndef MMSELEMENTDESCRIPTOR_H
#define MMSELEMENTDESCRIPTOR_H

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
*  Object for storing X-Mms-Element-Descriptor header
*  Only present in a notification
*
*  @lib mmsutil.lib
*  @since 2.6
*/
class CMmsElementDescriptor : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMmsElementDescriptor();

        /**
        * Destructor.
        */
        virtual ~CMmsElementDescriptor();

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

        /*
        * How much room this needs on disk
        * @since 2.6
        * @return amount of bytes the data in this item needs
        */
        TInt Size() const;

        /*
        * Content reference accessor
        * @since 2.6
        * @return reference to content reference
        */
        IMPORT_C TPtrC ContentReference() const;

        /*
        * Content reference mutator
        * @since 2.6
        * @param aContentReference Content reference from X-Mms-Element-Descriptor header
        */
        IMPORT_C void SetContentReferenceL( const TDesC& aContentReference );

        /*
        * Content type accessor
        * @since 2.6
        * @return reference to content type
        */
        IMPORT_C TPtrC8 ContentType() const;

        /*
        * Content reference mutator
        * @since 2.6
        * @param aContentType Content type from X-Mms-Element-Descriptor header
        */
        IMPORT_C void SetContentTypeL( const TDesC8& aContentType );


    public: // Functions from base classes

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

    public:     // Data
    
    protected:  // Data

    private:    // Data
        // Content reference
        HBufC*  iContentReference; // text-string, actual meaning unknown
        // Content type of the top level message
        HBufC8* iContentType;      // text string like in mime headers
        // More parameters to be defined later.
        // There could be an array of strings depicting parameter-name
        // parameter-value pairs, but at the moment they have no use.
         

    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#endif      // MMSELEMENTDESCRIPTOR_H   
            
// End of File
