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
#ifndef CCPIXNPSEARCHEROBSERVER_H_
#define CCPIXNPSEARCHEROBSERVER_H_

// INCLUDES
#include <e32base.h>

#ifdef __S60_50__
#include <npscript.h>
#else
#include <stdint.h>
#include <JavaScriptCore/npruntime.h>
#endif

#include "ICPixNPSearcherObserver.h"

/**
 * CCPixNPSearcherObserver
 * Implements the functionality described in the MCPixNPSearcherObserver.
 * This class represents and wraps a JavaScript object observing plugin's 
 * searcher object. This class is used to invoke the observer methods of 
 * the associated JavaScript object. 
 */
class CCPixNPSearcherObserver : public CBase, public MCPixNPSearcherObserver
{
public:
	CCPixNPSearcherObserver( NPP& aNpp, NPObject* aSelf );
	virtual ~CCPixNPSearcherObserver();

public: // From MCPixNPSearcherObserver

	virtual void HandleSearchResultsL(const TDesC8* aError, TInt aEstimatedResultCount);
	virtual void HandleDocumentL(const TDesC8* aError, NPSearchDocumentObject* aDocument);

private: // Utility
	
	/**
	 * FIXME: This method is copied from CObjectInterface and is harmful redundancy
	 */
	void DescriptorToVariant(const TDesC8& aString, NPVariant& aVariant);

private:
    
    NPP       &iNpp;
    NPObject  *iSelf;

};


#endif // CCPIXNPSEARCHEROBSERVER_H_
