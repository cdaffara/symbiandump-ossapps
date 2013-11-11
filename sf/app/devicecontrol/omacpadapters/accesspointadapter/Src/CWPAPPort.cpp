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
* Description:    Defines a class, which holds proxy port information.
*
*/





// INCLUDE FILES

#include "CWPAPPort.h"					// Own header
#include "WPAPAdapter.pan"
#include "CWPLog.h"
#include "WPAPDefs.h"

#include <CWPCharacteristic.h>
#include <CWPParameter.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPAPPort::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPAPPort* CWPAPPort::NewLC( CWPCharacteristic& aCharacteristic )
	{
	CWPAPPort* self = new(ELeave) CWPAPPort(); 
	CleanupStack::PushL( self );
	self->ConstructL();
    aCharacteristic.AcceptL( *self );
	return self;
	}

// -----------------------------------------------------------------------------
// CWPAPPort::CWPAPPort
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPAPPort::CWPAPPort() : CWPAPItemBase( KNullDesC )
	{
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CWPAPPort::~CWPAPPort()
	{
	}

// -----------------------------------------------------------------------------
// CWPAPPort::Validate
// -----------------------------------------------------------------------------
//
TBool CWPAPPort::ValidateL()
	{
	// If we got either service or portnumber and 
	// Those we successfully interpreted the port
	// information is valid.
	if ( IsReceived( EWPParameterService_id ) ||
		IsReceived( EWPParameterPortNbr_id ))
		{
		return ETrue;
		}
	LOG("CWPAP ERROR! Port not valid");
	return EFalse; // Nothing received so this is invalid.
	}

// -----------------------------------------------------------------------------
// CWPAPPort::AddData
// -----------------------------------------------------------------------------
//
void CWPAPPort::AddDataL( RCmConnectionMethodExt& aCmItem )
	{    
	LOG("--CWPAP CWPAPPort::AddDataL begin--");	
	TInt err(KErrNone);
	
	// CMManager
	TRAP (err, aCmItem.SetBoolAttributeL( CMManager::ECmWapIPSecurity, iConnectionSecurity ));
	LOG2("CWPAP EApWapSecurity, value: %d, err: %d", iConnectionSecurity, err);

	// CMManager
	TRAP (err, aCmItem.SetIntAttributeL( CMManager::ECmWapIPWSPOption, iSessionMode ));
	LOG2("CWPAP EApWapWspOption, value: %d, err: %d", iSessionMode, err);

	if ( iPortNbr )
		{
		const TDesC& value = iPortNbr->Value();
		TLex lex( value );
		TUint32 intValue=0;
		err = lex.Val( intValue, EDecimal );
		if ( err == KErrNone )
			{
			if( intValue > 65535 )
				{
				intValue = 0;
				}
				
			// CMManager 
			TRAP (err, aCmItem.SetIntAttributeL( CMManager::ECmProxyPortNumber, intValue ));
			LOG2("CWPAP EApProxyPortNumber, value: %d, err: %d",
			                                                 intValue, err);
			}
		}
	LOG("--CWPAP CWPAPPort::AddDataL end--");	
	}

// -----------------------------------------------------------------------------
// CWPAPPort::Name
// -----------------------------------------------------------------------------
//
const TDesC& CWPAPPort::Name()
	{
    return KNullDesC;
	}

// -----------------------------------------------------------------------------
// CWPAPPort::AddItemsL
// -----------------------------------------------------------------------------
//
TInt CWPAPPort::AddItemsL( RPointerArray<CWPAPAccesspointItem>& /*aItems*/,
                           CWPAPItemBase* /*aLogicalProxy*/,
                           CWPAPItemBase* /*aPhysicalProxy*/ ) 
	{
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CWPAPPort::VisitL
// -----------------------------------------------------------------------------
//
void CWPAPPort::VisitL( CWPCharacteristic& /*aCharacteristic*/ )
	{	
	}

// -----------------------------------------------------------------------------
// CWPAPPort::VisitL
// -----------------------------------------------------------------------------
//
void CWPAPPort::VisitL( CWPParameter& aParameter )
	{
	TInt id = aParameter.ID();
	const TDesC& value = aParameter.Value();

	if ( value.Length() == 0 )
		{
		// No use of checking the zero legth parameter.
		return;
		}

	switch ( id )
		{
		case EWPParameterService: // iConnectionSecurity and iSessionMode
			{
			// If EWPParameterService value have already received
			// or service type has been solved from EWPParameterPortNbr value
			// don't accept this anymore. 
			if ( IsReceived( EWPParameterService_id ) )
				{
				break;
				}

			if ( value == KCLWSP)
				{
				// CMManager
				SetValues( EFalse, CMManager::ECmWapWspOptionConnectionless, 
					EWPParameterPortNbr_id );
				}
			else if ( value == KCOWSP)
				{
				// CMManager
				SetValues( EFalse, CMManager::ECmWapWspOptionConnectionOriented, 
					EWPParameterPortNbr_id );
					
				}
			else if ( value == KCLSECWSP)
				{
				// CMManager
				SetValues( ETrue, CMManager::ECmWapWspOptionConnectionless, 
					EWPParameterPortNbr_id );
				}
			else if ( value == KCOSECWSP)
				{
				// CMManager
				SetValues( ETrue, CMManager::ECmWapWspOptionConnectionOriented, 
					EWPParameterPortNbr_id );
				}
			else
				{
				// Not supported...
				}
			break;
			}
		case EWPParameterPortNbr: // iConnectionSecurity and iSessionMode
			{
			// If EWPParameterPortNbr value have already handled 
			// don't accept it anymore.
			if ( IsReceived( EWPParameterPortNbr_id ) )
				{
				break;
				}

			// Store the pointer to port parameter
			iPortNbr = &aParameter;
            MarkReceived( EWPParameterPortNbr_id );

			if ( value == K9200 )
				{
				// CMManager
				SetValues( EFalse, CMManager::ECmWapWspOptionConnectionless, 
					EWPParameterPortNbr_id );
					
				// Type of service identified. 
				// No need for Service parameter anymore.
				MarkReceived( EWPParameterService_id );
				}
			else if ( value == K9201 || value == K80 ||	value == K8080)
				{
				// CMManager
				SetValues( EFalse, CMManager::ECmWapWspOptionConnectionOriented, 
					EWPParameterPortNbr_id );
				MarkReceived( EWPParameterService_id );
				}
			else if ( value == K9202 )
				{
				// CMManager
				SetValues( ETrue, CMManager::ECmWapWspOptionConnectionless, 
					EWPParameterPortNbr_id );
				MarkReceived( EWPParameterService_id );
				}
			else if ( value == K9203 || value == K443 )
				{
				// CMManager
				SetValues( ETrue, CMManager::ECmWapWspOptionConnectionOriented, 
					EWPParameterPortNbr_id );
				MarkReceived( EWPParameterService_id );
				}
			else
				{
				// Not supported...
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
// CWPAPPort::SetValues
// -----------------------------------------------------------------------------
//
void CWPAPPort::SetValues( TBool aConnectionSecurity, 
                           TInt aSessionMode,
	                       TMarkedField aReceivedField )
	{
	__ASSERT_DEBUG( aReceivedField == EWPParameterService_id || 
                    aReceivedField == EWPParameterPortNbr_id,
                    Panic( EPreConditionFail ) );

	iConnectionSecurity = aConnectionSecurity;
	iSessionMode = aSessionMode;

	MarkReceived( aReceivedField );
	}

// -----------------------------------------------------------------------------
// CWPAPPort::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPAPPort::ConstructL()
	{
	}

//  End of File
