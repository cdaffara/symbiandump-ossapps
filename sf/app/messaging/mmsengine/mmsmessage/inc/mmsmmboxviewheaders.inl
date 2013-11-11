/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     Inline methods for CMmsMMBoxViewHeaders
*
*/



inline TUint CMmsMMBoxViewHeaders::MmsStart() const
    {
    return iMmsStart;
    }

inline void CMmsMMBoxViewHeaders::SetMmsStart ( TUint aMmsStart )
    {
    iMmsStart = aMmsStart;
    }

inline TUint CMmsMMBoxViewHeaders::MmsLimit() const
    {
    return iMmsLimit;
    }

inline void CMmsMMBoxViewHeaders::SetMmsLimit ( TUint aMmsLimit )
    {
    iMmsLimit = aMmsLimit;
    }

inline TInt CMmsMMBoxViewHeaders::MmsTotals() const
    {
    return iMmsTotals;
    }

inline void CMmsMMBoxViewHeaders::SetMmsTotals ( TInt aMmsTotals )
    {
    iMmsTotals = aMmsTotals;
    }

inline TInt CMmsMMBoxViewHeaders::MmsQuotas() const
    {
    return iMmsQuotas;
    }

inline void CMmsMMBoxViewHeaders::SetMmsQuotas ( TInt aMmsQuotas )
    {
    iMmsQuotas = aMmsQuotas;
    }

inline TUint CMmsMMBoxViewHeaders::MmsMessageCount() const
    {
    return iMmsMessageCount;
    }

inline void CMmsMMBoxViewHeaders::SetMmsMessageCount ( TUint aMmsMessageCount )
    {
    iMmsMessageCount = aMmsMessageCount;
    }

inline TUint CMmsMMBoxViewHeaders::MMBoxTotalNumber() const
    {
    return iMMBoxTotalNumber;
    }

inline void CMmsMMBoxViewHeaders::SetMMBoxTotalNumber ( TUint aMMBoxTotalNumber )
    {
    iMMBoxTotalNumber = aMMBoxTotalNumber;
    }

inline TUint CMmsMMBoxViewHeaders::MMBoxTotalSize() const
    {
    return iMMBoxTotalSize;
    }

inline void CMmsMMBoxViewHeaders::SetMMBoxTotalSize ( TUint aMMBoxTotalSize )
    {
    iMMBoxTotalSize = aMMBoxTotalSize;
    }

inline TUint CMmsMMBoxViewHeaders::MMBoxQuotaNumber() const
    {
    return iMMBoxQuotaNumber;
    }

inline void CMmsMMBoxViewHeaders::SetMMBoxQuotaNumber ( TUint aMMBoxQuotaNumber )
    {
    iMMBoxQuotaNumber = aMMBoxQuotaNumber;
    }

inline TUint CMmsMMBoxViewHeaders::MMBoxQuotaSize() const
    {
    return iMMBoxQuotaSize;
    }

inline void CMmsMMBoxViewHeaders::SetMMBoxQuotaSize ( TUint aMMBoxQuotaSize )
    {
    iMMBoxQuotaSize = aMMBoxQuotaSize;
    }

inline RArray<TUint>& CMmsMMBoxViewHeaders::AttributeArray()
    {
    return iAttributes; //lint !e1536
    }

inline RArray<TInt>& CMmsMMBoxViewHeaders::MMStateArray()
    {
    return iMmsMMStates; //lint !e1536
    }

// End of File
