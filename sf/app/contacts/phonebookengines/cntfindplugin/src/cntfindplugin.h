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
* Description: 
*     Contact model ECOM plugin for chinese find.
*
*/


#ifndef __CntFindPlugin_H__
#define __CntFindPlugin_H__

// INCLUDES
#include <cntviewfindconfig.h>

// FORWARD DECLARATIONS
class CFindUtil;

// CLASS DECLARATION

/**
 * This class implements the functionality promised by 
 * the CFindUtilInterface definition class.
 */
class CAknFindUtilBase : public CContactViewFindConfigInterface
    {
    public: // Construction / Destruction
        /**
         * Two phase static constructor.
         * @return Newly created instance of CAknFindUtilBase.
         */
	    static CAknFindUtilBase* NewL();
	    ~CAknFindUtilBase();

    private:	// From CContactViewFindConfigInterface
	    void OpenL();
	    void Close();
	    TBool Match(const TDesC& aContactsField, const TDesC& aWord);	
	    TBool IsWordValidForMatching(const TDesC& aWord);
	    TBool MatchRefineL( const TDesC& aItemString, const TDesC& aSearchText);

    private: // Implementation
        /**
         * Standard C++ constructor.
         */
	    CAknFindUtilBase();

    private: // Data
        /// Own: Find utility that implements the actual finding functionality
	    CFindUtil* iFindUtil;
    };

#endif // __CAknFindUtilBase_H__

// End of File
