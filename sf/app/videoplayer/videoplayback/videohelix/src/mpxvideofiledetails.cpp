/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  class for MPX Video File Details
*
*/

// Version : %version: 11 %



//
//  INCLUDE FILES
//
#include "mpxvideo_debug.h"
#include "mpxvideofiledetails.h"

// ============================ MEMBER FUNCTIONS ===============================

CMPXVideoFileDetails*
CMPXVideoFileDetails::NewL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoFileDetails::NewL()"));

    CMPXVideoFileDetails* p = new (ELeave) CMPXVideoFileDetails();
    CleanupStack::PushL(p);
    p->ConstructL();
    CleanupStack::Pop(p);
    return p;
}

CMPXVideoFileDetails::~CMPXVideoFileDetails()
{
    MPX_ENTER_EXIT(_L("CMPXVideoFileDetails::~CMPXVideoFileDetails()"));

    ClearFileDetails();
}

void
CMPXVideoFileDetails::ConstructL()
{
    MPX_ENTER_EXIT(_L("CMPXVideoFileDetails::ConstructL()"));
    iSeekable = ETrue;
    iPausableStream = ETrue;
}

void
CMPXVideoFileDetails::ClearFileDetails()
{
    MPX_ENTER_EXIT(_L("CMPXVideoFileDetails::ClearFileDetails()"));

    if ( iMimeType )
    {
        delete iMimeType;
        iMimeType = NULL;
    }

    if ( iTitle )
    {
        delete iTitle;
        iTitle = NULL;
    }

    if ( iArtist )
    {
        delete iArtist;
        iArtist = NULL;
    }

    if ( iClipName )
    {
        delete iClipName;
        iClipName = NULL;
    }

    if ( iDescription )
    {
        delete iDescription;
        iDescription = NULL;
    }

    if ( iLocation )
    {
        delete iLocation;
        iLocation = NULL;
    }

    if ( iCopyright )
    {
        delete iCopyright;
        iCopyright = NULL;
    }

    if ( iLanguage )
    {
        delete iLanguage;
        iLanguage = NULL;
    }
    
    if ( iKeywords )
    {
        delete iKeywords;
        iKeywords = NULL;
    }
        
    iResolutionWidth = 0;
    iResolutionHeight = 0;
    iMaxVolume = 0;
	iBitRate = 0;
    iSeekable = ETrue;
    iPausableStream = ETrue;
    iLiveStream = 0;
    iAudioEnabled = 0;
    iVideoEnabled = 0;
    iDrmProtected = EFalse;
    iFourCCCode = 0;

    iDuration = 0;
    iCreationTime = 0;
    iModificationTime = 0;
}

//  EOF
