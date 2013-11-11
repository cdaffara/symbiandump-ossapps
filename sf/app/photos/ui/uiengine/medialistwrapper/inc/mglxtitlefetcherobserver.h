/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Title fetcher observer 
*
*/




#ifndef MGLXTITLEFETCHEROBSERVER_H
#define MGLXTITLEFETCHEROBSERVER_H

#include <e32base.h>
//#include <coedef.h>
//#include <w32std.h>

/**
 * MGlxTitleFetcherObserver
 * 
 * Title fetcher interface
 */
class MGlxTitleFetcherObserver
	{
public:
	/**
	 * Handle the title
	 * @param aTitle The title
	 */
	virtual void HandleTitleAvailableL(const TDesC& aTitle) = 0;
	
	};
	



#endif // MGLXTITLEFETCHEROBSERVER_H
