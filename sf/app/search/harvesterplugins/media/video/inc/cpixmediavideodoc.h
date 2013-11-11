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
* Description:  MDS to CPIX Video Document fetcher 
 *
*/


#ifndef CPIXMEDIAVIDEODOC_H_
#define CPIXMEDIAVIDEODOC_H_

#include "cpixdocfetcher.h"

class CCPIXMediaVideoDoc : public CCPIXDocFetcher
    {
public:
    /*
     * NewL
     * @return instance of media Image doc
     */
    static CCPIXMediaVideoDoc* NewL();
    /*
     * NewLC
     * @return instance of media Image doc
     */    
    static CCPIXMediaVideoDoc* NewLC();
    /*
     * Destructor
     */
    ~CCPIXMediaVideoDoc();
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
    CCPIXMediaVideoDoc();
    };

#endif /* CPIXMEDIAVIDEODOC_H_ */
