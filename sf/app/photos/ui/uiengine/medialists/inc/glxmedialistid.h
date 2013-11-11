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
* Description:    Media item list unique id
*
*/




#ifndef T_GLXMEDIALISTID_H
#define T_GLXMEDIALISTID_H

#include <e32std.h>

#include <glxid.h>

/**
 *  TGlxMediaListId
 *
 *  Type safe id class for media list reader id. 
 *
 *  @lib none
 */
class TGlxIdMediaListBase {}; // Don't use this. This is just to make TGlxId type-safe
typedef TGlxId<TGlxIdMediaListBase> TGlxMediaListId; // Use this

#endif // T_GLXMEDIALISTID_H

