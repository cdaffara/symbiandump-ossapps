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
* Description:  Message store search criteria client implementation.
*
*/

#include "MsgStoreSearchCriteria.h"

// --------------------------
// RMsgStoreAddress
// --------------------------

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
EXPORT_C RMsgStoreSearchCriteria::RMsgStoreSearchCriteria()
    {
    iSearchFields = ( EMsgStoreSearchTo | EMsgStoreSearchCc | EMsgStoreSearchBcc | EMsgStoreSearchSubject | EMsgStoreSearchBody );
    iSortBy = EMsgStoreSortByReceivedDate;  
    iSortOrder = EMsgStoreSortDescending;
    iSecondarySortOrder = EMsgStoreSortDescending;
    }
    
// ==========================================================================
// FUNCTION: Close
// ==========================================================================
EXPORT_C void RMsgStoreSearchCriteria::Close()
    {
    iFolderIds.Close();
    iSearchStrings.ResetAndDestroy();
    iPropertyKeys.ResetAndDestroy();
    }

// ==========================================================================
// FUNCTION: AddSearchStringL
// ==========================================================================
EXPORT_C void RMsgStoreSearchCriteria::AddSearchStringL( const TDesC& aSearchString )
    {
    iSearchStrings.Append( aSearchString.AllocL() );
    }

// ==========================================================================
// FUNCTION: AddFolderId
// ==========================================================================
EXPORT_C void RMsgStoreSearchCriteria::AddFolderId( TMsgStoreId aFolderId )
    {
    iFolderIds.Append( aFolderId );
    }
        
// ==========================================================================
// FUNCTION: AddResultPropertyL
// ==========================================================================
EXPORT_C void RMsgStoreSearchCriteria::AddResultPropertyL( const TDesC8& aPropertyKey )
    {
    iPropertyKeys.Append( aPropertyKey.AllocL() );
    }

