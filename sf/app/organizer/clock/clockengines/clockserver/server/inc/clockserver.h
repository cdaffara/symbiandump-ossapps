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
* Description:   The header file of the CClkSrvMain class.
*
*/

#ifndef __CLOCKSERVER_H__
#define __CLOCKSERVER_H__

// System includes
#include <e32base.h>

// User includes
#include "clocktimesourceinterface.hrh"

// Forward declarations
class CClkSrvSession;
class CClkSrvImpl;

// Class declaration
/**
* @class CClkSrvMain
* @brief ClockServer server side module.
*/
class CClkSrvMain : public CServer2
	{
	public:			// New functions
	
		/**
		* @brief Two phased constructor
		* @return CClkSrvMain* Pointer to an object of CClkSrvMain
		*/
		static CClkSrvMain* NewL();
		
		/**
		* @brief The destructor
		*/
		~CClkSrvMain();
		
	public:			// New functions
	
		/**
        * @brief Returns a reference to iSessionQue.
        * @return TDblQue<CClkNitzSession>&
        */
        TDblQue< CClkSrvSession >& SessionQue();
        
        /**
        * @brief Asks the session to notify the client about change in data.
        * @param aWhatChanged This indicates what was changed.
        * @param aWhoChanged The entity which caused the change. Will usually be the plug-in id.
        * @param aNotification KErrNone if there was no error.
        */
        void NotifyAboutChange( TChangedComponent aWhatChanged, TInt aWhoChanged, TInt aNotification );
        
        /**
		* @brief Returns the id of the currently loaded plugin.
		* @param aPluginId This will be filled with the id of the active plugin read from cenrep.
		* @return TInt An error code.
		*/
		TInt GetActivePluginL( TInt& aPluginId ) const;
		
		/**
		* @brief Updates the id of the currently loaded plugin.
		* @param aPluginId The id of the plugin with which the cenrep should be updated.
		* @return TInt An error code.
		*/				
		TInt SetActivePluginL( TInt aPluginId );	
		
	public:			// From base classes
	
		/**
		* @brief From CServer2, returns a new session for communication.
		* @param aVersion The version object having the version info of the server
		* @param aMessage of type RMessage
		* @return CSession2* a Pointer to a CSession2 object.
		*/
		CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;
		
	private:		// New functions
		
		/**
		* @brief Performs the second phase of construction
		*/
		void ConstructL();
		
		/**
		* @brief The C++ default constructor
		*/
		CClkSrvMain( TInt aPriority );
		
		/**
		* @brief Performs the restoration operations like activating plugins that were previously activated and so on.
		*/
		void RestoreStateL();
		
    private:		// Data
    
    	/**
    	* @var iSessionQue
    	* @brief Is a doubly linked list of type CClkSrvSession
    	*/
    	TDblQue< CClkSrvSession >			iSessionQue;
    	
    	/**
    	* @var iClkSrvImpl
    	* @brief Is a pointer to an object of CClkSrvImpl. Only one instance, used by all the sessions.
    	*/
    	CClkSrvImpl*							iClkSrvImpl;
	};

#endif 		// __CLOCKSERVER_H__

// End of file
