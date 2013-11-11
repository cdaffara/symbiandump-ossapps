
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
* Description:  Helper class for creating mediaobjects for testing CVideoListDataModel class methods*
*/

// INCLUDES
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxmediaarray.h>
#include <mpxmedia.h>
#include <qdatetime.h>
#include "vcxmyvideosdefs.h"
#include "mediaobjectfactory.h"

// -----------------------------------------------------------------------------
// MediaObjectFactory
// -----------------------------------------------------------------------------
//
MediaObjectFactory::MediaObjectFactory()
{
    
}

// -----------------------------------------------------------------------------
// ~CMediaObjectFactory
// -----------------------------------------------------------------------------
//
MediaObjectFactory::~MediaObjectFactory()
{
}
 
// -----------------------------------------------------------------------------
// newMediaArray
// -----------------------------------------------------------------------------
//
CMPXMediaArray* MediaObjectFactory::newMediaArray()
{
    CMPXMediaArray *array = 0;
    TRAP_IGNORE(array = CMPXMediaArray::NewL());
    return array;
}


// -----------------------------------------------------------------------------
// newMedia
// -----------------------------------------------------------------------------
//
CMPXMedia* MediaObjectFactory::newMedia(int id1, int id2)
{
    CMPXMedia *media = NULL;
    TRAP_IGNORE(media = CMPXMedia::NewL());
    if(!media)
    {
        return 0;
    }
    if(id1 == -1)
    {
        return media;
    }
    TMPXItemId itemId;
    
    itemId.iId1 = id1;
    itemId.iId2 = id2;
    TRAPD(error, media->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, itemId));
    if(error != KErrNone)
    {
        delete media;
        return 0;
    }

    return media;
    
}

// -----------------------------------------------------------------------------
// addValuePtr
// -----------------------------------------------------------------------------
//
void MediaObjectFactory::putArrayContent(CMPXMediaArray *whereToPut, CMPXMedia *mediaToPut)
{
    TRAP_IGNORE(whereToPut->AppendL(mediaToPut));
}


// End of file
    


