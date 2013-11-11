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
* Description:    Class for removing a fetch context from a list 
*
*/




#ifndef T_GLXFETCHCONTEXTREMOVER_H
#define T_GLXFETCHCONTEXTREMOVER_H

#include <e32base.h>

// Forward declarations
class MGlxFetchContext; 
class MGlxMediaList; 
	
/**
 * Class that removes the given fetch context from a media list when goes out 
 * of scope. Use this to avoid trap, and to achieve safe cleanup, when a fetch
 * context is created and used within a function.
 *
 * Note: the media list must not have been deleted when the destructor of this
 * class is called.
 *
 */
class TGlxFetchContextRemover 
    {
public:
    /**
     * Constructor
     * @param aContext the context to remove upon destruction
     * @param aMediaList List from which to remove the context
     */
    IMPORT_C TGlxFetchContextRemover(MGlxFetchContext* aContext, MGlxMediaList& aMediaList);
    /** 
     * Destructor 
     */
    IMPORT_C ~TGlxFetchContextRemover();
    
    /** 
     * Remove the context from the list 
     */
    IMPORT_C void Close();
    
private:
    /// Context that needs to be removed upon destruction
    MGlxFetchContext* iContext;
    
    /// Media list from which to remove the context from
    MGlxMediaList& iMediaList;
    };

#endif // T_GLXFETCHCONTEXTREMOVER_H
