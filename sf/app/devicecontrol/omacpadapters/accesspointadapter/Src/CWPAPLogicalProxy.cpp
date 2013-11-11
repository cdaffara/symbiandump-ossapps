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
* Description:    Defines a class, which holds logical proxy information.
*
*/





// INCLUDE FILES

#include "CWPAPLogicalProxy.h"
#include <e32svr.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include "CWPLog.h"
#include "CWPAPPhysicalProxy.h"
#include "CWPAPPort.h"
#include "WPAPAdapter.pan"
#include "WPAPDefs.h"
#include <cmconnectionmethodext.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPAPLogicalProxy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPAPLogicalProxy* CWPAPLogicalProxy::NewLC( 
                                        const TDesC& aDefaultName, 
                                        CWPCharacteristic& aCharacteristic )
	{
	CWPAPLogicalProxy* self = new(ELeave) CWPAPLogicalProxy( aCharacteristic, 
                                                             aDefaultName ); 
	CleanupStack::PushL( self );
	self->ConstructL();
    aCharacteristic.AcceptL( *self );
	return self;
	}
	
// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CWPAPLogicalProxy::~CWPAPLogicalProxy()
	{
	delete iPort;
	iPhysicalProxies.ResetAndDestroy();
	iPhysicalProxies.Close();
	}

// -----------------------------------------------------------------------------
// CWPAPLogicalProxy::ValidateL
// -----------------------------------------------------------------------------
//
TBool CWPAPLogicalProxy::ValidateL()
	{
	// Logical proxy is valid if it contains either one or more
	// physical proxies
	return iPhysicalProxies.Count() > 0;
	}



// -----------------------------------------------------------------------------
// CWPAPLogicalProxy::AddDataL
// -----------------------------------------------------------------------------
//
void CWPAPLogicalProxy::AddDataL( RCmConnectionMethodExt& aCmItem )
	{    
	LOG("--CWPAP CWPAPLogicalProxy::AddDataL begin--");	
	// This method is called by child physical proxy. 

	// Store name if came along with the document. If missing then access point
	// name is used.
	if ( iName )
		{
		// CMManager
		aCmItem.SetStringAttributeL( CMManager::ECmName, iName->Value() );
		}

	// Store homepage if defined.
	if ( iHomepage )
		{
		// CMManager 
		TRAPD( err, aCmItem.SetStringAttributeL( CMManager::ECmStartPage, iHomepage->Value() ));
		
		LOG2("CWPAP EApWapStartPage, value: %S, err: %d", &iHomepage->Value(), err);
		User::LeaveIfError( err );
		}

	// Store port data, which overrides the data stored by child physical proxy.

	if ( iPort )
		{
		//CMManager 
		iPort->AddDataL (aCmItem);
		
		}
	LOG("--CWPAP CWPAPLogicalProxy::AddDataL end--");	
	}


// -----------------------------------------------------------------------------
// CWPAPLogicalProxy::AddItemsL
// -----------------------------------------------------------------------------
//
TInt CWPAPLogicalProxy::AddItemsL( RPointerArray<CWPAPAccesspointItem>& aItems,
                                   CWPAPItemBase* /*aLogicalProxy*/,
                                   CWPAPItemBase* /*aPhysicalProxy*/ ) 
	{
    TInt count( 0 );
    for( TInt i( 0 ); i < iPhysicalProxies.Count(); i++ )
        {
        CWPAPPhysicalProxy* pp = iPhysicalProxies[ i ];
        if( pp->ValidateL() )
            {
            count += pp->AddItemsL( aItems, this, NULL );
            }
        }

    return count;
    }

// -----------------------------------------------------------------------------
// CWPAPLogicalProxy::Name
// -----------------------------------------------------------------------------
//
const TDesC& CWPAPLogicalProxy::Name()
	{
	if ( iName )
		{
		return iName->Value();
		}
	else
		{
		return KNullDesC;
		}
	}

// -----------------------------------------------------------------------------
// CWPAPLogicalProxy::VisitL
// -----------------------------------------------------------------------------
//
void CWPAPLogicalProxy::VisitL( CWPCharacteristic& aCharacteristic )
	{
	TInt type = aCharacteristic.Type();

	// KWPPort
	
	// One valid port is enough for us.
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

	// KWPPxPhysical

	else if ( type == KWPPxPhysical )
		{
		CWPAPPhysicalProxy* physicalProxy = CWPAPPhysicalProxy::NewLC( 
                                                            iDefaultName, 
                                                            aCharacteristic,
                                                            iCharacteristic,
                                                            iPort );

		if ( physicalProxy->ValidateL() )
			{
            User::LeaveIfError( iPhysicalProxies.Append( physicalProxy ) );
			CleanupStack::Pop( physicalProxy );
			}
		else // Data not valid.
			{
			CleanupStack::PopAndDestroy( physicalProxy );
			}
		}
	else if ( type == KWPPxAuthInfo )
		{
		// Not supported
		}
	}

// -----------------------------------------------------------------------------
// CWPAPLogicalProxy::VisitL
// -----------------------------------------------------------------------------
//
void CWPAPLogicalProxy::VisitL( CWPParameter& aParameter)
	{
	TInt id = aParameter.ID();
	const TDesC& value = aParameter.Value();

	if ( value.Length() == 0 )
		{
		// No use process zero length value.
		return;
		}

	switch ( id )
		{
		case EWPParameterStartPage: // iHomepage
			{
			if ( !iHomepage )
				{
				iHomepage = &aParameter;
				}
			break;
			}
		case EWPParameterName:
			{
			if ( !iName )
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
// CWPAPLogicalProxy::CWPAPLogicalProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPAPLogicalProxy::CWPAPLogicalProxy( CWPCharacteristic& aCharacteristic,
                                      const TDesC& aDefaultName )
                                    : CWPAPItemBase( aDefaultName ),
                                      iPhysicalProxies( KInitialArraySize ),
                                      iCharacteristic( aCharacteristic )
	{
	}

// -----------------------------------------------------------------------------
// CWPAPLogicalProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPAPLogicalProxy::ConstructL()
	{
	}

//  End of File
