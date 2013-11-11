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
* Description:   Blocking attribute retriever
*
*/



#ifndef C_GLXATTRIBUTERETRIEVER_H
#define C_GLXATTRIBUTERETRIEVER_H

#include <e32std.h>
#include "mglxmedialistobserver.h"

// Forward declarations
class MGlxFetchContext;
class MGlxMediaList;
class CGlxAttributeRetrieverObj;

const TInt KGlxFetchContextPriorityBlocking = KMaxTInt32;

/**
 *  GlxAttributeRetriever
 *
 *  This class blocks until all requested metadata has been retrieved
 *
 *	@ingroup mlm_media_list_manager_design
 *  @lib glxuiutilities.lib
 */
class GlxAttributeRetriever 
	{
public:
	/**
	 * Retrieves all metadata required by the context
	 * @param aContext Fetch context that informs what metadata needs to be retrieved
	 * @param aList List for which metadata is required for
	 * @param aShowDialog Whether to show a wait dialog while retrieval takes place. Should only
	 *                    be false if there is another wait dialog already active
	 * @return Error code
	 */
	IMPORT_C static TInt RetrieveL(const MGlxFetchContext& aContext, MGlxMediaList& aList, TBool aShowDialog = ETrue);
	};

#endif // C_GLXATTRIBUTERETRIEVER_H

