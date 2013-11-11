/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Panic codes for IPS Plugin for debugging purposes
*
*/

#ifndef IPSPLGPANIC_H
#define IPSPLGPANIC_H

#include <e32cmn.h>
#include <e32def.h>

_LIT(KIpsPlgPanicCategory, "IpsPlgSos");
// <qmail>
_LIT( KIpsPlgIpsConnPanic, "IpsConn" );
_LIT( KIpsPlgPopConnectPanic, "PopConnectOp" );
// </qmail>

enum TIpsPlgPanic {
    EIpsPlgInvalidMessageIndex,
    EIpsPlgNotAService,
    EIpsPlgIsNotRoot,
    EIpsPlgInvalidEntry,
    EIpsPlgNoParameters,
    EIpsPlgNullReturn,
};

#endif // IPSPLGPANIC_H
