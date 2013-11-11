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
* Description:  DCMO DM Adapter
*
*/

#ifndef __DCMODMADAPTER_H
#define __DCMODMADAPTER_H

// INCLUDES
#include <e32base.h>
#include <smldmadapter.h>
#include <centralrepository.h>
#include "dcmoclient.h"
#include "dcmoconst.h"

const TUid KCRUidDCMOServer={0x2001FE47};
const TInt KDCMOKeyMaxNumber = 16;
const TInt KDCMOPropertyNumber = 1;

_LIT8( KMimeType, "text/plain" );
_LIT8( KNSmlDCMOAdapterTitle,       "DCMO DM Adapter" );
_LIT8( KNSmlTrue,						"True" );
_LIT8( KNSmlFalse,					"False" );

_LIT8( KNSmlHardware,				"Hardware" );
_LIT8( KNSmlIO,							"IO" );
_LIT8( KNSmlConnectivity,		"Connectivity" );
_LIT8( KNSmlSoftware,				"Software" );
_LIT8( KNSmlService,				"Service" );

//DDF accepted DM-fieldnames
_LIT8( KNSmlDdfProperty,				"Property" );
_LIT8( KNSmlDdfGroup,						"Group" );
_LIT8( KNSmlDdfDescription,			"Description" );
_LIT8( KNSmlDdfAttached,				"Attached" );
_LIT8( KNSmlDdfEnabled,					"Enabled" );
_LIT8( KNSmlDdfDCMOConfig,			"DCMOConfig" );
_LIT8( KNSmlDdfDenyUserEnable,	"DenyUserEnable" );
_LIT8( KNSmlDdfNotifyUser,			"NotifyUser" );
_LIT8( KNSmlDdfOperations,			"Operations" );
_LIT8( KNSmlDdfEnable,					"Enable" );
_LIT8( KNSmlDdfDisable,					"Disable" );
_LIT8( KNSmlDdfOpExt,						"Ext" ); 
_LIT8( KNSmlDdfExt,							"Ext" );
_LIT8( KNSmlDdfOpExtSemiEnable,	"SemiEnable" );

//DDF DM-field descriptions
_LIT8( KDCMOCapabilityDescription,				"DCMO capability node");
_LIT8( KNSmlDdfPropertyDescription,				"Property of DCMO" );
_LIT8( KNSmlDdfGroupDescription,					"Group of DCMO" );
_LIT8( KNSmlDdfDescDescription,						"The Description node" );
_LIT8( KNSmlDdfAttachedDescription,				"Attached" );
_LIT8( KNSmlDdfEnabledDescription,				"Enabled" );
_LIT8( KNSmlDdfDCMOConfigDescription,			"The DCMO Configuration node" );
_LIT8( KNSmlDdfDenyUserEnableDescription,	"DenyUserEnable" );
_LIT8( KNSmlDdfNotifyUserDescription,			"NotifyUser" );
_LIT8( KNSmlDdfOperationsDescription ,    "Operations node " );
_LIT8( KNSmlDdfEnableDescription ,       	"Enable Operation " );
_LIT8( KNSmlDdfDisableDescription ,       "Disable Operation " );
_LIT8( KNSmlDdfOpExtDescription ,       	"Extra Operations " );
_LIT8( KNSmlDdfExtDescription,						"Extra" );
_LIT8( KNSmlDdfOpExtSemiEnableDescription,	"Semi Enable Operation" );

_LIT8( KDcmoMatch, 											"DCMO" );
_LIT8( KDcmoMatch2, 										"DCMO/*" );
_LIT8( KDcmoMatch3, 										"DCMO/*/*" );
_LIT8( KDcmoConfigMatch, 								"DCMO/*/DCMOConfig");
_LIT8( KSegmDcmo,												"Property/Group/Description/Attached/Enabled/DCMOConfig/Operations/Ext");
_LIT8( KSegmDcmoConfig,									"DenyUserEnable/NotifyUser");
_LIT8( KSegmDcmoOperations,							"Enable/Disable/Ext");
_LIT8( KDcmoConfigDenyUserEnableMatch, 	"DCMO/*/DCMOConfig/DenyUserEnable");
_LIT8( KDcmoConfigNotifyUserMatch, 			"DCMO/*/DCMOConfig/NotifyUser");
_LIT8( KDcmoOperationsMatch, 						"DCMO/*/Operations");
_LIT8( KDcmoOperationsExtMatch, 				"DCMO/*/Operations/Ext");


_LIT8( KNSmlBluetooth,									"Bluetooth" );

// CLASS DECLARATION

/**
* Adapter class
* Provides DM adapter interface
*/
class CDcmoDMAdapter : public CSmlDmAdapter
    {
    
    public: // Constructors, destructor
        /**
        * Create instance of CDcmoDMAdapter
        * @param aDmCallBack
        * @return Instance of CDcmoDMAdapter
        */    
        static CDcmoDMAdapter* NewL( MSmlDmCallback* aDmCallback );

        /**
        * Create instance of CDcmoDMAdapter
        * @param aDmCallBack
        * @return Instance of CDcmoDMAdapter
        */   
        static CDcmoDMAdapter* NewLC( MSmlDmCallback* aDmCallback );

        /**
        * C++ Destructor
        */
        virtual ~CDcmoDMAdapter();

    public:
	    /**
	    * The function returns current version of the DDF.
	    * By asking current DDF versions from adapters DM Module can control
	    * possible changes in the data structure and send the changed DDF
	    * description to a management server.
	    * This function is always called after DDFStructureL.
        * @param aVersion DDF version of the adapter. (filled by the adapter)
	    * @publishedPartner
	    * @prototype
	    */
	    void DDFVersionL( CBufBase& aVersion );
	
	    /**
	    * The function for filling the DDF structure of the adapter
	    * This function is only called once, immediately after the adapter is created.
	    * @param aDDFObject	Reference to root object. A DM adapter starts filling
		         the data structure by calling AddChildObjectL to the root object and
				 so describes the DDF of the adapter. 
	    * @publishedPartner
	    * @prototype
	    */
        void DDFStructureL( MSmlDmDDFObject& aDDF );
	
	    /**
	    * The function creates new leaf objects, or replaces data in existing leaf
	    * objects. The information about the success of the command should be
	    * returned by calling SetStatusL function of MSmlDmCallback callback
	    * interface. This makes it possible to buffer the commands. However, all
	    * the status codes for buffered commands must be returned at the latest when
	    * the adapter's CompleteOutstandingCmdsL() is called.
	    * @param aURI			URI of the object
	    * @param aLUID		LUID of the object (if the adapter has earlier returned a
	 	     				LUID to the DM Module). For new objects, this is the LUID
 			    			inherited through the parent node.
 	    * @param aObject		Data of the object. 
	    * @param aType		MIME type of the object
	    * @param aStatusRef	Reference to correct command, i.e. this reference
	 					must be used when calling the SetStatusL of this command
	    * @publishedPartner
	    * @prototype
	    */
	    void UpdateLeafObjectL( const TDesC8& aURI, 
	                            const TDesC8& aLUID,
			                    const TDesC8& aObject, 
			 				    const TDesC8& aType,
							    TInt aStatusRef 
							  );
		
        /**
	    * The function creates new leaf objects, or replaces data in existing leaf
	    * objects, in the case where data is large enough to be streamed. The
	    * information about the success of the command should be returned by calling
	    * SetStatusL function of MSmlDmCallback callback interface. This makes it
	    * possible to buffer the commands.  However, all the status codes for buffered
	    * commands must be returned at the latest when the CompleteOutstandingCmdsL()
	    * of adapter is called.
	    * @param aURI		URI of the object
	    * @param aLUID		LUID of the object (if the adapter has earlier returned a
    						LUID to the DM Module). For new objects, this is the LUID
	    					inherited through the parent node.
	    * @param aStream	Data of the object. Adapter should create write stream
		    				and return, when data is written to stream by DM agent,
			    			StreamCommittedL() is called by DM engine
	    * @param aType		MIME type of the object
	    * @param aStatusRef	Reference to correct command, i.e. this reference
    						must be used when calling the SetStatusL of this
		    				command.
	    * @publishedPartner
	    * @prototype
	    */
	    void UpdateLeafObjectL( const TDesC8& aURI, 
	                            const TDesC8& aLUID,
								RWriteStream*& aStream, 
								const TDesC8& aType,
		 						TInt aStatusRef 
		 				      );
	
	    /**
	    * The function deletes an object and its child objects. The SetStatusL
	    * should be used as described in UpdateLeafObjectL()
	    * @param aURI		URI of the object
	    * @param aLUID		LUID of the object (if the adapter have earlier returned
    						LUID to the DM Module).
	    * @param aStatusRef	Reference to correct command, i.e. this reference must
						be used when calling the SetStatusL of this command.
        * @publishedPartner
	    * @prototype
	    */
	    void DeleteObjectL( const TDesC8& aURI, 
	                        const TDesC8& aLUID,
							TInt aStatusRef 
					      );
	
	    /**
	    * The function fetches data of a leaf object. The SetStatusL should be used
	    * as described in UpdateLeafObjectL(). The data is returned by using the
	    * SetResultsL function of MSmlCallback callback interface, and may be streamed.
	    * @param aURI			URI of the object
	    * @param aLUID			LUID of the object (if the adapter have earlier
    							returned LUID to the DM Module).   
	    * @param aType 			MIME type of the object
	    * @param aResultsRef	Reference to correct results, i.e. this reference
    							must be used when returning the result by calling
	    						the SetResultsL.
	    * @param aStatusRef		Reference to correct command, i.e. this reference
		    					must be used when calling the SetStatusL of this
    							command.
	    * @publishedPartner
	    * @prototype
	    */
	    void FetchLeafObjectL( const TDesC8& aURI, 
	                           const TDesC8& aLUID,
							   const TDesC8& aType, 
							   TInt aResultsRef,
							   TInt aStatusRef 
							 );
	
	    /**
	    * The function fetches the size of the data of a leaf object. The size is
	    * in bytes, and must reflect the number of bytes that will be transferred
	    * when the framework calls FetchLeafObjectL. The SetStatusL should be used
	    * as described in FetchLeafObjectL(). The size value is returned by using
	    * the SetResultsL function of MSmlCallback callback interface, and must be
	    * a decimal integer expressed as a string, eg. "1234".
	    * Results from this call MUST NOT be streamed.
	    * @param aURI			URI of the object
	    * @param aLUID			LUID of the object (if the adapter have earlier
    							returned LUID to the DM Module).   
	    * @param aType 			MIME type of the object
	    * @param aResultsRef	Reference to correct results, i.e. this reference
    							must be used when returning the result by calling
	    						the SetResultsL.
	    * @param aStatusRef		Reference to correct command, i.e. this reference
     							must be used when calling the SetStatusL of this
	    						command.
	    * @publishedPartner
	    * @prototype
	    */
	    void FetchLeafObjectSizeL( const TDesC8& aURI, 
	                               const TDesC8& aLUID,
								   const TDesC8& aType, 
								   TInt aResultsRef,
								   TInt aStatusRef 
								 );
	    /**
	    * The function fetches URI list. An adapter returns the list of URI segments
	    * under the given URI be separated by slash ("/"). The URI segment names for
	    * new objects must be given by the adapter.
	    * The list is returned by calling the SetResultsL function of MSmlCallback
	    * callback interface.	Results from this call MUST NOT be streamed.
	    * @param aParentURI					URI of the parent object
	    * @param aParentLUID				LUID of the parent object (if the
    										adapter have earlier returned LUID to
		    								the DM Module).   
	    * @param aPreviousURISegmentList	URI list with mapping LUID information,
			    							which is known by DM engine. An adapter
				    						can use this information when verifying
					    					if old objects still exists. An adapter
						    				also knows what objects are new to DM
							    			engine and can provide LUID mapping for
								    		them. aPreviousURISegmentList parameter
									    	(see above) helps to recognise new
    										objects.
	    * @param aResultsRef				Reference to correct results, i.e. this
    										reference must be used when returning
	    									the result by calling the SetResultsL.
	    * @param aStatusRef					Reference to correct command, i.e. this
		    								reference must be used when calling the
			    							SetStatusL of this command.
	    * @publishedPartner
	    * @prototype
	    */
	    void ChildURIListL( const TDesC8& aURI, 
	                        const TDesC8& aLUID,
					        const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
					        TInt aResultsRef, 
					        TInt aStatusRef 
					      );
	
        /**
	    * The function adds node object. In some cases an implementation of the
	    * function may be empty function, if the node object does not need concrete
	    * database update. Still this function may be helpful to an adapter, i.e. in
	    * passing mapping LUID of the node to DM Module. The SetStatusL should be
	    * used as described in UpdateLeafObjectL()
	    * @param aURI			URI of the object
	    * @param aParentLUID	LUID of the parent object (if the adapter have
     							earlier returned LUID to the DM Module).   
	    * @param aStatusRef		Reference to correct command, i.e. this reference
		    					must be used when calling the SetStatusL of this
			    				command.
	    * @publishedPartner
	    * @prototype
	    */
	    void AddNodeObjectL( const TDesC8& aURI, 
	                         const TDesC8& aParentLUID,
		  					 TInt aStatusRef 
		  				   );

	    /**
	    * The function implements execute command. The information about the success
	    * of the command should be returned by calling SetStatusL function of
	    * MSmlDmCallback callback interface. This makes it possible to buffer the
	    * commands.
	    * However, all the status codes for buffered commands must be returned at
	    * the latest when the CompleteOutstandingCmdsL() of adapter is called.
	    * @param aURI			URI of the command
	    * @param aLUID			LUID of the object (if the adapter have earlier
    							returned LUID to the DM Module).   
	    * @param aArgument		Argument for the command
	    * @param aType			MIME type of the object 
	    * @param aStatusRef		Reference to correct command, i.e. this reference
    							must be used when calling the SetStatusL of this
	    						command.
	    * @publishedPartner
	    * @prototype
	    */
	    void ExecuteCommandL( const TDesC8& aURI, 
	                          const TDesC8& aLUID,
							  const TDesC8& aArgument, 
							  const TDesC8& aType,
							  TInt aStatusRef 
							);

	    /**
	    * The function implements execute command. The information about the
	    * success of the command should be returned by calling SetStatusL function
	    * of MSmlDmCallback callback interface. This makes it possible to buffer the
	    * commands.
	    * However, all the status codes for buffered commands must be returned at
	    * the latest when the CompleteOutstandingCmdsL() of adapter is called.
	    * @param aURI			URI of the command
	    * @param aLUID			LUID of the object (if the adapter have earlier
    							returned LUID to the DM Module).   
	    * @param aStream		Argument for the command. Adapter should create
		    					write stream and return, when data is written to
			    				stream by DM agent, StreamCommittedL() is called by
				    			DM engine
	    * @param aType			MIME type of the object 
	    * @param aStatusRef		Reference to correct command, i.e. this reference
    							must be used when calling the SetStatusL of this
	    						command.
	    * @publishedPartner
	    * @prototype
	    */
	    void ExecuteCommandL( const TDesC8& aURI, 
	                          const TDesC8& aLUID,
							  RWriteStream*& aStream, 
							  const TDesC8& aType,
							  TInt aStatusRef 
							);

	    /**
	    * The function implements copy command. The information about the success of
	    * the command should be returned by calling SetStatusL function of
	    * MSmlDmCallback callback interface. This makes it possible to buffer the
	    * commands.
	    * However, all the status codes for buffered commands must be returned at
	    * the latest when the CompleteOutstandingCmdsL() of adapter is called.
	    * @param aTargetURI		Target URI for the command
	    * @param aSourceLUID	LUID of the target object (if one exists, and if the adapter
    							has	earlier returned a LUID to the DM Module).   
	    * @param aSourceURI		Source URI for the command
	    * @param aSourceLUID	LUID of the source object (if the adapter has
    							earlier returned a LUID to the DM Module).   
	    * @param aType			MIME type of the objects
	    * @param aStatusRef		Reference to correct command, i.e. this reference
    							must be used when calling the SetStatusL of this
	    						command.
	    * @publishedPartner
	    * @prototype
	    */
	    void CopyCommandL( const TDesC8& aTargetURI, 
	                       const TDesC8& aTargetLUID,
						   const TDesC8& aSourceURI, 
						   const TDesC8& aSourceLUID,
						   const TDesC8& aType, 
						   TInt aStatusRef 
						 );

	    /**
	    * The function indicates start of Atomic command.
	    * @publishedPartner
	    * @prototype
	    */
	    void StartAtomicL();
	
	    /**
	    * The function indicates successful end of Atomic command. The adapter
	    * should commit all changes issued between StartAtomicL() and
	    * CommitAtomicL()
	    * @publishedPartner
	    * @prototype
	    */
	    void CommitAtomicL();
	
	    /**
	    * The function indicates unsuccessful end of Atomic command. The adapter
	    * should rollback all changes issued between StartAtomicL() and
	    * RollbackAtomicL(). If rollback fails for a command, adapter should use
	    * SetStatusL() to indicate it.
	    * @publishedPartner
	    * @prototype
	    */
	    void RollbackAtomicL();
	
	    /**
        * Returns ETrue if adapter supports streaming otherwise EFalse.
	    * @param aItemSize size limit for stream usage
        * @return TBool ETrue for streaming support
	    * @publishedPartner
	    * @prototype
        */
	    TBool StreamingSupport( TInt& aItemSize );
	
	    /**
        * Called when stream returned from UpdateLeafObjectL or ExecuteCommandL has
	    * been written to and committed. Not called when fetching item.
	    * @publishedPartner
	    * @prototype
        */	
	    void StreamCommittedL();
	
	    /**
	    * The function tells the adapter that all the commands of the message that
	    * can be passed to the adapter have now been passed.  This indicates that
	    * the adapter must supply status codes and results to any buffered commands.
	    * This must be done at latest by the time this function returns.
	    * This function is used at the end of SyncML messages, and during processing
	    * of Atomic.   In the case of Atomic processing, the function will be
	    * followed by a call to CommitAtomicL or RollbackAtomicL.
	    * @publishedPartner
	    * @prototype
	    */
	    void CompleteOutstandingCmdsL();

    private: // New methods
    	
    	/**
	    * The function fills the adapter node with the details
	    * @publishedPartner
	    * @prototype
	    */
        void FillNodeInfoL( MSmlDmDDFObject& aDDFObject, 
                                            TSmlDmAccessTypes& aAccessTypes, 
                                            MSmlDmDDFObject::TScope aScope,
                                            MSmlDmDDFObject::TDFFormat aFormat, 
                                            MSmlDmDDFObject::TOccurence aOccurence,
                                            const TDesC8& aDescription,
                                            const TDesC8& aMIMEType );
       
      /**
	    * This function loads all the DCMO plug-in adapters 
	    * @publishedPartner
	    * @prototype
	    */
        void DDFDCMOAdaptersL(MSmlDmDDFObject& rootdcmo);
        
     /**
	    * This function constructs the DDFnode with the details 
	    * @publishedPartner
	    * @prototype
	    */ 
        void FillDCMODDFL(MSmlDmDDFObject& aDDF);  
        
          
      /**
	    * This function returns the Category of the plug-in 
	    * adapter from the aURI.
	    * @param aURI
	    * @return TPtrC8 category
	    */    
        
        TPtrC8 GetCategoryFromUriL(const TDesC8& aURI )  ;    
                            
      /**
    	* Maps a system wide error code to a TError.
    	* @param aError A system wide error code.
    	* @return A TError value depending on aError.
    	*/
    		CSmlDmAdapter::TError MapErrorToStatus( TInt aError ) const;     
    	
    	/**
	    * This function returns the dcmo enforcement 
	    * @param None
	    * @return TBool true if dcmo enforcement or false
	    */        	
    	TBool CheckEnforcementL();
                                
    private: // Constructor
        /**
        * Symbian 2nd phase constructor
        */
        void ConstructL();

        /**
        * C++ Constructor
        */    
        CDcmoDMAdapter( TAny* aEcomArguments );
                                                                                 
    private: // Data
        RDCMOClient* idcmoClient;
        TBool iExtOpNode;
    };

#endif // __DCMODMADAPTER_H

// End Of File
