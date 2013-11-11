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
*           Messaging Text Info class.
*
*/



#ifndef __MSGTEXTINFO_H
#define __MSGTEXTINFO_H

// INCLUDES

#include <e32std.h>
#include <f32file.h>

#include <MsgMediaInfo.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS

class RFile;
class TDataType;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* Messaging Text Info class.
*
* @lib MsgMedia.lib
* @since 3.1
*/
NONSHARABLE_CLASS( CMsgTextInfo ) : public CMsgMediaInfo
    {
    public:  // New methods

        /**
        * Factory method that creates this object.
        *
        * @param    aFile       IN File handle to create info class from.
        * @param    aMimeType   IN Mime type of the file.
        * @param    aFs         IN Reference to a file server session
        * @return   pointer to instance
        */
        IMPORT_C static CMsgTextInfo* NewL(
            RFile& aFile, 
            TDataType& aMimeType,
            RFs& aFs );

        /**
        * Destructor
        */
        virtual ~CMsgTextInfo();

        /**
        * Get character set
        *
        * @return   charset 
        */
        inline TUint CharacterSet() const;

        /**
        * Set character set
        * This can be used to disable automatic character set recognition.
        *
        * @param    aCharSet    IN Character set CharConv id
        */
        inline void SetCharacterSet( TUint aCharSet );
    
    protected:

        /**
        * From CActive
        */
    	void DoCancel();
        
        /**
        * From CActive
        */
    	void RunL();

    protected:

        /**
        * Constructor.
        *
        * @param    aMimeType   IN Mime type of the file.
        * @param    aFs         IN Reference to a file server session
        */
        CMsgTextInfo( TDataType& aMimeType, RFs& aFs );

        /**
        * 2nd phase constructor.
        *
        * @param    aFile       IN File handle to create info class from.
        */
        void ConstructL( RFile& aFile );

        /**
        * Initialize members for empty instance.
        */
        void ConstructEmpty();

    protected: // data

        RFs& iFs;
        TUint iCharSet;
        TBool iCharSetRecognized;

    };

#include <MsgTextInfo.inl>

#endif // __MSGTEXTINFO_H
