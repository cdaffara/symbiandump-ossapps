// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the TChineseDate class.
// 
//

// User includes
#include <calendarconverter.h>

//
// Construction/Destruction
//

//------------------------------------------------------
// Class:       TChineseDate
// Function:    TChineseDate
// Arguments:   None
//
// Comments:    Constructor
//
// Return:      None
//------------------------------------------------------
EXPORT_C TChineseDate::TChineseDate()
/** Default C++ constructor. */
	{
	}


//------------------------------------------------------
// Class:       TChineseDate
// Function:    operator==
// Arguments:   const TChineseDate &
//
// Comments:    overloaded == operator
//
// Return:      TBool - ETrue if the TChineseDate classes
//				are the same
//------------------------------------------------------
EXPORT_C TBool TChineseDate::operator==(const TChineseDate &aDate) const
/** Tests whether two objects of this class are equivalent. 

@param aDate An object of this class.
@return ETrue: the objects are equivalent. */
	{
	TBool rtn = ETrue;

	if(iCycle != aDate.iCycle)
		{
		rtn = EFalse;
		}
	if(iYear != aDate.iYear)
		{
		rtn = EFalse;
		}
	if(iMonth != aDate.iMonth)
		{
		rtn = EFalse;
		}
	if(iLeapMonth != aDate.iLeapMonth)
		{
		rtn = EFalse;
		}
	if(iDay != aDate.iDay)
		{
		rtn = EFalse;
		}

	return rtn;
	}
