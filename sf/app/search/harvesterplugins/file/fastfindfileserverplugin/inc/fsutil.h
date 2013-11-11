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
* Description:  Some generic utilities.
*
*/

#ifndef FSUTIL_H
#define FSUTIL_H

#include <e32base.h>

class FSUtil
    {
	public:
	    
	    static inline TUint32 MediaID( const RFs& aFs, TInt aDrive );
	        
	    static inline TUint32 MediaID( const RFs& aFs, const TDesC& aUri );
    
	private:
	
	    static inline TInt DriveNumber( const TDesC& aUri );
    };

#include "fsutil.inl" // inlined methods

#endif // FSUTIL_H
