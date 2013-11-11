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
* Description:  This file contains the header file of the class TPEMatcher. 
*
*/

#ifndef TPEMATCHER_H_
#define TPEMATCHER_H_

#include <e32def.h>
#include <e32cmn.h>

class TPEMatcher 
    {
public:
	
	 /**
    * C++ default constructor.
    */
    TPEMatcher();
    
   /**
	  * Destructor.
    */
    ~TPEMatcher();
    
    /**
     * Finds if the two numbers match. Numbers are matched from
     * right to left. If either or both of the numbers passed as a parameter
     * are shorter than aCount the shortes number count is used in
     * matching.     
     * 
     * @since S60 v5.2
     * @param aNumber1 phone number.
     * @param aNumber2 phone number.
     * @param aCount Digit count used to match numbers.
     * @return Found match. 
     */
    TBool numbersMatch( const TDesC& aNumber1, const TDesC& aNumber2, TInt aCount );
    
    };


#endif /* TPEMATCHER_H_ */
