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
* Description:   The header file of the RClkSrvInterface class.
*
*/

#ifndef __CLOCKSERVER_CLT_H__
#define __CLOCKSERVER_CLT_H__

// System includes
#include <e32base.h>

// User includes
#include "clocktimesourceinterface.hrh"

// Constants
enum TClkSrvSessionPanic
	{
	EClkSrvCltNotifyChangeCancel
	};
	
// Function declarations
GLDEF_C void Panic( TClkSrvSessionPanic aPanic );

// Class declaration
/**
* @class RClkSrvInterface
* @brief ClockServer client side module, which is I/F for ClockServer.
*/
class RClkSrvInterface : public RSessionBase
	{
	public:			// New functions
	
		/**
		* @brief Default c++ constructor.
		*/
		IMPORT_C RClkSrvInterface();

		/**
		* @brief Connects the server by creating a session object.
		* @return TInt Error code.
		*/
		IMPORT_C TInt Connect();

		/**
		* @brief Gets the TVersion object.
		* @return TVersion
        */
        IMPORT_C TVersion Version() const;
        
        /**
		* @brief Issue a request to the server with service message EClkSrvActivateProtocol
		* @param aClkSrvProtocol The protocol to be activated.
		* @return TInt Error code.
        */
        IMPORT_C TInt ActivateProtocol( TInt aClkSrvProtocol );
        
        /**
		* @brief Issue a request to the server with service message EClkSrvActivateProtocol.
		* calls the overloaded function to activate all the installed protocols.
		* @return TInt Error code.
        */
        IMPORT_C TInt ActivateAllProtocols();
        
        /**
		* @brief Issue a request to the server with service message EClkSrvIsProtocolActive
		* @param aClkSrvProtocol The protocol to be checked.
		* @param aActive ETrue if the protocol sent above is active, EFalse otherwise.
		* @return TInt Error code.
        */
        IMPORT_C TInt IsProtocolActive( TInt aClkSrvProtocol, TBool& aActive );
        
        /**
		* @brief Issue a request to the server with service message EClkSrvDeactivateProtocol
		* @param aClkSrvProtocol The protocol to be activated.
		* @return TInt Error code.
        */
        IMPORT_C TInt DeActivateProtocol( TInt aClkSrvProtocol );
        
        /**
		* @brief Issue a request to the server with service message EClkSrvDeactivateProtocol
		* Call to deactivate all the installed protocols.
		* @return TInt Error code.
        */
        IMPORT_C TInt DeActivateAllProtocols();
        
        /**
		* @brief Issue a request to the server with service message EClkSrvGetProtocolInfo
		* @param aClkSrvProtocol The protocol to be checked. For later use.
		* @param aTimeAttribute Of type struct STimeAttributes, containing one of the time
				 				attributes requested by the user application.
		* @return TInt Error code.
        */
        IMPORT_C TInt GetProtocolInfo( TInt aClkSrvProtocol, STimeAttributes& aTimeAttribute );
        
        /**
		* @brief Issue a request to the server with service message EClkSrvGetCurrentMcc
		* @param aCurrentMcc The value of MCC. Will be filled by the server.
		* @return TInt Error code.
        */
        IMPORT_C TInt GetCurrentMcc( TInt& aCurrentMcc );
        
        /**
		* @brief Issue a request to the server with service message EClkSrvGetCurrentTimeZoneId
		* @param aCurrentTimeZoneId The value of timezone Id. Will be filled by the server.
		* @return TInt Error code.
        */
        IMPORT_C TInt GetCurrentTimeZondId( TInt& aCurrentTimeZoneId );
        
        /**
		* @brief Issue a request to the server with service message EClkSrvNotifyOnChange
		* @param aWhatChaned A reference argument to be filled by clockserver with the component that has been changed.
		* @param aWhoChanged The entity which caused the change. Will usually be the plug-in id.
		* @param aStatus Reference containing the status of the asynchronous request.
		*/
        IMPORT_C void NotifyOnChange( TInt32& aWhatChanged, TInt32& aWhoChanged, TRequestStatus& aStatus );
        
        /**
		* @brief Issue a request to the server with service message EClkSrvCancelNotifyOnChange
		*/
        IMPORT_C void NotifyOnChangeCancel();
        
        /**
        * @brief Issue a request to the server with service message EClkSrvIsAutoTimeUpdadeOn.
        * @param aEnabled ETrue if atleast one of the protocols is active, EFalse otherwise.
        * @return TInt Error code.
        */
        IMPORT_C TInt IsAutoTimeUpdateOn( TBool& aEnabled );
        
	private:		// New functions.
		
		/**
		* @brief Asks if the clock server is running.
		* @return TInt Error code.
		*/
		TInt IsClockSrvRunning() const;

		/**
		* @brief Starts the clock server if it's not running before.
		*/
		void StartClockSrvL();
		
	private:		// Data
	
		/**
		* @var iWhatChanged
		* @brief A TPtr8 descriptor.
		*/
		TPtr8						iWhatChanged;
		
		/**
		* @var iWhoChanged
		* @brief A TPtr8 descriptor.
		*/
		TPtr8						iWhoChanged;
	};

#endif		// __CLOCKSERVER_CLT_H__

// End of file
