/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#ifndef MIPSPLGMAILSTOREROBSERVER_H
#define MIPSPLGMAILSTOREROBSERVER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include "CFSMailCommon.h"

class MIpsPlgMailStorerObserver
 	{
    public:

     virtual void StoringCompleteL( TInt aRetCode ) = 0;
     };

#endif  // MIPSPLGMAILSTOREROBSERVER_H

// End of File
