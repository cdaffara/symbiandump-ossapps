/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/
#ifndef ICPIXSEARCHEROBSERVER_
#define ICPIXSEARCHEROBSERVER_

#include "INPSearchDocument.h"


struct NPSearchDocumentObject;
struct NPSearchTermListObject;

/**
 * MSearchObserver
 * Mixin class.
 * Observer which handles search results received from the server.
 * 
 * This interface represents a JavaScript object provided
 * by a searcher client. Calling provided methods will invoke 
 * corresponding methods in the JavaScript object. 
 */
class MCPixNPSearcherObserver 
	{
public:
	
	/**
	 * Callback method of ICPixNPSearcher::SearchL
	 * @param aError Description of error, may be NULL
	 */
    virtual void HandleSearchResultsL(const TDesC8* aError, TInt aEstimatedResultCount) = 0;
    
    /**
	 * Callback method of ICPixNPSearcher::GetDocumentL
	 * @param aError Description of error, may be NULL
	 * @param aDocument Next document, may be NULL
     */
    virtual void HandleDocumentL(const TDesC8* aError, NPSearchDocumentObject* aDocument) = 0;
	};

#endif /*ICPIXSEARCHEROBSERVER_*/
