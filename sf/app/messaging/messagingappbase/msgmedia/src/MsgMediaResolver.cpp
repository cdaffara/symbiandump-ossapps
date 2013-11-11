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
*       Recognizes MIME type for a file.
*       Maps MIME types to media types.
*       Creates media info object for a file.
*
*/




// INCLUDE FILES
#include <e32std.h>
#include <e32base.h> // CBase
#include <apmstd.h>  // TDataType
#include <f32file.h> // RFs, RFile
#include <apgcli.h>  // RApaLsSession
#include <apmrec.h>

#include <coemain.h>
#include <barsread.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh> 

#include <DRMHelper.h>
#include <fileprotectionresolver.h>

// Features
#include <featmgr.h>    
#include <bldvariant.hrh>
#include <MessagingVariant.hrh>

// Resources
#include <msgmimetomediamapping.rsg>

#include "MsgMedia.hrh"
#include "MsgMediaResolver.h"
#include "MsgMediaTypeGroup.h"
#include "MsgMediaPanic.h"
#include "MsgMediaInfo.h"
#include "MsgImageInfo.h"
#include "MsgAudioInfo.h"
#include "MsgTextInfo.h"
#include "MsgVideoInfo.h"
#include "MsgMimeTypes.h"

// LOCAL CONSTANTS AND MACROS
// Unrecognised media type is 'x-epoc/x-app0'. 
// Media type 'x-epoc/x-appN', in which N>0 may also be possible. 
// N stands for Uid of the app handling the content on the Symbian OS platform 
_LIT8( KMmsSymbianInternalPattern, "x-epoc/x-app0");

_LIT( KMsgMediaResourceFileName, "MsgMimeToMediaMapping.rsc" ); 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Factory
// -----------------------------------------------------------------------------
//
EXPORT_C CMsgMediaResolver* CMsgMediaResolver::NewLC()
    {
    CMsgMediaResolver* self = new ( ELeave ) CMsgMediaResolver();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
         
// -----------------------------------------------------------------------------
// Factory
// -----------------------------------------------------------------------------
//
EXPORT_C CMsgMediaResolver* CMsgMediaResolver::NewL()
    {
    CMsgMediaResolver* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// C++ constructor
// -----------------------------------------------------------------------------
//
CMsgMediaResolver::CMsgMediaResolver() :
    iRecognizeCharSet( ETrue ) // CharSet recognition is on by default.
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CMsgMediaResolver::~CMsgMediaResolver()
    {
    if ( iWait.IsStarted() )
        {
        // Stop active wait.
        iWait.AsyncStop();
        }
    
    iFs.Close();
    iApaLsSession.Close();
    iMimeGroupArray.ResetAndDestroy();
    delete iFileProt;
    delete iDRMHelper;
    }

// -----------------------------------------------------------------------------
// 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CMsgMediaResolver::ConstructL()
    {
    // Create own shared file server session.
    User::LeaveIfError( iFs.Connect() );
    User::LeaveIfError( iFs.ShareProtected() );
    User::LeaveIfError( iApaLsSession.Connect() );
    
    iDRMHelper = CDRMHelper::NewL();
    iFileProt = CFileProtectionResolver::NewL( iFs );
    CreateMediaTypeGroupsL();
    }

// -----------------------------------------------------------------------------
// CMsgMediaResolver::CreateMediaTypeGroupsL
// -----------------------------------------------------------------------------
//
void CMsgMediaResolver::CreateMediaTypeGroupsL()
    {
    	TFileName fileName;	
		TFindFile finder( iFs );
		//Search Y->A then Z:
		TInt err = finder.FindByDir( KMsgMediaResourceFileName, KDC_RESOURCE_FILES_DIR );
		if(err == KErrNone)
		{
	    	fileName = finder.File();
		    RResourceFile resourceFile;
		    resourceFile.OpenL( iFs, fileName );
		    CleanupClosePushL( resourceFile );
		    resourceFile.ConfirmSignatureL( 0 );

		    HBufC8* dataBuffer = resourceFile.AllocReadLC( R_MSG_MEDIAGROUPS );

		    TResourceReader reader;
		    reader.SetBuffer( dataBuffer );

		    TInt numMediaGroups( reader.ReadInt16() );

		    for ( TInt i = 0; i < numMediaGroups; i++ )
		        {
		        CMsgMediaTypeGroup* mimeTypeGroup = CMsgMediaTypeGroup::NewLC( reader );
		        iMimeGroupArray.AppendL( mimeTypeGroup );
		        CleanupStack::Pop( mimeTypeGroup );
		        }

		    CleanupStack::PopAndDestroy( 2, &resourceFile ); // resourceFile.Close, dataBuffer
		}
    }

// -----------------------------------------------------------------------------
// CMsgMediaResolver::FileHandleL
// -----------------------------------------------------------------------------
//
EXPORT_C RFile CMsgMediaResolver::FileHandleL( const TDesC& aFileName )
    {
    RFile file;
    User::LeaveIfError( file.Open( iFs, aFileName, EFileRead | EFileShareReadersOnly ) );
    return file;
    }

// -----------------------------------------------------------------------------
// CMsgMediaResolver::RecognizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMsgMediaResolver::RecognizeL( RFile& aFile,
                                             TDataType& aMimeType )
    {
    TFileName* name = new ( ELeave ) TFileName;
    CleanupStack::PushL( name );
    User::LeaveIfError( aFile.Name( *name ) );
    TParsePtrC parse( *name );
    if ( parse.Ext().CompareF( KMsgExtSmil1 ) == 0 ||
         parse.Ext().CompareF( KMsgExtSmil2 ) == 0 ||
         parse.Ext().CompareF( KMsgExtSmil3 ) == 0 )
        {
        aMimeType = KMsgMimeSmil();
        }
    else
        {
        TDataRecognitionResult dataType;
        TInt err = iApaLsSession.RecognizeData( aFile, dataType );
        // Unrecognised content and no leaves in recogniser plugins
        // return err = KErrNone & dataType = 'x-epoc/x-app0'.
        // Unrecognised content and a leave in recogniser plugins
        // return err = <leave-code-in-recogniser>        
        if (    err
            ||  dataType.iDataType.Des8().Length() == 0
            ||  !dataType.iDataType.Des8().CompareF( KMmsSymbianInternalPattern ) )
            {
            // Not recognized
            aMimeType = KMsgMimeUnknown();
            }
        else
            {
            aMimeType = dataType.iDataType;
            }
        }
    CleanupStack::PopAndDestroy( name );
    }

// -----------------------------------------------------------------------------
// CMsgMediaResolver::MediaType
// -----------------------------------------------------------------------------
//
EXPORT_C TMsgMediaType CMsgMediaResolver::MediaType( const TDesC8& aMimeType ) const
    {
    TInt mimeGroupCount( iMimeGroupArray.Count() );
    for ( TInt i = 0; i < mimeGroupCount; i++ )
        {
        CMsgMediaTypeGroup* group = iMimeGroupArray[ i ];
        if ( group->HasMimeType( aMimeType ) )
            {
            return group->MediaType();
            }
        }
    return EMsgMediaUnknown;
    }
    
// -----------------------------------------------------------------------------
// CMsgMediaResolver::MediaInfoL
// -----------------------------------------------------------------------------
//
EXPORT_C CMsgMediaInfo* CMsgMediaResolver::CreateMediaInfoL( RFile& aFile )
    {
    TDataType* mimeType = new ( ELeave ) TDataType;
    CleanupStack::PushL( mimeType );
    RecognizeL( aFile, *mimeType );
    CMsgMediaInfo* info = DoCreateMediaInfoL( aFile, *mimeType );
    CleanupStack::PopAndDestroy( mimeType );
    return info;
    }

// -----------------------------------------------------------------------------
// CMsgMediaResolver::MediaInfoL
// -----------------------------------------------------------------------------
//
EXPORT_C CMsgMediaInfo* CMsgMediaResolver::CreateMediaInfoL( RFile& aFile, TDataType& aMimeType )
    {
    return DoCreateMediaInfoL( aFile, aMimeType );
    }

// -----------------------------------------------------------------------------
// CMsgMediaResolver::ParseInfoDetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CMsgMediaResolver::ParseInfoDetailsL( CMsgMediaInfo* aInfo, RFile& aFile )
    {
    aInfo->ParseInfoDetailsL( aFile, *iDRMHelper, *this );
     if ( iWait.IsStarted() )
        {
        // Stop active wait.
        iWait.AsyncStop();
        }
    // Start active wait
    iWait.Start();
    }

// -----------------------------------------------------------------------------
// CMsgMediaResolver::DoCreateMediaInfoL
// -----------------------------------------------------------------------------
//
CMsgMediaInfo* CMsgMediaResolver::DoCreateMediaInfoL( RFile& aFile, 
                                                      TDataType& aMimeType )
    {
    CMsgMediaInfo* mediaInfo = NULL;
    TMsgMediaType mediaType = MediaType( aMimeType.Des8() );
    switch ( mediaType )
        {
        case EMsgMediaText:
            {
            mediaInfo = CMsgTextInfo::NewL( aFile, aMimeType, iFs );
            if ( !iRecognizeCharSet )
                {
                // Suppress charset recognition.
                static_cast< CMsgTextInfo* >( mediaInfo )->SetCharacterSet( 0 );
                }
            break;
            }
        case EMsgMediaImage:
            {
            mediaInfo = CMsgImageInfo::NewL( aFile, aMimeType );
            break;
            }
        case EMsgMediaVideo:
            {
            mediaInfo = CMsgVideoInfo::NewL( aFile, aMimeType );
            break;
            }
        case EMsgMediaAudio:
            {
            mediaInfo = CMsgAudioInfo::NewL( aFile, aMimeType );
            break;
            }
        case EMsgMediaUnknown:
        default:
            {
            mediaInfo = CMsgMediaInfo::NewL( aFile, aMimeType, mediaType );
            break;
            }
        }
    
    CleanupStack::PushL( mediaInfo );

    TUid appUid( KNullUid );
    // Ignore possible error.
    TInt err = iApaLsSession.AppForDataType( aMimeType, appUid );
    mediaInfo->SetHandlerApplicationUid( appUid );

    HBufC8* drmUri = NULL;
    TInt protection = iFileProt->ProtectionStatusL( aFile, aMimeType, drmUri );
    mediaInfo->SetProtection( protection );
    mediaInfo->SetDRMContentURI( drmUri ); // ownership transferred

    CleanupStack::Pop( mediaInfo );
        
    return mediaInfo;
    }

// -----------------------------------------------------------------------------
// CMsgMediaResolver::MediaInfoParsed
// -----------------------------------------------------------------------------
//
void CMsgMediaResolver::MediaInfoParsed()
    {
    if ( iWait.IsStarted() )
        {
        // Stop active wait.
        iWait.AsyncStop();
        }
    }

//  End of File  
