/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares CUPFileSharingActive class
*
*/


#ifndef CUPFILESHARINGACTIVE_H
#define CUPFILESHARINGACTIVE_H

#include <e32std.h>
#include <e32base.h>

class CUpnpFileSharing;
class CUpnpItem;

class CUPFileSharingActive : public CActive
{
	
public: 

	static CUPFileSharingActive* NewL();
	
	~CUPFileSharingActive();
	
private: // Constructors

	CUPFileSharingActive();
	
	void ConstructL();

protected: // from CActive

	void RunL();
	
	void DoCancel();
	
	TInt RunError( TInt aError );
	
public: // New Functions

	void ShareItemL( CUpnpItem& aItem );
	
	void UnshareItemL( TInt aId );

	void GetSharedItemL( TInt aId, CUpnpItem& aItem );
		
private: // data

	CUpnpFileSharing* iFileSharing;
	
};

#endif // CUPFILESHARINGACTIVE_H

//  End of File
