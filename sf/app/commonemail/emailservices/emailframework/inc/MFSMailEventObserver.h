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
* Description:  defines framework event observer
*
*/


#ifndef MFSMAILEVENTOBSERVER_H
#define MFSMAILEVENTOBSERVER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
//<cmail>
#include "CFSMailCommon.h"
//</cmail>

/**
 *  email framework event observer
 *
 *  @lib FSFWCommonLib
 *  @since S60 v3.1
 */
 class MFSMailEventObserver
    {
    public:

    /**
     * event from plugin
     *
     * @param aEvent one of pre-defined events in TFSMailEvent
     * @param aMailbox maibox related for this event
     * note : if mailbox id is null id, event is plugin level ie. corcerns all
     * mailboxes
     * @param aParam1 intepretation is event specific (see TFSMailEvent descriptions)
     * @param aParam2 intepretation is event specific (see TFSMailEvent descriptions)
     * @param aParam3 intepretation is event specific (see TFSMailEvent descriptions)
     */
     virtual void EventL(TFSMailEvent aEvent, TFSMailMsgId aMailbox, TAny* aParam1, TAny* aParam2, TAny* aParam3) = 0;

     };

#endif  // MFSMAILEVENTOBSERVER_H

// End of File
