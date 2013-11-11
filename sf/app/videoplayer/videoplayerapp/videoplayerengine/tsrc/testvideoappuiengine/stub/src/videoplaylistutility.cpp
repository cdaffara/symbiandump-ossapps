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
* Description:  video playlist utility class stub for unit testing
*
*/

// Version : %version: 2 %

#include <mpxmedia.h>

#include "videoplaylistutility.h"
#include "mpxvideo_debug.h"

CVideoPlaylistUtility::CVideoPlaylistUtility()
{    
    MPX_DEBUG(_L("CVideoPlaylistUtility::CVideoPlaylistUtility()"));
}

CVideoPlaylistUtility::~CVideoPlaylistUtility()
{
    MPX_DEBUG(_L("CVideoPlaylistUtility::~CVideoPlaylistUtility()"));
}	

CVideoPlaylistUtility* CVideoPlaylistUtility::NewL()
{
    MPX_DEBUG(_L("CVideoPlaylistUtility::NewL()"));
    
    CVideoPlaylistUtility* p = new CVideoPlaylistUtility();
    return 	p;
}

void CVideoPlaylistUtility::GetFileInfoL( RFile& /* aFile */, 
                                          CMediaRecognizer::TMediaType /* aMediaType */,
                                          TBool& aSingleLink, 
                                          TBool& aLocalFile)
{
    MPX_DEBUG(_L("CVideoPlaylistUtility::GetFileInfoL( RFile )"));
    
    aSingleLink = ETrue;
    aLocalFile = EFalse;            	
}
                                                      
void CVideoPlaylistUtility::GetFileInfoL( const TDesC& /* aFileName */, 
                                          CMediaRecognizer::TMediaType /* aMediaType */,
                                          TBool& aSingleLink, 
                                          TBool& aLocalFile)
{
    MPX_DEBUG(_L("CVideoPlaylistUtility::GetFileInfoL( aFileName )"));
    
    aSingleLink = ETrue;
    aLocalFile = EFalse;            	
}                                                        
                                                        
                                                        

CMPXMedia* CVideoPlaylistUtility::GetPlayListL( TInt /* aAccessPointId */)
{    
    MPX_DEBUG(_L("CVideoPlaylistUtility::GetPlayListL()"));

    CMPXMedia* playlist = NULL;        
    return playlist;
}


TBool CVideoPlaylistUtility::IsSeekable()
{
    MPX_DEBUG(_L("CVideoPlaylistUtility::IsSeekable()"));
    return ETrue;
}


TPtrC CVideoPlaylistUtility::GetLinkLC()
{
    MPX_DEBUG(_L("CVideoPlaylistUtility::GetLinkLC()"));
    
    _LIT(KTestRtspUrl, "rtsp://testserver/test.3gp"); 
    
    TBool localUrl;
    HBufC* tempBuffer = HBufC::NewLC( 255 );
  
    TPtr tempurl(tempBuffer->Des());

    tempurl.Append(KTestRtspUrl);
            
    return tempurl;               
}
