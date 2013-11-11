/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Implementation of CGlxtnFileInfo
*
*/


#include "glxtnfileinfo.h"

#include <glxtracer.h>
// -----------------------------------------------------------------------------
// IdentifyFileL
// -----------------------------------------------------------------------------
//
void CGlxtnFileInfo::IdentifyFileL(TBool& aIsVideo, TBool& aIsProtected)
    {
    TRACER("void CGlxtnFileInfo::IdentifyFileL()");
    User::LeaveIfNull(iFilePath);

    aIsVideo = iIsVideo;
    aIsProtected = iIsProtected;
    }
