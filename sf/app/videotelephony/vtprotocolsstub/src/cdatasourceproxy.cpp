/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Data source proxy implementation
*
*/

#include "mvtprotocolhandler.h"
#include "cdatasourceproxy.h"

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// -----------------------------------------------------------------------------
// CVideoSourceProxy::CVideoSourceProxy
// Constructor.
// -----------------------------------------------------------------------------
//
CVideoSourceProxy::CVideoSourceProxy(MVTVideoSource* aDataSource) : iVideoSource(aDataSource) 
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::CVideoSourceProxy<")));
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::CVideoSourceProxy iVideoSource %d>"), iVideoSource));	
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::ConstructL
// -----------------------------------------------------------------------------
//
void CVideoSourceProxy::ConstructL() 
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::ConstructL<")));
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::ConstructL>")));	
    }
    
// -----------------------------------------------------------------------------
// CVideoSourceProxy::NewL
// -----------------------------------------------------------------------------
//
CVideoSourceProxy* CVideoSourceProxy::NewL(MVTVideoSource* aDataSource) 
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::NewL<")));
    CVideoSourceProxy* self = new (ELeave)CVideoSourceProxy(aDataSource);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::NewL>")));  
    return self;
    }
//  End of File
