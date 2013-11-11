/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: stub array of media objects for unit testing CVideocollectionClient
*
*/

#ifndef CMPXMEDIAARRAY_H
#define CMPXMEDIAARRAY_H

#include <s32strm.h>


class CMPXMedia;

class CMPXMediaArray : public CBase
    {
public:

    /**
     * creates new array
     */
    static CMPXMediaArray* NewL(){return new CMPXMediaArray();};
    
    /**
     * Constructor 
     */
    CMPXMediaArray():itemCount(0){};
    
    /**
     * Destructor 
     */
    ~CMPXMediaArray(){};

    /**
     * NOP 
     */
    void AppendL(const CMPXMedia* /*aMedia*/){itemCount++;};
    
    /**
     * NOP 
     */
    void AppendL(const CMPXMedia& /*aMedia*/){itemCount++;};
    
    /**
     * return itemCount
     */
    TInt Count() const{ return itemCount;};
 
    int itemCount;
    
    };

#endif // CMPXMEDIAARRAY_H
