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
* Description:   Media list hierarchy id
*
*/



#ifndef T_GLXHIERARCHYID_H
#define T_GLXHIERARCHYID_H

#include <e32base.h>

#include "glxid.h"

/**
 *  TGlxIdHierarchyBase
 *
 *  Don't use this. This is just to make TGlxId type-safe for TGlxHierarchyId
 *
 *  @lib none
 */
class TGlxIdHierarchyBase {}; 

/**
 *  TGlxHierarchyId
 *
 *  Typesafe Hierarchy identifier.
 *
 *  @lib none
 */
typedef TGlxId<TGlxIdHierarchyBase> TGlxHierarchyId;

#endif // T_GLXHIERARCHYID_H
