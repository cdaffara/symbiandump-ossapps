#ifndef ICALCOMPONENT_H
#define ICALCOMPONENT_H/*
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
* Description:   Holds the definition of CICalComponent.
*
*/



// User includes.
#include "ICalBase.h"	// CICalBase

/**
Abstract class of classes representing specific iCalendar components.
@publishedPartner
*/
class CICalComponent : public CICalBase
	{
public:	// Construction/destruction.
	~CICalComponent();
		
	// Factory construction methods for concrete derived classes.
	IMPORT_C static CICalComponent* CreateICalComponentL(const TDesC& aName, TICalMethod aMethod);
	IMPORT_C static CICalComponent* CreateICalComponentLC(const TDesC& aName, TICalMethod aMethod);
		
	IMPORT_C static CICalComponent* CreateICalComponentL(CICalBase::TICalComponentType aType, TICalMethod aMethod);
	IMPORT_C static CICalComponent* CreateICalComponentLC(CICalBase::TICalComponentType aType, TICalMethod aMethod);
		
protected:	// Construction.
	CICalComponent();	
	};
	
#endif // ICALCOMPONENT_H

// End of File
