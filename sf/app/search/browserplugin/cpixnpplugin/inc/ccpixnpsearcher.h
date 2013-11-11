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
#ifndef CPIXNPSEARCHER_H_
#define CPIXNPSEARCHER_H_

//  INCLUDES
#include "CCPixNPSearcherInterface.h"
#include "CCPixNPSearcherObserver.h"
#include "MCPixSearcherObserver.h"
#include <RSearchServerSession.h>


// Forward declarations

// CLASS DECLARATION
class CCPixSearcher;

/**
 * CCPixNPSearcher
 * Implements services described in MCPixNPSearcher. Provides
 * the functionality for committing searches into database associated
 * with its base application class. 
 */
class CCPixNPSearcher : public CCPixNPSearcherInterface, MCPixSearchRequestObserver, MCPixNextDocumentRequestObserver 
    {
public:
    static CCPixNPSearcher* NewL( const TDesC& aBaseAppClass, const TDesC& aDefaultSearchField );
    static CCPixNPSearcher* NewL( const TDesC& aBaseAppClass );
    virtual ~CCPixNPSearcher();
    
private:
	CCPixNPSearcher();
	
    void ConstructL( const TDesC& aBaseAppClass, const TDesC& aDefaultSearchField );

    void ConstructL( const TDesC& aBaseAppClass );

    virtual void Invalidate(); 

public: // From MCPixNPSearcher 
	
	virtual void SetObserverL( NPObject* aObserver ); 
	
	virtual void SetAnalyzerL( const TDesC& aAnalyzer ); 
	
	virtual void SearchL( const TDesC& aSearchTerms, const TDesC& aDocumentField = KNullDesC);

	virtual void GetDocumentL(TInt aIndex);

	virtual void Cancel();

	virtual TBool IsActive();
    
public: // From MCPixSearcherObserver

	virtual void HandleSearchResultsL(TInt aError, TInt aEstimatedResultCount);

    virtual void HandleDocumentL(TInt aError, CSearchDocument* aDocument);
	    
private:
	RSearchServerSession iSearchSession;
	CCPixSearcher* iSearcher; 
	
	CCPixNPSearcherObserver* iObserver; 
	
    };

#endif /*CPIXNPSEARCHER_H_*/
