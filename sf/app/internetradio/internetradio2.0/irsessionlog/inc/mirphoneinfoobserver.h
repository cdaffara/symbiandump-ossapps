/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  The implementation for presentation elements.
*
*/


#ifndef MIRPHONEINFOOBSERVER_H
#define MIRPHONEINFOOBSERVER_H

class MIRPhoneInfoObserver
	{
	
public:
	
	virtual void ImeiUpdatedL(const TDesC& aImei) = 0;
	
	};


#endif //MIRPHONEINFOOBSERVER_H
