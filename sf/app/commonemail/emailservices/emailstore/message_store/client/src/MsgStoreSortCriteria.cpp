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
* Description:  Message store sort criteria client implementation.
*
*/



//<cmail>
#include "MsgStoreSortCriteria.h"
//</cmail>

// --------------------------
// RMsgStoreAddress
// --------------------------

// ==========================================================================
// FUNCTION: Close
// ==========================================================================
EXPORT_C void RMsgStoreSortCriteria::Close()
    {
    iPropertyKeys.ResetAndDestroy();
    }

// ==========================================================================
// FUNCTION: AddResultPropertyL
// ==========================================================================
EXPORT_C void RMsgStoreSortCriteria::AddResultPropertyL( const TDesC8& aPropertyKey )
    {
    iPropertyKeys.Append( aPropertyKey.AllocL() );
    }
