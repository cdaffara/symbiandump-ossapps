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
* Description:  Message store session context client implementation.
*
*/

#include "MsgStoreSessionContext.h"

// ==========================================================================
// FUNCTION: Constructor
// ==========================================================================
CMsgStoreSessionContext::CMsgStoreSessionContext()
    {
    __LOG_CONSTRUCT( "MsgClient", "CMsgStoreSessionContext" )
    } // end Constructor
    
// ==========================================================================
// FUNCTION: Destructor
// ==========================================================================
CMsgStoreSessionContext::~CMsgStoreSessionContext()
    {
    __LOG_DESTRUCT
    } // end Destructor
        
// ==========================================================================
// FUNCTION: VerifyTypeL
// ==========================================================================
void CMsgStoreSessionContext::VerifyTypeL( TMsgStoreId aId, TMsgStoreId aType )
	{
	__LOG_ENTER_SUPPRESS( "VerifyTypeL" )
	
	if( (aId & EMsgStoreContainerMask) != aType && aId != KMsgStoreInvalidId )
		{
		__LOG_WRITE8_FORMAT2_ERROR( "type mismatch, id=%08x expectedtype=%08x", aId, aType );
		User::Leave( KErrArgument );
		} // end if
		
	} // end VerifyType

// ==========================================================================
// FUNCTION: VerifyTypeL, check if aId is either aType1 or aType2
// ==========================================================================
void CMsgStoreSessionContext::VerifyTypeL( TMsgStoreId aId, TMsgStoreId aType1, TMsgStoreId aType2 )
	{
	__LOG_ENTER_SUPPRESS( "VerifyType2L" )
	
	TRAPD( err, VerifyTypeL(aId, aType1) )
	if ( err == KErrArgument )
		{
		VerifyTypeL(aId, aType2);
		}
	} // end VerifyType
