/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/


#ifndef MIRCACHEOBSERVER_H
#define MIRCACHEOBSERVER_H

#include <e32def.h>

class MIRCacheObserver
{
    public:
    //!General error condition   
    virtual void CacheError() = 0;
    
    //! If seeked data not in cache    
    virtual void CacheFailed() = 0;
    
    //!If cached data is stale    
    virtual void CacheInvalid() = 0;
    
    //!Data is valid and extracted into the structure    
    virtual void CachedStructureL(TInt aChoice) = 0;
                    
};

#endif //MIRCACHEOBSERVER_H
