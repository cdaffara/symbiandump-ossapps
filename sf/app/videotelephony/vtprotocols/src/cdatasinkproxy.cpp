/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include <pv_plugin_interfaces.h>
#include "mvtprotocolhandler.h"
#include "cdatasinkproxy.h"

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// -----------------------------------------------------------------------------
// CPVDisplaySinkNodeProxy::SetSupplier
// Set new supplier .
// -----------------------------------------------------------------------------
//
void CPVDisplaySinkNodeProxy::SetSupplier(MPVDataSourceBase* aSupplier)
    {
    __IF_DEBUG(Print(_L("VTProto: CPVDisplaySinkNodeProxy::SetSupplier<")));
    iSupplier = aSupplier;
    __IF_DEBUG(Print( _L( "VTProto: CPVDisplaySinkNodeProxy::SetSupplier>" ) ));
    }

// -----------------------------------------------------------------------------
// CPVDisplaySinkNodeProxy::BufferEmptiedL
// Method called by a data sink to pass back an emptied buffer to the source.
// -----------------------------------------------------------------------------
//
void CPVDisplaySinkNodeProxy::BufferEmptiedL(CMMFBuffer* aBuffer)
    {
    __IF_DEBUG(Print( _L( "VTProto: CPVDisplaySinkNodeProxy::BufferEmptiedL<" ) ));
    iSupplier->BufferEmptiedL(aBuffer);   //call actually PV code
    __IF_DEBUG(Print( _L( "VTProto: CPVDisplaySinkNodeProxy::BufferEmptiedL>" ) ));    
    }

// -----------------------------------------------------------------------------
// CPVDisplaySinkNodeProxy::CPVDisplaySinkNodeProxy
// Constructor.
// -----------------------------------------------------------------------------
//
CPVDisplaySinkNodeProxy::CPVDisplaySinkNodeProxy() : MVTVideoSource(KNullUid) 
    {
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::CDisplaySinkProxy
// Constructor.
// -----------------------------------------------------------------------------
//
CDisplaySinkProxy::CDisplaySinkProxy(MVTVideoSink* aDisplaySink) : MPVDataSink(KNullUid), iDisplaySink(aDisplaySink) 
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
    iDSNodeProxy = new (ELeave)CPVDisplaySinkNodeProxy();
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

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::QueryInterface
// This API is to allow for extensibility of the plugin interface.
// -----------------------------------------------------------------------------
//
void CDisplaySinkProxy::QueryInterface( const TPVUuid& aUuid, TPVInterfacePtr& aInterfacePtr )
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::QueryInterface<" ) ));
    if ( aUuid == KPVUidVideoOutputInterface )
        {
        MPVVideoInput* interfaceP = (MPVVideoInput*) this;
        aInterfacePtr = (TPVInterfacePtr) interfaceP;
        }
    else
        {
        aInterfacePtr = NULL;
        }
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::QueryInterface aInterfacePtr %d>" ), aInterfacePtr ));  
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::GetMultimediaTypesL
// This API returns multimedias type supported by the data source/sink.
// -----------------------------------------------------------------------------
//
const RArray<TPVMIMEType *>& CDisplaySinkProxy::GetMultimediaTypesL() const
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::GetMultimediaTypesL" ) ));	
    return iDisplaySink->GetMultimediaTypesL();
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::QueryUUID
// This API is to allow for extensibility of the plugin interface.
// -----------------------------------------------------------------------------
//
void CDisplaySinkProxy::QueryUUID( const TPVMIMEType& aMimeType, RArray<TPVUuid>& aUuids, bool aExactUuidsOnly)
    {    
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::SetTimebaseL
// This API is used to provide a timebase for plugins that deal with synchronized media data.
// -----------------------------------------------------------------------------
//
void CDisplaySinkProxy::SetTimebaseL(MPVSymbianTimebaseInterface* aTimebase)
    {    
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::EmptyBufferL
// Method called by a MDataSource to request the data sink to empty aBuffer of data.
// -----------------------------------------------------------------------------
//
void CDisplaySinkProxy::EmptyBufferL(CMMFBuffer *aBuffer, MPVDataSourceBase* aSupplier, TMediaId)
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::EmptyBufferL<" ) ));	
    iDSNodeProxy->SetSupplier(aSupplier);
    CPVMMFPointerBuffer& pvMMFBuffer = *static_cast< CPVMMFPointerBuffer* >( aBuffer );
    TVtMMFDataBuffer mmfDataBuffer(aBuffer, pvMMFBuffer.GetFrameSize(), static_cast<TPtr8&>(pvMMFBuffer.Data()));
    iDisplaySink->EmptyBufferL(mmfDataBuffer, iDSNodeProxy, TMediaId());
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::EmptyBufferL>" ) ));	
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::BufferFilledL
// Method called by a data source to pass back an filled buffer to the sink.
// -----------------------------------------------------------------------------
//
void CDisplaySinkProxy::BufferFilledL(CMMFBuffer* aBuffer)
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::BufferFilledL" ) ));	
    return iDisplaySink->BufferFilledL(aBuffer);
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::CanCreateSinkBuffer
// Method to indicate whether the data sink can create a buffer.
// -----------------------------------------------------------------------------
//
TBool CDisplaySinkProxy::CanCreateSinkBuffer()
    {
     __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::CanCreateSinkBuffer" ) ));	
    return iDisplaySink->CanCreateSinkBuffer();
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::CreateSinkBufferL
// Returns a buffer created by the data sink
// -----------------------------------------------------------------------------
//
CMMFBuffer* CDisplaySinkProxy::CreateSinkBufferL(TMediaId aMediaId, TBool &aReference )
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::CreateSinkBufferL" ) ));		
    return iDisplaySink->CreateSinkBufferL(aMediaId, aReference);
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::SinkThreadLogon
// Method to 'logon' the data sink to the same thread that sink will be consuming data in.
// -----------------------------------------------------------------------------
//
TInt CDisplaySinkProxy::SinkThreadLogon( MAsyncEventHandler& aEventHandler )
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::SinkThreadLogon" ) ));		
    return iDisplaySink->SinkThreadLogon(aEventHandler);
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::SinkThreadLogoff
// Method to 'logoff' the data sink from the same thread that sink consumes data in.
// -----------------------------------------------------------------------------
//
void CDisplaySinkProxy::SinkThreadLogoff()
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::SinkThreadLogoff" ) ));	
    iDisplaySink->SinkThreadLogoff();
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::SinkPrimeL
// Method to 'prime' the data sink.
// -----------------------------------------------------------------------------
//
TInt CDisplaySinkProxy::SinkPrimeL()
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::SinkPrimeL" ) ));		
    return iDisplaySink->SinkPrimeL();
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::SinkPlayL
// Method to 'play' the data sink.
// -----------------------------------------------------------------------------
//
TInt CDisplaySinkProxy::SinkPlayL()
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::SinkPlayL" ) ));	
    return iDisplaySink->SinkPlayL();
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::SinkPauseL
// Method to 'pause' the data sink.
// -----------------------------------------------------------------------------
//
TInt CDisplaySinkProxy::SinkPauseL()
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::SinkPauseL" ) ));	
    return iDisplaySink->SinkPauseL();
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::SinkStopL
// Method to 'stop' the data sink.
// -----------------------------------------------------------------------------
//
TInt CDisplaySinkProxy::SinkStopL()
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::SinkStopL" ) ));
    return iDisplaySink->SinkStopL();
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::SetFormatL
// Set the video frame format.  This must be from the list of supported formats.
// -----------------------------------------------------------------------------
//
void CDisplaySinkProxy::SetFormatL(const TDesC8& aFormat)
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::SetFormatL" ) ));
    iDisplaySink->SetFormatL(aFormat);
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::SetVideoFrameSizeL
// Set the video frame size.
// -----------------------------------------------------------------------------
//
void CDisplaySinkProxy::SetVideoFrameSizeL(const TSize& aSize)
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::SetVideoFrameSizeL" ) ));
    iDisplaySink->SetVideoFrameSizeL(aSize);
    }

// -----------------------------------------------------------------------------
// CDisplaySinkProxy::GetVideoFrameSizeL
// Get the video frame size.
// -----------------------------------------------------------------------------
//
void CDisplaySinkProxy::GetVideoFrameSizeL(TSize& aSize) const
    {
    __IF_DEBUG(Print( _L( "VTProto: CDisplaySinkProxy::GetVideoFrameSizeL" ) ));
    iDisplaySink->GetVideoFrameSizeL(aSize);
    }
//  End of File 
