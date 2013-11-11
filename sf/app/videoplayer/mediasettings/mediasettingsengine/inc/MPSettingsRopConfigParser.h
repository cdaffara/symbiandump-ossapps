/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Config descriptor parser for ROP specific settings.*
*/



// Version : %version: 2 %




#ifndef MPROPCONFIGPARSER_H
#define MPROPCONFIGPARSER_H

//  INCLUDES
#include    <e32base.h>

// DATA TYPES
enum TMPRopConfType
    {
    EMPRopConfTypeInteger = 1,
    EMPRopConfTypeString = 2,
    EMPRopConfTypeIntArray = 3
    };

// CLASS DECLARATION

/**
*  CMPRopSettingItem
*   Setting item class, for storing item specific data.
*   
*
*  @lib MPSettROPModel.dll
*  @since 2.0
*/
class CMPRopSettingItem : public CBase
    {
    public:

        /**
        * Two-phased constructor.
        */
        static CMPRopSettingItem* NewLC(TInt aId, const TDesC& aKey);

        /**
        * Destructor.
        */
        virtual ~CMPRopSettingItem();

    private:

        /**
        * C++ default constructor.
        */
        CMPRopSettingItem(TInt aId);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TDesC& aKey);

    private: // Data
        HBufC8* iKey;
        TInt iId;

        TBool iValueChanged;
        TInt iError;
        TInt iIntValue;
        HBufC* iStringValue;
        RArray<TInt> iIntArray;
        TMPRopConfType iType;

    private:    // Friend classes
        friend class CMPSettingsModelForROP;
        friend class CMPSettingsRopConfigParser;
    };


/**
*  CMPSettingsRopConfigParser
*  Config descriptor parser for ROP specific settings.
*
*  @lib MPSettROPModel.dll
*  @since 2.0
*/
class CMPSettingsRopConfigParser : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CMPSettingsRopConfigParser* NewL();
       
        /**
        * Destructor.
        */
        virtual ~CMPSettingsRopConfigParser();

    public: // New functions
        
        /**
        * Parses config string and places parsed setting items to item array.
        * @since 2.0
        * @param aConfigString Configuration string
        * @param aItemArray Item array
        * @return Descriptor containing config string header.
        */
        HBufC8* ParseConfigStringL(const TDesC8& aConfigString, const CArrayPtr<CMPRopSettingItem>* aItemArray);

        /**
        * Creates configuration string from item's data in the item array.
        * @since 2.0
        * @param aItemArray Item array
        * @param aConfigHeader Configuration string's header.
        * @return Descriptor containing configuration string.
        */
        HBufC8* CreateConfigStringL(const CArrayPtr<CMPRopSettingItem>* aItemArray, const TDesC8& aConfigHeader);

    private:

        /**
        * C++ default constructor.
        */
        CMPSettingsRopConfigParser();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:

        /**
        * Parses config header from config string.
        */
        HBufC8* ParseConfigHeaderL();

        /**
        * Returns descriptor containing next config entry.
        */
        HBufC8* NextConfigEntryL(TInt& aError);

        /**
        * Populates entry array from configuration string.
        */
        void PopulateEntryArrayFromConfigStringL();

        /**
        * Populates entry array from item array.
        */
        void PopulateEntryArrayFromItemArrayL();

        /**
        * Creates config entry from setting item.
        */
        HBufC8* CreateConfigEntryL(CMPRopSettingItem* aItem);

        /**
        * Creates configuration string from entry array.
        */
        HBufC8* CreateConfigStringFromEntryArrayL();

        /**
        * Parses next entry from config string.
        */
        TInt GetNextEntry(TPtrC8& aEntry);

        /**
        * Parses setting item data from all config entries.
        */
        void ParseSettingsBufferL();

        /**
        * Parses setting item data from a config entry for predefined setting items.
        */
        TInt GetConfigDataL(const TDesC8& aKey, TPtrC8& aValue, TMPRopConfType& aType);

        /**
        * Parses value string from a config entry.
        */
        TPtrC8 ValueStringPtr(const TDesC8& aValue, TInt& aError);

    private:    // Data

        CArrayPtr<HBufC8>* iEntries;
        const CArrayPtr<CMPRopSettingItem>* iItemArray;

        HBufC8* iConfigHeader;
        HBufC8* iTmpBuf;
        TPtrC8 iBufferPtr;    
    };

#endif      // MPROPCONFIGPARSER_H   
            
// End of File
