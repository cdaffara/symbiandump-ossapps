/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Profile list container
*
*/



#ifndef CNSMLDMSYNCPROFILELIST_H
#define CNSMLDMSYNCPROFILELIST_H

//  INCLUDES

#include <e32base.h>
#include "nsmldmsyncprofileitem.h"

// FORWARD DECLARATIONS
class TNSmlDMSyncProfileItem;
class CNSmlDMSyncProfile;
class CNSmlDMSyncAppEngine;

/**
* CNSmlDMSyncProfileList 
*
* CNSmlDMSyncProfileList is a list of profiles
*/
NONSHARABLE_CLASS (CNSmlDMSyncProfileList) : public CBase
    {

    public:
        /**
        * Two-phased constructor.
        */
        static CNSmlDMSyncProfileList* NewL( CNSmlDMSyncAppEngine* aEngine  );

	    /**
        * Destructor.
        */
        virtual ~CNSmlDMSyncProfileList();

    private:
        /**
        * C++ default constructor.
        */
	    CNSmlDMSyncProfileList( CNSmlDMSyncAppEngine* aEngine  );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
	    void ConstructL();

	public:
		/**
		* Returns profile count.
        * @return Profile count.
        */
		TInt Count();

		/**
		* Returns profile from list.
        * @param aIndex List index.
		* @return TNSmlDMSyncProfileItem.
        */
		TNSmlDMSyncProfileItem Item( TInt aIndex );
	
		/**
		* Finds profile with name aProfileName.
		* @param aProfileName Profile name.
		* @return Profile id or KErrNotFound.
        */
		TInt FindProfile( const TDesC& aProfileName );

		/**
		* Read one profile from database.
		* @param aProfile.
		* @return TNSmlDMSyncProfileItem.
        */
        TNSmlDMSyncProfileItem ReadProfileItemL( CNSmlDMSyncProfile* aProfile );

		/**
		* Reads all profiles from database.
		* @return None.
        */
		void ReadProfileItemsL();

		/**
		* Reads all profiles from database.
		* @param aIncludeHidden Should hidden profile be included
		* @return None.
        */
		void ReadProfileItemsL( TBool aIncludeHidden );

		/**
		* Reads one profile from database.
		* @param aIndex profile index
		* @return None.
        */
		void ReadProfileItemL( TInt aIndex );

		/**
		* Resets profile list.
		* @param None.
		* @return None.
        */
		void Reset();

		/**
		* Remove item from profilelist
		* @param aProfileId.
		* @return None.
        */
		void Remove( TInt aProfileId );

		/**
		* Checks that mandatory data exists.
		* @param aProfile.
		* @return Return code.
        */
        static TInt CheckMandatoryDataL( CNSmlDMSyncProfile* aProfile );
		
		/**
		* Replaces profile items
		* @param aProfileItem.
		* @return None.
        */	
		void ReplaceProfileItemL( TNSmlDMSyncProfileItem& aProfileItem );
		
		/**
		* Sort list
		* @param None
		* @return None.
        */		
		void Sort();
				
	private:
		
		/**
		* Chceks empty characters
		* @param aText
		* @return ETrue if text is empty
        */
		static TBool IsEmpty( const TDesC& aText );

	private:
	    // Sync app engine
        CNSmlDMSyncAppEngine*           iEngine;
		// list of profiles
		RArray<TNSmlDMSyncProfileItem>  iList;
    };

#endif  // CNSMLDMSYNCPROFILELIST_H

// End of File
