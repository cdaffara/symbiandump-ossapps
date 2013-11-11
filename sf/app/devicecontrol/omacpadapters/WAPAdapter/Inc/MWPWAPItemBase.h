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
*     Base class for WAP setting items.
*
*/


#ifndef MWPWAPITEMBASE_H
#define MWPWAPITEMBASE_H

// INCLUDES
#include <e32base.h>

// CONSTANTS
const TUint32 KWPWAPNoUID = 0;
const TUint32 KWPWAPItemTypeNone = 0;
const TUint32 KWPWAPItemTypeBookmark = 1;
const TUint32 KWPWAPItemTypeAccesspoint = 2;

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * MWPWAPItemBase is a base class for wap settings.
 *
 * @lib WPWAPAdapter
 * @since 2.0
 */ 
class MWPWAPItemBase
    {
	public:
		struct TWPWAPSaveItem
			{
			TUint32 iSaveItemType;
			TUint32 iUID;
			};

    public:  // Constructors and destructor

        /**
        * Destructor.
        */
        virtual ~MWPWAPItemBase() {};

    public:   // New functions

        /**
        * Returns the name of the item to be returned in
        * SummaryText().
        * @return Name of the item
        */
        virtual const TDesC& Name() const = 0;

        /**
        * Returns the title of the item to be returned in
        * SummaryTitle().
        * @return Name of the item
        */
        virtual const TDesC& Title() const = 0;

        /**
        * Returns the address of the service pointed by 
        * the item.
        * @return Address of the service
        */
        virtual const TDesC& Address() const = 0;

        /**
        * Saves the item. Call BaseSaveL() in beginning.
        */
        virtual void SaveL() = 0;

        /**
        * Returns ETrue if the item can be saved as
        * default setting.
        * @return Whether the item can be made default
        */
        virtual TBool CanSetAsDefault() const = 0;

        /**
        * Sets the item as default setting.
        */
        virtual void SetAsDefaultL() = 0;

        /**
        * Returns ETrue if the item is valid.
        * @return ETrue if valid
        */
        virtual TBool ValidateL() = 0;

        /**
        * Adds the item and its children to a list of items.
        * @param aShownItems The list of shown items to be appended to
        * @param aHiddenItems The list of hidden items to be appended to
        */
        virtual void AddL( RPointerArray<MWPWAPItemBase>& aShownItems, 
            RPointerArray<MWPWAPItemBase>& aHiddenItems  ) = 0;

        /**
        * Returns the ID of the saved item.
        * @return ID of the saved item, or KWPWAPNoUID
        */
        virtual const TDesC8& SaveData() const = 0;

    };

#endif  // MWPWAPITEMBASE_H
            
// End of File
