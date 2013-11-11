/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This defines the interface for E-com plugins
*
*/

#ifndef __C_CS_PLUGIN_INTERFACE_H
#define __C_CS_PLUGIN_INTERFACE_H

// SYSTEM INCLUDE FILES

// USER INCLUDES

// FORWARD DECLARATION	
class CCsPlugin;
class CCsServer;
// CLASS DECLARATION
/**
* Class is used to instantiate the required plugins when the 
* conversation deatils has been asked. 
* Acts as interface to all Plugins.
*/

class CCsPluginInterface: public CBase 
{
	public:

		/** 
		* NewL
		* Symbian OS 2 phased constructor
		* 
		* @return	A pointer to the created instance of CCsPluginInterface.
		*/
		static CCsPluginInterface* NewL();

		/** 
		* NewLC
		* Symbian OS 2 phased constructor
		* 
		* @return	A pointer to the created instance of CCsPluginInterface.
		*/
		static CCsPluginInterface* NewLC();

		/**
		* Destructor.
		*/      
		virtual ~CCsPluginInterface();

		/**
		* InstantiatePluginL
		* Instantiates the required plug-ins known by the ecom framework.
		* basically this function first calls ListAllImplementationsL()
		* 
		* @param aCsServer server instance
		*/
		void InstantiatePluginL(CCsServer* aCsServer);

		/**
		* UnloadPlugIns
		* Unloads all plugins
		* This should internally call  REComSession::DestroyedImplementation( iDtor_ID_Key )
		* and then REComSession::FinalClose()
		*/
		void UnloadPlugIns(); 

		/**
		* ListAllImplementationsL
		* Lists all implementations which satisfy this ecom interface
		* 
		* @param aImplInfoArray On return, contains the list of available implementations
		*/   
		inline void ListAllImplementationsL( 
		        RImplInfoPtrArray& aImplInfoArray )
		    {
		    REComSession::ListImplementationsL( 
		            KCsPluginInterfaceUid, 
		            aImplInfoArray );
		    }
		
		/**
		 * GetConversationsL
		 * Forwards the request to the plugin instance ,
		 * to get the all conversations from the plugins
		 */
		void  GetConversationsL();

		/**
		 * GetPluginsCount
		 * Gets the  plugins count
		 * 
		 * @return  Plugin count
		 */
		TInt  GetPluginsCount();

	private: 

	  /** 
	   * Performs the first phase of two phase construction.
	   */
	  CCsPluginInterface();  

		/** 
		* Symbian OS 2 phased constructor.
		*/
		void ConstructL();

		/**
		* InstantiatePlugInFromImpUidL
		* Instantiates a CS plug-in, knowing the implementation uid
		* 
		* @param aImpUid imp uID
		* @param aCsServer, instance of server class
		*/
		CCsPlugin* InstantiatePlugInFromImpUidL( 
		        const TUid& aImpUid, 
		        CCsServer* aCsServer );


	private: 
	    
	    /**
	     * iDtor_ID_Key
	     * Unique Key, handle by ECom plugin
	     */
	    TUid iDtor_ID_Key;

		/**
		 * iCsPluginInstances
		 * Array of all the plugin instances
		 */
		RPointerArray<CCsPlugin>  iCsPluginInstances; 

		/**
		 * iPluginInUse
		 * Uid of the plugin in use.
		 */
		TUid iPluginInUse;
};

#endif // __C_CS_PLUGIN_INTERFACE_H

//End of File