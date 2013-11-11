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




#ifndef CREATORWEPSECURITYSETTINGS_H_
#define CREATORWEPSECURITYSETTINGS_H_

//
// Methods copied from WEPSecuritySettingsImpl.h 
// (Part of  : WEP Security Settings UI)
// @see \s60\mw\networking\networkingutilities\WEPSecuritySettingsUI\inc\WEPSecuritySettingsImpl.h
//


// INCLUDES

//#include "engine.h"

#include <e32base.h>
#include <metadatabase.h>
using namespace CommsDat;

// FORWARD DECLARATIONS

class CCommsDatabase;

// Index of first key
const TInt KFirstKey = 0;

// Index of second key
const TInt KSecondKey = 1;

// Index of third key
const TInt KThirdKey = 2;

// Index of fourth key
const TInt KFourthKey = 3;

const TInt KMaxNumberofKeys = 4;

// Number of characters for a 232 bits key
const TUint KKeyDataLength232Bits = 58;

// The maximum length of key data
const TUint KMaxLengthOfKeyData = KKeyDataLength232Bits;




class CCreatorWEPSecuritySettings : public CBase
    {
public: 
    static CCreatorWEPSecuritySettings* NewL();
    static CCreatorWEPSecuritySettings* NewLC();
    ~CCreatorWEPSecuritySettings();

private:
    CCreatorWEPSecuritySettings();
    void ConstructL();


public:
/**
* Save to database.
* @param aIapId Wlan Service Table Id of the IAP to be saved
* @param aCommsDb Comms database.
*/
void SaveL( TUint32 aIapId, CCommsDatabase& aCommsDb ) const;

public:
// Enumeration of the possible keys in use
enum TWEPKeyInUse
    {
    EKeyNumber1,            // Key number 1
    EKeyNumber2,            // Key number 2
    EKeyNumber3,            // Key number 3
    EKeyNumber4             // Key number 4
    };


// Enumeration of the possible authentication types
enum TWEPAuthentication
    {
    EAuthOpen,              // Open authentication
    EAuthShared             // Shared authentication
    };


// Possible lengths of the keys
enum TWEPKeyLength
    {
    E40Bits,                // 40 bits
    E104Bits,               // 104 bits
    E232Bits                // 232 bits
    };


// Possible formats of the keys
enum TWEPKeyFormat
    {
    EAscii,                 // Ascii format
    EHexadecimal            // Hex format
    };
    
    

private:

// Index of the key currently in use (EKeyNumber1, EKeyNumber2, 
// EKeyNumber3, EKeyNumber4
CCreatorWEPSecuritySettings::TWEPKeyInUse iKeyInUse;

// Type of authentication (EAuthOpen, EAuthShared)
CCreatorWEPSecuritySettings::TWEPAuthentication iAuthentication;

// Length of the key (E40Bits, E104Bits, E232Bits)
CCreatorWEPSecuritySettings::TWEPKeyLength iKeyLength[KMaxNumberofKeys];

// Format of the key (EAscii, EHexadecimal)
CCreatorWEPSecuritySettings::TWEPKeyFormat iKeyFormat[KMaxNumberofKeys];


    };



#endif /*CREATORWEPSECURITYSETTINGS_H_*/
