/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
*     Inline methods for CMmsSendingChain
*
*/



inline TUint CMmsSendingChain::Order() const
    {
    return iOrder;
    }

inline void CMmsSendingChain::SetOrder( TUint aOrder )
    {
    iOrder = aOrder;
    }

inline TInt64 CMmsSendingChain::Date() const
    {
    return iDate;
    }

inline void CMmsSendingChain::SetDate( const TInt64 aDate )
    {
    iDate = aDate;
    }

// End of File
