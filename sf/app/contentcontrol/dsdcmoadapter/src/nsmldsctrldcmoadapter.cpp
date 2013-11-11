/*
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
* Description:  Implementation of DSCtrl plug-in adapter for DCMO
*
*/

//SYSTEM INCLUDES
#include <centralrepository.h>
#include <e32base.h>
#include <stringresourcereader.h> 
#include <f32file.h> 
#include <data_caging_path_literals.hrh>

//USER INCLUDES
#include "nsmldsctrldcmoadapter.h"
#include "hbtextresolversymbian.h"

// CONSTANTS
const TUid KCRUidDSDCMOConfig = { 0x20022FC0 };
const TInt KNsmlDesktopSync = 0; // 0x0000000 defined in DS cenrep

// Name of the Plug-in adapter
_LIT(KNsmlDesktopSyncTitle, "Desktop Sync"); 
// Description for Desktop Sync adapter
_LIT(KNsmlDSDCMODescription, "Used to enable/disable the Desktop Sync." );
// Filename in which title is present
_LIT( KdcmoResourceFileName, "deviceupdates_" );
// Path to the translation file
_LIT( KdcmoResourceFilePath, "z:/resource/qt/translations/" );	

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNsmlDsCtrlDCMOAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CNsmlDsCtrlDCMOAdapter* CNsmlDsCtrlDCMOAdapter::NewL(TAny* aParams)
	{
	CNsmlDsCtrlDCMOAdapter* self=new(ELeave) CNsmlDsCtrlDCMOAdapter(aParams);  
	CleanupStack::PushL(self);
	self->ConstructL(); 
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CNsmlDsCtrlDCMOAdapter::~CNsmlDsCtrlDCMOAdapter()
// Destructor.
// -----------------------------------------------------------------------------
CNsmlDsCtrlDCMOAdapter::~CNsmlDsCtrlDCMOAdapter()
	{	
	delete iProperty;
	delete iDescription;
	}

// -----------------------------------------------------------------------------
// CNsmlDsCtrlDCMOAdapter::CNsmlDsCtrlDCMOAdapter
// Constructor.
// -----------------------------------------------------------------------------
CNsmlDsCtrlDCMOAdapter::CNsmlDsCtrlDCMOAdapter(TAny* aInitParams)
:  iInitParams((CDCMOInterface::TDCMOInterfaceInitParams*)aInitParams)
	{
	// See ConstructL() for initialisation completion.
	}

// -----------------------------------------------------------------------------
// CNsmlDsCtrlDCMOAdapter::ConstructL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
void CNsmlDsCtrlDCMOAdapter::ConstructL()
    // Safely complete the initialization of the constructed object	
	{	
	// Set up the data to pass back
	if(iInitParams->descriptor)
	    {
            iProperty = (iInitParams->descriptor)->AllocL();
	    }
	else
	    {
            iProperty = KNsmlDesktopSyncTitle().AllocL();
	    }
	iDescription = KNsmlDSDCMODescription().AllocL();
	}


// Implementation of CDCMOInterface
// -----------------------------------------------------------------------------
// CNsmlDsCtrlDCMOAdapter::GetDCMOPluginIntAttributeValueL
// - Get the Value of any attribute which is an integer
// - If its existing attribute, then success is returned. Else Failure.
// -----------------------------------------------------------------------------
//
TDCMOStatus CNsmlDsCtrlDCMOAdapter::GetDCMOPluginIntAttributeValueL(TDCMONode aId, TInt& aValue)
{
	TDCMOStatus status(EDcmoSuccess);
	TInt err(KErrNone);
	
	switch(aId)
	{
        case EGroup:              {
            		                  aValue = ESoftware;
            		              }
            		              break;
        case ENotifyUser:         {
            		                  aValue = 0;
            		              }
								  break;
        case EAttached:           { 
								  status = EDcmoNotSupported;
        		                  }
								  break;
        case EEnabled:	          {
            						  CRepository * rep = CRepository::NewLC( KCRUidDSDCMOConfig );
            						  err = rep->Get( KNsmlDesktopSync, aValue );
            						  if(err != KErrNone)
	   										 	{
														status = EDcmoFail;
	   										 	}
            						  CleanupStack::PopAndDestroy( rep );
            					  }
            					  break;
		case EDenyUserEnable:     {
		                              aValue = 1; // DenyUserEnable
		                          }
						          break;
        default:                  {
                                      status = EDcmoNotFound;
                                  }						          
	}
	
	return status;
}

// -----------------------------------------------------------------------------
// CNsmlDsCtrlDCMOAdapter::GetDCMOPluginStrAttributeValueL
// - Get the Value of any attribute which is a string
// - If its existing attribute, then success is returned. Else Failure.
// -----------------------------------------------------------------------------
//
TDCMOStatus CNsmlDsCtrlDCMOAdapter::GetDCMOPluginStrAttributeValueL(TDCMONode aId, TDes& aStrValue) 
{
	TDCMOStatus status(EDcmoSuccess);
	switch(aId)
	{
		case EProperty: 	{
		                        aStrValue = *iProperty;
		                    }
							break;
		
		case EDescription:	{
                    		    aStrValue = *iDescription;
                    		}
						    break;
		default:			{
                    		    status = EDcmoNotFound;
                    		}
	}
	return status;
}

// -----------------------------------------------------------------------------
// CNsmlDsCtrlDCMOAdapter::SetDCMOPluginIntAttributeValueL
// - Set the Value of any attribute which is an integer
// - If its existing attribute, then success is returned. Else Failure.
// -----------------------------------------------------------------------------
//
TDCMOStatus CNsmlDsCtrlDCMOAdapter::SetDCMOPluginIntAttributeValueL(TDCMONode aId, TInt aValue)
{
	TDCMOStatus status(EDcmoFail);
	TInt err(KErrNone);
	switch(aId)
	{
		case EDenyUserEnable:
		case ENotifyUser:
							 break;
        case EEnable:	     {
                                 CRepository * rep = CRepository::NewLC( KCRUidDSDCMOConfig );
                                 rep->Set( KNsmlDesktopSync, aValue );
                                 CleanupStack::PopAndDestroy( rep );
                             }
                             break;
	}
	if(err == KErrNone)
	    {
		    status = EDcmoSuccess;
	    }
	return status;
}

// -----------------------------------------------------------------------------
// CNsmlDsCtrlDCMOAdapter::SetDCMOPluginStrAttributeValueL
// - Set the Value of any attribute which is a string
// - Here no Attribute comes as we dont have any String value to be set
// -----------------------------------------------------------------------------
//
TDCMOStatus CNsmlDsCtrlDCMOAdapter::SetDCMOPluginStrAttributeValueL(TDCMONode /*aId */, const TDes& /*aStrValue */)
{
	// Nothing to do
	return EDcmoNotSupported;	
}

// -----------------------------------------------------------------------------
// CNsmlDsCtrlDCMOAdapter::GetLocalizedNameL
// - Get localised name of the Plug-in
// -----------------------------------------------------------------------------
//
void CNsmlDsCtrlDCMOAdapter::GetLocalizedNameL (HBufC*& aLocName)
{
	TBool result = HbTextResolverSymbian::Init(KdcmoResourceFileName, KdcmoResourceFilePath );				
	_LIT(KTextDesktopSync, "txt_device_update_info_desktop_sync");
	aLocName = HbTextResolverSymbian::LoadL(KTextDesktopSync);
}
