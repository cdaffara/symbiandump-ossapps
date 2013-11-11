/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of applicationmanagement components
*
*/


#ifndef __APPLICATIONMANAGEMENTCLIENT_H__
#define __APPLICATIONMANAGEMENTCLIENT_H__

// INCLUDES

#include <e32std.h>
#include "ApplicationManagementCommon.h"
#include "ApplicationManagementClientServer.h"
#include "ApplicationManagementConst.h"
#include <ssl.h>
#include <f32file.h>
//#include <AknServerApp.h>
typedef RArray<TUint32> RElementIdArray;

namespace NApplicationManagement
	{

// CLASS DECLARATION

/**
*  RApplicationManagement class to establish ApplicationManagement server connection
*/
class RAMTC : public RSessionBase
	{
		public:
    /**
	* Connect Connects client side to application management server
    * @return KErrNone Symbian error code
    */
	IMPORT_C TInt Connect();
	
	
    /**
	* Close Close server connection
    */
	IMPORT_C void Close();

	/**
	* GetApplicationUidL finds uid for given deployment component.
	* @param aInternalId Id of the deployment component, whose uid is wished
	* @param aUid	After successful call, will hold the uid
	*/	
	IMPORT_C void GetApplicationUidL( TInt aInternalId, TUid &aUid );
	};
	


class RApplicationManagement : public RSessionBase
	{
	public:
	    /**
		* Connect Connects client side to application management server
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt Connect( const TCertInfo &aCertInfo );
		IMPORT_C TInt Connect( );
		
	    /**
		* Close Close server connection
        */
		IMPORT_C void Close();
		
	
//Session management		
	    /**
		* DeliverL Add deployment component to the system. At least some parameter
		* (in the aInfo or aData) must not be empty.
		*
		* @param aInfo  TDeploymentComponentInfo containing the essentials of component
		* @param aData The Delivery deployment component Data
		* @param aInstallOpts Optional install options
		* @param aMetaData Optional meta data for install operation
	    * @return Created Internal Id for the deployment component
        */
	
		/**
		* DownloadL Adds deployment component with EDCSDownload to the system. 
		* @param aUserId The user id of the component
	    * @return Created Internal Id for the deployment component
        */
		IMPORT_C TUint32 DownloadL( const TDCUserId &aUserId ) const;
			
		IMPORT_C TUint32 DeliverL( const TDCUserId &aUserId ) const;
		/**
		* DeploymentComponent Returns the deployment component having the given Id.
		* If there's no such deployment component, method returns
		* KErrNotFound. 
		
		* @param aId Id of the component to be found
	    * @return KErrNone  Symbian error code (KErrNotFound if not found)
		*/
		IMPORT_C TInt DeploymentComponent( 
			TUint32 aInternalId, TDeploymentComponent &aReturnValue ) const;
			
		/**
		* DeploymentComponentUserId Returns the user id of deployment component 
		* having the given internal Id.
		* @param aId Id of the component to be found
		* @param aResponse The buffer in which the user id will be placed
	    * @return KErrNone  Symbian error code (KErrNotFound if not found)
		*/
		IMPORT_C void DeploymentComponentUserIdL( 
			TUint32 aInternalId, TDCUserId &aResponse ) const;
	
		/**
		* DeploymentComponentDataL Fetches data for component that's in EDCSDelivered state.
		* @param aInternalId Id of the component to be found
		* @param aDataType Type of the data to be found. Must be one of EDCInstallOptions
		*		EDCMetaData, EDCData, 
		* @param aBuffer The buffer that will be filled with the data
	    * @return 
		*/
		IMPORT_C void DeploymentComponentDataL( 
			TUint32 aInternalId, TDeplCompAttrType aDataType, CBufBase &aBuffer ) const;
			
		
		/**
		* UpdateDeploymentComponentL Updates deployment component.
		* @param aInternalId Id of the component to be updated
		* @param aItem Type of the data to be updated.
		* @param aItemData The data
	    * @leave The deployment component, or NULL if there's not one
		*/
		IMPORT_C void UpdateDeploymentComponentL( TUint32 aInternalId,
			TDeplCompAttrType aItem, const TDesC8 &aItemData ) const;
		
		/**
		* UpdateDeploymentComponentL Updates deployment component with mime type too.
		* @param aInternalId Id of the component to be updated
		* @param aItem Type of the data to be updated. This must be 
		*	EDCMetaData or EDCData for call to be successful
		* @param aItemData The data
		* @param aDataMime The data mime type
	    * @leave The deployment component, or NULL if there's not one
		*/	
		IMPORT_C void UpdateDeploymentComponentDataL( 
			TUint32 aInternalId, TDeplCompAttrType aItem, 
			const TDesC8 &aItemData, const TDesC8 &aDataMime ) const; 

		/**
		* UpdateDeploymentComponentStreamedDataL Updates deployment component with mime type too.
		* @param aInternalId Id of the component to be updated
		* @param aItem Type of the data to be updated. This must be 
		*	EDCMetaData or EDCData for call to be successful
		* @param aDataMime The data mime type
	    * @leave The deployment component, or NULL if there's not one
		*/	
		IMPORT_C void UpdateDeploymentComponentStreamedDataL( 
			TUint32 aInternalId, TDeplCompAttrType aItem, const TDesC8 &aDataMime ) const; 
		
		/**
		* DeploymentComponentIds Returns array of the deployment component Ids, that exist in the
		* system. Optionally one can give a TDeploymentComponentState flag to restrict
		* resulted ids to those components that has the given state.
		* @param aArray Will hold the response
		* @param aState Optional state of the wished deployment components
	    * @return KErrNone Symbian error code
		*/
		IMPORT_C TInt DeploymentComponentIdsL(
			RElementIdArray &aArray, 
			const TDeploymentComponentState aState = EDCSNone  ) const;
			
		/**
		* InstallL Installs given deployment component.
		* @param aInternalId The id of deployment component to be installed
	    * @param aTargetState Will the installed component be actibve or inactive (default active)
		* @return KErrNone Symbian error code
		*/
		IMPORT_C void InstallL( TUint32 aInternalId , 
			TDeploymentComponentState aTargetState = EDCSActive) const;

		/**
		* RemoveL Removes given deployment component.
		* @param aInternalId The id of the component to be removed
	    * @return 
		*/
		IMPORT_C void RemoveL( TUint32 aInternalId ) const;
		
		/**
		* UpdateL Updates the given deployment
		* @param aSourceInternalId The id of the component that is source of update
		* @param aTargetInternalId The id of the component that is target of update
	    * @leave 
		*/
		IMPORT_C void UpdateL( TUint32 aSourceInternalId, TUint32 aTargetInternalId ) const;
		
		/**
		* StartDownloadL Starts download of deployment component.
		* @param aInternalId The id of the component to be downloaded
		* @param aTargetState The target state of download (
	    * @return 
		*/
		IMPORT_C void StartDownloadL( TUint32 aInternalId, 
			const TDownloadTarget aTarget= EDeliver  ) const;
		
		
	 	/**
		* StateChangingDCIds Returns array of the deployment component Ids, that are
		* in the state changing list. Currently they are those that are being downloaded.
		* @param aArray Will hold the response
		
	    * @return KErrNone Symbian error code
		*/
		IMPORT_C TInt StateChangingDCIdsL( RElementIdArray &aArray ) const;
		
		/**
		* StateChangeComplete Informs server that the state of component is updated in the
		*	adapter, and that it thus can be removed from the list of state changing objects.
		* @param aInternalId The id of the component to be removed from list
		*
	    * @return KErrNone Symbian error code
		*/
		IMPORT_C TInt StateChangeComplete( TUint32 aInternalId ) const;
		
		/**
		* ActivateL Tries to activate given deployment component
		*	
		* @param aInternalId The id of the component to be activated
		*/
		IMPORT_C void ActivateL( TUint32 aInternalId ) const;
		
		/**
		* DeactivateL Tries to deactivate given deployment component
		*	
		* @param aInternalId The id of the component to be deactivated
		*/
		IMPORT_C void DeactivateL( TUint32 aInternalId ) const;
		
		/**
		* DeploymentComponentDataSizeL Get deployment component attribute size
		*	
		* @param aInternalId The id of the component 
		* @param aItem The type of attribute whose size is to be got
		* @return The size
		*/
		IMPORT_C TInt DeploymentComponentDataSizeL( TUint32 aInternalId, 
			const TDeplCompAttrType &aItem ) const;

		/**
		* DeploymentComponentTempInstFile ask file handle from server to be used
		*	
		* @param aTempFile temporary file handle 
		* @param aItem The type of attribute whose type of temporary file to get
	    * @return KErrNone Symbian error code
		*/
		IMPORT_C void DeploymentComponentTempInstFileL( TUint32 aInternalId, RFile& aTempFile, const TDeplCompAttrType &aItem ) const;

		/**
		 * GetFinalResultsForGAL ask results from AM Server. In response AM 
         * server returns final result and target URI of node
         *	
         * @param aInternalId The id of the component 
         * @param aFinalResult Final result of deployment component
         * @param aTargetURI Target URI of deployment component
         * @return void
         */
		IMPORT_C void GetFinalResultsForGAL(TUint32 aInternalId, TInt& aFinalResult,  TDes8& aTargetURI) const;
		
		IMPORT_C void GenericAlertSentL(TUint32 aInternalId) const;
		
		/**
		* Implementation of Service UID() from base class. Returns AM service UID 
		* to framework
		*	
		* @return uid of AM service UID
		*/	
		TUid ServiceUid() const;  
        /**
         * CheckStatusNodes Checks all deployed,delivered and download component's status node values
         * and set the state accordingly. For ex, on first SCOMO session if status node = Download progress
         * which means that download operation is failed in previous sessiona and hence we need to set the
         * value of status node to Download failed.
         *   
         */
        IMPORT_C void CheckStatusNodesL( ) const;
		  
	private:	
	};


	}

#endif //__APPLICATIONMANAGEMENTCLIENT_H__


//  End of File
