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
* Description:  DM Tree module
*
*/


#ifndef __NSMLDMIAPMATCHER_H__
#define __NSMLDMIAPMATCHER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include "nsmldmuri.h"

//CLASS FORWARDS
class MSmlDmCallback;

_LIT8( KNSmlDMIAPUri, "AP" );
_LIT8( KNSmlDMSeparator8, "/" );
_LIT8( KNSmlDMIAPUriDotSlash, "./" );
_LIT8( KNSmlDMConnMOIAPUri, "NAP" );

const TUint8 KNSmlDMSlash = 0x2f; //forward slash
/**
* CNSmlDMIAPMatcher contains services for generating URI from IAPId and
* vice versa in DM plug-in adapters. The class needs the working
* MSmlDmCallback callback interface for operating, so ths can be used only
* by DM plug-in adapters
*
*  @since
*/
class CNSmlDMIAPMatcher : public CBase
	{
    public:
		/**
		* Destructor
		*/
	    ~CNSmlDMIAPMatcher();

		/**
		* Two-phased constructor
		* @param	aDmCallback			Pointer to callback interface
		* @return						Pointer to newly created module instance
		*/
        static CNSmlDMIAPMatcher* NewL( MSmlDmCallback* aDmCallback);

		/**
		* Two-phased constructor, pushes pointer to cleanup stack.
		* @param	aDmCallback			Pointer to callback interface
		* @return						Pointer to newly created module instance
		*/
        static CNSmlDMIAPMatcher* NewLC( MSmlDmCallback* aDmCallback);

		/**
		* Gets the IAP id which corresponds to given URI
		* @param	aURI				Accespoint URI
		* @return						Accespoint id in AP database
		*/
        TInt IAPIdFromURIL( const TDesC8& aURI );

		/**
		* Generates the URI which corresponds to given IAP Id
		* @param	aIAPId				Accespoint Id
		* @return						Accesspoint URI in DM
		*/
        HBufC8* URIFromIAPIdL( TInt aIAPId );

    private:
		/**
		* Second phase constructor
		* @param	aDmCallback			Pointer to callback interface
		*/
        void ConstructL(MSmlDmCallback* aDmCallback);

		/**
		* Changes the given descriptor to Integer
		* @param	aLuid				Luid in descriptor format
		* @return						Luid in TInt type
		*/
        TInt DesToInt(const TDesC8& aLuid) const;

		/**
		* Return the URI without ./ at the beginning, if those exist
		* @param	aURI		Uri
		* @return 		 		Uri without ./
		*/
        TPtrC8 RemoveDotSlash(const TDesC8& aURI) const;

		/**
		* Return the last uri segment of the aURI
		* @param	aURI		Uri
		* @return 		 		The last uri segment
		*/
        TPtrC8 LastURISeg(const TDesC8& aURI) const;
        
    /**
		* Return the last uri segment of the aURI
		* @return 		 		The AccessPoint Reference
		*/
        const TDesC8& GetConRef() ;

    private:
    
 		/**
		* Pointer to callback interface
		*/
		MSmlDmCallback* iCallback;
		
		/**
		* Boolean to check FeatMgr Initialization
		*/
		TBool iFeatMgrInitialized;
	
	};

#endif // __NSMLDMIAPMATCHER_H__
