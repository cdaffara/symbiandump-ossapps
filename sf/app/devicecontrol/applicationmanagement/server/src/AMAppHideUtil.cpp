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
 * Description: Implementation of applicationmanagement components
 *
 */

#include <centralrepository.h>
#include "AMAppHideUtil.h"
#include "debug.h"

using namespace NApplicationManagement;

// The uid list delimiter
static const TChar KUidDelimiter = ',';

// -----------------------------------------------------------------------------
// CAMAppHideUtil::CAMAppHideUtil()
// -----------------------------------------------------------------------------
CAMAppHideUtil::CAMAppHideUtil() :
    iChanged(EFalse)
    {

    }

// -----------------------------------------------------------------------------
// CAMAppHideUtil::ConstructL()
// -----------------------------------------------------------------------------
void CAMAppHideUtil::ConstructL()
    {
    LoadUidsL();
    }

// -----------------------------------------------------------------------------
// CAMAppHideUtil::~CAMAppHideUtil()
// -----------------------------------------------------------------------------
CAMAppHideUtil::~CAMAppHideUtil()
    {
    iHiddenUids.Close();
    }

// -----------------------------------------------------------------------------
// CAMAppHideUtil::NewL()
// -----------------------------------------------------------------------------
CAMAppHideUtil* CAMAppHideUtil::NewL()
    {
    CAMAppHideUtil *me = new ( ELeave ) CAMAppHideUtil();
    CleanupStack::PushL(me);
    me->ConstructL();
    CleanupStack::Pop(me);
    return me;
    }

// -----------------------------------------------------------------------------
// CAMAppHideUtil::AddUidL(const TUid& aUid )
// -----------------------------------------------------------------------------
void CAMAppHideUtil::AddUidL(const TUid& aUid, TBool aPersist /*= EFalse */)
    {
    TInt idx(iHiddenUids.Find(aUid));
    if (idx == KErrNotFound)
        {
        TInt err(iHiddenUids.Append(aUid) );
        if (err == KErrNone)
            {
            iChanged = ETrue;
            if (aPersist)
                {
                PersistUidsL();
                }
            }
        else
            {
            RDEBUG_2( "Failed to append uid to hidden list: %d", err );
            }
        }
    }

// -----------------------------------------------------------------------------
// CAMAppHideUtil::RemoveUidL( const TUid& aUid )
// -----------------------------------------------------------------------------
void CAMAppHideUtil::RemoveUidL(const TUid& aUid, TBool aPersist /*= EFalse */)
    {
    TInt idx(iHiddenUids.Find(aUid));
    if (idx != KErrNotFound)
        {
        iHiddenUids.Remove(idx);
        iChanged = ETrue;
        if (aPersist)
            {
            PersistUidsL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CAMAppHideUtil::PersistUid( const TUid &aUid, TDes &aBuf, TBool aHasMore )
// -----------------------------------------------------------------------------
void CAMAppHideUtil::PersistUid(const TUid &aUid, TDes &aBuf, TBool aHasMore) const
    {
    aBuf.AppendNumFixedWidth(aUid.iUid, EHex, 8);
    if (aHasMore)
        {
        aBuf.Append(KUidDelimiter);
        }
    }

// -----------------------------------------------------------------------------
// CAMAppHideUtil::PersistUidsL()
// -----------------------------------------------------------------------------
void CAMAppHideUtil::PersistUidsL()
    {
    TInt err = KErrNone;
    if (iChanged)
        {
        TBuf<NCentralRepositoryConstants::KMaxUnicodeStringLength> buf;
        RDEBUG_2( "PersistUids: count %d", iHiddenUids.Count() );
        for (TInt i(iHiddenUids.Count() - 1); i >= 0; --i)
            {
            RDEBUG_3( "PersistUids: cur %d %x", i,iHiddenUids[i].iUid );
            PersistUid(iHiddenUids[i], buf, i > 0) ;
            }
        /*CRepository *rep = CRepository::NewLC(KCRUidMenu);
        TInt err(rep->Set(KMenuHideApplication, buf) );
        CleanupStack::PopAndDestroy(rep);*/
        if (err != KErrNone)
            {
            RDEBUG_2( "PersistUids: ERROR failed to set key: %d", err );
            User::Leave(err);
            }
        RDEBUG_2( "CAMAppHideUtil::PersistUidsL - Saved '%S'", &buf);
        iChanged = EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CAMAppHideUtil::ParseUid( TLex &, TUid &aUid ) 
// -----------------------------------------------------------------------------
TInt CAMAppHideUtil::ParseUid(TLex &aLex, TUid &aUid) const
    {
    aUid = TUid::Null();
    aLex.Mark();
    while (aLex.Peek().IsHexDigit() )
        {
        aLex.Inc();
        }
    TPtrC uidToken = aLex.MarkedToken();
    TLex uidLex(uidToken);
    TInt err(uidLex.Val( (TUint32& )aUid.iUid, EHex) );
    return err;
    }

// ------------------------------------------------------------------------------
// CAMAppHideUtil::SkipDelim( TLex & )
// -----------------------------------------------------------------------------
TBool CAMAppHideUtil::SkipDelim(TLex &aLex) const
    {
    TBool ret(EFalse);
    if (aLex.Peek() == KUidDelimiter)
        {
        aLex.Inc();
        ret = ETrue;
        }
    return (ret );
    }

// -----------------------------------------------------------------------------
// CAMAppHideUtil::LoadUidsL()
// -----------------------------------------------------------------------------
void CAMAppHideUtil::LoadUidsL()
    {
    TInt err = KErrNone;
    TBuf<NCentralRepositoryConstants::KMaxUnicodeStringLength> buf;
    /*CRepository *rep = CRepository::NewLC(KCRUidMenu);
    TBuf<NCentralRepositoryConstants::KMaxUnicodeStringLength> buf;
    TInt err(rep->Get(KMenuHideApplication, buf) );
    CleanupStack::PopAndDestroy(rep);*/
    if (err == KErrNone)
        {
        RDEBUG_2( "CAMAppHideUtil::LoadUidsL - Loading '%S'", &buf);
        TLex lex(buf);
        TUid aUid(TUid::Null());
        do
            {
            err = ParseUid(lex, aUid) ;
            }
        while (err == KErrNone && iHiddenUids.Append(aUid) == KErrNone
                && !lex.Eos() && SkipDelim(lex) );
        }
    RDEBUG_2( "CAMAppHideUtil::LoadUidsL-Loaded (last parsing status: %d)", err);
    }

// -----------------------------------------------------------------------------
// CAMAppHideUtil::Reset()
// -----------------------------------------------------------------------------
void CAMAppHideUtil::Reset()
    {
    if (iHiddenUids.Count() > 0)
        {
        iChanged = ETrue;
        }
    iHiddenUids.Reset();
    }

// End of File
