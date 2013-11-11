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
* Description:  Defines EMail Framework mailbox search API observer
*
*/


#ifndef MFSMAILBOXSEARCHOBSERVER_H
#define MFSMAILBOXSEARCHOBSERVER_H

//  INCLUDES
#include <e32base.h>

//<cmail>
#include "CFSMailMessage.h"
//</cmail>

/**
 * Freestyle email framework search API observer
 *
 */
class MFSMailBoxSearchObserver
    {
 public:

   /** 
    * Notifies the email search API client that a match has been found
    * 
    * @param aMatchMessage contains a pointer to the matched message.
    *         Ownership is transfered to the observer.
    *
    */
    virtual void MatchFoundL( CFSMailMessage* aMatchMessage ) = 0;

   /**
    * Notifies the email search API client that the search has completed
    *
    */
    virtual void SearchCompletedL() = 0;
      
//</cmail>
    /**
    * server asks client if to change the search priority (i.e. when calling to contact) 
    *
    */
    virtual void ClientRequiredSearchPriority(TInt *apRequiredSearchPriority) = 0; 
//</cmail>
    };

#endif  // MFSMAILBOXSEARCHOBSERVER_H

// End of File
