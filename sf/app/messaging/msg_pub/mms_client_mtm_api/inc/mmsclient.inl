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
*     Inline functions for CMmsClientMtm
*
*/



inline TUid CMmsClientMtm::Type() const
    {
    return CBaseMtm::Type();
    }

inline CMsvSession& CMmsClientMtm::Session()
    {
    return CBaseMtm::Session();
    }

inline void CMmsClientMtm::SetCurrentEntryL( CMsvEntry * aEntry )
    {
    CBaseMtm::SetCurrentEntryL( aEntry );
    }

inline void CMmsClientMtm::SwitchCurrentEntryL( TMsvId aId )
    {
    CBaseMtm::SwitchCurrentEntryL( aId );
    }

inline CMsvEntry& CMmsClientMtm::Entry() const
    {
    return CBaseMtm::Entry();
    }

inline TBool CMmsClientMtm::HasContext() const
    {
    return CBaseMtm::HasContext();
    }

inline CRichText& CMmsClientMtm::Body()
    {
    return CBaseMtm::Body();
    }

inline const CRichText& CMmsClientMtm::Body() const
    {
    return CBaseMtm::Body();
    }

// End of File
