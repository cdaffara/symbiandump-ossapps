/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   MediaResolverAPITestBlocks implementation*
*/




// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "MediaResolverAPITest.h"

// TESTED MODULE RELATED INCLUDES 
#include <MsgMediaResolver.h>
#include <MsgMimeTypes.h>
#include <MsgAudioInfo.h>
#include <MsgVideoInfo.h>
#include <MsgTextInfo.h>
#include <MsgImageInfo.h>

// EXTERNAL DATA STRUCTURES


// EXTERNAL FUNCTION PROTOTYPES  


// CONSTANTS


// MACROS


// LOCAL CONSTANTS AND MACROS


// MODULE DATA STRUCTURES


// LOCAL FUNCTION PROTOTYPES


// FORWARD DECLARATIONS


// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediaResolverAPITest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMediaResolverAPITest::Delete() 
    {
    if( iMediaResolver )
        {
        delete iMediaResolver;
        iMediaResolver = NULL;
        }
    if ( iMediaInfo )
        {
        delete iMediaInfo;
        iMediaInfo = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMediaResolverAPITest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
       
        //ADD NEW ENTRY HERE
        ENTRY( "MediaResolverNewL", CMediaResolverAPITest::TestMediaResolverNewL ),
        ENTRY( "MediaResolverNewLC", CMediaResolverAPITest::TestMediaResolverNewLC ),
        ENTRY( "FileHandle", CMediaResolverAPITest::TestFileHandleL ),
        ENTRY( "Recognize", CMediaResolverAPITest::TestRecognizeL ),
        ENTRY( "MediaType", CMediaResolverAPITest::TestMediaType ),
        ENTRY( "CreateMediaInfo", CMediaResolverAPITest::TestCreateMediaInfoL ),
        ENTRY( "CreateMediaInfo2", CMediaResolverAPITest::Test2CreateMediaInfoL ),
        ENTRY( "ParseInfoDetails", CMediaResolverAPITest::TestParseInfoDetailsL ),        
        ENTRY( "MediaInfoNewL", CMediaResolverAPITest::TestMediaInfoNewL ),
        ENTRY( "MediaInfoParseInfoDetails", CMediaResolverAPITest::TestMediaInfoParseInfoDetailsL ),
        ENTRY( "SetFileL", CMediaResolverAPITest::TestSetFileL ),
        ENTRY( "Corrupt", CMediaResolverAPITest::TestCorruptL ),        
        ENTRY( "AnyMediaInfoNewL", CMediaResolverAPITest::TestAnyMediaInfoNewL ),                
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::TestNewL
// Tests NewL of MsgMediaResolver
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::TestMediaResolverNewL( CStifItemParser& /*aItem*/ )
    {
    _LIT(KTestFunction,"MediaResolverNewL");
    iLog->Log( KTestFunction );
    if( iMediaResolver )
        {
        return KErrNone;
        }
    else
        {
        return KErrCancel;
        }
    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::TestNewLC
// Calls NewLC of MsgMediaResolver
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::TestMediaResolverNewLC( CStifItemParser& /*aItem*/ )
    {
    _LIT(KTestFunction,"MediaResolverNewLC");
    iLog->Log( KTestFunction );    
    CMsgMediaResolver*  mediaResolver = NULL;
    mediaResolver = CMsgMediaResolver::NewLC();
    CleanupStack::PopAndDestroy(mediaResolver);
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::TestFileHandleL
// Calls FileHandleL of MsgMediaResolver
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::TestFileHandleL( CStifItemParser& aItem )
    {
    _LIT(KTestFunction,"FileHandleL");
    iLog->Log( KTestFunction );     
    RFile fileHandle;
    if ( OpenFile( aItem, fileHandle )!= KErrNone  )
        {
        return KErrCancel;
        }
    CleanupClosePushL( fileHandle );
    CleanupStack::PopAndDestroy( &fileHandle );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::TestRecognizeL
// Calls RecognizeL of MsgMediaResolver
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::TestRecognizeL( CStifItemParser& aItem )
    {
    _LIT(KTestFunction,"RecognizeL");
    iLog->Log( KTestFunction );    
    TDataType mimeType;
    RFile fileHandle;
    if ( OpenFile( aItem, fileHandle )!= KErrNone  )
        {
        return KErrCancel;
        }  
    CleanupClosePushL( fileHandle );
    TRAPD(err, iMediaResolver->RecognizeL(fileHandle, mimeType))
    if ( err != KErrNone )
        {
        _LIT( KError , "RecognizeL failed with %d code");
        iLog->Log( KError,err );
        }
    CleanupStack::PopAndDestroy( &fileHandle );
    return err;
    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::TestMediaType
// Calls MediaType of MsgMediaResolver.
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::TestMediaType( CStifItemParser& /*aItem*/ )
    {
    _LIT(KTestFunction,"MediaType");
    iLog->Log( KTestFunction );  
    TBool testPass = ETrue;
    if(iMediaResolver->MediaType( KMsgMimeTextPlain )!= EMsgMediaText)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeAudioAmr )!= EMsgMediaAudio)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeAudioSpMidi )!= EMsgMediaAudio)
        {
        testPass = EFalse;
        }    
    if(iMediaResolver->MediaType( KMsgMimeImageJpeg )!= EMsgMediaImage)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeImageGif )!= EMsgMediaImage)
        {
        testPass = EFalse;
        }    
    if(iMediaResolver->MediaType( KMsgMimeImageWbmp )!= EMsgMediaImage)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeImagePng )!= EMsgMediaImage)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeImageBmp )!= EMsgMediaImage)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeImageOtaBmp )!= EMsgMediaImage)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeVideo3gpp )!= EMsgMediaVideo)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeVideo3gpp2 )!= EMsgMediaVideo)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeVideoMp4 )!= EMsgMediaVideo)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeVCard )!= EMsgMediaUnknown)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeVCal )!= EMsgMediaUnknown)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeICal )!= EMsgMediaUnknown)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeSmil )!= EMsgMediaUnknown)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeImageSvg )!= EMsgMediaSvg)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeTextXhtml )!= EMsgMediaXhtml)
        {
        testPass = EFalse;
        }
    if(iMediaResolver->MediaType( KMsgMimeUnknown )!= EMsgMediaUnknown)
        {
        testPass = EFalse;
        }    
    if(testPass)
        {
        return KErrNone;
        }
    else
        {
        _LIT(KError, "CMediaResolverAPITest::MediaType failed to check mediatype");
        iLog->Log( KError );
        return KErrCancel;
        }
    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::TestCreateMediaInfoL
// Tests CreateMediaInfoL of MsgMediaResolver with 1 parameter
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::TestCreateMediaInfoL( CStifItemParser& aItem )
    {
    _LIT(KTestFunction,"CreateMediaInfoL");
    iLog->Log( KTestFunction );    
    RFile fileHandle;
    if ( OpenFile( aItem, fileHandle )!= KErrNone  )
        {
        return KErrCancel;
        }
    CleanupClosePushL( fileHandle );
    TRAPD(err, iMediaResolver->CreateMediaInfoL( fileHandle ))
    if ( err != KErrNone )
        {
        _LIT( KError , "Createmediainfo failed with %d code");
        iLog->Log( KError,err );
        }    
    CleanupStack::PopAndDestroy( &fileHandle );
    return err;
    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::Test2CreateMediaInfoL
// Tests CreateMediaInfoL of MsgMediaResolver with 2 parameters
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::Test2CreateMediaInfoL( CStifItemParser& aItem )
    {
    _LIT(KTestFunction,"CreateMediaInfoL 2 params");
    iLog->Log( KTestFunction );
    
    RFile fileHandle;
    if ( OpenFile( aItem, fileHandle )!= KErrNone  )
        {
        return KErrCancel;
        }
    CleanupClosePushL( fileHandle );
    
    TDataType mimeType( KMsgMimeAudioAmr );
    TRAPD(err, iMediaResolver->CreateMediaInfoL( fileHandle, mimeType ))
    if ( err != KErrNone )
        {
        _LIT( KError , "Createmediainfo failed with %d code");
        iLog->Log( KError,err );
        }
    CleanupStack::PopAndDestroy( &fileHandle );
    return err;
    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::TestParseInfoDetailsL
// Calls MsgMediaResolver's PraseInfoDetailsL
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::TestParseInfoDetailsL( CStifItemParser& aItem )
    {
    _LIT(KTestFunction,"ParseInfoDetailsL");
    iLog->Log( KTestFunction );
    if( InitMediaInfoL( aItem ) == KErrNone )
        {
        TRAPD(err ,iMediaResolver->ParseInfoDetailsL( iMediaInfo, iFileHandle ))
        if( err != KErrNone )
            {
            iLog->Log(_L("ParseInfoDetailsL failed"));
            }
        MediaInfoCleanUp();
        return err;
        }
    else
        {
        return KErrCancel;
        }
    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::TestMediaInfoNewL
// calls MsgMeidaInfo's NewL
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::TestMediaInfoNewL( CStifItemParser& aItem )
    {
    _LIT(KTestFunction,"MediaInfo_NewL");
    iLog->Log( KTestFunction );
    
    if ( InitMediaInfoL( aItem ) == KErrNone )
        {
        MediaInfoCleanUp();
        return KErrNone;
        }
    else
        {
        return KErrCancel;
        }
    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::ParseInfoDetails
// Calls ParseInfoDetails of MsgMediaInfo.
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::TestMediaInfoParseInfoDetailsL( CStifItemParser& aItem )
    {
    _LIT(KTestFunction,"MediaInfo ParseInfoDetails");
    iLog->Log( KTestFunction );
    if ( InitMediaInfoL( aItem ) == KErrNone )
        {
        iMediaInfo->ParseInfoDetails( iFileHandle, *( iMediaResolver->DRMHelper() ), *iMediaResolver);
        MediaInfoCleanUp();
        return KErrNone;
        }
    else
        {
        return KErrCancel;
        }
    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::TestSetFileL
// This function calls MsgMediainfo's SetFile function .
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::TestSetFileL( CStifItemParser& aItem )
    {
    _LIT(KTestFunction,"MediaInfo SetFile");
    iLog->Log( KTestFunction );
    
    if ( InitMediaInfoL( aItem ) == KErrNone )
        {
        RFile fileHandle;
        if ( OpenFile( aItem, fileHandle )!= KErrNone  )
            {
            MediaInfoCleanUp();
            return KErrCancel;
            }  
        CleanupClosePushL( fileHandle );
        TRAPD( err, iMediaInfo->SetFileL( fileHandle ))
        if( err != KErrNone )
            {
            iLog->Log( _L("error in setfileL"));
            }
        CleanupStack::PopAndDestroy( &fileHandle );
        MediaInfoCleanUp();
        return err;
        }
    else
        {
        return KErrCancel;
        }
    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::TestCorrupt
// Calls corrupt function of MsgMediaObject to check whether provided mediafile
// is corrupted or not.
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::TestCorruptL( CStifItemParser& aItem )
    {
    _LIT(KTestFunction,"MediaInfo Corrupt");
    iLog->Log( KTestFunction );    
    if( InitMediaInfoL( aItem ) == KErrNone )
        {
        iMediaInfo->ParseInfoDetails( iFileHandle, *( iMediaResolver->DRMHelper() ), *iMediaResolver);
        TBool corrupted = iMediaInfo->Corrupt();
        if( corrupted )
            {
            iLog->Log( _L("given file is corrupted"));
            }
        else
            {
            iLog->Log( _L("given file is not corrupted"));
            }
        MediaInfoCleanUp();
        return KErrNone;
        }
    else
        {
        return KErrCancel;
        }
    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::TestAnyMediaInfoNewL
// This function takes a mediafile and creates corresponding mediaobject
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::TestAnyMediaInfoNewL( CStifItemParser& aItem )
    {
    _LIT(KTestFunction,"AnyMediaInfo_NewL");
    iLog->Log( KTestFunction );
    
    RFile fileHandle;
    if ( OpenFile( aItem, fileHandle )!= KErrNone  )
        {
        return KErrCancel;
        }  
    CleanupClosePushL( fileHandle );
    TDataType mimeType;    
    iMediaResolver->RecognizeL(fileHandle, mimeType);
    TMsgMediaType mediaType = iMediaResolver->MediaType( mimeType.Des8());
    CMsgMediaInfo* mediaInfo = NULL;
    switch( mediaType )
        {
        case EMsgMediaText:
             {
             RFs fs;
             User::LeaveIfError( fs.Connect() );
             User::LeaveIfError( fs.ShareProtected() );
             mediaInfo = CMsgTextInfo::NewL( fileHandle, mimeType, fs );
             fs.Close();
             iLog->Log( _L("Givenfile:- Text file"));
             break;
             }
         case EMsgMediaImage:
             {
             mediaInfo = CMsgImageInfo::NewL( fileHandle, mimeType );
             iLog->Log( _L("Givenfile:- Image file"));
             break;
             }
         case EMsgMediaVideo:
             {
             mediaInfo = CMsgVideoInfo::NewL( fileHandle, mimeType );
             iLog->Log( _L("Givenfile:- Video file"));
             break;
             }
         case EMsgMediaAudio:
             {
             mediaInfo = CMsgAudioInfo::NewL( fileHandle, mimeType );
             iLog->Log( _L("Givenfile:- Audio file"));
             break;
             }
         case EMsgMediaUnknown:
         default:
             {
             mediaInfo = CMsgMediaInfo::NewL( fileHandle, mimeType, mediaType );
             iLog->Log( _L("Givenfile:- Unknown file"));
             break;
             }
        }
    delete mediaInfo;
    CleanupStack::PopAndDestroy( &fileHandle );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::OpenFile
// This function is used to provide a file handle from given file as parameter
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::OpenFile( CStifItemParser& aItem, RFile& aFile )
    {
    TPtrC string;
    if ( aItem.GetNextString ( string ) != KErrNone )
        {
        iLog->Log( _L("Parameter Not Provided"));
        return KErrCancel;
        }
    else
        {
        TBuf16<256> fileName;
        TPtrC attachfilepath;

        /*Start:-Added For Hard Code Path Elimanation*/ 
#ifdef __WINSCW__
        fileName.Format( KTestFilePath, &string );
        aItem.GetNextString(attachfilepath);
#else
        aItem.GetNextString(attachfilepath);
        fileName.Format( attachfilepath, &string );
#endif
        /*End:-Added For Hard Code Path Elimanation*/	
        	
        TRAPD(err , aFile = iMediaResolver->FileHandleL(fileName))
        if ( err != KErrNone )
            {
            if ( err == KErrNotFound )
                {
                iLog->Log( _L("FileNotFound in TestPath"));
                }
            else
                {
                _LIT( KError, "Returned error : %d" );
                iLog->Log( KError, err);
                }
            }
        return err;
        }
    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::InitMediaInfoL
// This function creates MsgMediaInfo object and a filehandle. 
// -----------------------------------------------------------------------------
//
TInt CMediaResolverAPITest::InitMediaInfoL( CStifItemParser& aItem )
    {
    if ( OpenFile( aItem, iFileHandle )!= KErrNone  )
        {
        return KErrCancel;
        }
    CleanupClosePushL( iFileHandle );
    TDataType mimeType;    
    iMediaResolver->RecognizeL(iFileHandle, mimeType);
    TMsgMediaType mediaType = iMediaResolver->MediaType( mimeType.Des8());
    if( !iMediaInfo )
        {
        iMediaInfo = CMsgMediaInfo::NewL(iFileHandle, mimeType, mediaType);
        }
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CMediaResolverAPITest::MediaInfoCleanUp
// This function cleans up created MsgMediaInfo object and file handle.
// -----------------------------------------------------------------------------
//
void CMediaResolverAPITest::MediaInfoCleanUp( )
    {
    if( iMediaInfo )
        {
        delete iMediaInfo;
        iMediaInfo = NULL;
        }
    CleanupStack::PopAndDestroy( &iFileHandle );
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
