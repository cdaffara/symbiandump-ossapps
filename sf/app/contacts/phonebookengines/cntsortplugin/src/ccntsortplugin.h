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
*     Contact model ECOM sort plugin
*
*/


#ifndef __CCntSortPlugin_H__
#define __CCntSortPlugin_H__

// INCLUDES
#include <cntviewsortplugin.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <cntviewsortpluginbase.h>
#endif 

// FORWARD DECLARATIONS
class CSortUtil;
class CSortKeyArray;

// CLASS DECLARATION

/**
 * This class implements the functionality promised by 
 * the CViewContactSortInterfaceDefinition definition class.
 */
class CCntSortPlugin : public CViewContactSortPlugin
    {
    public: // Construction / Destruction
        /**
         * Two phase static constructor.
         * @param aParams Parameters for construction.
         * @return Newly created instance of CCntSortPlugin.
         */
        static CCntSortPlugin* NewL(TAny* aParams);
        ~CCntSortPlugin();

    private:	// From CContactViewSortConfigInterface
        void SetSortOrderL(const RContactViewSortOrder& aViewSortOrder);
        TInt SortStart(TSortStartTypes aSortStartType, TInt aCount);
        void SortCompleted();
        TInt SortCompareViewContactsL
            (const CViewContact& aLhs, const CViewContact& aRhs);
        TInt ApiCompareViewContactsL
            (const CViewContact& aLhs, const CViewContact& aRhs);
        TInt CompareViewContactsL(const CViewContact& aLhs, const CViewContact& aRhs);
        TBool ViewContactIsSortable(const CViewContact& aViewContact);

    private: // Implementation
        /**
         * Standard C++ constructor.
         */
        CCntSortPlugin();

        /**
         * 2nd phase constructor.
         */
        void ConstructL(TSortPluginParams* aParams);

        /**
         * Do the actual sort starting.
         * @param aSortStartType Type of the sort to start.
         * @param aCount Approximate number of contacts to process.
         */
        void DoSortStartL(TSortStartTypes aSortStartType, TInt aCount);
    private: // Data
        /// Own: The sort util
        CSortUtil* iSortUtil;
        /// Currently active sort order
        RContactViewSortOrder iSortOrder;
        /// Ref: Is sortable function 
	    TBool(*iIsSortable)(const CViewContact& aViewContact);
        /// Own: sort key array for left hand side
        CSortKeyArray* iLeftSortKeyArray;
        /// Own: sort key array for right hand side
        CSortKeyArray* iRightSortKeyArray;
    };

#endif // __CCntSortPlugin_H__

// End of File
