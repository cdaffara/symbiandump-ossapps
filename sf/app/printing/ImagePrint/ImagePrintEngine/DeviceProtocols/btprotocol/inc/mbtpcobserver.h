/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares the MBtPCObserver interface class
*
*/


#ifndef MBTPCOBSERVER_H
#define MBTPCOBSERVER_H

#include <e32def.h>
#include <e32std.h>

class CRsBtDevice;
class TPrintCapability;


// CLASS DECLARATION

/**
*  @brief A virtual MBtPCObserver class.
*
*  @since Series60 3.1
*/
class MBtPCObserver
    {
    public: // New interface functions

        /**
    	* CreateJobResponse callback function
        * @since Series60 3.1
        * @param aAction the action
        * @param aErrorCode System wide error code
        */
        virtual void CreateJobResponse( TInt aJobId, TInt aErrorCode ) = 0;

        /**
    	* CancelJobResponse callback function
        * @since Series60 3.1
        * @param aAction the action
        * @param aErrorCode System wide error code
        */
        virtual void CancelJobResponse( TInt aErrorCode ) = 0;

        /**
    	* GetPrinterAttributesResponse callback function
        * @since Series60 3.1
        * @param aAction the action
        * @param aErrorCode System wide error code
        *
        virtual void GetPrinterAttributesResponse( TInt aState, TInt aStateReasons,
								 				   RArray<TPrintCapability>& aCapabilities,
								 				   TInt aOperationStatus, TInt aDeviceId) = 0;

        **
    	* GetPrinterAttributesResponse callback function
        * @since Series60 3.1
        * @param aAction the action
        * @param aErrorCode System wide error code
        */
        virtual void GetPrinterStateResponse( TInt aState, TInt aStateReasons,
								 			  TInt aOperationStatus, TInt aDeviceId) = 0;

        /**
    	* GetJobAttributesResponse callback function
        * @since Series60 3.1
        * @param aAction the action
        * @param aErrorCode System wide error code
        */
        virtual void GetJobAttributesResponse( TInt  aJobState, 
											   const TDesC8&  aJobName,
											   const TDesC8&  aOriginator,
											   TInt aSheetsCompleted, 
											   TInt aPendingJobs,
											   TInt aOperationStatus ) = 0;

        /**
    	* GetEventResponse callback function
        * @since Series60 3.1
        * @param aAction the action
        * @param aErrorCode System wide error code
        */
		virtual void GetEventResponse(TInt aJobId,
							  TInt aJobState,
							  TInt aPrinterState,
							  TInt aStateReasons,
							  TInt aOperationStatus) = 0;
        /**
    	* DeviceDiscoveredL callback function
        * @since Series60 3.1
        * @param aDevice the device discovered
        */
        virtual void DeviceDiscovered( CRsBtDevice& aDevice ) = 0;

        /**
    	* DeviceDisappeared callback function
        * @since Series60 3.1
        * @param aDevice the device disappeared
        */
        virtual void DeviceDisappeared( CRsBtDevice& aDevice ) = 0;

        /**
    	* DiscoveryError callback function
        * @since Series60 3.1
        * @param aError error occured during discovery on cancel discovery operation
        */
		virtual void DiscoveryError(TInt aError) = 0;
   		 
   		/*
		 * Updates progress bar. 
		 * */
		virtual void OPPSendProgress( TInt aProgress ) = 0;
    };

#endif // MBTPCOBSERVER_H

// End Of File