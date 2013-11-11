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
#include <common.h>

#include "CCPixNPSearcher.h"

#include "CNPSearchDocument.h"
#include "CPixNPExceptions.h"

#include <CSearchDocument.h>
#include <CCPixSearcher.h>

CCPixNPSearcher::CCPixNPSearcher()
:	iSearcher( NULL ),
	iObserver( NULL )
	{
	}

CCPixNPSearcher::~CCPixNPSearcher()
	{
	delete iSearcher; 
	delete iObserver; 
	iSearchSession.Close();
	}
    
CCPixNPSearcher* CCPixNPSearcher::NewL( const TDesC& aBaseAppClass, const TDesC& aDefaultSearchField )
	{
	CCPixNPSearcher* self = new (ELeave)CCPixNPSearcher();
    CleanupStack::PushL(self);
    self->ConstructL( aBaseAppClass, aDefaultSearchField );
    CleanupStack::Pop(); // self;
    return self;
	}

CCPixNPSearcher* CCPixNPSearcher::NewL( const TDesC& aBaseAppClass )
	{
	CCPixNPSearcher* self = new (ELeave)CCPixNPSearcher();
    CleanupStack::PushL(self);
    self->ConstructL( aBaseAppClass );
    CleanupStack::Pop(); // self;
    return self;
	}

void CCPixNPSearcher::ConstructL( const TDesC& aBaseAppClass, const TDesC& aDefaultSearchField )
	{
	InitInterfaceL();
	User::LeaveIfError(iSearchSession.Connect());
	iSearcher = CCPixSearcher::NewL(iSearchSession, aDefaultSearchField);
	iSearcher->OpenDatabaseL( aBaseAppClass  );
	}

void CCPixNPSearcher::ConstructL( const TDesC& aBaseAppClass  )
	{
	InitInterfaceL();
	User::LeaveIfError(iSearchSession.Connect());
	iSearcher = CCPixSearcher::NewL(iSearchSession);
	iSearcher->OpenDatabaseL( aBaseAppClass  );
	}

void CCPixNPSearcher::Invalidate() 
	{
	// Get rid of observer. Trying to call JavaScript methods
	delete iObserver; 
	iObserver = NULL; 
	
	iSearcher->Cancel(); 
	}

void CCPixNPSearcher::SetObserverL( NPObject* aObserver )
	{
	delete iObserver; 
	iObserver = NULL;
	if ( aObserver != NULL ) 
		{
		iObserver = new (ELeave)CCPixNPSearcherObserver( iInstanceHandle, aObserver ); 
		}
	}

void CCPixNPSearcher::SetAnalyzerL( const TDesC& aAnalyzer ) 
	{
	if ( !iSearcher->IsActive() )
		{
		iSearcher->SetAnalyzerL( aAnalyzer ); 
		}
	else 
		{
		// TODO Raise error
		}
	}

void CCPixNPSearcher::SearchL( const TDesC& aSearchTerms, const TDesC& aDocumentField )
	{
	PERFORMANCE_LOG_START("CCPixNPSearcher::SearchL");
	PERFORMANCE_LOG_MESSAGE(aSearchTerms);
	
	if ( !iSearcher->IsActive() ) 
		{
		iSearcher->SearchL( *this, aSearchTerms, aDocumentField );
		}
	else 
		{
		// TODO Raise error
		}
	
	}

void CCPixNPSearcher::GetDocumentL(TInt aIndex)
	{
    PERFORMANCE_LOG_START("CCPixNPSearcher::GetDocumentL");

	if ( !iSearcher->IsActive() ) 
		{
		iSearcher->GetDocumentL( aIndex, *this );
		}
	else 
		{
		// TODO Raise error
		}
	}

void CCPixNPSearcher::Cancel()
	{
	iSearcher->Cancel();
	}

TBool CCPixNPSearcher::IsActive()
	{
	return iSearcher->IsActive(); 
	}

void CCPixNPSearcher::HandleSearchResultsL(TInt aError, TInt aEstimatedResultCount)
	{
	PERFORMANCE_LOG_START("CCPixNPSearcher::HandleSearchResultsL");
	if ( iObserver != NULL ) 
		{
		iObserver->HandleSearchResultsL( DescribeErrorCode( aError ), 
										 aEstimatedResultCount ); 
		}
	}

void CCPixNPSearcher::HandleDocumentL( TInt aError, CSearchDocument* aDocument )
	{
	if ( iObserver != NULL ) 
		{
		if ( aDocument != NULL ) 
			{
			NPSearchDocumentObject *object = reinterpret_cast<NPSearchDocumentObject *>(NPN_CreateObject (iInstanceHandle, NPSearchDocumentClass));
			CNPSearchDocument* documentObject = CNPSearchDocument::NewL( aDocument );
			documentObject->SetInstance( iInstanceHandle, &object->object );
	        object->plugin = documentObject;
			iObserver->HandleDocumentL( DescribeErrorCode( aError ), 
											object ); 
		} else {
			iObserver->HandleDocumentL( DescribeErrorCode( aError ), 
											NULL ); 
			}
		} 
	else 
		{
		delete aDocument; 
		}
	}


