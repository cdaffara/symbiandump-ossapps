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
* Description: 
*
*/




#ifndef CREATORDATAFIELD_H_
#define CREATORDATAFIELD_H_

#include <e32base.h>

class MCreatorRandomDataField
{
public:
    enum TRandomLengthType
    {
    ERandomLengthUndefined,
    ERandomLengthDefault, // Default length (use data from engine)
    ERandomLengthMax,     // Maximum length
    ERandomLengthExplicit // Explicitly defined length
    };
    
    /**
     * Set field content to random.     
     * @param aRandomLenType Random length type
     * @param aExplicitRandomLen Random length when ERandomLengthExplicit is used. Ignored otherwise.
     */
    virtual void SetRandomParametersL( TRandomLengthType aRandomLenType, TInt aRandomLen ) = 0;
};

#endif /*CREATORDATAFIELD_H_*/
