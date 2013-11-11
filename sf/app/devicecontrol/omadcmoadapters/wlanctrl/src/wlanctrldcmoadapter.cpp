/*
 * ==============================================================================
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * ==============================================================================
 */

#include <featurecontrol.h>
#include <featmgr.h>
#include <data_caging_path_literals.hrh> 
#include <bautils.h>
#include "wlanctrldcmoadapter.h"
#include "hbtextresolversymbian.h"

_LIT( KdcmoResourceFileName, "deviceupdates_" );	
_LIT( KdcmoResourceFilePath, "z:/resource/qt/translations/" );	
_LIT( KWlanCtrlProperty, "WLAN" );
_LIT( KDisableWlanDescription, "Used to enable/disable the WLAN connectivity." ); // Description


TDCMOStatus CWLanCtrlDCMOAdapter::MapFeatureControlError( TInt aErrorCode )
    {
    TDCMOStatus status( EDcmoFail );
    
    switch ( aErrorCode )
        {
        case KErrNone:
            status = EDcmoSuccess;
            break;
        case KErrPermissionDenied:
        case KErrAccessDenied:
            status = EDcmoAccessDenied;
            break;
        case KErrNotFound:
            status = EDcmoNotFound;
            break;
        default:
            break;
        }
    return status;
    }

// Construction and destruction functions

CWLanCtrlDCMOAdapter* CWLanCtrlDCMOAdapter::NewL( TAny* aParams )
	{
	CWLanCtrlDCMOAdapter* self = new ( ELeave ) CWLanCtrlDCMOAdapter( aParams );  
	CleanupStack::PushL( self );
	self->ConstructL(); 
	CleanupStack::Pop();
	return self;
	}

CWLanCtrlDCMOAdapter::~CWLanCtrlDCMOAdapter()
	{	
	FeatureManager::UnInitializeLib();
	
	delete iProperty;
	delete iDescription;
	}

CWLanCtrlDCMOAdapter::CWLanCtrlDCMOAdapter( TAny* aInitParams )
:  iInitParams( ( CDCMOInterface::TDCMOInterfaceInitParams* ) aInitParams )
	{
	// See ConstructL() for initialisation completion.
	}

void CWLanCtrlDCMOAdapter::ConstructL()
	{	
	// Set up the data to pass back
	iProperty = KWlanCtrlProperty().AllocL();
	iDescription = KDisableWlanDescription().AllocL();
	
    FeatureManager::InitializeLibL();

	}


// Implementation of CDCMOInterface
TDCMOStatus CWLanCtrlDCMOAdapter::SetDCMOPluginIntAttributeValueL(TDCMONode aId, TInt aValue)
{
	TDCMOStatus status(EDcmoFail);
	TInt err( KErrNone );
	
	switch( aId )
	{
		case EDenyUserEnable:
		    status = EDcmoNotSupported;
		    break;
		case ENotifyUser:
		    status = EDcmoNotSupported;
			break;
		case EEnable:	
		    {
            RFeatureControl featureControl;
            TUid wlanUid( TUid::Uid( KFeatureIdProtocolWlan ) ); 
            err = featureControl.Connect();
            
            if ( err == KErrNone )
                {
                if( aValue == 0 )
                    {
                    // Disable WLAN
                    err = featureControl.DisableFeature( wlanUid );
		            }
                else if ( aValue == 1 )
                    {
                    // Enable WLAN
                    err = featureControl.EnableFeature( wlanUid );
                    }
                status = MapFeatureControlError( err );
                featureControl.Close();
                }
            
		    }
		    break;
		default:
		    break;
	}
	
	return status;
}

TDCMOStatus CWLanCtrlDCMOAdapter::GetDCMOPluginIntAttributeValueL(TDCMONode aId, TInt& aValue)
{
	TDCMOStatus status( EDcmoSuccess );
	
	switch(aId)
	{
		case EGroup: 
		    aValue = EConnectivity;
		    break;
		case EAttached:
		    aValue = EFalse;
		    break;
		case EEnabled:	
		    aValue = FeatureManager::FeatureSupported( KFeatureIdProtocolWlan );		    
		    break;    
		case EDenyUserEnable:
		    aValue = EFalse;
		    break; 
		case ENotifyUser:
		    aValue = ETrue;
		    break;
		case EEnable:
		    status = EDcmoAccessDenied;
		    break;
		default:
		    break;
	}
	
	return status;
}

TDCMOStatus CWLanCtrlDCMOAdapter::SetDCMOPluginStrAttributeValueL( TDCMONode   /* aId */, 
                                                                   const TDes& /* aStrValue */ )
{
	// Nothing to do
	TDCMOStatus status( EDcmoFail );
	
	return status;	
}

TDCMOStatus CWLanCtrlDCMOAdapter::GetDCMOPluginStrAttributeValueL( TDCMONode aId, TDes& aStrValue ) 
{
	TDCMOStatus status( EDcmoSuccess );
	switch( aId )
	{
		case EProperty: 	
		    aStrValue = *iProperty;
		    break;
		case EDescription:	
		    aStrValue = *iDescription;
		    break;
		default:			
		    status = EDcmoNotFound;
		    break;
	}
		
	return status;
}

void CWLanCtrlDCMOAdapter::GetLocalizedNameL( HBufC*& aLocName )
    {
    	TBool result = HbTextResolverSymbian::Init(KdcmoResourceFileName, KdcmoResourceFilePath );					
			_LIT(KTextWLAN, "txt_device_update_info_wlan");
			aLocName = HbTextResolverSymbian::LoadL(KTextWLAN);	
    }
