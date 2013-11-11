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
#include "cdatasourceproxy.h"

#ifdef _DEBUG
#    define __IF_DEBUG(t) {RDebug::t;}
#else
#    define __IF_DEBUG(t)
#endif

// -----------------------------------------------------------------------------
// CPVCameraSourceNodeProxy::CPVCameraSourceNodeProxy
// Constructor.
// -----------------------------------------------------------------------------
//
CPVCameraSourceNodeProxy::CPVCameraSourceNodeProxy() : MVTVideoSink(KNullUid) 
    { 
    }

// -----------------------------------------------------------------------------
// CPVCameraSourceNodeProxy::SetConsumer
// Set consumer object.
// -----------------------------------------------------------------------------
//
void CPVCameraSourceNodeProxy::SetConsumer(MPVDataSinkBase* aConsumer)
    {
    __IF_DEBUG(Print(_L("VTProto: CPVCameraSourceNodeProxy::SetConsumer<")));
    iConsumer = aConsumer;    
    __IF_DEBUG(Print(_L("VTProto: CPVCameraSourceNodeProxy::SetConsumer>")));
    }

// -----------------------------------------------------------------------------
// CPVCameraSourceNodeProxy::BufferFilledL
// Method called by a data source to pass back an filled buffer to the sink.
// -----------------------------------------------------------------------------
//
void CPVCameraSourceNodeProxy::BufferFilledL(CMMFBuffer* aBuffer)
    {
    __IF_DEBUG(Print(_L("VTProto: CPVCameraSourceNodeProxy::BufferFilledL<")));
    iConsumer->BufferFilledL(aBuffer);
    __IF_DEBUG(Print(_L("VTProto: CPVCameraSourceNodeProxy::BufferFilledL>")));
    }

// -----------------------------------------------------------------------------
// CPVCameraSourceNodeProxy::SendEventToClient
// Send event to client.
// -----------------------------------------------------------------------------
//
TInt CPVCameraSourceNodeProxy::SendEventToClient(const TMMFEvent &aEvent)
    {
    __IF_DEBUG(Print(_L("VTProto: CPVCameraSourceNodeProxy::SendEventToClient<")));	
    MAsyncEventHandler* eventHandler = dynamic_cast<MAsyncEventHandler*>(iConsumer);
    TInt result = eventHandler->SendEventToClient(aEvent);
    __IF_DEBUG(Print(_L("VTProto: CPVCameraSourceNodeProxy::SendEventToClient>")));
    return result;
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::CVideoSourceProxy
// Constructor.
// -----------------------------------------------------------------------------
//
CVideoSourceProxy::CVideoSourceProxy(MVTVideoSource* aDataSource) : MPVDataSource(KNullUid), iVideoSource(aDataSource) 
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
    iCameraSourceNodeProxy = new (ELeave) CPVCameraSourceNodeProxy();
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

// -----------------------------------------------------------------------------
// CVideoSourceProxy::QueryInterface
// This API is to allow for extensibility of the plugin interface.
// -----------------------------------------------------------------------------
//
void CVideoSourceProxy::QueryInterface( const TPVUuid& aUuid, TPVInterfacePtr& aInterfacePtr )
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::QueryInterface<")));	
    if ( aUuid == KPVUidVideoInputInterface )
            {
            MPVVideoInput* interfaceP = (MPVVideoInput*) this;
            aInterfacePtr = (TPVInterfacePtr) interfaceP;
            }
        else
            {
            aInterfacePtr = NULL;
            }
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::QueryInterface aInterfacePtr %d<"), aInterfacePtr));        
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::GetMultimediaTypesL
// This API returns multimedias type supported by the data source/sink.
// -----------------------------------------------------------------------------
//
const RArray<TPVMIMEType *>& CVideoSourceProxy::GetMultimediaTypesL() const
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::GetMultimediaTypesL iVideoSource %d"), iVideoSource));	
    const RArray<TPVMIMEType *>& multimediaType = iVideoSource->GetMultimediaTypesL();
    return multimediaType;
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::QueryUUID
// This API is to allow for extensibility of the plugin interface.
// -----------------------------------------------------------------------------
//
void CVideoSourceProxy::QueryUUID( const TPVMIMEType& aMimeType, RArray<TPVUuid>& aUuids, bool aExactUuidsOnly)
    {
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::SetTimebaseL
// This API is used to provide a timebase for plugins that deal with synchronized media data.
// -----------------------------------------------------------------------------
//
void CVideoSourceProxy::SetTimebaseL(MPVSymbianTimebaseInterface* aTimebase)
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::SetTimebaseL<")));
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::SetTimebaseL>")));
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::FillBufferL
// Method called by a MDataSink to request the data source to fill aBuffer with data.
// -----------------------------------------------------------------------------
//
void CVideoSourceProxy::FillBufferL(CMMFBuffer* aBuffer, MPVDataSinkBase* aConsumer, TMediaId aMediaId )
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::FillBufferL<")));
    iCameraSourceNodeProxy->SetConsumer(aConsumer);
    iVideoSource->FillBufferL(aBuffer, iCameraSourceNodeProxy, aMediaId);
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::FillBufferL>")));
    }    

// -----------------------------------------------------------------------------
// CVideoSourceProxy::CreateSourceBufferL
// Returns a buffer created by the data source.
// -----------------------------------------------------------------------------
//
CMMFBuffer* CVideoSourceProxy::CreateSourceBufferL(TMediaId aMediaId, TBool &aReference )
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::CreateSourceBufferL")));
    return iVideoSource->CreateSourceBufferL(aMediaId, aReference);
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::CreateSourceBufferL
// Returns a buffer created by the data source.
// -----------------------------------------------------------------------------
//
CMMFBuffer* CVideoSourceProxy::CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer &aSinkBuffer, TBool &aReference)
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::CreateSourceBufferL")));	
    return iVideoSource->CreateSourceBufferL(aMediaId, aSinkBuffer, aReference);
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::SourceThreadLogon
// Method to 'logon' the data source to the same thread that source will be supplying data in.
// -----------------------------------------------------------------------------
//
TInt CVideoSourceProxy::SourceThreadLogon( MAsyncEventHandler& aEventHandler )
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::SourceThreadLogon")));	
    return iVideoSource->SourceThreadLogon(aEventHandler);
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::SourceThreadLogoff
// Method to 'logoff' the data source from the same thread that source supplies data in.
// -----------------------------------------------------------------------------
//
void CVideoSourceProxy::SourceThreadLogoff()
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::SourceThreadLogoff<")));		
    iVideoSource->SourceThreadLogoff();
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::SourceThreadLogoff>")));
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::SourcePrimeL
// Method to 'prime' the data source.
// -----------------------------------------------------------------------------
//
TInt CVideoSourceProxy::SourcePrimeL()
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::SourcePrimeL<")));			
    return iVideoSource->SourcePrimeL();
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::SourcePlayL
// Method to 'play' the data source.
// -----------------------------------------------------------------------------
//
TInt CVideoSourceProxy::SourcePlayL()
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::SourcePlayL")));	
    return iVideoSource->SourcePlayL();
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::SourcePauseL
// Method to 'pause' the data source.
// -----------------------------------------------------------------------------
//
TInt CVideoSourceProxy::SourcePauseL()
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::SourcePauseL")));
    return iVideoSource->SourcePauseL();
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::SourceStopL
// Method to 'stop' the data source.
// -----------------------------------------------------------------------------
//
TInt CVideoSourceProxy::SourceStopL()
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::SourceStopL")));	
    return iVideoSource->SourceStopL();
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::BufferEmptiedL
// Method called by a data sink to pass back an emptied buffer to the source.
// -----------------------------------------------------------------------------
//
void CVideoSourceProxy::BufferEmptiedL(CMMFBuffer* aBuffer)
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::BufferEmptiedL")));		
    iVideoSource->BufferEmptiedL(aBuffer);
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::CanCreateSourceBuffer
// Method to indicate whether the data source can create a buffer.
// -----------------------------------------------------------------------------
//
TBool CVideoSourceProxy::CanCreateSourceBuffer()
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::CanCreateSourceBuffer")));		
    return iVideoSource->CanCreateSourceBuffer();
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::SetFormatL
// Set the video frame format.  This must be from the list of supported formats.
// -----------------------------------------------------------------------------
//
void CVideoSourceProxy::SetFormatL(const TDesC8& aFormat)
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::SetFormatL")));		
    iVideoSource->SetFormatL(aFormat);
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::SetFrameRateL
// Set the video frame rate.  This must be within the range of supported frame rates.
// -----------------------------------------------------------------------------
//
void CVideoSourceProxy::SetFrameRateL(TReal32 aFrameRate)
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::SetFrameRateL")));		
    iVideoSource->SetFrameRateL(aFrameRate);
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::SetVideoFrameSizeL
// Set the video frame size.
// -----------------------------------------------------------------------------
//
void CVideoSourceProxy::SetVideoFrameSizeL(const TSize& aSize)
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::SetVideoFrameSizeL <")));		
    iVideoSource->SetVideoFrameSizeL(aSize);
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::SetVideoFrameSizeL >")));	
    }

// -----------------------------------------------------------------------------
// CVideoSourceProxy::GetVideoFrameSizeL
// Get the video frame size.
// -----------------------------------------------------------------------------
//
void CVideoSourceProxy::GetVideoFrameSizeL(TSize& aSize) const
    {
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::GetVideoFrameSizeL<")));		
    iVideoSource->GetVideoFrameSizeL(aSize);
    __IF_DEBUG(Print(_L("VTProto: CVideoSourceProxy::GetVideoFrameSizeL>")));
    }
//  End of File
