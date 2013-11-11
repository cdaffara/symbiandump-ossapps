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


#include "csortkeyarray.h"

namespace {

#ifdef _DEBUG
    
enum TPanicReason
    {
    EPanicPreCond_SetText
    };
    
void Panic(TPanicReason aReason)
    {
    _LIT(KPanicText, "CSortKeyArray");
    User::Panic(KPanicText, aReason);
    }

#endif // _DEBUG

} // namespace

inline CSortKeyArray::CSortKeyArray()
    {
    }

CSortKeyArray* CSortKeyArray::NewL()
    {
    return new(ELeave) CSortKeyArray;
    }

CSortKeyArray::~CSortKeyArray()
    {
    iKeyTypes.Close();
    iTexts.Close();
    }

void CSortKeyArray::AppendL(const TSortKey& aKey)
    {
    User::LeaveIfError(iKeyTypes.Append(aKey.Type()));
    TInt err = iTexts.Append(aKey.Text());
    if (err != KErrNone)
        {
        // if appending to iTexts was not successful remove also the
        // item in iKeyTypes to keep arrays in sync.
        iKeyTypes.Remove(iKeyTypes.Count() - 1);
        User::Leave(err);
        }
    }

void CSortKeyArray::SetText(const TDesC& aText, TInt aIndex)
    {
    __ASSERT_DEBUG(aIndex < iTexts.Count(), Panic(EPanicPreCond_SetText));
    iTexts[aIndex].Set(aText);
    }

void CSortKeyArray::Reset()
    {
    iKeyTypes.Reset();
    iTexts.Reset();
    }

TInt CSortKeyArray::SortKeyCount() const
    {
    return iKeyTypes.Count();
    }

TSortKey CSortKeyArray::SortKeyAt(TInt aIndex) const
    {
    return TSortKey(iTexts[aIndex], iKeyTypes[aIndex]);
    }

// End of File
