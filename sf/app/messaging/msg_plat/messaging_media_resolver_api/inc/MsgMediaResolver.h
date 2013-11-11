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
*           Utility class for resolving detailed (media) file information
*
*/



#ifndef __MSGMEDIARESOLVER_H
#define __MSGMEDIARESOLVER_H

// INCLUDES

#include <e32std.h>
#include <e32base.h> // CBase
#include <f32file.h>
#include <apgcli.h>  // RApaLsSession

#include <MsgMedia.hrh>
#include <MsgMediaInfo.h>

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CMsgMediaInfo;
class RApaLsSession;
class CFileProtectionResolver;
class CDRMHelper;
class RFile;
class CMsgMediaTypeGroup;

// DATA TYPES

// FUNCTION PROTOTYPES

// CLASS DECLARATION

/**
* CMsgMediaResolver...
*
* @lib MsgMedia.lib
* @since 3.1
*/
NONSHARABLE_CLASS( CMsgMediaResolver ) : public CBase,
                                         public MMediaInfoObserver
    {
    public:  // New methods

        /**
        * Factory method.
        *
        * @param aFs        IN Fileserver session.
        * @return Pointer to instance in CleanupStack
        */
        IMPORT_C static CMsgMediaResolver* NewLC();
         
        /**
        * Factory method.
        *
        * @param aFs        IN Fileserver session.
        * @return Pointer to instance
        */
        IMPORT_C static CMsgMediaResolver* NewL();

        /**
        * Destructor
        */
        virtual ~CMsgMediaResolver();

        /**
        * Opens a (sharable) file handle from a file path
        *
        * @param aFileName  IN File path.
        * @return Open file handle.
        */
        IMPORT_C RFile FileHandleL( const TDesC& aFileName );
        
        /**
        * Recognizes the MIME type of a file.
        *
        * @param aFile      IN Open file handle
        * @param aMimeType  OUT Mime type of the file
        */
        IMPORT_C void RecognizeL( RFile& aFile, TDataType& aMimeType );
        
        /**
        * Returns the media type of the MIME type.
        *
        * @param aMimeType  IN Mime type
        * @return Media type
        */
        IMPORT_C TMsgMediaType MediaType( const TDesC8& aMimeType ) const;
        
        /**
        * Resolve the file information of the given file and
        * return the media info instance to the caller.
        *
        * - Does the same as ResolveFileL except returns the
        *   media info class instead of boolean value.
        *
        * @param aFile      IN Open file handle
        * @return Pointer to CMsgMediaInfo (ownership transferred)
        */
        IMPORT_C CMsgMediaInfo* CreateMediaInfoL( RFile& aFile );

        /**
        * Resolve the file information of the given file and
        * return the media info instance to the caller.
        *
        * - Does the same as ResolveFileTransferInfoL except does not
        *   recognize the file (uses given mime type).
        *
        * @param aFile      IN Open file handle
        * @param aMimeType  IN Mime type of the file.
        * @return Pointer to CMsgMediaInfo (ownership transferred)
        */
        IMPORT_C CMsgMediaInfo* CreateMediaInfoL( RFile& aFile, TDataType& aMimeType );
        
        /**
        * Parse media info details synchronously. 
        *
        * The usage of this method is highly discouraged since it uses
        * CActiveSchedulerWait for synchronizing the parsing. The 
        * preferred way is to use the asynchronous version of
        * ParseInfoDetailsL provided by CMsgMediaInfo.
        *
        * @param aInfo      IN Media info.
        * @param aFile      IN Open file handle. Must be the same from which
        *                      the media info is created from.
        */
        IMPORT_C void ParseInfoDetailsL( CMsgMediaInfo* aInfo, RFile& aFile );

        /**
        * Get DRM Helper
        *
        * @return Pointer to CDRMHelper
        */
        inline CDRMHelper* DRMHelper() const;

        /**
        * Get File Protection Resolver
        *
        * @return Pointer to CFileProtectionResolver
        */
        inline CFileProtectionResolver* FileProtectionResolver() const;

        /**
        * Enable/disable character set recognition.
        * If disabled it is assumed that the client knows and 
        * sets the character set to text info objects itself.
        *
        * @param    aEnable ETrue, Set CharSet recognition on,
        *                   EFalse, Set CharSet recognition off.
        */
        inline void SetCharacterSetRecognition( TBool aEnable );

    public:
            
        /**
        * From MMediaInfoObserver
        */
        void MediaInfoParsed();

    private:

        /**
        * Constructor.
        *
        * @param aFs        IN Fileserver session.
        */
        CMsgMediaResolver();

        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Reads media type group information from resources.
        */
        void CreateMediaTypeGroupsL();

        /**
        * Factory method that creates object of proper kind.
        *
        * @param aFile      IN File handle to create info class from.
        * @param aMimeType  IN Already recognized mime type.
        */
        CMsgMediaInfo* DoCreateMediaInfoL(
            RFile& aFile, 
            TDataType& aMimeType );

    private: // data

        RFs                      iFs;
        RApaLsSession            iApaLsSession;

        RPointerArray<CMsgMediaTypeGroup> iMimeGroupArray;

        CDRMHelper*              iDRMHelper;
        CFileProtectionResolver* iFileProt;

        TBool                    iRecognizeCharSet;
        CActiveSchedulerWait     iWait;
    };

#include <MsgMediaResolver.inl>

#endif // __MSGMEDIARESOLVER_H
