/**
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
* Description: stub CMPXCollectionPath for testing CVideoCollectionClient
* 
*/

#ifndef CMPXCOLLECTIONPATH_H
#define CMPXCOLLECTIONPATH_H

#include <mpxcommonframeworkdefs.h>

class CMPXCollectionPathNode;
class TMPXAttribute;


class CMPXCollectionPath : public CBase
    {

public:
    
    /**
    *  
    **/
    static CMPXCollectionPath* NewL(const CMPXCollectionPath& aPath);

    /**
    *  
    */
    static CMPXCollectionPath* NewL();
    
    /**
     * constructor.
     *
     */
    CMPXCollectionPath();

    /**
    *  Destructor.
    *
    */
    virtual ~CMPXCollectionPath();


public: 

    /**
     * returns 1 always
     */
    TInt Levels() const;

    /**
     * NOP
     */
    void AppendL(const TArray<TMPXItemId>& aIds);
    
    /**
     * NOP
     */
    void AppendL(const TMPXItemId& aId);
    
    /**
     * NOP
     */
    void SelectL(const TMPXItemId& aId);

    /**
     * set path level 
     */
    static void setLevel(int level);

    };

#endif // CMPXCOLLECTIONPATH_H
