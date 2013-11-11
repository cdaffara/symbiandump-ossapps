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
* Description:  Stripper and merger base class for versit data and 
*				 implementation for vCard and vCalendar stripping and merging.
*
*/


#ifndef __NSMLDATABASE_H__
#define __NSMLDATABASE_H__

// INCLUDES
#include <e32base.h>
#include <vprop.h>
#include <SmlDataFormat.h>


// CLASS DECLARATION

/**
* Class that is inherited from CParserProperty to make it possible to handle parameters.
*
* @lib nsmldatamod.lib
*/
class CNSmlProperty : public CParserProperty
	{
	public:
		/**
	    * Creates new property based on given parameters.
	    * @param aPropertyValue. 	Value of property.
	    * @param aName. 			Name of property.
	    * @param aArrayOfParams. 	Parameters of property.
	    * @return 					New instance of this class.
	    */
		static CNSmlProperty* NewL( CParserPropertyValue& aPropertyValue, const TDesC8& aName, CArrayPtr<CParserParam>* aArrayOfParams );
		
		/**
	    * Returns paramaters of property.
	    * @return	Parameters of property.
	    */
		CArrayPtr<CParserParam>* Parameters() const;
		
		/**
	    * Returns count of paramaters.
	    * @return	Count of paramaters.
	    */
		TInt ParamCount() const;
		
		/**
	    * Compares names of two properties
	    * @param aFirst. 	First property to compare.
	    * @param aSecond. 	Second property to compare.
	    * @return 			Returns 0 if names are equal.
	    */
		static TInt Compare( const CNSmlProperty& aFirst, const CNSmlProperty& aSecond );
		
	protected:
		/**
	    * Basic constructor of class
	    */
		CNSmlProperty( CParserPropertyValue& aPropertyValue, CArrayPtr<CParserParam>* aArrayOfParams );
	};

/**
* Base class for Calendar and Contacts Data Modifications.
*
* @lib nsmldatamod.lib
*/
class CNSmlDataModBase : public CBase
	{
	public:
	    /**
	    * C++ destructor.
	    */
		IMPORT_C virtual ~CNSmlDataModBase();
		
		/**
	    * Sets own database format.
	    * @param aOwnStoreFormat.	Own database format.
	    */
		IMPORT_C void SetOwnStoreFormat( CSmlDataStoreFormat& aOwnStoreFormat );

		/**
	    * Sets remote database format.
	    * @param aRemoteStoreFormat.	Remote database format.
	    */
		IMPORT_C void SetPartnerStoreFormat( CSmlDataStoreFormat& aRemoteStoreFormat );

		/**
	    * Sets mime type that is used for sending and receiving.
	    * This method can be called separately before every item.
	    * @param aMimeType.		Mime type.
	    * @param aMimeVersion.	Mime type version.
	    * @return				KErrNone if partnerStoreFormat is passed and same mime type with same version
	    *						can be found. Otherwise KErrNotFound is returned.
	    */
		IMPORT_C TInt SetUsedMimeType( const RStringF aMimeType, const RStringF aMimeVersion );
		
		/**
	    * Strips data that is to be transmitted to the sync partner.
	    * @param aItem	Item's data. On returns this data may have changed due to stripping.
	    */
		IMPORT_C void StripTxL( CBufBase& aItem );

		/**
	    * Merges received item with item in exported from the local database.
	    * @param aNewItem 	Received item. On return aItem contains merged item.
		* @param aOldItem 	Item exported from the local database with which aNewItem is merged.
	    */
		IMPORT_C void MergeRxL( CBufBase& aNewItem, CBufBase& aOldItem, TBool aFieldLevel = EFalse );

		/**
	    * Checks whether merging is needed or not.
		* @return	ETrue if mergins is needed EFalse otherwise.
	    */
		IMPORT_C TBool NeedsMerge();

	protected:
		/**
	    * Basic constructor of class.
	    */
		CNSmlDataModBase();
		
		/**
	    * Creates correct type of parser for each Data Modifications.
		* @return	Correct type of versit parser.
	    */
		virtual CVersitParser* ChildCreateParserLC() = 0;

		/**
	    * Strips data that is to be transmitted to the sync partner.
	    * @param aItem	Item's data. On returns this data may have changed due to stripping.
	    */
		void StripL( TDes8& aItem );
		
		/**
	    * Strips data that is to be transmitted to the sync partner from entity.
	    * @param aEntity	Item's data. On returns this data may have changed due to stripping.
	    * @param aModified	If item's data is modified then this is ETrue at return. Otherwise EFalse.
	    */
		void StripEntityL( CVersitParser* aEntity, TBool& aModified ) const;
		
		/**
	    * Strips all data from entity that is supported by remote server.
	    * @param aEntity			Item's data. On returns this data may have changed due to stripping.
	    * @param aModified			If item's data is modified then this is ETrue at return. Otherwise EFalse.
	    * @param aParamLevelCheck	This is used to decide if parameters are compared when properties are stripped or not.
	    */
		void StripAllOnPartnerListL( CVersitParser* aEntity, TBool& aModified, TBool aParamLevelCheck = ETrue ) const;
		
		/**
	    * Strips all data from entity that is not supported by remote server.
	    * @param aEntity			Item's data. On returns this data may have changed due to stripping.
	    * @param aModified			If item's data is modified then this is ETrue at return. Otherwise EFalse.
	    * @param aParamLevelCheck	This is used to decide if parameters are compared when properties are stripped or not.
	    */
		void StripAllNotOnPartnerListL( CVersitParser* aEntity, TBool& aModified, TBool aParamLevelCheck = ETrue ) const;
		
		/**
	    * Merges data from old item to new item.
	    * @param aNewItem		Data of receive item. On returns this data may have changed due to merging.
	    * @param aOldData		Data of old item.
	    * @param aFieldLevel	This is used to decide if operation is field level update or not. Are all supported
	    *						properties updated or just received properties.
	    */
		void MergeL( TDes8& aNewItem, const TDesC8& aOldItem,TBool aFieldLevel );
		
		/**
	    * Merges data from old entity to new entity.
	    * @param aNewEntity		Data of receive entity. On returns this data may have changed due to merging.
	    * @param aOldEntity		Data of old entity.
	    * @param aModified		If aNewEntity is modified then this is ETrue at return. Otherwise EFalse.
	    * @param aFieldLevel	This is used to decide if operation is field level update or not. Are all supported
	    *						properties updated or just received properties.
	    */
		void MergeEntityL( CVersitParser* aNewEntity, CVersitParser* aOldEntity, TBool& aModified, TBool aFieldLevel ) const;

	private:
		CSmlDataStoreFormat* iOwnStoreFormat;
		CSmlDataStoreFormat* iRemoteStoreFormat;
		TInt iUsedOwnMimeType;
		TInt iUsedRemoteMimeType;
		RStringF iMimeType;
		RStringF iMimeVersion;
	};


/**
* Data Modifications for Calendar.
*
* @lib nsmldatamod.lib
*/
class CNSmlVCalMod : public CNSmlDataModBase
	{
	public:
		/**
	    * Constuctor
	    */
		IMPORT_C CNSmlVCalMod();
		
	protected:
		/**
	    * Creates correct type of parser for Calendar Data Modifications.
		* @return	Correct type of versit parser.
	    */
		CVersitParser* ChildCreateParserLC();
	};

/**
* Data Modifications for Contacts.
*
* @lib nsmldatamod.lib
*/
class CNSmlVCardMod : public CNSmlDataModBase
	{
	public:
		/**
	    * Constuctor
	    */
		IMPORT_C CNSmlVCardMod();
		
	protected:
		/**
	    * Creates correct type of parser for Contacts Data Modifications.
		* @return	Correct type of versit parser.
	    */
		CVersitParser* ChildCreateParserLC();
	};


#endif // __NSMLDATABASE_H__

// End of file
