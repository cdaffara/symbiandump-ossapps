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
* Description:  Inline implementation of MPXVideoRegion
*
*/

// Version : %version: 3 %



#include <e32std.h>
#include <s32strm.h>

#include "mpxvideoregion.h"
#include "mpxvideo_debug.h"

CMPXVideoRegion*
CMPXVideoRegion::NewL()
{
    CMPXVideoRegion* self = CMPXVideoRegion::NewLC();
    CleanupStack::Pop( self );
    return self;
}

CMPXVideoRegion*
CMPXVideoRegion::NewLC()
{
    CMPXVideoRegion* self = new (ELeave) CMPXVideoRegion();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}

void
CMPXVideoRegion::ConstructL()
{
    iRegion = new RRegion( 10 );
}

CMPXVideoRegion::CMPXVideoRegion()
{
}

CMPXVideoRegion::~CMPXVideoRegion()
{
    iRegion->Destroy();
}

void
CMPXVideoRegion::Print()
{
    if ( iRegion->CheckError() )
    {
        MPX_DEBUG(_L("CMPXVideoRegion::Print() Error in DSA Region"));
        return;
    }

    MPX_DEBUG(_L("CMPXVideoRegion::Print() Rect Count = %d"),
        iRegion->Count() );

    const TRect* rectList = iRegion->RectangleList();

    for ( TInt i = 0 ; i < iRegion->Count() ; ++i )
    {
        MPX_DEBUG(_L("CMPXVideoRegion::Print() Rect %d"), i);

        MPX_DEBUG(_L( "CMPXVideoRegion::Print() (%d, %d), (%d, %d)"),
            rectList[i].iTl.iX, rectList[i].iTl.iY,
            rectList[i].iBr.iX, rectList[i].iBr.iY );
    }
}

void
CMPXVideoRegion::SetRegion( RRegion& aRegion )
{
    iRegion->Copy( aRegion );
}

TRegion&
CMPXVideoRegion::GetRegion()
{
    return *iRegion;
}

void
CMPXVideoRegion::ExternalizeL( RWriteStream& aStream ) const
{
    MPX_ENTER_EXIT(_L("CMPXVideoRegion::ExternalizeL()"));

    aStream.WriteInt32L( iRegion->Count() );

    const TRect* rectList = iRegion->RectangleList();

    for ( TInt i = 0 ; i < iRegion->Count() ; ++i )
    {
        aStream.WriteInt32L( rectList[i].iTl.iX );
        aStream.WriteInt32L( rectList[i].iTl.iY );
        aStream.WriteInt32L( rectList[i].iBr.iX );
        aStream.WriteInt32L( rectList[i].iBr.iY );
    }
}

void
CMPXVideoRegion::InternalizeL( RReadStream& aStream )
{
    MPX_ENTER_EXIT(_L("CMPXVideoRegion::InternalizeL()"));

    TInt32 count = aStream.ReadInt32L();

    for ( TInt i = 0 ; i < count; ++i )
    {
        TInt32 a = aStream.ReadInt32L();
        TInt32 b = aStream.ReadInt32L();
        TInt32 c = aStream.ReadInt32L();
        TInt32 d = aStream.ReadInt32L();

        TRect* rect = new (ELeave) TRect( a, b, c, d );
        iRegion->AddRect( *rect );
    }
}
