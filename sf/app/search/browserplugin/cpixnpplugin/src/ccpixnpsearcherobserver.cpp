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
#include "CCPixNPSearcherObserver.h"

#include "CNPSearchDocumentInterface.h"
#include <string.h>

#include "CCPixNPPluginEcom.h"
#include <utf.h>
#include <common.h>

/**
 * These are the methods that are called in the script object,
 * observer object is wrapping. 
 */
_LIT8( KHandleSearchResultsMethodName, "handleSearchResults"); 
_LIT8( KHandleDocumentMethodName, "handleDocument"); 

CCPixNPSearcherObserver::CCPixNPSearcherObserver( NPP& aNpp, NPObject* aSelf )
: iNpp( aNpp ), iSelf( aSelf )
    {
    NPN_RetainObject( iSelf ); 
    }

CCPixNPSearcherObserver::~CCPixNPSearcherObserver()
    {
    if (iSelf) NPN_ReleaseObject( iSelf );
    }

void CCPixNPSearcherObserver::HandleSearchResultsL(const TDesC8* aError, TInt aEstimatedResultCount)
	{
	PERFORMANCE_LOG_START("CCPixNPSearcherObserver::HandleSearchResultsL");
	
    NPVariant result;
    NPString function;
    function.UTF8Characters = (const NPUTF8 *)KHandleSearchResultsMethodName().Ptr();
    function.UTF8Length = KHandleSearchResultsMethodName().Length();

    NPVariant args[2];
    if ( aError ) {
    	DescriptorToVariant( *aError, args[0] ); 
    } else {
    	NULL_TO_NPVARIANT( args[0] );
    }
    INT32_TO_NPVARIANT( aEstimatedResultCount, args[1] );
    
    PERFORMANCE_LOG_MESSAGE(_L("----------Start document fetching----------"));
    NPN_Invoke( iNpp, iSelf, &function, args, 2, &result );
	}

void CCPixNPSearcherObserver::HandleDocumentL(const TDesC8* aError, NPSearchDocumentObject* aDocument)
	{
    NPVariant result;
    NPString function;
    function.UTF8Characters = (const NPUTF8 *)KHandleDocumentMethodName().Ptr();
    function.UTF8Length = KHandleDocumentMethodName().Length();

    NPVariant args[2];
    if ( aError ) {
    	DescriptorToVariant( *aError, args[0] ); 
    } else {
    	NULL_TO_NPVARIANT( args[0] );
    }
    if ( aDocument ) {
    	OBJECT_TO_NPVARIANT( &aDocument->object, args[1] );
    } else {
    	NULL_TO_NPVARIANT( args[1] );
    }

    NPN_Invoke( iNpp, iSelf, &function, args, 2, &result );
	}

void CCPixNPSearcherObserver::DescriptorToVariant(const TDesC8& aString, NPVariant& aVariant)
	{
    CCPixNPPluginEcom* pluginEcom = (CCPixNPPluginEcom*) Dll::Tls();
    NPNetscapeFuncs* netscapeFuncs = pluginEcom->Funcs(); 

    int length = aString.Length();
    char* newString = (char *)netscapeFuncs->memalloc( length );
    if (newString)
    	{
        Mem::Copy(newString, aString.Ptr(), length);
        STRINGN_TO_NPVARIANT(newString, length, aVariant);
    	}
	}

