/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Profile item
*
*/


#include "nsmldmsyncprofileitem.h"
#include "nsmldmsyncdebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TNSmlDMSyncProfileItem::Id
// -----------------------------------------------------------------------------
//
TInt TNSmlDMSyncProfileItem::Id()
	{
	FLOG( "[OMADM] TNSmlDMSyncProfileItem::Id:" );
	
	return iProfileId;
	}

// -----------------------------------------------------------------------------
// TNSmlDMSyncProfileItem::SetId
// -----------------------------------------------------------------------------
//
void TNSmlDMSyncProfileItem::SetId(TInt aId)
	{
	FLOG( "[OMADM] TNSmlDMSyncProfileItem::SetId:" );
	
	iProfileId = aId;
	}

// -----------------------------------------------------------------------------
// TNSmlDMSyncProfileItem::Name
// -----------------------------------------------------------------------------
//
const TDesC& TNSmlDMSyncProfileItem::Name()
	{
	FLOG( "[OMADM] TNSmlDMSyncProfileItem::Name:" );
	
	return iProfileName;
	}

// -----------------------------------------------------------------------------
// TNSmlDMSyncProfileItem::SetName
// -----------------------------------------------------------------------------
//
void TNSmlDMSyncProfileItem::SetName( const TDesC& aName )
	{
	FLOG( "[OMADM] TNSmlDMSyncProfileItem::SetName:" );
	
	iProfileName.Copy( aName.Left( iProfileName.MaxLength() ) );
	}

// -----------------------------------------------------------------------------
// TNSmlDMSyncProfileItem::CompareItem
// -----------------------------------------------------------------------------
//
TInt TNSmlDMSyncProfileItem::CompareItem( 
                                        const TNSmlDMSyncProfileItem& aFirst,
                                        const TNSmlDMSyncProfileItem& aSecond )
	{
	FLOG( "[OMADM] TNSmlDMSyncProfileItem::CompareItem:" );
	
	return aFirst.iProfileName.Compare( aSecond.iProfileName );
	}

// End of File
