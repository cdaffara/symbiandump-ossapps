// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implementation of the TArithmeticalDate class.
// 
//

// User includes
#include "calconv.h"

//
// Construction/Destruction
//

//------------------------------------------------------
// Class:       TArithmeticalDate
// Function:    TArithmeticalDate
// Arguments:   None
//
// Comments:    Constructor
//
// Return:      None
//------------------------------------------------------
EXPORT_C TArithmeticalDate::TArithmeticalDate()
	{
	}

//------------------------------------------------------
// Class:       TArithmeticalDate
// Function:    operator ==
// Arguments:   const TArithmeticalDate &
//
// Comments:    overloaded == operator
//
// Return:      TBool - ETrue if both the TArithmeticalDate 
//				classes are the same
//------------------------------------------------------
EXPORT_C TBool TArithmeticalDate::operator ==(const TArithmeticalDate &aDate) const
	{
	TBool rtn = ETrue;
	if(iYear != aDate.iYear)
		{
		rtn = EFalse;
		}
	if(iMonth != aDate.iMonth)
		{
		rtn = EFalse;
		}
	if(iDay != aDate.iDay)
		{
		rtn = EFalse;
		}
	return rtn;
	}
