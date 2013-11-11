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
* Description:  MDS to CPIX Audio Document fetcher 
 *
*/

#ifndef CPIXMEDIAAUDIODOC_H_
#define CPIXMEDIAAUDIODOC_H_

#include "cpixdocfetcher.h"

NONSHARABLE_CLASS(CCPIXMediaAudioDoc) : public CCPIXDocFetcher
    {
public:
    /*
     * NewL
     * @return instance of media audio doc
     */
    static CCPIXMediaAudioDoc* NewL();
    /*
     * NewLC
     * @return instance of media audio doc
     */    
    static CCPIXMediaAudioDoc* NewLC();
    /*
     * Destructor
     */
    ~CCPIXMediaAudioDoc();
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
    CCPIXMediaAudioDoc();
    };

#endif /* CPIXMEDIAAUDIODOC_H_ */
