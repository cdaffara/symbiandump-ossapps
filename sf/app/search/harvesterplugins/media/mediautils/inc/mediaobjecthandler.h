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
* Description:  media object handler callbacks
 *
*/


#ifndef MEDIAOBJECTHANDLER_H_
#define MEDIAOBJECTHANDLER_H_
#include <mdccommon.h>
#include "common.h"

class MdeMEdiaObserver
   {
public:
   /*
    * HarvestingCompletedL signals when harvesting completed
    */
   virtual void HarvestingCompletedL() = 0;
   };

class MMediaObjectHandler
    {
public:
    /*
     * HandleMdeItemL callback to notify for Mde item
     * @param TItemId aObjId object id of item
     * @param TCPixActionType aActionType cpix action type
     */
    virtual void HandleMdeItemL( TItemId aObjId, TCPixActionType aActionType) = 0;
#ifdef __PERFORMANCE_DATA
    virtual void UpdateLogL() = 0;
# endif	
    };

#endif /* MEDIAOBJECTHANDLER_H_ */
