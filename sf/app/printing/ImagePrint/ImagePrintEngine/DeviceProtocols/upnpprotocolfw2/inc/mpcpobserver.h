/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares the MPCPObserver interface class
*
*/


#ifndef MPCPOBSERVER_H
#define MPCPOBSERVER_H

#include <e32def.h>
#include <e32std.h>

class CUpnpAction;
class CUpnpService;
class CUpnpDevice;
class CUpnpHttpMessage;

/**
*  @brief A virtual MPCPObserver class.
*
*  @since Series60 3.1
*/
class MPCPObserver
    {
    public: // New interface functions

        /**
    	* CreateJobResponse callback function
        * @since Series60 3.1
        * @param aAction the action
        * @param aErrorCode System wide error code
        */
        virtual void CreateJobResponse( CUpnpAction* aAction, TInt aErrorCode ) = 0;

        /**
    	* CancelJobResponse callback function
        * @since Series60 3.1
        * @param aAction the action
        * @param aErrorCode System wide error code
        */
        virtual void CancelJobResponse( CUpnpAction* aAction, TInt aErrorCode ) = 0;

        /**
    	* GetPrinterAttributesResponse callback function
        * @since Series60 3.1
        * @param aAction the action
        * @param aErrorCode System wide error code
        */
        virtual void GetPrinterAttributesResponse( CUpnpAction* aAction, TInt aErrorCode ) = 0;

        /**
    	* GetJobAttributesResponse callback function
        * @since Series60 3.1
        * @param aAction the action
        * @param aErrorCode System wide error code
        */
        virtual void GetJobAttributesResponse( CUpnpAction* aAction, TInt aErrorCode ) = 0;

        /**
    	* GetMarginsResponse callback function
        * @since Series60 3.1
        * @param aAction the action
        * @param aErrorCode System wide error code
        */
        virtual void GetMarginsResponse( CUpnpAction* aAction, TInt aErrorCode ) = 0;

        /**
    	* GetMediaListResponse callback function
        * @since Series60 3.1
        * @param aAction the action
        * @param aErrorCode System wide error code
        */
        virtual void GetMediaListResponse( CUpnpAction* aAction, TInt aErrorCode ) = 0;


        /**
    	* StateUpdatedResponse callback function
        * @since Series60 3.1
		* @param	TDesC8& New printer state
		* @param	TDesC8& Reason for the printer state		
		* @param	TDesC8& List of job ids pending at the printer queue		
		* @param	TDesC8& The end state of last printed job. Format: 
		*					<id>,<name>,<originator>,<completed sheets>,<state>
		* @param	TDesC8& Number of the printed sheets
		* @param	TDesC8& The abort state of last aborted job. Format: 
		*					<id>,<name>,<originator>,<completed sheets>,aborted,<reason>
		* @param	TDesC8& Number of the sheets completedly received in printer
        */
        virtual void StateUpdatedResponse( const TDesC8& aPrinterState, 
                                           const TDesC8& aPrinterStateReasons, 
                                           const TDesC8& aJobIdList,
	                                       const TDesC8& aJobEndState, 
                                           const TDesC8& aJobMediaSheetsCompleted, 
                                           const TDesC8& aJobAbortState,
                                           const TDesC8& aContentCompleteList ) = 0;

        /**
    	* DeviceDiscoveredL callback function
        * @since Series60 3.1
        * @param aDevice the device discovered
        */
        virtual void DeviceDiscoveredL( CUpnpDevice* aDevice ) = 0;

        /**
    	* DeviceDisappeared callback function
        * @since Series60 3.1
        * @param aDevice the device disappeared
        */
        virtual void DeviceDisappeared( CUpnpDevice* aDevice ) = 0;

		/*
		* @function		PrinterStateUpdated
		* @description	Returns the state of the job after printer state updated 
		* @param		TDesC8& New printer state
		*/
		virtual void PrinterStateUpdated( const TDesC8& aEvent ) = 0;
		
		/*
		* @function		PrinterStateReasonsUpdated
		* @description	Returns the state of the job after printer state reason updated 
		* @param		TDesC8& Reason for the printer state		
		*/
		virtual void PrinterStateReasonsUpdated( const TDesC8& aEvent ) = 0;
		
		/*
		* @function		JobIdListUpdated
		* @description	Returns the state of the job after job ID list updated 
		* @param		TDesC8& List of job ids pending at the printer queue		
		*/
		virtual void JobIdListUpdated( const TDesC8& aEvent ) = 0;

		/*
		* @function		JobEndStateUpdatedL
		* @description	Returns the state of the job after job end state updated 
		* @param		TDesC8& The end state of last printed job. Format: 
		*						<id>,<name>,<originator>,<completed sheets>,<state>
		*/
		virtual void JobEndStateUpdated( const TDesC8& aEvent ) = 0;

		/*
		* @function		JobMediaSheetsCompletedUpdated
		* @description	Returns the state of the job after media sheets completed list updated 
		* @param		TDesC8& Number of the printed sheets
		*/
		virtual void JobMediaSheetsCompletedUpdated( const TDesC8& aEvent ) = 0;

		/*
		* @function		JobAbortStateUpdatedL
		* @description	Returns the state of the job after job abort state updated 
		* @param		TDesC8& The abort state of last aborted job. Format: 
		*						<id>,<name>,<originator>,<completed sheets>,aborted,<reason>
		*/
		virtual void JobAbortStateUpdated( const TDesC8& aEvent ) = 0;

		/*
		* @function		ContentCompleteListUpdated
		* @description	Returns the state of the job after content completed list updated 
		* @param		TDesC8& Number of the sheets completedly received in printer
		*/
		virtual void ContentCompleteListUpdated( const TDesC8& aEvent ) = 0;

    };

#endif // MPCPOBSERVER_H

// End Of File
