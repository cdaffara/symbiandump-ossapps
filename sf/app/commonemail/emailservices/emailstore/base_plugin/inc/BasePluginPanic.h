/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Panic definitions.
*
*/


#ifndef __BASEPLUGINPANIC_H__
#define __BASEPLUGINPANIC_H__

#include <e32std.h>

/**
 *
 */
enum TBasePluginPanic
   {
   EFetchMessagesAssert     = 1,
   //the mru list does not have valid count of elements.
   ESetMrusInvalidAssert    = 2,
   //no msgstore session created.
   ENoMsgStoreSessionAssert = 3,
   //the msgstore cache is in invalid state.
   EInvalidMsgStoreCacheState = 4
   };

/**
 *
 */
GLREF_C void BasePluginPanic( TBasePluginPanic aPanic );

#endif //__BASEPLUGINPANIC_H__
