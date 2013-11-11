/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CCapList header file.
*
*/


#ifndef _CAPLIST_H_
#define _CAPLIST_H_

// INCLUDES

#include <e32base.h>
#include <badesca.h>
#include "stringlist.h"

//
// Class CCapList
//
NONSHARABLE_CLASS( CCapList ) : public CBase
{
public:
    /**
     * Two-phase constructor.
     * @return CCapList instance
     */
    static CCapList* NewL();
    /**
     * Destructor
     * @return none
     */
    ~CCapList();

private:
    /**
     * Constructor
     * @return none
     */
    CCapList();
    /**
     * Initializes the member data
     * @return none
     */
    void ConstructL();
    /**
     * Operator =
     * @param aList The data to be copied
     * @return none
     */
    void operator=(const CCapList& aList); // not allowed

public:
    /**
     * Returns the list
     * @return The CStringList object
     */
    CStringList* List();

public:
    /**
     * Find element
     * @param aId
     * @param aType
     * @param aIndex (Default=0)
     * @return position
     */
    TInt Find(TInt aId, TInt aType, TInt aIndex=0);
    /**
     * Find from mark
     * @param aId
     * @param aType
     * @return position
     */
    TInt FindFromMark(TInt aId, TInt aType);
    /**
     * Finds service xml-file identification header. This header must be the first 
     * line in every service file.
     * @return ETrue if service header found
     */
    TBool FindServiceHeader();
    /**
     * Find service
     * @param aList The source list
     * @return ETrue if Service is found
     */
    TBool FindServiceL(CStringList* aList);

private:
    CStringList*   iList;           // has
};

#endif 

// End of file


