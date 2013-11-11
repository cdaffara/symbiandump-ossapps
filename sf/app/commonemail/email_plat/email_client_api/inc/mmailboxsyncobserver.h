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
* Description: Observer interface for mailbox synchronisation.
*
*/

#ifndef MEMAILSYNCOBSERVER_H
#define MEMAILSYNCOBSERVER_H

#include <emailapidefs.h>

namespace EmailInterface {

class MMailboxSyncObserver
{
public:
    virtual void MailboxSynchronisedL( TInt aResult ) = 0; 
};

} // namespace EmailInterface

#endif // MEMAILSYNCOBSERVER_H
