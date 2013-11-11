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
* Description:    Defines a class, which holds physical proxy information.
*
*/





// INCLUDE FILES

#include "CWPAPPhysicalProxy.h"				// Own header
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <WPAdapterUtil.h>
#include <featmgr.h>
#include "CWPAPLogicalProxy.h"
#include "CWPAPPort.h"
#include "CWPAPNapdef.h"
#include "WPAPAdapter.pan"
#include "CWPLog.h"
#include "WPAPDefs.h"
#include <cmconnectionmethodext.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPAPPhysicalProxy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPAPPhysicalProxy* CWPAPPhysicalProxy::NewLC( 
                                            const TDesC& aDefaultName, 
                                            CWPCharacteristic& aCharacteristic, 
    CWPCharacteristic& aLogicalCharacteristic,
    CWPAPPort*& aPort )
	{
	CWPAPPhysicalProxy* self = new(ELeave) CWPAPPhysicalProxy( 
	                                                    aDefaultName, 
                                                        aLogicalCharacteristic,
                                                        aPort ); 
	CleanupStack::PushL( self );
	self->ConstructL();
    aCharacteristic.AcceptL( *self );
	return self;
	}
	
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CWPAPPhysicalProxy::~CWPAPPhysicalProxy()
	{
	delete iPort;

    iNapdefs.ResetAndDestroy();
    iNapdefs.Close();
	}

// -----------------------------------------------------------------------------
// CWPAPPhysicalProxy::ValidateL
// -----------------------------------------------------------------------------
//
TBool CWPAPPhysicalProxy::ValidateL()
	{
	TBool isValid( EFalse );

	// We must have proxy address otherwise this proxy isn't valid.
	if ( iProxyAddr )
		{
		switch ( iPxAddrType )
			{
			case EPxAddrTypeIPv4:
				{
				isValid = WPAdapterUtil::CheckIPv4( iProxyAddr->Value() );
				break;
				}
            case EPxAddrTypeIPv6:
                {
                isValid = FeatureManager::FeatureSupported( KFeatureIdIPv6 )
                    && WPAdapterUtil::CheckIPv6( iProxyAddr->Value() );
                break;
                }
			case EPxAddrTypeUnsupported:
				{
				isValid = EFalse;
				break;
				}
			default: // default value is assumed to be EPxAddrTypeIPv4
				{
				isValid = WPAdapterUtil::CheckIPv4( iProxyAddr->Value() );
				}
			}
		}
	return isValid;
	}

// -----------------------------------------------------------------------------
// CWPAPPhysicalProxy::AddDataL
// -----------------------------------------------------------------------------
//
void CWPAPPhysicalProxy::AddDataL( RCmConnectionMethodExt& aCmItem )
	{  
	LOG("--CWPAP CWPAPPhysicalProxy::AddDataL begin--");	
	
	// CMManager
	aCmItem.SetStringAttributeL( CMManager::ECmWapIPGatewayAddress, iProxyAddr->Value());
	LOG1("CWPAP EApWapStartPage, value: %S ", &iProxyAddr->Value());

	//CMManager
	aCmItem.SetStringAttributeL( CMManager::ECmIPGateway, iProxyAddr->Value());
	LOG1("CWPAP EApIspIPGateway, value: %S ", &iProxyAddr->Value());

	//CMManager 
	aCmItem.SetStringAttributeL( CMManager::ECmProxyServerName, iProxyAddr->Value() );
	LOG1("CWPAP EApProxyServerAddress, value: %S ", &iProxyAddr->Value());
	                                               

	// Store port data
	if ( iPort )
        {
        iPort->AddDataL( aCmItem );
        }
    
    if ( iLogicalPort)
        {
        iLogicalPort->AddDataL( aCmItem );
        }
  
	LOG("--CWPAP CWPAPPhysicalProxy::AddDataL end--");	
	}

// -----------------------------------------------------------------------------
// CWPAPPhysicalProxy::AddItemsL
// -----------------------------------------------------------------------------
//
TInt CWPAPPhysicalProxy::AddItemsL( RPointerArray<CWPAPAccesspointItem>& aItems,
                                    CWPAPItemBase* iLogicalProxy, 
                                    CWPAPItemBase* /*iPhysicalProxy*/ ) 
	{
    TInt count( 0 );
    for( TInt i( 0 ); i < iNapdefs.Count(); i++ )
        {
        CWPAPNapdef* pp = iNapdefs[ i ];
        if( pp->ValidateL() )
            {
            count += pp->AddItemsL( aItems, iLogicalProxy, this );
            }
        }

    return count;
    }

// -----------------------------------------------------------------------------
// CWPAPPhysicalProxy::Name
// -----------------------------------------------------------------------------
//
const TDesC& CWPAPPhysicalProxy::Name()
	{
    if( iName )
        {
        return iName->Value();
        }
    else 
        {
        return KNullDesC;
        }
	}

// -----------------------------------------------------------------------------
// CWPAPPhysicalProxy::VisitL
// -----------------------------------------------------------------------------
//
void CWPAPPhysicalProxy::VisitL( CWPCharacteristic& aCharacteristic )
	{
	TInt type = aCharacteristic.Type();
	
	// Accept only KWPPort characteristics
	// One valid port is enough for us
	if ( type == KWPPort && !iPort )
		{
        CWPAPPort* port = CWPAPPort::NewLC( aCharacteristic );

		if ( port->ValidateL() )
			{
			iPort = port;
			CleanupStack::Pop( port );
			}
		else // Data not valid.
			{
			CleanupStack::PopAndDestroy( port );
			}
		}
	}

// -----------------------------------------------------------------------------
// CWPAPPhysicalProxy::VisitL
// -----------------------------------------------------------------------------
//
void CWPAPPhysicalProxy::VisitL( CWPParameter& aParameter)
	{
	TInt id = aParameter.ID();
	const TDesC& value = aParameter.Value();

	if ( value.Length() == 0 )
		{
		// No use to store zero length value.
		return;
		}

	switch ( id )
		{
		case EWPParameterPxAddrFQDN: // iGatewayIPAddress
			{
			// Implementation comments:
			// The value of this field is a domain name but we can't 
			// at the moment resolve this to IP address 
			// (at least from this adapter).
			// Therefore this field can't be used. 
			// EWPParameterPxAddr is supported
			// and should be used.

			/// Validate the domain name
			/// Resolve to IP address

			/* // Uncomment if domain name can be validated and IP address resolved
			// Add only if we haven't already received this field
			if ( !IsReceived( EWPParameterPxAddrFQDN_id ))
				{
				iGatewayIPAddress = &aParameter;
				MarkReceived( EWPParameterPxAddrFQDN_id );
				}
			*/
			break;
			}
		case EWPParameterPxAddr: // iGatewayIPAddress
			{
			// Accept only first occurrence of either PxAddr or PxAddrFQDN
			if ( IsReceived( EWPParameterPxAddrFQDN_id ) ||
				IsReceived( EWPParameterPxAddr_id ) )
				{
				return;
				}
			else 
				{
				// The max allowed lenght for proxy address is 45 bytes. 
				// Nothing is stored if max lenght is exceeded
				if ( aParameter.Value().Length() < 46 )
					{
					iProxyAddr = &aParameter;
					MarkReceived( EWPParameterPxAddr_id );
					}
				}
			break;
			}
		case EWPParameterPxAddrType:
			{
			if ( !IsReceived( EWPParameterPxAddrType_id ) )
				{
				// Only IPv4 addresses are supported.
				if ( value == KIPV4 )
					{
					iPxAddrType = EPxAddrTypeIPv4;
					}
                else if( value == KIPV6 )
                    {
                    iPxAddrType = EPxAddrTypeIPv6;
                    }
				else
					{
					iPxAddrType = EPxAddrTypeUnsupported;
					}
				MarkReceived( EWPParameterPxAddrType_id );
				}
			break;
			}
        case EWPParameterName:
            {
            if( !iName )
                {
                iName = &aParameter;
                }
            break;
            }
		default:
			{
			// Just let through
			}
		}
	}

// -----------------------------------------------------------------------------
// CWPAPPhysicalProxy::VisitL
// -----------------------------------------------------------------------------
//
void CWPAPPhysicalProxy::VisitLinkL( CWPCharacteristic& aLink )
	{
    CWPAPNapdef* item = CWPAPNapdef::NewLC( ETrue,
                                            iDefaultName,
                                            iLogicalCharacteristic,
                                            aLink );
    User::LeaveIfError( iNapdefs.Append( item ) );
    CleanupStack::Pop( item ); // item
    }

// -----------------------------------------------------------------------------
// CWPAPPhysicalProxy::CWPAPPhysicalProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPAPPhysicalProxy::CWPAPPhysicalProxy( 
                               const TDesC& aDefaultName, 
                               CWPCharacteristic& aLogicalCharacteristic,
                               CWPAPPort*& aPort ) 
	                         : CWPAPItemBase( aDefaultName ), 
                               iLogicalPort( aPort ),
                               iNapdefs( KInitialArraySize ),
                               iLogicalCharacteristic( aLogicalCharacteristic )
	{
	}

// -----------------------------------------------------------------------------
// CWPAPPhysicalProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPAPPhysicalProxy::ConstructL()
	{
	}

// End of File
