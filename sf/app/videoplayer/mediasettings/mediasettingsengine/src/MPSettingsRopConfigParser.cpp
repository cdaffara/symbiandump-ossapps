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
* Description:   Config descriptor parser for ROP specific settings.*
*/


// Version : %version: 3 %




// INCLUDE FILES
#include    <utf.h>
#include    "MPSettingsRopConfigParser.h"
#include	"mpxlog.h"

// CONSTANTS
const TInt KMPRopEntryArrayGranularity = 16;

_LIT8(KMPRopTypeInteger, "Integer");
_LIT8(KMPRopTypeString, "String");
_LIT8(KMPRopEntrySeparator, ";");
_LIT8(KMPRopKeySeparator, ":");
_LIT8(KMPRopValueSeparator, "=");
_LIT8(KMPRopStringConstrainer, "\"");
_LIT8(KMPRopArrayOpeningMarker, "{");
_LIT8(KMPRopArrayClosingMarker, "}");
_LIT8(KMPRopArraySeparator, ",");
_LIT8(KMPRopIntegerEntryPrototype, "%S:Integer = %d;");
_LIT8(KMPRopStringEntryPrototype, "%S:String = \"%S\";");

const TInt KMPRopIntegerEntryPrototypeStaticLength = 23; // static length + 11 chars for the (32bit) TInt
const TInt KMPRopStringEntryPrototypeStaticLength = 13;


// ============================ MEMBER FUNCTIONS ===============================

// *******************************
// *** class CMPRopSettingItem ***
// *******************************

// -----------------------------------------------------------------------------
// CMPRopSettingItem::CMPRopSettingItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPRopSettingItem::CMPRopSettingItem(TInt aId) : iId(aId),
    iValueChanged(EFalse)
    {
    MPX_DEBUG1(_L("#MS# CMPRopSettingItem::CMPRopSettingItem()"));
    }

// -----------------------------------------------------------------------------
// CMPRopSettingItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMPRopSettingItem::ConstructL(const TDesC& aKey)
    {
    MPX_DEBUG2(_L("#MS# CMPRopSettingItem::ConstructL(%S)"),&aKey);
    iKey = HBufC8::NewL(aKey.Length());
    TPtr8 ptr = iKey->Des();
    CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr, aKey);
    }

// -----------------------------------------------------------------------------
// CMPRopSettingItem::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPRopSettingItem* CMPRopSettingItem::NewLC(TInt aId, const TDesC& aKey)
    {
    MPX_DEBUG3(_L("#MS# CMPRopSettingItem::NewLC(%d,%S)"),aId,&aKey);
    CMPRopSettingItem* self = new(ELeave) CMPRopSettingItem(aId);
    
    CleanupStack::PushL(self);
    self->ConstructL(aKey);

    return self;
    }
   
// -----------------------------------------------------------------------------
// CMPRopSettingItem::~CMPRopSettingItem
// Destructor
// -----------------------------------------------------------------------------
//
CMPRopSettingItem::~CMPRopSettingItem()
    {
    MPX_DEBUG1(_L("#MS# CMPRopSettingItem::~CMPRopSettingItem()"));
    if (iKey) 
		{
		delete iKey;
		}
    delete iStringValue;
    iIntArray.Close();
    }


// ****************************************
// *** class CMPSettingsRopConfigParser ***
// ****************************************

// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::CMPSettingsRopConfigParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMPSettingsRopConfigParser::CMPSettingsRopConfigParser()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::CMPSettingsRopConfigParser()"));
    }
   
// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::~CMPSettingsRopConfigParser
// Destructor
// -----------------------------------------------------------------------------
//
CMPSettingsRopConfigParser::~CMPSettingsRopConfigParser()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::~CMPSettingsRopConfigParser()"));
	if (iEntries)
      iEntries->ResetAndDestroy();
    delete iEntries;
    delete iTmpBuf;
    delete iConfigHeader;
    }

// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMPSettingsRopConfigParser::ConstructL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::ConstructL()"));
    iEntries = new(ELeave) CArrayPtrFlat<HBufC8>(KMPRopEntryArrayGranularity);
    }

// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMPSettingsRopConfigParser* CMPSettingsRopConfigParser::NewL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::NewL()"));
    CMPSettingsRopConfigParser* self = new(ELeave) CMPSettingsRopConfigParser;
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::ParseConfigStringL
// -----------------------------------------------------------------------------
//
HBufC8* CMPSettingsRopConfigParser::ParseConfigStringL(const TDesC8& aConfigString, const CArrayPtr<CMPRopSettingItem>* aItemArray)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::ParseConfigStringL()"));
    iBufferPtr.Set(aConfigString);
    iItemArray = aItemArray;
    
    // Parse header
    HBufC8* header = ParseConfigHeaderL();
    CleanupStack::PushL(header);

    // Read all config entries to an array
    PopulateEntryArrayFromConfigStringL();

    // Parse config entries from the array, delete items from the entry array
    ParseSettingsBufferL();
    iEntries->ResetAndDestroy();

    CleanupStack::Pop(); // header

    return header;
    }

// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::CreateConfigStringL
// -----------------------------------------------------------------------------
//
HBufC8* CMPSettingsRopConfigParser::CreateConfigStringL(const CArrayPtr<CMPRopSettingItem>* aItemArray, const TDesC8& aConfigHeader)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::CreateConfigStringL()"));
    HBufC8* configString = NULL;
    iItemArray = aItemArray;

    delete iConfigHeader;
    iConfigHeader = NULL;
    iConfigHeader = HBufC8::NewL(aConfigHeader.Length() + 1); // magic, leave room for line separator (';')
    TPtr8 tmp = iConfigHeader->Des();

    tmp.Append(aConfigHeader);
    tmp.Append(KMPRopEntrySeparator);

    PopulateEntryArrayFromItemArrayL();
    
    // If iEntries->Count() == 1 -> iEntries contains only config header ->
    // none of the setting values has change => no need to create config string
    if (iEntries->Count() > 1)
        {
        configString = CreateConfigStringFromEntryArrayL();
        }

    iEntries->ResetAndDestroy();

    return configString;
    }

// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::ParseConfigHeaderL
// -----------------------------------------------------------------------------
//
HBufC8* CMPSettingsRopConfigParser::ParseConfigHeaderL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::ParseConfigHeaderL()"));
    HBufC8* buf = NULL;
    TPtrC8 ptr;
    TInt error = GetNextEntry(ptr);
    if (error)
        {
        // If header line cannot be found, the string is corrupted => Leave
        User::Leave(KErrCorrupt);
        }

    buf = ptr.AllocL();
    return buf;
    }

// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::NextConfigEntryL
// -----------------------------------------------------------------------------
//
HBufC8* CMPSettingsRopConfigParser::NextConfigEntryL(TInt& aError)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::NextConfigEntryL()"));
    HBufC8* buf = NULL;

    TPtrC8 ptr;
    aError = GetNextEntry(ptr);

    if (!aError)
        {
        buf = ptr.AllocL();
        buf->Des().Trim();
        }

    return buf;
    }

// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::PopulateEntryArrayFromConfigStringL
// -----------------------------------------------------------------------------
//
void CMPSettingsRopConfigParser::PopulateEntryArrayFromConfigStringL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::PopulateEntryArrayFromConfigStringL()"));
    TInt error(KErrNone);
    iEntries->ResetAndDestroy();

    do
        {
        HBufC8* buf = NextConfigEntryL(error);

        if (!error)
            {
            iEntries->AppendL(buf);
            }
        }
    while (!error);

    if (error != KErrNotFound)
        {
        User::Leave(error);
        }
    }

// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::PopulateEntryArrayFromItemArrayL
// -----------------------------------------------------------------------------
//
void CMPSettingsRopConfigParser::PopulateEntryArrayFromItemArrayL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::PopulateEntryArrayFromItemArrayL()"));
    CMPRopSettingItem* item = NULL;
    HBufC8* buf = NULL;
    iEntries->ResetAndDestroy();

    // Append config string header to array
    iEntries->AppendL(iConfigHeader);
    iConfigHeader = NULL;

    TInt count = iItemArray->Count();
    for (TInt i = 0; i < count; ++i)
        {
        item = iItemArray->At(i);

        if (item->iValueChanged)
            {
            buf = CreateConfigEntryL(item);

            if (buf)
                {
                CleanupStack::PushL(buf);
                iEntries->AppendL(buf);
                CleanupStack::Pop(); // buf
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::CreateConfigEntryL
// -----------------------------------------------------------------------------
//
HBufC8* CMPSettingsRopConfigParser::CreateConfigEntryL(CMPRopSettingItem* aItem)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::CreateConfigEntryL()"));
    HBufC8* buf = NULL;

    switch (aItem->iType)
        {
        case EMPRopConfTypeInteger:
            {
            buf = HBufC8::NewL(aItem->iKey->Length() + KMPRopIntegerEntryPrototypeStaticLength);
            TPtr8 ptr = buf->Des();
            ptr.Format(KMPRopIntegerEntryPrototype, aItem->iKey, aItem->iIntValue);
            break;
            }
        case EMPRopConfTypeString:
            {
            HBufC* str = aItem->iStringValue;
            TInt strLength = str->Length();
            HBufC8* str8 = HBufC8::NewLC(strLength);
            TPtr8 str8Ptr = str8->Des();
            CnvUtfConverter::ConvertFromUnicodeToUtf8(str8Ptr, *str);
            
            buf = HBufC8::NewL(aItem->iKey->Length() + strLength + KMPRopStringEntryPrototypeStaticLength);
            TPtr8 ptr = buf->Des();
            ptr.Format(KMPRopStringEntryPrototype, aItem->iKey, &(*str8));
            CleanupStack::PopAndDestroy(); //str8
            break;
            }
        default:
            break;
        }

    return buf;
    }


// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::GetNextEntry
// -----------------------------------------------------------------------------
//
TInt CMPSettingsRopConfigParser::GetNextEntry(TPtrC8& aEntry)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::GetNextEntry()"));
    TInt error(KErrNone);

    TInt offset = iBufferPtr.Find(KMPRopEntrySeparator);
    if (offset != KErrNotFound)
        {
        aEntry.Set(iBufferPtr.Left(offset));
        iBufferPtr.Set(iBufferPtr.Mid(++offset)); // don't include ';'
        }
    else
        {
        error = KErrNotFound;
        }

    return error;
    }

// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::ParseSettingsBufferL
// -----------------------------------------------------------------------------
//
void CMPSettingsRopConfigParser::ParseSettingsBufferL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::ParseSettingsBufferL()"));
    CMPRopSettingItem* item;
    TPtrC8 value;

    TInt count = iItemArray->Count();
    for (TInt i = 0; i < count; ++i)
        {
        item = iItemArray->At(i);

        item->iError = GetConfigDataL(*item->iKey, value, item->iType);
        
        if (!item->iError)
            {
            switch (item->iType)
                {
                case EMPRopConfTypeInteger:
                    {
                    // Parse integer value
                    TLex8 lex(value);
                    item->iError = lex.Val(item->iIntValue);
                    break;
                    }
                case EMPRopConfTypeIntArray:
                    {
                    HBufC8* tmp = NULL;
                    TInt offset = 0; // Magic: should not be KErrNotFound initially
                    TInt error = KErrNone;
                    TInt intVal = 0;
                    item->iIntArray.Reset();
                    
                    while (offset != KErrNotFound && !error)
                        {
                        offset = value.Find(KMPRopArraySeparator);

                        if (offset != KErrNotFound)
                            {
                            tmp = value.Left(offset).AllocLC();
                            }
                        else
                            {
                            tmp = value.AllocLC();
                            }

                        tmp->Des().Trim();
                        TLex8 lex(*tmp);
                        error = lex.Val(intVal);

                        if (!error)
                            {
                            User::LeaveIfError(item->iIntArray.Append(intVal));

                            if (offset != KErrNotFound)
                                {
                                value.Set(value.Mid(offset + 1));
                                }
                            }

                        CleanupStack::PopAndDestroy(); // tmp
                        tmp = NULL;
                        }
                    
                    item->iError = error;
                    break;
                    }
                case EMPRopConfTypeString:
                    // Parse string value
                    value.Set(ValueStringPtr(value, item->iError));

                    if (!item->iError)
                        {
                        delete item->iStringValue;
                        item->iStringValue = NULL;
                        item->iStringValue = HBufC::NewL(value.Length());
    
                        TPtr tmp = item->iStringValue->Des();
                        CnvUtfConverter::ConvertToUnicodeFromUtf8(tmp, value);
                        }
                    break;
                default:
                    item->iError = KErrNotSupported;
                    break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::GetConfigDataL
// -----------------------------------------------------------------------------
//
TInt CMPSettingsRopConfigParser::GetConfigDataL(const TDesC8& aKey, TPtrC8& aValue, TMPRopConfType& aType)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::GetConfigDataL()"));
    TInt ret = KErrNotFound;
    TInt offset = KErrNotFound;
    HBufC8* buf = NULL;

    // Try to find aConfigKey from the beginning of each entry untill one is found
    // (offset has to be 0, as there can be several different config keys that
    // include some other config key, e.g. "Timeout" vs. "ConnTimeout").
    for (TInt i = 0; offset != 0 && i < iEntries->Count(); i++)
        {
        buf = iEntries->At(i);
        offset = buf->Find(aKey);
        }

    // Parse type and value if correct key was found
    if (offset == 0)
        {
        offset = buf->Find(KMPRopKeySeparator);
        TInt offset2 = buf->Find(KMPRopValueSeparator);
        TInt arrayOffset = buf->Find(KMPRopArrayOpeningMarker);

        if (offset != KErrNotFound && offset2 != KErrNotFound && offset < offset2)
            {
            ++offset; // Move past the key separator

            delete iTmpBuf;
            iTmpBuf = NULL;
            iTmpBuf = buf->Mid(offset, offset2 - offset).AllocL();
            iTmpBuf->Des().Trim();
            
            // Parse type
            if (*iTmpBuf == TPtrC8(KMPRopTypeInteger))
                {
                if (arrayOffset == KErrNotFound)
                    {
                    aType = EMPRopConfTypeInteger;
                    }
                else
                    {
                    aType = EMPRopConfTypeIntArray;
                    }
                }
            else if (*iTmpBuf == TPtrC8(KMPRopTypeString))
                {
                aType = EMPRopConfTypeString;
                }
            else
                {
                ret = KErrNotSupported;
                }
 
            if (ret != KErrNotSupported)
                {
                // Parse value
                delete iTmpBuf;
                iTmpBuf = NULL;

                if (aType == EMPRopConfTypeIntArray)
                    {
                    TInt arrayOffset2 = buf->Find(KMPRopArrayClosingMarker);

                    if (arrayOffset2 != KErrNotFound && arrayOffset < arrayOffset2)
                        {
                        // Copy, but do not include markers
                        iTmpBuf = buf->Mid(arrayOffset + 1, arrayOffset2 - arrayOffset - 1).AllocL();
                        ret = KErrNone;
                        }
                    else
                        {
                        return KErrCorrupt;
                        }
                    }
                else
                    {
                    iTmpBuf = buf->Mid(++offset2).AllocL(); // Move past the value separator
                    ret = KErrNone;
                    }

                iTmpBuf->Des().Trim();

                aValue.Set(*iTmpBuf);
                }
            }
        }

    return ret;
    }

// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::ValueStringPtr
// -----------------------------------------------------------------------------
//
TPtrC8 CMPSettingsRopConfigParser::ValueStringPtr(const TDesC8& aValue, TInt& aError)
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::ValueStringPtr()"));
    TPtrC8 ptr;
    aError = KErrNotFound;
    // Find first string constrainer
    TInt offset = aValue.Find(KMPRopStringConstrainer);
    
    if (offset != KErrNotFound)
        {
        TPtrC8 tmp;
        // 1.) Set offset1 to point the first character of the config string
        // 2.) Set characters after the first string constrainer to tmp
        tmp.Set(aValue.Mid(++offset));
        // Find second string constrainer
        offset = tmp.Find(KMPRopStringConstrainer);

        if (offset != KErrNotFound)
            {
            // Set characters between the string constrainers to aString
            ptr.Set(tmp.Left(offset));
            aError = KErrNone;
            }
        }

    return ptr;
    }

// -----------------------------------------------------------------------------
// CMPSettingsRopConfigParser::CreateConfigStringFromEntryArrayL
// -----------------------------------------------------------------------------
//
HBufC8* CMPSettingsRopConfigParser::CreateConfigStringFromEntryArrayL()
    {
    MPX_DEBUG1(_L("#MS# CMPSettingsRopConfigParser::CreateConfigStringFromEntryArrayL()"));
    TInt length(0);
    TInt count = iEntries->Count();
    for (TInt i = 0; i < count; ++i)
        {
        length += iEntries->At(i)->Length();
        }

    HBufC8* configString = HBufC8::NewL(length);
    TPtr8 ptr = configString->Des();

    for (TInt j = 0; j < count; ++j)
        {
        ptr.Append(*iEntries->At(j));
        }

    return configString;
    }

//  End of File  
