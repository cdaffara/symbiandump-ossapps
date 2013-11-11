/*
* Copyright (c) 2003 -2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     Takes care of DRM attachments
*
*/



#ifndef MMSDRM_H
#define MMSDRM_H

//  INCLUDES

#include    <e32std.h>
#include    <f32file.h>
#include    <s32strm.h>

#include    <badesca.h>
#include    <caf/importfile.h>
#include    <caf/supplier.h>
#include    <caf/supplieroutputfile.h>
#include    <caf/metadataarray.h>
#include    <caf/metadata.h>
#include    <caf/caferr.h>
using namespace ContentAccess;

// CONSTANTS
_LIT8( KMmsDrmImportFile, "application/x-vnd.oma.drm.import" ); // DRM Import file
const TInt KMmsImportFileLength = 32;
_LIT8( KMmsForwardHeader, "fwd" );
_LIT8( KMmsRingtoneHeader, "ringtone" );
_LIT8( KMmsNoHeader, "no" );
_LIT8( KMmsContentTypeHeader, "Content-type" );
// Temporary definition until the URI is defined in some common include file
_LIT8( KMmsHutchContentURI, "flk:flkS60_3_0_Hutchinson_2005");

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

class CMsvMimeHeaders;
class CMsvStore;

// CLASS DECLARATION

NONSHARABLE_CLASS( CMmsDrmCaf ) : public CBase
    {
    public:  // Constructors and destructor
    
        /**
        * Two-phased constructor.
        */
        static CMmsDrmCaf* NewL();

        /**
        * Destructor.
        */
        virtual ~CMmsDrmCaf();
        
        /**
        * Check if DRM handling is needed.
        * If DRM handling is needed, a CImportFile is created using Mime content
        *    type and X-type parameters found in aMimeHeaders.
        * If this function returns ETrue, CMmsDrm is ready to process data
        * @param aMimeHeaders reference to callers mime headers.
        *    Should contain content-type and X-type headers (copied to metadata)
        * @return ETrue: registration was successful, and DRM handling will be needed
        *     EFalse: no DRM handling needed, use attachment as is
        */
        TBool RegisterL( const CMsvMimeHeaders& aMimeHeaders );
        
        /**
        *
        */
        void StartProcessingL( RFile& aStartFile, const TDesC& aAttachmentFilePath, CMsvStore& aStore );
        
        /**
        *
        */
        TInt WriteData( const TDesC8& aData );
        
        /**
        *
        */
        void EndProcessingL();
    
    private:
    
        /**
        * C++ constructor.
        */
        CMmsDrmCaf();
        
        /**
        *
        */
        void ConstructL();
        
        /**
        *
        */
        void NewFileL();


   
    private:    // Data
    	CImportFile*			iImportFile;
      	CSupplier*				iSupplier;
      	HBufC*                  iAttachmentFilePath;
      	// This is caller's store. Must not be deleted or closed
      	CMsvStore*              iStore;
      	// Open file handle for writing the attachment
        RFile                   iAttachmentFile;

    
    };

#endif      // MMSDRM_H   
            
// End of File
