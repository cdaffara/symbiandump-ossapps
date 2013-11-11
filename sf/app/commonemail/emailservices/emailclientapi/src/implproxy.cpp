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
* Description:  ECom impementation proxy for Email client API
*
*/

#include "emailclientapiimpl.hrh"
#include "emailclientapiimpl.h"
#include "emailinterfacefactoryimpl.h"
#include <implementationproxy.h>	//TImplementationProxy

// -----------------------------------------------------------------------------
// Defines the pairing required by the ECOM framework to correctly identify the
// instantiation method pointer for client's request.
// -----------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(
            KEmailClientApiImplUid,
            CEmailInterfaceFactoryImpl::NewL )
    };

// -----------------------------------------------------------------------------
// Returns the TImplementationProxy for email client api implementation
// -----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

    return ImplementationTable;
    }

// End of file
