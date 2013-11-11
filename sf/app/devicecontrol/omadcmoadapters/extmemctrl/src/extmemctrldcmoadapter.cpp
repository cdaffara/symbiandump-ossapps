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
* Description:  ExtMemCtrl plug-in adapter for DCMO
*
*/


#include <f32file.h> 
#include <driveinfo.h>
#include <barsread.h> 
#include <barsc2.h>
#include <data_caging_path_literals.hrh>
#include <DevManInternalCRKeys.h>
#include <centralrepository.h>
#include "extmemctrldcmoadapter.h"
#include "hbtextresolversymbian.h"

_LIT( KdcmoResourceFileName, "deviceupdates_" );	
_LIT( KdcmoResourceFilePath, "z:/resource/qt/translations/" );	
_LIT( KExtMemCtrlProperty, "ExternalMemory");
_LIT( KExtMemCtrlDescription, "Used to enable/disable the ExtMem");


// Construction and destruction functions

// ----------------------------------------------------------------------------------------
// CExtMemCtrlDCMOAdapter::NewL
// ----------------------------------------------------------------------------------------
CExtMemCtrlDCMOAdapter* CExtMemCtrlDCMOAdapter::NewL(TAny* aParams)
	{
		CExtMemCtrlDCMOAdapter* self=new(ELeave) CExtMemCtrlDCMOAdapter(aParams);  
		CleanupStack::PushL(self);
		self->ConstructL(); 
		CleanupStack::Pop();
		return self;
	}

// ----------------------------------------------------------------------------------------
// CExtMemCtrlDCMOAdapter::~CExtMemCtrlDCMOAdapter
// ----------------------------------------------------------------------------------------
CExtMemCtrlDCMOAdapter::~CExtMemCtrlDCMOAdapter()
	{
		delete iProperty;
		iProperty = NULL;
		delete iDescription;
		iDescription = NULL;
	}

// ----------------------------------------------------------------------------------------
// CExtMemCtrlDCMOAdapter::CExtMemCtrlDCMOAdapter
// ----------------------------------------------------------------------------------------
CExtMemCtrlDCMOAdapter::CExtMemCtrlDCMOAdapter(TAny* aInitParams)
:  iInitParams((CDCMOInterface::TDCMOInterfaceInitParams*)aInitParams)
	{
	// See ConstructL() for initialisation completion.
	}

// ----------------------------------------------------------------------------------------
// CExtMemCtrlDCMOAdapter::ConstructL
// ----------------------------------------------------------------------------------------
void CExtMemCtrlDCMOAdapter::ConstructL()
{	
	// Safely complete the initialization of the constructed object	
	if(iInitParams->descriptor)
		iProperty =  (iInitParams->descriptor)->AllocL();
	else
		iProperty = KExtMemCtrlProperty().AllocL();
	iDescription = KExtMemCtrlDescription().AllocL();
}

// Implementation of CDCMOInterface

// ----------------------------------------------------------------------------------------
// CExtMemCtrlDCMOAdapter::GetDCMOPluginIntAttributeValueL
// Gets the integer attribute value.
// ----------------------------------------------------------------------------------------
TDCMOStatus  CExtMemCtrlDCMOAdapter::GetDCMOPluginIntAttributeValueL(TDCMONode aId, TInt& aValue)
{
	TDCMOStatus status(EDcmoSuccess);
	TInt err(KErrNone);
	
	switch(aId)
	{
		case EGroup: {
									aValue = EHardware;
								}
						break;		
		case EAttached:	{
						aValue = 0;
						RFs fsMemoryCard;
                  		err = fsMemoryCard.Connect();
                  		if ( err != KErrNone )
                  		{
                  			status = EDcmoFail;
                  		}
                  		TDriveInfo dInfo;
						TVolumeInfo vInfo;
    					TInt error = fsMemoryCard.Drive( dInfo, EDriveF );
     					if( error == KErrNone )
         				{
         					error = fsMemoryCard.Volume( vInfo, EDriveF );
         					if( error == KErrNone )
             				{
             					aValue = 1;
             				}
         				}										
						fsMemoryCard.Close();  
						}
						break;
		case EEnabled:{
			            RFs fsMemoryCard;
                  err = fsMemoryCard.Connect();
                  if ( err != KErrNone )
                  {
                  	status = EDcmoFail;
                  }
                      
                  TDriveInfo  info;
                  err=fsMemoryCard.Drive(info, EDriveF);
 //               DBG_ARGS8( _S8("ExtMem err =  %d info.iType = %d"), err, info.iType );
                                       
                  if (info.iType == EMediaNotPresent || err != KErrNone ||info.iType == EMediaUnknown )
                  {
                    aValue = 0;
                  }
                  else
                  { 
                    aValue = 1;
                  }                        
                  fsMemoryCard.Close();          
            		  }
						break;		
		case EDenyUserEnable:	{
										aValue = 1;
									}
						break;
		case ENotifyUser:	{
											aValue = 1;
											}
						break;	
		default :
						status = 	EDcmoNotFound;		
	}	
	return status;
}

// ----------------------------------------------------------------------------------------
// CExtMemCtrlDCMOAdapter::GetDCMOPluginStrAttributeValueL
// Gets the string attribute value.
// ----------------------------------------------------------------------------------------
TDCMOStatus  CExtMemCtrlDCMOAdapter::GetDCMOPluginStrAttributeValueL(TDCMONode aId, TDes& aStrValue) 
{
	TDCMOStatus status(EDcmoSuccess);		
	switch(aId)
	{
		case EProperty: 	aStrValue = *iProperty;
							break;		
		case EDescription:	aStrValue = *iDescription;
							break;
		default :	
						status = EDcmoNotFound;	
	}
	return status;
}

// ----------------------------------------------------------------------------------------
// CExtMemCtrlDCMOAdapter::SetDCMOPluginIntAttributeValueL
// Sets the integer attribute value.
// ----------------------------------------------------------------------------------------
TDCMOStatus  CExtMemCtrlDCMOAdapter::SetDCMOPluginIntAttributeValueL(TDCMONode aId, TInt aValue)
{
	TDCMOStatus status(EDcmoFail);
	TInt err(KErrNone);
	
	switch(aId)
	{
		case EDenyUserEnable:
							break;
		case ENotifyUser:
							break;
		case EEnable:	{
			        			RFs fsMemoryCard;
              			err = fsMemoryCard.Connect();
              			if ( err != KErrNone )
               			{
                		  return status ;
               			}
               			TBuf<255> fsysname;
               			fsysname.Zero();
               			if (!aValue)
               			{               	
                			fsMemoryCard.FileSystemName(fsysname,EDriveF);
                			TRequestStatus stat;
                			fsMemoryCard.NotifyDismount(EDriveF, stat, EFsDismountForceDismount);
                			User::WaitForRequest(stat);
               			 	//err= fsMemoryCard.DismountFileSystem(fsysname, EDriveF);
                			//if (err == KErrNone)
                   		status =EDcmoSuccess;                   
               			}
               			else
               			{
               	 			TInt err(fsMemoryCard.FileSystemName(fsysname,EDriveF));
               	 			if (err != KErrNone && !fsysname.Length()) 
               	 			{
               	 				_LIT( KFmgrFatFSName, "Fat" );
               	  			err = fsMemoryCard.MountFileSystem( KFmgrFatFSName, EDriveF );
               	 			} 
                   		status = EDcmoSuccess;                                 	
               			} 
               			fsMemoryCard.Close();
			          		CRepository *rep = NULL;
		            		TRAPD( err1, rep = CRepository::NewL( KCRUidDeviceManagementInternalKeys )) ;
		            		if(!err1) 
		              		rep->Set( KDevManMemCardCtrl, aValue ); 		              
		              	delete rep;
		              	rep = NULL; 							
		              }
						break;
		default :
							status = EDcmoNotFound;
	}	
	return status;
}

// ----------------------------------------------------------------------------------------
// CExtMemCtrlDCMOAdapter::SetDCMOPluginStrAttributeValueL
// Sets the string attribute value.
// ----------------------------------------------------------------------------------------
TDCMOStatus  CExtMemCtrlDCMOAdapter::SetDCMOPluginStrAttributeValueL(TDCMONode /* aId */, const TDes& /* aStrValue */)
{
	// Nothing to do
	TDCMOStatus status(EDcmoNotSupported);	
	return status;	
}

// ----------------------------------------------------------------------------------------
// CExtMemCtrlDCMOAdapter::GetLocalizedNameL
// returns the localized plug-in name.
// ----------------------------------------------------------------------------------------
void CExtMemCtrlDCMOAdapter::GetLocalizedNameL (HBufC*& aLocName)
{	
	TBool result = HbTextResolverSymbian::Init(KdcmoResourceFileName, KdcmoResourceFilePath );					
	_LIT(KTextExtMemory, "txt_device_update_info_external_memory_card");
	aLocName = HbTextResolverSymbian::LoadL(KTextExtMemory);	
}
