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
* Description:  Engine video quality level and container.
*
*/


#include <e32std.h>
#include <e32base.h> 


#include "VideoRecordingQualityLevels.h"


// CONSTANTS
const TInt KQualityLevelArrayGranularity = 3;

CVideoRecordingQualityLevels::CVideoRecordingQualityLevels()
    {
    // TODO Auto-generated constructor stub

    }

CVideoRecordingQualityLevels::~CVideoRecordingQualityLevels()
    {
    delete iQualityLevels;
    }

void CVideoRecordingQualityLevels::ConstructL()
    {

    iQualityLevels = new( ELeave ) CArrayFixFlat<CVideoRecordingQualityLevel>( KQualityLevelArrayGranularity );

    }

CVideoRecordingQualityLevels* CVideoRecordingQualityLevels::NewL()
    {

    CVideoRecordingQualityLevels* self = new( ELeave ) CVideoRecordingQualityLevels;

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );


    return self;
    }

TInt CVideoRecordingQualityLevels::InitDefaultsL()
    {

    // Delete all elements from the array and 
    // free the memory allocated to the array buffer.
    iQualityLevels->Reset();

    // Initialize hardcoded default quality levels.

    TSize size;

    CVideoRecordingQualityLevel* videoQualityLevel = new( ELeave ) CVideoRecordingQualityLevel;
    CleanupStack::PushL( videoQualityLevel );
    
    // Level 0:
    size.SetSize( KImgWidthQCIF, KImgHeightQCIF );
    videoQualityLevel->iFrameSize    = size;
    videoQualityLevel->iFrameRate    = 15.0;  // Frames per second. Basically HW dependent.
    videoQualityLevel->iBitRate      = 64000; // Bits per second. Basically HW dependent.
    videoQualityLevel->iAudioEnabled = EFalse;
    videoQualityLevel->iAudioBitRate = 12200; // Bits per second. Basically HW dependent.
    videoQualityLevel->iMimeType.Copy( KVideoMimeType );
    videoQualityLevel->iPreferredSupplier.Copy( KPreferredSupplier );
    videoQualityLevel->iVideoType.Copy( KVideoType );
    videoQualityLevel->iAudioType.Copy( KAudioType );
    videoQualityLevel->iStorageRate = 78000;  // That many bits per second to store. Estimate only

    iQualityLevels->AppendL( *videoQualityLevel );

    // Level 1:
    size.SetSize( KImgWidthSubQCIF, KImgHeightSubQCIF );
    videoQualityLevel->iFrameSize    = size;
    videoQualityLevel->iFrameRate    = 15.0;  // Frames per second. Basically HW dependent.
    videoQualityLevel->iBitRate      = 60000; // Bits per second. Basically HW dependent.
    videoQualityLevel->iAudioEnabled = EFalse;
    videoQualityLevel->iAudioBitRate = 6700;  // Bits per second. Basically HW dependent.
    videoQualityLevel->iMimeType.Copy( KVideoMimeType );
    videoQualityLevel->iPreferredSupplier.Copy( KPreferredSupplier );
    videoQualityLevel->iVideoType.Copy( KVideoType );
    videoQualityLevel->iAudioType.Copy( KAudioType );
    videoQualityLevel->iStorageRate = 68000;  // That many bits per second to store. Estimate only

    iQualityLevels->AppendL( *videoQualityLevel );

    // Level 2:
    size.SetSize( KImgWidthSubQCIF, KImgHeightSubQCIF );
    videoQualityLevel->iFrameSize    = size;
    videoQualityLevel->iFrameRate    = 5.0;   // Frames per second. Basically HW dependent.
    videoQualityLevel->iBitRate      = 40000; // Bits per second. Basically HW dependent.
    videoQualityLevel->iAudioEnabled = EFalse;
    videoQualityLevel->iAudioBitRate = 5150;  // Bits per second. Basically HW dependent.
    videoQualityLevel->iMimeType.Copy( KVideoMimeType );
    videoQualityLevel->iPreferredSupplier.Copy( KPreferredSupplier );
    videoQualityLevel->iVideoType.Copy( KVideoType );
    videoQualityLevel->iAudioType.Copy( KAudioType );
    videoQualityLevel->iStorageRate = 47000;  // That many bits per second to store. Estimate only

    iQualityLevels->AppendL( *videoQualityLevel );

    CleanupStack::PopAndDestroy( videoQualityLevel );


    return iQualityLevels->Count();
    }


// -----------------------------------------------------------------------------
// CCaeVideoQualityLevelsCont::Count
// -----------------------------------------------------------------------------
//
TInt CVideoRecordingQualityLevels::Count() const
    {
    return iQualityLevels->Count();
    }

        
// -----------------------------------------------------------------------------
// CCaeVideoQualityLevelsCont::At
// -----------------------------------------------------------------------------
//
CVideoRecordingQualityLevel& CVideoRecordingQualityLevels::At( 
    TInt aIndex ) const
    {
    return iQualityLevels->At( aIndex );
    }
