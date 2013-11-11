/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  defines framework request observer
*
*/


#ifndef MFSMAILREQUESTOBSERVER_H
#define MFSMAILREQUESTOBSERVER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>

//<cmail>
#include "CFSMailCommon.h"
#include "CFSMailMessage.h"
//</cmail>

/**
 * freestyle email framework plugin response observer
 * for async requests and sync status
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
class MFSMailRequestObserver
    {
    public:

    /**
     * asynchronous request response message
     *
     * @param aEvent plugin event description
     * @param aRequestId request id of asyncronous operation
     */
     virtual void RequestResponseL( TFSProgress aEvent, TInt aRequestId ) = 0;
    
    };

#endif  // MFSMAILREQUESTOBSERVER_H

// End of File
