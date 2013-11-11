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
* Description:  MDS to CPIX Image Document fetcher 
 *
*/

#ifndef CPIXMEDIAIMAGEDOC_H_
#define CPIXMEDIAIMAGEDOC_H_

#include "cpixdocfetcher.h"

class CCPIXMediaImageDoc : public CCPIXDocFetcher
    {
public:
    /*
     * NewL
     * @return instance of media Image doc
     */
    static CCPIXMediaImageDoc* NewL();
    /*
     * NewLC
     * @return instance of media Image doc
     */    
    static CCPIXMediaImageDoc* NewLC();
    /*
     * Destructor
     */
    ~CCPIXMediaImageDoc();
    //Overwride
    /*
     * GetCpixDocumentL get cpix document from mde object
     * @param CMdEObject& aObject mde object
     * @param TDesC& aAppClass app class
     * @param CMdEObjectDef& aObjectDef mde object def
     * @return instance of CPIX document 
     */
    CSearchDocument* GetCpixDocumentL(const CMdEObject& aObject, 
                                      const TDesC& aAppClass,
                                      CMdEObjectDef& aObjectDef);    
private:
    /*
     * Second phase constructor
     */
    void ConstructL();
    /*
     * Default constructor
     */
    CCPIXMediaImageDoc();
    };

#endif /* CPIXMEDIAIMAGEDOC_H_ */
