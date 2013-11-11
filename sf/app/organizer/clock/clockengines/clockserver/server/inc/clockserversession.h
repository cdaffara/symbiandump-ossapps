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
* Description:   The header file of the CClkSrvSession class.
*
*/

#ifndef __CLOCKSERVER_SESSION_H__
#define __CLOCKSERVER_SESSION_H__

// System includes
#include <e32base.h>

// User includes

// Forward declaration
class CClkSrvImpl;

// Constants
/**
* @enum TSessionPendingFlags
* @brief These flags hold the status of notification of the message.
*/
enum TSessionPendingFlags
    {
    ENotifyMsgPending = 0x0001,
    EFirstChangeMsg = 0x0002
    };

// Class declaration
/**
* @class CClkSrvSession
* @brief A CSession2 derived object. Handles the communications of CClkSrvMain
*/
class CClkSrvSession : public CSession2
	{
	public:			// New functions
	
		/**
		* @brief The constructor
		*/
		CClkSrvSession( CClkSrvImpl* aClkSrvImpl );
		
		/**
		* @brief The destructor
		*/
		~CClkSrvSession();
		
	public:			// New functions		
		
		/**
		* @brief Returns the offset to the link member.
        * @return TInt
        */
        static TInt LinkOffset();
        
        /**
        * @brief Notifies the client about change in the server data.
        * @param aWhatChanged This indicates what was changed.
        * @param aWhoChanged The entity which caused the change. Will usually be the plug-in id.
        * @param aNotification KErrNone if there was no error.
        */
        void NotifyAboutChange( TInt32 aWhatChanged, TInt32 aWhoChanged, TInt aNotification );
		
	public:			// From base classes
		
		/**
		* @brief From CSession2. Called when client requests for a service.
		* @param aMessage of type RMessage2, contains the client data.
		*/
		void ServiceL( const RMessage2& aMessage );
		
	private:		// New functions
	
		/**
		* @brief Handles the servicing of client requests.
		* @param aMessage Of type RMessage2, contains the client data.
		* @return TInt The error code.
		*/
		TInt DoServiceL( const RMessage2& aMessage );
		
		/**
		* @brief Calls the server implementation to activate the protocol with the id mentioned
		*		 in the message.
		* @param aMessage Of type RMessage2, contains the client data.
		* @return TInt The error code.
		*/
		TInt DoActivateProtocolL( const RMessage2& /*aMessage*/ );
		
		/**
		* @brief Calls the server implementation to check if the protocol with the id mentioned
		*		 in the message is active.
		* @param aMessage Of type RMessage2, contains the client data.
		* @return TInt The error code.
		*/
		TInt DoCheckProtocolActiveL( const RMessage2& aMessage );
		
		/**
		* @brief Calls the server implementation to deactivate a protocol.
		* @param aMessage Of type RMessage2, contains the client data.
		* @return TInt The error code.
		*/
		TInt DoDeActivateProtocolL( const RMessage2& /*aMessage*/ );
		
		/**
		* @brief Calls the server implementation to get the protocol information.
		* @param aMessage Of type RMessage2, contains the client data.
		* @return TInt The error code.
		*/
		TInt DoGetProtocolInfoL( const RMessage2& aMessage );
		
		/**
		* @brief Calls the server implementation to get the current mcc.
		* @param aMessage Of type RMessage2, contains the client data.
		* @return TInt The error code.
		*/
		TInt DoGetCurrentMccL( const RMessage2& aMessage );
		
		/**
		* @brief Calls the server implementation to get the current timezone Id.
		* @param aMessage Of type RMessage2, contains the client data.
		* @return TInt The error code.
		*/
		TInt DoGetCurrentTimeZoneIdL( const RMessage2& aMessage );
		
		/**
		* @brief Sets the message for notification later.
		* @param aMessage Of type RMessage2, contains the client data.
		*/
		void DoNotifyOnChange( const RMessage2& aMessage );
		
		/**
		* @brief Calls the server implementation to check if any protocols are active.
        * @param aMessage Of type RMessage2, contains the client data.
        * @return TInt The error code.
        */
		TInt DoCheckAutoTimeUpdateOn( const RMessage2& aMessage );
		
		/**
		* @brief Stores the message for notification later.
		* @param aMessage Of type RMessage2, contains the client data.
		*/
		void SetNotifyMessagePtr( const RMessage2& aMessage );
		
		/**
		* @brief Returns whether this session is the fisrt requst of notification.
		* @return ETrue if the Session is the first request.
		*/
		TBool FirstNotifyRequest() const;
		
		/**
		* @brief Returs if the notification is still pending.
		* @return ETrue if the Client is yet to be notified.
		*/
		TBool NotifyPending() const;
		
	private:		// Data
	
		/**
		* @var iSessionLink 
		* @brief The Session link.
		*/
		TDblQueLink 				iSessionLink;
		
		/**
		* @var iClkSrvImpl 
		* @brief Passed on by the server. Does the actual service. Not owned.
		*/
		CClkSrvImpl* 				iClkSrvImpl;
		
		/**
		* @var iNotifyChangeMsg 
		* @brief Used to notify the client about change in the server data.
		*/
		RMessagePtr2 				iNotifyChangeMsg;
		
		/**
		* @var iMsgPendingFlags 
		* @brief Has the status of notification of the message.
		*/
		TInt    					iMsgPendingFlags;
		
		/**
		* @var iMessageComplete
		* @brief This indicates that the message has been Completed by the server.
		*/
		TBool						iMessageComplete;
		
	};

#endif 		// __CLOCKSERVER_SESSION_H__

// End of file
