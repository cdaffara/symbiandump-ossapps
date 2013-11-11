/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Surface container interface for the SimpleVideoPlayback MPX
*               view plugin.
*
*/

#ifndef SVPBSURFACECONTAINER_H_
#define SVPBSURFACECONTAINER_H_

#include <mmf/common/mmfvideo.h>
#include <graphics/surface.h>

class SvpbSurfaceContainer
{
public:
    virtual int attachSurface(
        const TSurfaceId &surfaceId,
        const TRect &cropRect,
        const TVideoAspectRatio &aspectRatio) = 0;
    virtual void detachSurface() = 0;
};

#endif /* SVPBSURFACECONTAINER_H_ */
