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
*           Data utility class for UniEditor & MMS related editors and viewers
*
*
*/



#ifndef __UNIDATAUTILS_H
#define __UNIDATAUTILS_H

//  INCLUDES
#include <apmstd.h>         //TDataType
#include <apgcli.h>         //RApaLsSession
#include <barsc.h>
#include <UniModelConst.h>
#include <cmsvattachment.h> // TMsvAttachmentId


// CONSTANTS
_LIT8( KUniEmptySlide,      "application/X-MmsEmptySlide" );

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CMsgTextUtils;
class CBaseMtm;
class MMsvAttachmentManager;

// CLASS DECLARATION

/**
* CUniDataUtils - Miscellaneous UI indenependent utilities.
*
* @lib UniDataUtils.lib
* @since 3.1
*/
class CUniDataUtils : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Constructor.
        *
        * @since    3.1
        * @param    aFs     IN Fileserver session.
        * @return   Pointer to instance
        */
        IMPORT_C static CUniDataUtils* NewL( RFs& aFs );

        /**
        * Destructor.
        *
        * @since 3.1
        */
        virtual ~CUniDataUtils();

        /**
        * DefaultFileName
        * (e.g. "noname" - no prefix)
        *
        * @since    3.1
        * @return   Pointer to default file name buffer
        */
        inline TPtrC DefaultFileName() const;

        /**
        * EmptyPageString
        * (e.g. "Empty page")
        *
        * @since    3.1
        * @return   Pointer to "empty page" buffer
        */
        inline TPtrC EmptyPageString() const;

        /**
        * CharconvIdToMibIdL
        * Converts between id's.
        *
        * @since    3.1
        * @param    aCharconvCharsetId  IN charconv id
        * @return   MIB enumeration
        */
        IMPORT_C TUint CharconvIdToMibIdL( TUint aCharconvCharsetId );

        /**
        * MibIdToCharconvIdL
        * Converts between id's.
        *
        * @since    3.1
        * @param    aMibId  IN MIB enumeration
        * @return   Charconv id
        */
        IMPORT_C TUint MibIdToCharconvIdL( TUint aMibId );

        /**
        * Gets bytes size of UTF8 formatted text
        *
        * @since    3.1
        * @param aText
        * @return Size of text in bytes
        */
        IMPORT_C static TInt UTF8Size( TPtrC aText );

        /**
        * Gets bytes size of file.
        *
        * @since    3.1
        * @param aFileName
        * @return Size of the file.
        */
        IMPORT_C TInt FileSizeL( const TFileName& aFileName );

        /**
        * Get open file handle for an attachment
        *
        * @since    3.1
        * @param    aMtm    Client MTM
        * @param    aId     Attachment ID
        * @return   Open read-only file handle.
        *           It is callers responsibilty to close the handle.
        */
        IMPORT_C static RFile GetAttachmentFileL( CBaseMtm& aMtm, TMsvAttachmentId aId );

        /**
        * Get the "attachment manager index position" for an attachment.
        *
        * @since    3.1
        * @return "attachment manager index position"
        * @leave  KErrNotFound if attachment is not found from the store
        */
        IMPORT_C static TInt IndexPositionOfAttachmentL(
            MMsvAttachmentManager& aManager,
            TMsvAttachmentId aId );

    protected:

        /**
        * C++ constructor
        *
        * @since    3.1
        * @param    aFs     IN Fileserver session.
        */
        CUniDataUtils( RFs& aFs );

        /**
        * ConstructL
        * Symbian OS constructor
        *
        * @since    3.1
        */
        void ConstructL();

        /**
        * RegisterDataL
        * Reads data from resources.
        *
        * @since    3.1
        */
        void RegisterDataL();

        /**
        * ReadResStringsL
        * Reads strings from resources to internal mimetype arrays.
        *
        * @since    3.1
        * @param    aResourceFile   IN opened resourcefile
        * @param    aSrcResId   IN resource id to read from
        * @return   Pointer to read string
        **/
        HBufC* ReadResStringL( const RResourceFile& aResourceFile, TInt aSrcResId );

    protected:  // data

        RFs& iFs;
        CMsgTextUtils* iTextUtils;
        HBufC* iDefaultFileName;
        HBufC* iEmptyPageString;

    };

#include <UniDataUtils.inl>

#endif   // __UNIDATAUTILS_H

// End of file
