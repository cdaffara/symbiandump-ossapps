/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Handles DRM attachments
*
*/



// INCLUDE FILES

#include    "mmsdrm.h"

// drm 
#include <e32base.h>
#include <e32std.h>
#include <f32file.h>

#include    <apmstd.h>

#include    <s32mem.h>
#include    <cmsvattachment.h>
#include    <cmsvmimeheaders.h>
#include    <msvstore.h>
#include    <DRMRights.h>
#include    <DrmPermission.h>
#include    <DrmConstraint.h>
#include    <DRMTypes.h>

// mms private headers
#include    "mmsconst.h"
#include    "mmserrors.h"

// CONSTANTS

const TInt KMms2 = 2; // just get rid of codescammer complaints
const TInt KMmsAssetLength = 1024; // just an estimate

// LOCAL FUNCTION PROTOTYPES


// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
CMmsDrmCaf::CMmsDrmCaf()
    {
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
CMmsDrmCaf* CMmsDrmCaf::NewL()
    {
    CMmsDrmCaf* self = new (ELeave) CMmsDrmCaf();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDrmCaf::ConstructL()
    {
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
CMmsDrmCaf::~CMmsDrmCaf()
    {
    delete iAttachmentFilePath;
	delete iSupplier;
	delete iImportFile;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CMmsDrmCaf::RegisterL( const CMsvMimeHeaders& aMimeHeaders )
    {
    // We allocate the supplier only when needed
	if( !iSupplier )
		{		
		iSupplier = CSupplier::NewL();
		}
    TBool retValue = EFalse;
    TInt length = Max( aMimeHeaders.ContentType().Length() +
       aMimeHeaders.ContentSubType().Length() + 1, KMmsImportFileLength );
       		
    HBufC8* contentType = HBufC8::NewL( length );
    CleanupStack::PushL( contentType );   
		
    contentType->Des().Copy( aMimeHeaders.ContentType() );
    contentType->Des().Append( KMmsSlash8 );
    contentType->Des().Append( aMimeHeaders.ContentSubType() );
    contentType->Des().LowerCase();
    
    TInt i = 0;
    CMetaDataArray* metaData = NULL;
    
    for ( i = 0; i < aMimeHeaders.ContentTypeParams().MdcaCount() - 1; i += KMms2 )
        {
        if ( aMimeHeaders.ContentTypeParams().MdcaPoint( i ).CompareF( KMmsForwardHeader ) == 0 
            && aMimeHeaders.ContentTypeParams().MdcaPoint( i + 1 ).CompareF( KMmsNoHeader ) == 0 )
            {
            metaData = CMetaDataArray::NewL();
            CleanupStack::PushL( metaData );
            metaData->AddL( KMmsContentTypeHeader , contentType->Des() );
            contentType->Des().Copy( KMmsDrmImportFile ); 
            }
        else if ( aMimeHeaders.ContentTypeParams().MdcaPoint( i ).
            CompareF( KMmsRingtoneHeader ) == 0 
            && aMimeHeaders.ContentTypeParams().MdcaPoint( i + 1 ).CompareF( KMmsNoHeader ) == 0 )
            {
            metaData = CMetaDataArray::NewL();
            CleanupStack::PushL( metaData );
            metaData->AddL( KMmsContentTypeHeader , contentType->Des() );
            contentType->Des().Copy( KMmsDrmImportFile ); 
            // Must also generate rights object and stream it into metadata
            //create rights
            CDRMRights* rights = CDRMRights::NewL();
            CleanupStack::PushL( rights );
            
            TInt size = 0;
            // Asset contains the content id
            // DrmAsset.h
            CDRMAsset* asset = CDRMAsset::NewLC();
            asset->iUid = KMmsHutchContentURI().AllocL(); // This will be freed by the assets destructor
            // Set the asset to the rights class, it will duplicate the asset
            rights->SetAssetL( *asset );
            size += KMmsAssetLength; // This is a guess, we don't know asset length.
            
            CleanupStack::PopAndDestroy( asset ); // Asset
            
            // DRMPermission.h
            CDRMPermission* permission = CDRMPermission::NewLC();
            // DRMConstraint.h
            // DRMTypes.h
            permission->iPlay = CDRMConstraint::NewL();
            permission->iDisplay = CDRMConstraint::NewL();
            permission->iExecute = CDRMConstraint::NewL();
            permission->iPrint = CDRMConstraint::NewL();
            permission->iAvailableRights = ERightsPlay | ERightsDisplay |
                ERightsExecute | ERightsPrint;
            // major version for Oma 1 Rights Objects    
            permission->iRightsObjectVersion.iVersionMain = 1;

            // "ringtone=no" present 
            permission->iInfoBits = ENoRingingTone;
            permission->iOriginalInsertTime.UniversalTime();
            // Set top level and export to empty ones
            permission->iTopLevel = CDRMConstraint::NewL();
            permission->iExport = CDRMConstraint::NewL();

            // Set the permission to the rights class, it will duplicate the permission
            rights->SetPermissionL( *permission );
            
            size += permission->Size();

            CleanupStack::PopAndDestroy( permission ); // Permission

            // stream the rights into metadata
                
            HBufC8* data = HBufC8::NewMaxLC( size );

            RMemWriteStream stream( (TAny*)( data->Ptr() ), size );
            CleanupClosePushL( stream );

            rights->ExternalizeL( stream );
            metaData->AddL( KOmaImportRightsField(), data->Des() );
    
            CleanupStack::PopAndDestroy( &stream ); // close stream
            CleanupStack::PopAndDestroy( data ); 
            
            CleanupStack::PopAndDestroy( rights );
            }
        }
    
    if ( iSupplier->IsImportSupported( contentType->Des() ) )
        {
        if ( !metaData )
            {
            metaData = CMetaDataArray::NewL();
            CleanupStack::PushL( metaData );
            }
        for ( i = 0; i < aMimeHeaders.XTypeParams().MdcaCount() - 1; i+= KMms2 )
            {
            metaData->AddL( aMimeHeaders.XTypeParams().MdcaPoint( i ),
                aMimeHeaders.XTypeParams().MdcaPoint( i + 1 ) );
            }
        // Prepare agent for writing data 
        if ( iImportFile )
            {
            // invalid state, previous process not finished
            User::Leave( KErrArgument );
            }
    	iImportFile = iSupplier->ImportFileL( contentType->Des(), *metaData );
        retValue = ETrue;
        }

    if ( metaData )
        {
      	CleanupStack::PopAndDestroy( metaData );
        }
    CleanupStack::PopAndDestroy( contentType );
    return retValue;

    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDrmCaf::StartProcessingL(
    RFile& aStartFile, const TDesC& aAttachmentFilePath, CMsvStore& aStore )
    {
    delete iAttachmentFilePath; // just in case the processing has been terminated incorreclty
    iAttachmentFilePath = NULL;
    iAttachmentFilePath = HBufC::NewL( aAttachmentFilePath.Length() );
    iAttachmentFilePath->Des().Copy( aAttachmentFilePath );
    iStore = &aStore;
    // We duplicate the handle, the caller may close its own copy
    iAttachmentFile.Duplicate( aStartFile );
    }


// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TInt CMmsDrmCaf::WriteData( const TDesC8& aData )
    {
    TInt error = KErrNone;
    
    error = iImportFile->WriteData( aData );
    if ( error != KErrCANewFileHandleRequired )
        {
        // either all went well or the data was corrupted
        return error;
        }
        
    // We need a new file handle (error == KErrCANewFileHandleRequired)
    if ( iAttachmentFile.SubSessionHandle() )
        {
        // Caller provided open file handle
        error = iImportFile->ContinueWithNewOutputFile( iAttachmentFile,
            iAttachmentFilePath->Des() );
        // Close the caller's file handle as CAF has made a copy
        iAttachmentFile.Close();
        if ( error == KErrCANewFileHandleRequired )
            {
            TRAP( error, NewFileL() );
            }
        }
    else
        {
        TRAP( error, NewFileL() );
        }
    // We hope all went well    
    return error;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDrmCaf::EndProcessingL()
    {
    TInt error = KErrNone;
    
    error = iImportFile->WriteDataComplete();
    if ( error == KErrCANewFileHandleRequired )
        {
        // CAF framework requires another file handle to finish the session
        TRAP( error, NewFileL() ); 
        }
        
    // We clean everything even in case of error
    delete iAttachmentFilePath;
    iAttachmentFilePath = NULL;
    // Terminate the session
    delete iImportFile;
    iImportFile = NULL;
    
    User::LeaveIfError( error );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMmsDrmCaf::NewFileL()
    {
    TInt error = KErrNone;
    TFileName* filename = new( ELeave )TFileName;
    CleanupStack::PushL( filename );
    
    do
        {
        if ( iImportFile->GetSuggestedOutputFileName( *filename ) != KErrNone )
            {
            TFileName* extension = new( ELeave )TFileName;
            CleanupStack::PushL( extension );
            _LIT( KDefAttName, "drmatt");
            *filename = KDefAttName;
            if ( iImportFile->GetSuggestedOutputFileExtension( *extension ) == KErrNone )
                {
                (*filename).Append( *extension );
                }
            CleanupStack::PopAndDestroy( extension );
            }
		CMsvAttachment* attachment = CMsvAttachment::NewL( CMsvAttachment::EMsvFile );
        RFile file;
   		iStore->CreateShareProtectedAttachmentL( *(filename), file, attachment );
		error = iImportFile->ContinueWithNewOutputFile( file, attachment->FilePath() );
		// import file duplicates the handle
        file.Close();
        } while  ( error == KErrCANewFileHandleRequired );
    
    CleanupStack::PopAndDestroy( filename );
    User::LeaveIfError( error );
    }
    

//  End of File  

