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




#include "glxfetchcontextremover.h"

#include "mglxmedialist.h"
#include <glxtracer.h>                         // For Logs
    
// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFetchContextRemover::TGlxFetchContextRemover(MGlxFetchContext* aContext, 
        MGlxMediaList& aMediaList)
            : iContext(aContext), iMediaList(aMediaList)
    {
    TRACER("TGlxFetchContextRemover::Default Constructor");
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C TGlxFetchContextRemover::~TGlxFetchContextRemover()
    {
    Close();
    }

// -----------------------------------------------------------------------------
// Close
// -----------------------------------------------------------------------------
EXPORT_C void TGlxFetchContextRemover::Close()
    {
    iMediaList.RemoveContext( iContext );
    }
