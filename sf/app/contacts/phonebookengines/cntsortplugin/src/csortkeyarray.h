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
*     Sort key array for Contact model ECOM sort plugin.
*
*/


#ifndef __CSortKeyArray_H__
#define __CSortKeyArray_H__

// INCLUDES
#include <SortUtil.h>

// CLASS DESCRIPTION
/**
 * Sort key array for Contact model ECOM sort plugin.
 */
class CSortKeyArray : public CBase, 
                      public MSortKeyArray
    {
    public:
        /**
         * Static constructor.
         * @return Newly created instance of this class.
         */
        static CSortKeyArray* NewL();

        /**
         * Standard c++ destructor.
         */
        ~CSortKeyArray();

    public: // Interface
        /**
         * Appends new sortkey to the array.
         * @param aKey New sortkey to append.
         */
        void AppendL(const TSortKey& aKey);

        /**
         * Resets the sortkey array.
         */
        void Reset();

        /**
         * Sets the text of the key in specified index.
         * @param aText The text to set to key in index.
         * @param aIndex The index where the key to be modified is.
         */
        void SetText(const TDesC& aText, TInt aIndex);

    public:  // from MSortKeyArray
        TInt SortKeyCount() const;
        TSortKey SortKeyAt(TInt aIndex) const;
        
    private:
        /**
         * C++ constructor.
         */
        CSortKeyArray();

    private:  // Data
        /// Own: Array of key types.
        RArray<TSortKeyType> iKeyTypes;
        /// Own: Array of texts that correspond to types in array iKeyTypes.
        RArray<TPtrC> iTexts;
    };

#endif // __CSortKeyArray_H__

// End of File
