/*
 * Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  DM tree etc.
 *
 */

#ifndef __NSMLDMURI_H
#define __NSMLDMURI_H

#include <e32std.h>
#include <e32base.h>

#define KNSmlDMUriSeparator '/'
_LIT8(KNSmlDmRootUri, ".");
_LIT8(KNSmlDmUriDotSlash, "./");
_LIT8( KNSmlDmQuestionMark, "?" );

// ===========================================================================
// NSmlDmURI
// ===========================================================================
class NSmlDmURI
    {
public:
    static TPtrC8 ParentURI(const TDesC8& aURI);
    static TPtrC8 LastURISeg(const TDesC8& aURI);
    static TPtrC8 RemoveDotSlash(const TDesC8& aURI);
    static TPtrC8 RemoveProp(const TDesC8& aURI);
    static TPtrC8 RemoveLastSeg(const TDesC8& aURI);
    static TPtrC8
            URISeg(const TDesC8& aURI, TInt aLocation, TInt aSegCount=1);
    static TInt NumOfURISegs(const TDesC8& aURI);
    };

#endif // __NSMLDMURI_H
