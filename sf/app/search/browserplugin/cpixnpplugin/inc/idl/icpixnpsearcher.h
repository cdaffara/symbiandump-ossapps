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
#ifndef ICPIXNPSEARCHER_H
#define ICPIXNPSEARCHER_H

#include "ICPixNPSearcherObserver.h"

/**
 * An instance of CCPixSearcher is used to commit search operations to CSearchServer
 * Is bound to a specific database identified by baseAppType
 */
class MCPixNPSearcher
    {
public: 

    /**
    * SetObserver
    * Sets observer for this searcher
    * @param aObserver A observer of this CCPixSearcher instance.
    */
	virtual void SetObserverL( NPObject* aObserver ) = 0; 

    /**
    * SetAnalyzer
    * Sets analyzer for this searcher
    * @param aAnalyzer Special syntax defining the analyzer
    */
	virtual void SetAnalyzerL( const TDesC& aAnalyzer ) = 0; 

    /**
    * SearchL
    * Issues a search request. When request has been completed a callback to 
    * MSearchObserver::HandleSearchResultL is issued. Leaves with KErrInUse, 
    * if asynchronous request is already pending.
    * @param aSearchTerms Search terms. May include wild card character. 
    * @param aDocumentField to search
    */
	virtual void SearchL( const TDesC& aSearchTerms, const TDesC& aDocumentField = KNullDesC ) = 0;

    /**
     * GetDocumentL
     * Iterates throught the search result's document list (hits), that is stored in the 
	 * server side. The accessed list is ranked and the best matching document is heading 
	 * the result lest. First call of this method, return best matching, then second best 
	 * matching and so forth until all documents have been found. Method returns NULL, 
	 * when no more documents can be found.
	 * 
     * Leaves with KErrInUse, if asynchronous request is pending and 
	 * KErrNotReady, if no database has been succefullly opened.
	 */
	virtual void GetDocumentL(TInt aIndex) = 0;
	
    /**
     * Cancels any outstanding operation.
     */
	virtual void Cancel() = 0;

    /**
     * Checks whether an outstanding request is still pending
     */
	virtual TBool IsActive() = 0;

    };

#endif /*ICPIXNPSEARCHER_*/
