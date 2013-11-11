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
//

#ifndef __CALENDARCONVERTERPLUGIN_H__
#define __CALENDARCONVERTERPLUGIN_H__

#include <calendarconverter.h>
#include <ecom/ecom.h> 

/**
The UID of the calendar converter ECom plugin interface.
@publishedPartner
@released
*/
const TUid KUidEcomChineseCalendarConverterInterface = { 0x1028702B };

/**
@publishedPartner
@released
*/
class CChineseCalendarConverterEcomPlugin : public CChineseCalendarConverter
	{
public:
	static CChineseCalendarConverter* NewL();	

protected:
	inline virtual ~CChineseCalendarConverterEcomPlugin();

private:
	TUid iDtor_ID_Key; 
	};

/**
The destruction of the interface implementation referred to by iDtor_ID_Key 
@publishedPartner
@released
*/
inline CChineseCalendarConverterEcomPlugin::~CChineseCalendarConverterEcomPlugin()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

#endif
