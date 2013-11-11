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
* Description:  Data sink proxy implementation
*
*/

#include "mvtprotocolhandler.h"
#include "cdatasinkproxy.h"

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::CDisplaySinkProxy
// Constructor.
// -----------------------------------------------------------------------------
//
CDisplaySinkProxy::CDisplaySinkProxy(MVTVideoSink* aDisplaySink) : iDisplaySink(aDisplaySink) 
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::CDisplaySinkProxy<" ) ));
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::CDisplaySinkProxy iDisplaySink %d>" ), iDisplaySink));
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::ConstructL
// -----------------------------------------------------------------------------
//
void CDisplaySinkProxy::ConstructL()
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::ConstructL<" ) ));
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::ConstructL>" ) ));
    }


// -----------------------------------------------------------------------------
// CDisplaySinkProxy::NewL
// -----------------------------------------------------------------------------
//
CDisplaySinkProxy* CDisplaySinkProxy::NewL(MVTVideoSink* aDisplaySink)
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::NewL<" ) ));
    CDisplaySinkProxy* self = new (ELeave)CDisplaySinkProxy(aDisplaySink);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::NewL>" ) ));
    return self;
    }
//  End of File 
