/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef MMSDELETERESULTARRAY_H
#define MMSDELETERESULTARRAY_H

//  INCLUDES
#include <e32base.h>
#include <mtclbase.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Object for storing individual results from MMbox delete confirmation.
*
*  @lib mmsutil.lib
*  @since 2.6
*/
class CMmsDeleteResultArray : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CMmsDeleteResultArray();

        /**
        * Destructor.
        */
        virtual ~CMmsDeleteResultArray();

    public: // New functions
        
        /**
        * Ordering function.
        * @since 2.6
        * @param aFirst first item to be compared
        * @param aSecond second item to be compared
        * @return 1 if aFirst > aSecond, -1 if aFirst < aSecond, 0 if aFirst == aSecond
        */
        static TInt Compare( const CMmsDeleteResultArray& aFirst, const CMmsDeleteResultArray& aSecond );

        /**
        * Order accessor.
        * @since 2.6
        * @return iOrder
        */
        inline TUint Order() const;

        /**
        * Order mutator.
        * @since 2.6
        * @param sets iOrder
        */
        inline void SetOrder( TUint aOrder );

        /**
        * Response status accessor.
        * @since 2.6
        * @return response status.
        */
        inline TInt32 ResponseStatus() const;

        /**
        * Response status mutator. 
        * @since 2.1
        * @param aResponseStatus response status.
        */
        inline void SetResponseStatus( const TInt32 aResponseStatus );

        /**
        * Content Location accessor.
        * @since 2.6
        * @return Content Location of a message whose deletion failed.
        */
        IMPORT_C TPtrC8 ContentLocation() const;

        /**
        * ContentLocation mutator.
        * @since 2.6
        * @param aContentLocation content location of a message whose deletion failed
        */
        IMPORT_C void SetContentLocationL( const TDesC8& aContentLocation );

        /**
        * Response text accessor.
        * @since 2.6
        * @return Response text string.
        */
        IMPORT_C TPtrC ResponseText() const;

        /**
        * Response text mutator.
        * @since 2.6
        * @param aText Response text string,
        */
        IMPORT_C void SetResponseTextL( const TDesC& aText );


    public: // Functions from base classes

    protected:  // New functions
        
    protected:  // Functions from base classes
        
    private:

    public:     // Data
    
    protected:  // Data

    private:    // Data
        
        // order number (ties fields together)
        TUint32 iOrder;
        // error code
        TInt32  iResponseStatus;
        // content location for message whose deleteion failed
        HBufC8* iContentLocation;
        // error text
        HBufC*  iResponseText;


    public:     // Friend classes

    protected:  // Friend classes

    private:    // Friend classes

    };

#include "mmsdeleteresultarray.inl"

#endif      // MMSDELETERESULTARRAY_H   
            
// End of File
