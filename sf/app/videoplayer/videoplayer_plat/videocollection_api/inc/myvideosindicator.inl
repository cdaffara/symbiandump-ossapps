/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    My Videos indicator plugin inlines.*
*/



#ifndef MYVIDEOSINDICATOR_INL
#define MYVIDEOSINDICATOR_INL

// -----------------------------------------------------------------------------
// Instantiates appropriate plugin implementation.
// -----------------------------------------------------------------------------
//
inline CMyVideosIndicator* CMyVideosIndicator::NewL()
    {
    TEComResolverParams resolverParams;
    TAny* ext = REComSession::CreateImplementationL( 
        TUid::Uid( KMyVideosIndicatorInterfaceUid ), 
        _FOFF( CMyVideosIndicator, iDtor_ID_Key ),
        resolverParams );

    CMyVideosIndicator* result = reinterpret_cast<CMyVideosIndicator*>( ext );

    return result;
    }

// -----------------------------------------------------------------------------
// Interface's destructor
// -----------------------------------------------------------------------------
//
inline CMyVideosIndicator::~CMyVideosIndicator()
    {
    // Inform the ECOM framework that this specific instance of the
    // interface has been destroyed.
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }

#endif // MYVIDEOSINDICATOR_INL
