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
* Description:  Inline methods of CIpsPlgMsgMapper class
*
*/


#include "ipsplgmsgmapper.h"    // for Lint

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
inline TBool CIpsPlgMsgMapper::LogicalXor( TInt aLeft, TInt aRight )
    {
		return ( ( aLeft || aRight ) && ( !aLeft || !aRight ) );
    }
