/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/


// INCLUDE FILES
#include <apgcli.h>
#include <DRMHelper.h>
#include <bldvariant.hrh>
#include <pathinfo.h>
#include <ProfileEngineDomainCRKeys.h>
#include "cphoneringingtone.h"
#include "cphonecenrepproxy.h"
#include "phonelogger.h"
#include "phoneconstants.h"
#include "phonelogger.h"

// CONSTANTS
// Rich audio file MIME types
_LIT(KAac, "audio/aac");
_LIT(KMp3, "audio/mp3");
_LIT(KMpeg, "audio/mpeg");
_LIT(K3gpp, "audio/3gpp");
_LIT(KMp4, "audio/mp4");
_LIT(KAmrWb, "audio/amr-wb");
_LIT(KWavX, "audio/x-wav");
_LIT(KWav, "audio/wav");

// Rich video file MIME types
_LIT(KV3gpp, "video/3gpp");
_LIT(KVMp4, "video/mp4");
_LIT(KV3gpp2, "video/3gpp2");

//file size limit
const TUint KPhoneToneFileSizeLimitKB = 5000;

// MACROS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhoneRingingTone::CPhoneRingingTone
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhoneRingingTone::CPhoneRingingTone( TBool aDrmInPlayback ) :
    iDrmInPlayback( aDrmInPlayback )
    {
    }

// -----------------------------------------------------------------------------
// CPhoneRingingTone::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhoneRingingTone::ConstructL( const TDesC& aFileName )
    {
    iFileName = aFileName.AllocL();
    
    GetMaxToneFileSize();
    }

// -----------------------------------------------------------------------------
// CPhoneRingingTone::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPhoneRingingTone* CPhoneRingingTone::NewL( 
    const TDesC& aFileName,
    TBool aDrmInPlayback )
    {
    CPhoneRingingTone* self = new( ELeave ) CPhoneRingingTone(
        aDrmInPlayback );

    CleanupStack::PushL( self );
    self->ConstructL( aFileName );
    CleanupStack::Pop( self );
    
    return self;
    }
    
// Destructor
CPhoneRingingTone::~CPhoneRingingTone()
    {
    delete iFileName;
    delete iMimeType;    
    }

// -----------------------------------------------------------------------------
// CPhoneRingingTone::SetFileName
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPhoneRingingTone::SetFileName( const TDesC& aFileName )
    {
    delete iFileName;
    iFileName = NULL;
    iFileName = aFileName.Alloc();
    }

// -----------------------------------------------------------------------------
// CPhoneRingingTone::SetVolume
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPhoneRingingTone::SetVolume( const TInt aVolume )
    {
    iVolume = aVolume;
    }

// -----------------------------------------------------------------------------
// CPhoneRingingTone::SetRingingType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPhoneRingingTone::SetRingingType( TProfileRingingType aRingingType)
    {
    iRingingType = aRingingType;
    }

// -----------------------------------------------------------------------------
// CPhoneRingingTone::Volume
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPhoneRingingTone::Volume() const
    {
    return iVolume;
    }

// -----------------------------------------------------------------------------
// CPhoneRingingTone::RingingType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TProfileRingingType CPhoneRingingTone::RingingType() const
    {
    return iRingingType; 
    }

// -----------------------------------------------------------------------------
// CPhoneRingingTone::FileName
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CPhoneRingingTone::FileName() const
    {
    // iFileName is never NULL
    return *iFileName;            
    }

// -----------------------------------------------------------------------------
// CPhoneRingingTone::SetTtsToneToBePlayed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CPhoneRingingTone::SetTtsToneToBePlayed( TBool aTtsToneToBePlayed ) 
    {
    iTtsToneToBePlayed = aTtsToneToBePlayed;          
    }
// -----------------------------------------------------------------------------
// CPhoneRingingTone::TtsToneToBePlayed
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CPhoneRingingTone::TtsToneToBePlayed() const
    {
    
    return iTtsToneToBePlayed;    
    }

// -----------------------------------------------------------------------------
// CPhoneRingingTone::MimeType
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
const TDesC& CPhoneRingingTone::MimeType() const
    {
    if ( iMimeType )
        {
        return *iMimeType;        
        }
    else
        {
        return KNullDesC;    
        }
    }

// -----------------------------------------------------------------------------
// CPhoneRingingTone::IsVideoRingingTone
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
TBool CPhoneRingingTone::IsVideoRingingTone()
    {
#ifdef RD_VIDEO_AS_RINGING_TONE
    if ( RefreshMime() != KErrNone  )
        {
        // try to handle as audio
        return EFalse;                
        }

    TBool isVideo( EFalse );    
        
    if ( iMimeType && iMimeType->MatchF( KPhoneRingingToneVideoMime ) != 
         KErrNotFound )
        {
        isVideo = ETrue;
        }
    else if ( iMimeType && iMimeType->MatchF( KPhoneRingingToneRealVideoMime ) != 
              KErrNotFound )
        {
        isVideo = ETrue;    
        }
    
    if ( isVideo )
        {
        if ( IsFileInRom() && !IsFileInVideoDirectory() )
            {
            // For ROM files check also location, because
            // MIME check is not fully reliable.
            isVideo = EFalse;        
            }            
        }
    
    return isVideo;
#else
    // if extended security -> refresh MIME
    if ( iDrmInPlayback )
        {
        RefreshMime();            
        }
    
    return EFalse;
#endif    
    }

// -----------------------------------------------------------------------------
// CPhoneRingingTone::IsFileDrmProtected
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
TBool CPhoneRingingTone::IsFileDrmProtected() const
    {
    const TDesC& type = MimeType();
        
    if ( type == KAac  || type == KMp3 || type == KMpeg ||
         type == K3gpp || type == KMp4 || type == KAmrWb ||
         type == KWavX || type == KWav || type == KV3gpp || 
         type == KVMp4 || type == KV3gpp2 )
        {
        ContentAccess::CContent* content = NULL;
        TRAPD( err, content = ContentAccess::CContent::NewL( *iFileName ) );
        if ( err == KErrNone && content )
            {
            TInt drmProtected( 0 );
            content->GetAttribute( ContentAccess::EIsProtected, drmProtected );
            delete content;
            return drmProtected;        
            }
        }
    
    return ETrue; // Other MIMEs can be played without DRM check.    
    }

// -----------------------------------------------------------------------------
// CPhoneRingingTone::IsFileInRom
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
TBool CPhoneRingingTone::IsFileInRom() const
    {
    TParsePtrC parsedName( *iFileName );
    
    // Files on rom are not DRM checked
    if ( parsedName.Drive().CompareF( KPhoneRingingToneDriveZ ) == 0 )
        {
        return ETrue;
        }
    else
        {
        return EFalse;    
        }        
    }    

// -----------------------------------------------------------------------------
// CPhoneRingingTone::IsFileInVideoDirectory
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CPhoneRingingTone::IsFileInVideoDirectory() const
    {
    TParsePtrC parsedName( *iFileName );
    
    if ( PathInfo::PathType( parsedName.DriveAndPath() ) == 
         PathInfo::EVideosPath )
        {
        return ETrue;
        }
    else
        {
        return EFalse;    
        }    
    }

// -----------------------------------------------------------------------------
// CPhoneRingingTone::RefreshMime
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CPhoneRingingTone::RefreshMime()
    {
    TRAPD( err, RefreshMimeL() );
    return err;        
    }

// -----------------------------------------------------------------------------
// CPhoneRingingTone::RefreshMimeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//    
void CPhoneRingingTone::RefreshMimeL()
    {
    RApaLsSession apaLsSession;
    User::LeaveIfError( apaLsSession.Connect() );
    CleanupClosePushL( apaLsSession );

    TUid dummyUid = { 0 };
    TDataType dataType( dummyUid );
    
    User::LeaveIfError(
        apaLsSession.AppForDocument( *iFileName, dummyUid, dataType ) );
        
    CleanupStack::PopAndDestroy(); // CleanupClosePushL
    
    delete iMimeType;
    iMimeType = NULL;
    iMimeType = dataType.Des().AllocL();
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TBool CPhoneRingingTone::CheckAndHandleToneSizeLimit()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtone::CheckAndHandleToneSizeLimit()" );
    
    TBool bValidSize = ETrue;
 
    // If user has somehow managed to get a too large file as ringing tone,
    // play default tone instead.
    if ( iToneFileSizeLimitKB )
        {
        if ( CheckToneFileSize( FileName(), iToneFileSizeLimitKB) != KErrNone )
            {
            bValidSize = EFalse;
            }            
        }
    __PHONELOG1( 
        EBasic,
        EPhoneControl, 
        "CPhoneRingingtone::CheckAndHandleToneSizeLimit - bValidSize(%d)",
        bValidSize);
        
    return bValidSize;
    }
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CPhoneRingingTone::CheckToneFileSize( const TDesC& aFile, 
                                           TInt aSizeLimitKB )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtone::CheckToneFileSize()" );
    //return KErrNone;
    
    // Get file size
    TInt size = 0;

    RFs fs;   
    TInt error = fs.Connect();
    TEntry entry;
    if ( KErrNone == error )
        {
        if (KErrNone == fs.Entry( aFile, entry ))
            {
            size = entry.iSize;
            }    
            
        // Check
        aSizeLimitKB *= Kkilo;
        if ( aSizeLimitKB  &&  size > aSizeLimitKB )
            {
            error = KErrTooBig;
            }
        }

    fs.Close();
    __PHONELOG1( 
        EBasic,
        EPhoneControl, 
        "CPhoneRingingtonePlayer::CheckToneFileSize - size (%d)",
        size );    
    return error;
   
    }


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CPhoneRingingTone::GetMaxToneFileSize()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneRingingtone::GetMaxToneFileSize()" );              
    iToneFileSizeLimitKB = KPhoneToneFileSizeLimitKB;   
    }

//  End of File  
