/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Application Management DM Adapter
*
*/


/**

1. Remember the applications managed by Corporate Admin Role certificate
 1.1 They must be removed during rfs
 1.2 Others must be left as is (applications at least must be left)

*/

/*

./SCM -+-> Inventory -+--> Deployed ---> <X> * ---+--> ID
      |            	 |              			 |
      |            	 |              			 |--> Name ?
      |            	 |              			 |
      |            	 |      			 		 |--> Version ?
      |            	 |      			 		 |
      |            	 |            			     |--> StateValue ?
      |            	 |      			 		 |
      |            	 |            			     |--> EnvType ? X
      |            	 |       					 |
      |        	  	 |				             |--> Operations ---+-> Activate ?
      |        	  	 |        					 |					|
      |        	  	 |        					 |					|-> Deactivate ?
      |        	  	 |        					 |					|
      |        	  	 |        					 |					|-> Remove ?
      |        	  	 |        					 |					|
      |        	  	 |        					 |					|-> Ext ? X
      |        	  	 |        					 |
      |        	  	 |				             |--> Ext ? X
      |        	  	 |            
      |        	  	 +--> Delivered ---> <X> * --+--> ID
      |            	                			 |
      |            	                			 |--> Data
      |            	                			 |
      |            	        			 		 |--> Name ?
      |				  							 |
      |				  							 |--> Version ?
      |				  							 |
      |				  							 |--> InstallOpts ?
      |            	        			 		 |
      |            	              			     |--> Descriptor ?
      |            	        			 		 |
      |            	              			     |--> EnvType ? X
      |            	        			 		 |
      |            	              			     |--> ContentType ? X
      |            	         					 |
      |        	  	  				             |--> Operations ---+-> Install ?
      |        	  	          					 |					|
      |        	  	          					 |					|-> Update ?
      |        	  	          					 |					|
      |        	  	          					 |					|-> InstallAndActivate ? 
      |        	  	          					 |					|
      |        	  	          					 |					|-> UpdateAndActivate ? 
      |        	  	          					 |					|
      |        	  	          					 |					|-> Remove ?
      |        	  	          					 |					|
      |        	  	          					 |					|-> Ext ?
      |        	  	          					 |
      |        	  	  				             |--> Ext ?
	  |
      |
      +-> Download --> <X> * ---+-> ID
      |							|
      |							|-> URI
      |							|
      |							|-> Name ?
      |							|
      |							|-> Version ?
      |							|
      |							|-> InstallOpts ? X      
      |							|
      |							|-> ConRef ? X      
      |							|
      |							|-> Status ?
      |							|
      |							|-> EnvType ? X
      |							|
      |							|-> ContentType ? X
	  |							|
      |			             	|--> Operations ----+-> Download ?
      |						    |					|
      |       	  	          	|				 	|-> DownloadAndInstall ? X
      |        	  	          	|				 	|
      |        	  	          	|				 	|-> DownloadAndInstallAndActivate ? X
      |        	  	          	|				 	|
      |        	  	          	|				 	|-> DownloadAndUpdate ? X
      |        	  	          	|				 	|
      |        	  	          	|				 	|-> DownloadAndUpdateAndActivate ? X
      |        	  	          	|				 	|
      |        	  	          	|				 	|-> Ext ?
      |        	  	          	|				 
      |        	  	  			|--> Ext ?
	  |
	  |--> Ext -> LastError (Debug Only) ?

*/

#define __AM_CHECKSTATE_CHANGES // uncomment to enable dynamic acl
#include <badesca.h>
#include <fbs.h>
#include <s32file.h> 
#include "amadapter.h"
#include <implementationproxy.h> // For TImplementationProxy definition
#include <f32file.h>
#include "nsmldmuri.h"
#include "nsmldmiapmatcher.h"
#include "debug.h"

#include <centralrepository.h>
#include "aminstalloptions.h"
#include <featmgr.h>
#include "amprivateCRKeys.h"
#ifdef __TARM_SYMBIAN_CONVERGENCY
#include <dmtreenode.h>
#include <devman.h>
#include "nsmldmuri.h"

// Constants
_LIT8( KDeployedURI, "./SCM/Inventory/Deployed/" );
_LIT8( KDeliveredURI, "./SCM/Inventory/Delivered/" );
_LIT8( KDownloadURI, "./SCM/Download/" );
#else
#include <nsmldmuri.h>

// Constants
_LIT8( KDeployedURI, "SCM/Inventory/Deployed/" );
_LIT8( KDeliveredURI, "SCM/Inventory/Delivered/" );
_LIT8( KDownloadURI, "SCM/Download/" );
#endif

#ifdef __NSML_DEBUG__
#pragma message("NSml Debugging is ON!")
#else
#endif


using namespace NApplicationManagement;



_LIT8( KAMStateValueActive, "Active" );
_LIT8( KAMStateValueInactive, "Inactive" );


_LIT8( KSisxMimeType, "x-epoc/x-sisx-app" );
_LIT8( KSisMimeType, "application/vnd.symbian.install" );
_LIT8( KPipMimeType, "application/x-pip" );
_LIT8( KJadMIMEType, "text/vnd.sun.j2me.app-descriptor" );
_LIT8( KJarMIMEType, "application/java-archive" );
_LIT8( KJavaMIMEType, "application/java" );

_LIT8( KInstallOptsMimeType, "text/xml" );



const TUint8 KNSmlDMAMSeparatorDef = 0x2f; //forward slash
#ifdef __AM_LASTERROR_NODE

#ifdef __TARM_SYMBIAN_CONVERGENCY
_LIT8( KAMLastErrorURI, "./SCM/Ext/LastError" );
#else
_LIT8( KAMLastErrorURI, "SCM/Ext/LastError" );
#endif
_LIT8( KStdError, "Error %d" );

#define MAPERROR(x,y,z) DefaultMapError(x,y,z)
#else

#define MAPERROR(x,y,z) DefaultMapError(x,y)
#endif


// ------------------------------------------------------------------------------------------------
// CAmAdapter* CAmAdapter::NewL( )
// ------------------------------------------------------------------------------------------------
CAmAdapter* CAmAdapter::NewL( MSmlDmCallback* aDmCallback )
	{
	RDEBUG("CAmAdapter::NewL(): begin");

	CAmAdapter* self = NewLC( aDmCallback );
	CleanupStack::Pop();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CAmAdapter* CAmAdapter::NewLC( )
// ------------------------------------------------------------------------------------------------
CAmAdapter* CAmAdapter::NewLC( MSmlDmCallback* aDmCallback )
	{
	CAmAdapter* self = new( ELeave ) CAmAdapter( aDmCallback );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}


// ------------------------------------------------------------------------------------------------
// CAmAdapter::CAmAdapter()
// ------------------------------------------------------------------------------------------------
CAmAdapter::CAmAdapter( TAny* aEcomArguments ) 
	: CSmlDmAdapter( aEcomArguments ), iSessionOpened ( EFalse ), iBuffer( 8 ), 
	iInAtomic( EFalse ), iUpdated( EFalse ), iIsStreamedContent( EFalse )
	{
	}


// ------------------------------------------------------------------------------------------------
// CAmAdapter::~CAmAdapter()
// ------------------------------------------------------------------------------------------------
CAmAdapter::~CAmAdapter()
	{
	RDEBUG( "CAmAdapter::~CAmAdapter" );
	iDbSession.Close();
	iManagement.Close(); // make sure we're closed
	iSessionOpened = EFalse ;
	iBuffer.Close();

	CloseStreaming();  // Ensures that no streams are left open and that the temporary file is removed
	
	delete	iStreamedURI;
	delete  iStreamedLuid;
	delete	iStreamedType;
	
#ifdef __AM_LASTERROR_NODE
	delete iLastError;
#endif


    if(iUriDel)
    delete iUriDel;
	RDEBUG( "CAmAdapter::~CAmAdapter end" );
	}

// ------------------------------------------------------------------------------------------------
//  CAmAdapter::DDFVersionL()
// ------------------------------------------------------------------------------------------------
void CAmAdapter::DDFVersionL( CBufBase& aDDFVersion )
	{
	aDDFVersion.InsertL( 0, KAMDDFVersion );
	}


// ------------------------------------------------------------------------------------------------
//  CAmAdapter::DDFStructureL()
//
// ------------------------------------------------------------------------------------------------
void CAmAdapter::DDFStructureL( MSmlDmDDFObject& aDDF )
	{
	RDEBUG( "CAmAdapter::DDFStructureL(): begin" );
	
	TSmlDmAccessTypes accessTypesGet;
	accessTypesGet.SetGet();
	
	TSmlDmAccessTypes accessTypesExec;
	accessTypesExec.SetExec();	

	TSmlDmAccessTypes accessTypesExecReplace = accessTypesExec;
	accessTypesExecReplace.SetReplace();
	
	TSmlDmAccessTypes accessTypesGetAdd = accessTypesGet;
	accessTypesGetAdd.SetAdd();
	
	TSmlDmAccessTypes accessTypesGetReplace = accessTypesGet;
	accessTypesGetReplace.SetGet();
		
	TSmlDmAccessTypes accessTypesGetAddReplace = accessTypesGetAdd;
	accessTypesGetAddReplace.SetReplace();
	
	TSmlDmAccessTypes accessTypesGetAddReplaceDelete = accessTypesGetAddReplace;
	accessTypesGetAddReplaceDelete.SetDelete();


	
	
	/*
	Node: ./SCM
	AM node is the common parent to all application management functionality nodes. 
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Get
	Values: N/A
	*/
	//MSmlDmDDFObject& root = aDDF.AddChildObjectGroupL();
	MSmlDmDDFObject& am = aDDF.AddChildObjectL( KAMNodeName );
	FillNodeInfoL( am, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMNodeDescription );
	
	/*
	Node: ./SCM/Inventory
	This node is start node to application inventory. 
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& inv = am.AddChildObjectL( KAMInventoryNodeName );
	FillNodeInfoL( inv, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMInventoryNodeDescription );
		
	/*
	Node: ./SCM/Inventory/Delivered
	This is node under which applications that are delivered but not installed are found.
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Get, Add
	Values: N/A
	*/
	MSmlDmDDFObject& del = inv.AddChildObjectL( KAMDeliveredNodeName );
	FillNodeInfoL( del, accessTypesGetAdd, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMDeliveredNodeDescription );
		
		
	/*
	Node: ./SCM/Inventory/Deployed
	This is a node under which active applications are found.
	The child node descriptions are same as Delivered sibling node, except that <X> cannot be created, data node does not exist and there's no InstallOpts leaf.
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Get
	Values: N/A
	*/
	
	MSmlDmDDFObject& ac = inv.AddChildObjectL( KAMDeployedNodeName );
	FillNodeInfoL( ac, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMDeployedNodeDescription );
	
	/*
	Node: ./SCM/Inventory/Delivered/<X>
	This dynamic node is placeholder applications that are in Delivered state. 
	Support: Mandatory
	Occurs: ZeroOrMore
	Format: Node
	Access Types: Get, Add, Replace, Delete
	Values: N/A
	*/
	MSmlDmDDFObject& deldyna = del.AddChildObjectGroupL();
	FillNodeInfoL( deldyna, accessTypesGetAddReplaceDelete, MSmlDmDDFObject::EZeroOrMore, 
		MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::ENode, KAMDeliveredDynaNodeDescription );

	/*
	Node: ./SCM/Inventory/Delivered/<X>/ID
	This leaf node holds an identifier for an application.
	Support: Mandatory
	Occurs: One
	Format: Chr
	Access Types: Get 
	Values: N/A
	*/
	MSmlDmDDFObject& delid = deldyna.AddChildObjectL( KAMIDNodeName );
	FillNodeInfoL( delid, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KAMIDNodeDescription );

	/*
	Node: ./SCM/Inventory/Delivered/<X>/Name
	This leaf node holds name of an application. 
	Support: Mandatory
	Occurs: One
	Format: Chr
	Access Types: Get ( + Add, Replace access within Delivered node only)
	Values: N/A
	*/
	MSmlDmDDFObject& delname = deldyna.AddChildObjectL( KAMNameNodeName );
	FillNodeInfoL( delname, accessTypesGetAddReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KAMNameNodeDescription );

	/*
	Node: ./SCM/Inventory/Delivered/<X>/Version
	This leaf node holds the version of an application. 
	Support: Mandatory
	Occurs: One
	Format: Chr
	Access Types: Get ( + Add, Replace access within Delivered node only)
	Values: N/A
	*/
	MSmlDmDDFObject& delver = deldyna.AddChildObjectL( KAMVersionNodeName );
	FillNodeInfoL( delver, accessTypesGetAddReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KAMVersionNodeDescription );


	/*
	Node: ./SCM/Inventory/Delivered/<X>/Data
	This leaf node holds the data of an application. 
	Support: Mandatory
	Occurs: One
	Format: Bin
	Access Types: Add, Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& delda = deldyna.AddChildObjectL( KAMDataNodeName );
	FillNodeInfoNoDefaultMimeL( delda, accessTypesGetAddReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EBin, KAMDataNodeDescription );
		
	delda.AddDFTypeMimeTypeL( KSisxMimeType );
	delda.AddDFTypeMimeTypeL( KSisMimeType );
	delda.AddDFTypeMimeTypeL( KPipMimeType );
	
	FeatureManager::InitializeLibL();
	if( FeatureManager::FeatureSupported(KFeatureIdJavaMIDP20) )
		{
		delda.AddDFTypeMimeTypeL( KJadMIMEType );
		delda.AddDFTypeMimeTypeL( KJarMIMEType );
		delda.AddDFTypeMimeTypeL( KJavaMIMEType);
		}
	
	FeatureManager::UnInitializeLib();
	
	

	/*
	Node: ./SCM/Inventory/Delivered/<X>/Descriptor
	This leaf node holds the possible metadata of an application. Descriptor can be for example such a data that is required by the actual data in the Data leaf, but for some reason they cannot be bundled into same package. An example is Java JAR and JAD file combination, in which JAD file could be placed in MetaData and JAR in Data leaf.
	Support: Optional
	Occurs: One
	Format: Bin
	Access Types: Add, Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& delmd = deldyna.AddChildObjectL( KAMDescriptorNodeName );
	FillNodeInfoL( delmd, accessTypesGetAddReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EBin, KAMDescriptorNodeDescription );

	/*
	Node: ./SCM/Inventory/Delivered/<X>/InstallOpts
	This is a node that holds the installation options of an application.  

	Support: Optional
	Occurs: ZeroOrOne
	Format: Xml
	Access Types: Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& delio = deldyna.AddChildObjectL( KAMInstallOptsNodeName );
	FillNodeInfoNoDefaultMimeL( delio, accessTypesGetAddReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EXml, KAMInstallOptsNodeDescription );
	delio.AddDFTypeMimeTypeL( KInstallOptsMimeType );
	
	/*
	Node: ./SCM/Inventory/Delivered/<X>/Operations
		This is a node that allows vendors to extend functionality.
	Support: Optional
	Occurs: One
	Format: Node
	Access Types: Get, Replace, Add, Delete
	Values: N/A
	*/
	MSmlDmDDFObject& delop = deldyna.AddChildObjectL( KAMOperationsNodeName );
	FillNodeInfoL( delop, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMOperationsNodeDescription );
		
	/*
	Node: ./SCM/Inventory/Delivered/<X>/Operations/Install
		Exec command causes device to install a delivered application. The data inside 
		exec command refers to the application to be installed. 
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Exec, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& loinst = delop.AddChildObjectL( KAMInstallNodeName );
	FillNodeInfoL( loinst, accessTypesExecReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMInstallNodeDescription );

	/*
	Node: ./SCM/Inventory/Delivered/<X>/Operations/InstallAndActivate
		Exec command causes device to install a delivered application. The data inside 
		exec command refers to the application to be installed. 
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Exec, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& loinstact = delop.AddChildObjectL( KAMInstallAndActivateNodeName );
	FillNodeInfoL( loinstact, accessTypesExecReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMInstallAndActivateNodeDescription );
	
	/*
	Node: ./SCM/Inventory/Delivered/<X>/Operations/Update
		Exec command causes device to update an application. The data inside exec command 
		refers to the application to be updated. 
		In practice the flow is so that:
		1.	Admin puts a new data with same ID in inventory/delivered or inventory/download.
		2.	Admin deactivates existing component with same ID. 
		3.	Server executes update with item data targeting to the ID.
		So actually update needs at least three "high level" commands; add data for update, 
			deactivate old, do the update.

	Support: Optional
	Occurs: One
	Format: Node
	Access Types: Exec, Replace
	Values: N/A

	*/
	MSmlDmDDFObject& loupd = delop.AddChildObjectL( KAMUpdateNodeName );
	FillNodeInfoL( loupd, accessTypesExecReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMUpdateNodeDescription );

	/*
	Node: ./SCM/Inventory/Delivered/<X>/Operations/UpdateAndActivate
	Exec command causes device to update an application and activate it. The data inside exec command refers to the application to be updated. 
	In practice the flow is so that:
	1.	Admin puts a new data with same ID in inventory/delivered or inventory/download.
	2.	Admin deactivates existing component with same ID. 
	3.	Server executes update with item data targeting to the ID.
	So actually update needs at least three "high level" commands; add data for update, deactivate old, do the update.

	Support: Optional
	Occurs: One
	Format: Node
	Access Types: Exec, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& loupda = delop.AddChildObjectL( KAMUpdateAndActivateNodeName );
	FillNodeInfoL( loupda, accessTypesExecReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMUpdateAndActivateNodeDescription );

	/*
	Node: ./SCM/Inventory/Delivered/<X>/Operations/Remove
	Exec command causes device to remove an application. The data inside exec command refers to the application to be removed. The application must be in Inactive state in order for remove to be successful.
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Exec, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& lorem = delop.AddChildObjectL( KAMRemoveNodeName );
	FillNodeInfoL( lorem, accessTypesExecReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMRemoveNodeDescription );
			
				
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
	DEPLOYED
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/


	/*
	Node: ./SCM/Inventory/Deployed/<X>
		This dynamic node is placeholder applications that are in Inactive state. 
	Support: Mandatory
	Occurs: ZeroOrMore
	Format: Node
	Access Types: Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& depldyna = ac.AddChildObjectGroupL();
	FillNodeInfoL( depldyna, accessTypesGetReplace, MSmlDmDDFObject::EZeroOrMore, MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::ENode, KAMDeployedDynaNodeDescription );

	/*
	Node: ./SCM/Inventory/Deployed/<X>/ID
		This leaf node holds an identifier for an application. This cannot be changed.
	Support: Mandatory
	Occurs: One
	Format: Chr
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& deplid = depldyna.AddChildObjectL( KAMIDNodeName );
	FillNodeInfoL( deplid, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KAMIDNodeDescription );
		
	/*
	Node: ./SCM/Inventory/Deployed/<X>/Name
		This leaf node holds name of an application.
	Support: Mandatory
	Occurs: One
	Format: Chr
	Access Types: Get, Add, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& deplname = depldyna.AddChildObjectL( KAMNameNodeName );
	FillNodeInfoL( deplname, accessTypesGetAddReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KAMNameNodeDescription );

	/*
	Node: ./SCM/Inventory/Deployed/<X>/Version
		This leaf node holds the version of an application.
	Support: Mandatory
	Occurs: One
	Format: Chr
	Access Types: Get, Add, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& deplver = depldyna.AddChildObjectL( KAMVersionNodeName );
	FillNodeInfoL( deplver, accessTypesGetAddReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KAMVersionNodeDescription );
		
	/*
	Node: ./SCM/Inventory/Deployed/<X>/StateValue
		This leaf node holds state value of an application. 
	Support: ?
	Occurs: One
	Format: Chr
	Access Types: Get 
	Values: N/A
	*/
	MSmlDmDDFObject& deplstate = depldyna.AddChildObjectL( KAMStateValueNodeName );
	FillNodeInfoL( deplstate, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KAMStateValueNodeDescription );



	/*
	Node: ./SCM/Inventory/Deployed/<X>/Operations
		This is a node that allows vendors to extend functionality.
	Support: Optional
	Occurs: One
	Format: Node
	Access Types: Get, Replace, Add, Delete
	Values: N/A

	*/
	MSmlDmDDFObject& deplop = depldyna.AddChildObjectL( KAMOperationsNodeName );
	FillNodeInfoL( deplop, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMOperationsNodeDescription );
		
		

	/*
	Node: ./SCM/Inventory/Deployed/<X>/Operations/Remove
		Exec command causes device to remove an application. The data inside exec command 
		refers to the application to be removed. The application must be in Inactive state 
		in order for remove to be successful.
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Exec, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& deplrem = deplop.AddChildObjectL( KAMRemoveNodeName );
	FillNodeInfoL( deplrem, accessTypesExecReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMRemoveNodeDescription );
		
	/*
	Node: ./SCM/Inventory/Deployed/<X>/Operations/Activate
		Exec command causes device to activate an application. 
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Exec, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& deplac = deplop.AddChildObjectL( KAMActivateNodeName );
	FillNodeInfoL( deplac, accessTypesExecReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMActivateNodeDescription );
		
	/*
	Node: ./SCM/Inventory/Deployed/<X>/Operations/Deactivate
		Exec command causes device to deactivate an application. 
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Exec, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& depldeac = deplop.AddChildObjectL( KAMDeActivateNodeName );
	FillNodeInfoL( depldeac, accessTypesExecReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMDeActivateNodeDescription );
			
/*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
	DOWNLOAD
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

	/*
	Node: ./SCM/Download
		This node is a root node for application download functionality. In practice download 
		works so that admin adds nodes under this node, and they are automatically moved to 
		Inventory/Delivered when the download has been finished. Running get command targeting 
		to this node returns a list of pending downloads. 
	Support: Mandatory 
	Occurs: One
	Format: Node
	Access Types: Get, Add, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& dow = am.AddChildObjectL( KAMDownloadNodeName );
	FillNodeInfoL( dow, accessTypesGetAddReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMDownloadNodeDescription );


	/*
	Node: ./SCM/Download/<X>
	This node is a placeholder for identifier of an application that is to be downloaded. 
	Support: Mandatory 
	Occurs: One
	Format: Node
	Access Types: Get, Add, Replace, Delete
	Values: N/A
	*/
	MSmlDmDDFObject& dowdyna = dow.AddChildObjectGroupL();
	FillNodeInfoL( dowdyna, accessTypesGetAddReplaceDelete, MSmlDmDDFObject::EZeroOrMore, MSmlDmDDFObject::EDynamic,
		MSmlDmDDFObject::ENode, KAMDownloadDynaNodeDescription );


		/*
	Node: ./SCM/Download/<X>/ID
	This leaf node holds an identifier for an application. 
	Support: Mandatory
	Occurs: One
	Format: Chr
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& dowid = dowdyna.AddChildObjectL( KAMIDNodeName );
	FillNodeInfoL( dowid, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KAMIDNodeDescription );
		
	/*
	Node: ./SCM/Download/<X>/Name
	This leaf node holds name of an application. 
	Support: Mandatory
	Occurs: One
	Format: Chr
	Access Types: Add, Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& downame = dowdyna.AddChildObjectL( KAMNameNodeName );
	FillNodeInfoL( downame, accessTypesGetAddReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KAMNameNodeDescription );

	/*
	Node: ./SCM/Download/<X>/Version
	This leaf node holds the version of an application. 
	Support: Mandatory
	Occurs: One
	Format: Chr
	Access Types: Add, Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& dowver = dowdyna.AddChildObjectL( KAMVersionNodeName );
	FillNodeInfoL( dowver, accessTypesGetAddReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KAMVersionNodeDescription );

	/*
	Node: ./SCM/Download/<X>/URI
	This leaf node holds the URL from which the application should be downloaded.
	Support: Mandatory 
	Occurs: One
	Format: Chr
	Access Types: Add, Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& dowuri = dowdyna.AddChildObjectL( KAMURINodeName );
	FillNodeInfoL( dowuri, accessTypesGetAddReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KAMURINodeDescription );
	
	/*
	Node: ./SCM/Download/<X>/Status
	This leaf node holds the status of the download. The client updates the node value. The values are typically error codes of download protocol. Initially after the URI leaf is filled, this will get value 100, and will be replaced by one.
	Support: Optional 
	Occurs: One
	Format: Chr
	Access Types: Get
	Values: N/A
	*/
	MSmlDmDDFObject& dowstat = dowdyna.AddChildObjectL( KAMStatusNodeName );
	FillNodeInfoL( dowstat, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KAMStatusNodeDescription );

	/*
	Node: ./SCM/Download/<X>/InstallOpts
	This is a node that holds the installation options of an application.  

	Support: Optional
	Occurs: ZeroOrOne
	Format: Xml
	Access Types: Add, Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& dowio = dowdyna.AddChildObjectL( KAMInstallOptsNodeName );
	FillNodeInfoNoDefaultMimeL( dowio, accessTypesGetAddReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EXml, KAMInstallOptsNodeDescription );
	dowio.AddDFTypeMimeTypeL( KInstallOptsMimeType );
	
	/*
	Node: ./SCM/Download/<X>/ConRef
	This is a node that holds the installation options of an application.  

	Support: Optional
	Occurs: ZeroOrOne
	Format: Xml
	Access Types: Add, Get, Replace
	Values: N/A
	*/
	MSmlDmDDFObject& dowcr = dowdyna.AddChildObjectL( KAMConRefNodeName );
	FillNodeInfoL( dowcr, accessTypesGetAddReplace, MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KAMConRefNodeDescription );
					
	/*
	Node: ./SCM/Download/<X>/Operations
	This is a node that holds the operation nodes.
	Support: Optional
	Occurs: One
	Format: Node
	Access Types: Get, Replace, Add, Delete
	Values: N/A

	*/
	MSmlDmDDFObject& dowop = dowdyna.AddChildObjectL( KAMOperationsNodeName );
	FillNodeInfoL( dowop, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMOperationsNodeDescription );

	/*
	Node: ./SCM/Download/<X>/Operations/Download
	Exec command causes device to download an application. The dynamic node specifies the application to be downloaded. 
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Exec
	Values: N/A
	*/
	MSmlDmDDFObject& dowdown = dowop.AddChildObjectL( KAMDownloadOperationNodeName );
	FillNodeInfoL( dowdown, accessTypesExecReplace, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMDownloadOperationNodeDescription );
					
	/*
	Node: ./SCM/Download/<X>/Operations/DownloadAndInstall
	Exec command causes device to download and install an application. The dynamic node specifies the application to be downloaded. 
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Exec
	Values: N/A
	*/
	MSmlDmDDFObject& dowdowni = dowop.AddChildObjectL( KAMDownloadAndInstallNodeName );
	FillNodeInfoL( dowdowni, accessTypesExec, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMDownloadAndInstallOperationNodeDescription );
		
	/*
	Node: ./SCM/Download/<X>/Operations/DownloadAndInstallAndActivate
	Exec command causes device to download and install and activate an application. The dynamic node specifies the application to be downloaded. 
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Exec
	Values: N/A
	*/
	MSmlDmDDFObject& dowdownia = dowop.AddChildObjectL( KAMDownloadAndInstallAndActivateNodeName );
	FillNodeInfoL( dowdownia, accessTypesExec, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMDownloadAndInstallAndActivateNodeDescription );
		
	/*
	Node: ./SCM/Download/<X>/Operations/DownloadAndUpdate
	Exec command causes device to download and update an application. The dynamic node specifies the application to be downloaded. 
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Exec
	Values: N/A
	*/
	MSmlDmDDFObject& dowdownu = dowop.AddChildObjectL( KAMDownloadAndUpdateNodeName );
	FillNodeInfoL( dowdownu, accessTypesExec, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMDownloadAndUpdateNodeDescription );
		
	/*
	Node: ./SCM/Download/<X>/Operations/DownloadAndUpdateAndActivate
	Exec command causes device to download and install and activate an application. The dynamic node specifies the application to be downloaded. 
	Support: Mandatory
	Occurs: One
	Format: Node
	Access Types: Exec
	Values: N/A
	*/
	MSmlDmDDFObject& dowdownua = dowop.AddChildObjectL( KAMDownloadAndUpdateAndActivateNodeName );
	FillNodeInfoL( dowdownua, accessTypesExec, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMDownloadAndUpdateAndActivateNodeName );
		
		
#ifdef __AM_LASTERROR_NODE
	MSmlDmDDFObject& amext = am.AddChildObjectL( KAMExtNodeName );
	FillNodeInfoL( amext, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::ENode, KAMExtNodeDescription );
		
	MSmlDmDDFObject& amerr = amext.AddChildObjectL( KAMLastErrorNodeName );
	FillNodeInfoL( amerr, accessTypesGet, MSmlDmDDFObject::EOne, MSmlDmDDFObject::EPermanent,
		MSmlDmDDFObject::EChr, KAMLastErrorDescription );
#endif
	
    if (GetAdapterValue() == KAMSCMAdapter)
        {
        CheckStateChangesL();
        SetAdapterValue(KAMDefaultValue);
        }
	RDEBUG( "CAmAdapter::DDFStructureL(): end" );	
	}

// ------------------------------------------------------------------------------------------------
// CAmAdapter::SessionL()
// ------------------------------------------------------------------------------------------------
RApplicationManagement &CAmAdapter::SessionL()
	{
	if ( !iSessionOpened )
		{
		TCertInfo info ;
		
		const TUid KCentralRepositoryUid = 
			{
			0x10207843
			};
		const TInt KCertKey = 0x01;
		CRepository *re = NULL;
		TRAPD( errx, re = CRepository::NewL ( KCentralRepositoryUid ) );
		if (errx == KErrNone && re)
			{
			TPckg<TCertInfo> pcert( info );
			errx = re->Get( KCertKey, pcert ) ;
				
			RDEBUG_2("aCertInfo.iFingerprint.Length() is  %u",info.iFingerprint.Length() );
			if ( errx == KErrNone )
				{
				RDEBUG("CAmAdapter::SessionL() errx == KErrNone");
				}
			else
				{
				if ( errx != KErrNotFound )
					{
					RDEBUG("CAmAdapter::SessionL() errx != KErrNotFound");
					}
				else
					{
					RDEBUG("CAmAdapter::SessionL() errx == KErrNotFound");
					}
				
				}	
			delete re ;
			}
					
		if ( errx == KErrNotFound || !iCertRequired)
			{
			RDEBUG("CAmAdapter::SessionL() calling iManagement.Connect()");
			User::LeaveIfError( iManagement.Connect() );
			iSessionOpened = ETrue;
			iTrustAdded = EFalse;
			}
		else
			{
			RDEBUG("CAmAdapter::SessionL() calling iManagement.Connect( info )");
			User::LeaveIfError( iManagement.Connect( info ) );
			iSessionOpened = ETrue;
			iTrustAdded = ETrue;
			}
		}
	return iManagement;		
	}

// ------------------------------------------------------------------------------------------------
//  CAmAdapter::UpdateLeafObjectL()
// ------------------------------------------------------------------------------------------------
void CAmAdapter::UpdateLeafObjectL( 
	const TDesC8& aURI, 
	const TDesC8& aLUID, 
	const TDesC8& aObject, 
	const TDesC8& aType, 
	TInt aStatusRef )
	{
	RDEBUG8_4("CAmAdapter::UpdateLeafObjectL(): begin, '%S', '%S' '%S'", &aURI, &aLUID, &aType );

	MSmlDmAdapter::TError status = EOk;
	SetAdapterValue(KAMSCMAdapter);
	_UpdateLeafObjectL( aURI, aLUID, aObject, aType, aStatusRef, status);

	SetStatusL( aStatusRef, status );
	if ( status == EOk )
		{
		if ( iInAtomic )
			{
			iBuffer.Append( TAMCommandBufferElement( aStatusRef, aURI ) );
			}
		}
	RDEBUG_2("CAmAdapter::UpdateLeafObjectL(): end (%d)", status);
	}

// ------------------------------------------------------------------------------------------------
//  CAmAdapter::UpdateLeafObjectL()
// ------------------------------------------------------------------------------------------------
void CAmAdapter::_UpdateLeafObjectL( 
	const TDesC8& aURI, 
	const TDesC8& aLUID, 
	const TDesC8& aObject, 
	const TDesC8& aType, 
	TInt /*aStatusRef*/,
	MSmlDmAdapter::TError& aStatus )
	{
	RDEBUG("CAmAdapter::_UpdateLeafObjectL(): begin");

	CheckStateChangesL();

	TError status( EError );

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
	TInt numSeqs( NSmlDmURI::NumOfURISegs( uriPtrc ) );
	TPtrC8 mapping( NSmlDmURI::LastURISeg( uriPtrc ) );
	TPtrC8 parent( NSmlDmURI::RemoveLastSeg( uriPtrc ) );
#else
	TInt numSeqs( NSmlDmURI::NumOfURISegs( aURI ) );
	TPtrC8 mapping( NSmlDmURI::LastURISeg( aURI ) );
	TPtrC8 parent( NSmlDmURI::RemoveLastSeg( aURI ) );
#endif

	if (mapping == KAMNameNodeName)
	{
		if (aObject.Size() > KDeploymentComponentNameMaxLength)
		{
			aStatus = ETooLargeObject;
			return ;
		}
	}
	else if (mapping == KAMVersionNodeName)
	{
		if (aObject.Size() > KDeploymentComponentVersionMaxLength)
		{
			aStatus = ETooLargeObject;
			return ;
		}
	}
	else if(mapping == KAMConRefNodeName)
	{
		if (aObject.Size() > KMaxConRef)
		{
			aStatus = ETooLargeObject;
			return ;
		}
	}

	TPtrC8 grandParent( NSmlDmURI::RemoveLastSeg( parent ) );
	TPtrC8 grandParentMapping( NSmlDmURI::LastURISeg( grandParent ) );		
	RApplicationManagement &session = SessionL();
	TDeploymentComponentState state( EDCSNone ) ;	
	if ( numSeqs == 4 || numSeqs == 5 )
		{
		if ( numSeqs == 4 )
			{
			state = EDCSDownload;
			}
		else 
			{
			if ( grandParentMapping == KAMDeliveredNodeName )
				{
				state = EDCSDelivered;
				}
			else if ( grandParentMapping == KAMDeployedNodeName )
				{
				state = EDCSActive;
				}
			else
				{
				RDEBUG8_3( "CAmAdapter::UpdateLeafObjectL(): ILLEGAL LEVEL %d NODE %S", numSeqs, &aURI );
				User::Leave( KErrArgument );
				}
			}
		}
	else
		{
		RDEBUG8_3("CAmAdapter::UpdateLeafObjectL(): ILLEGAL LEVEL %d NODE %S", numSeqs, &aURI );
		}
	if ( state == EDCSDelivered || state == EDCSDownload ||state == EDCSActive ||state == EDCSInactive )
		{
		if ( aLUID != KNullDesC8 )
			{
			TInt iluid ( DesToIntL( aLUID ) );
		    TDeploymentComponent comp ;
		    TInt err( session.DeploymentComponent( iluid, comp ) );
		    if ( err == KErrNone )
		    	{
	    		TDeplCompAttrType fl( UpdateFlagFromMapping( mapping ) );

		    	if ( mapping == KAMDataNodeName || mapping == KAMDescriptorNodeName )
		    		{
		    		TInt erx(KErrNone);
		    		
					if ( iIsStreamedContent )
						{
						CloseStreaming();
			    		TRAP( erx, session.UpdateDeploymentComponentStreamedDataL( iluid, fl, aType ) );
						}
					else
						{
						// 
			    		TRAP( erx, session.UpdateDeploymentComponentDataL( iluid, fl, aObject, aType ) );
						}
		    		MAPERROR( erx, status, _L8("Update Delivered Data or metadata %d") );
		    		}
		    	else
		    		{
		    		if ( fl != EDCNone )
		    			{
		    			TPtrC8 obj;
		    			TAMInstallOptions opts;
		    			TAMInstallOptionsPckg optsb( opts );
		    			TPckgBuf<TInt> iap;
		    			if ( fl == EDCInstallOptions )
		    				{
		    				InstallOptionsParser::ParseOptionsL( aObject, opts );
		    				if ( opts.iIAP == -1 && opts.iConRef != KNullDesC8() )
		    					{
#ifdef __TARM_SYMBIAN_CONVERGENCY
							
			    				//CNSmlDMIAPMatcher* iapfinder = CNSmlDMIAPMatcher::NewLC( &Callback() );
			    				//opts.iIAP = iapfinder->IAPIdFromURIL( opts.iConRef );
			    				//CleanupStack::PopAndDestroy( iapfinder );
					            CArrayFix<TSmlDmMappingInfo>* mapArray = new CArrayFixFlat<TSmlDmMappingInfo>(4);
					            CleanupStack::PushL(mapArray);
					        
						        Callback().GetMappingInfoListL( KNSmlDMMMSSlashIAPUri,*mapArray );

								TInt iap = KErrNotFound; 
						        TSmlDmMappingInfo mapInfo;
					        
						        for(TInt i = 0;i<mapArray->Count();i++)
							        {
							        if(LastURISeg(opts.iConRef)==mapArray->At(i).iURISeg)
								        {
								        iap=DesToInt(mapArray->At(i).iURISegLUID);
						                break;
								        }
							        }
							    opts.iIAP = iap;   
						        mapArray->Reset();
						        CleanupStack::PopAndDestroy(); //mapArray
					    				
#else
							CNSmlDMIAPMatcher* iapfinder = CNSmlDMIAPMatcher::NewLC( &Callback() );
			    				opts.iIAP = iapfinder->IAPIdFromURIL( opts.iConRef );
			    				CleanupStack::PopAndDestroy( iapfinder );
#endif			    				
			    				}
		    				obj.Set( optsb );
		    				}
		    			else if ( fl == EDCConRef )
		    				{
#ifdef __TARM_SYMBIAN_CONVERGENCY
						
		    				//CNSmlDMIAPMatcher* iapfinder = CNSmlDMIAPMatcher::NewLC( &Callback() );
			    			//TInt i( iapfinder->IAPIdFromURIL( aObject ) );
			    			//CleanupStack::PopAndDestroy( iapfinder );
			    			//iap() = i;
				            CArrayFix<TSmlDmMappingInfo>* mapArray = new CArrayFixFlat<TSmlDmMappingInfo>(4);
				            CleanupStack::PushL(mapArray);
				        
					        Callback().GetMappingInfoListL( KNSmlDMMMSSlashIAPUri,*mapArray );

							TInt iapValue = KErrNotFound; 
					        TSmlDmMappingInfo mapInfo;
				        
					        for(TInt i = 0;i<mapArray->Count();i++)
						        {
						        if(LastURISeg(opts.iConRef)==mapArray->At(i).iURISeg)
							        {
							        iapValue=DesToInt(mapArray->At(i).iURISegLUID);
					                break;
							        }
						        }
						    iap() = iapValue;   
					        mapArray->Reset();
					        CleanupStack::PopAndDestroy(); //mapArray
			    			
#else
						CNSmlDMIAPMatcher* iapfinder = CNSmlDMIAPMatcher::NewLC( &Callback() );
			    			TInt i( iapfinder->IAPIdFromURIL( aObject ) );
			    			CleanupStack::PopAndDestroy( iapfinder );
			    			iap() = i;
#endif
			    			obj.Set( iap );
		    				}
		    			else
		    				{
		    				obj.Set( aObject );
		    				}
		    			TRAPD( erx, session.UpdateDeploymentComponentL( iluid, fl, obj ) );
			    		MAPERROR( erx, status, _L8("Update some attribute %d") );
		    			}
		    		else
		    			{
		    			status = ENotFound;
		    			RDEBUG( "CAmAdapter::UpdateLeafObjectL(): Unknown Target leaf" );
		    			}
		    		}
		    	}
			else
				{
				RDEBUG_3( "CAmAdapter::UpdateLeafObjectL(): FAILED TO GET COMPONENT OF ID %d: %d" , iluid, err );
				status = ENotFound;
				}
			}
		else
			{
			RDEBUG8_2( "CAmAdapter::UpdateLeafObjectL(): Faild to get luid allocation and find buffered for '%S'", &parent );	
			status = ENotFound;
			}
		}
	else
		{
		RDEBUG8_2( "CAmAdapter::UpdateLeafObjectL(): WARNING Tried to update illegal state leaf! '%S'", &grandParentMapping );
		}

	aStatus = status;

	RDEBUG("CAmAdapter::_UpdateLeafObjectL(): end");
	}
	
// ------------------------------------------------------------------------------------------------
// CAmAdapter::UpdateLeafObjectL
// ------------------------------------------------------------------------------------------------
void CAmAdapter::UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID,
									RWriteStream*& aStream, const TDesC8& aType,
		 							TInt aStatusRef )
{
	RDEBUG8_4("CAmAdapter::UpdateLeafObjectL() aStream: begin, '%S', '%S' '%S'", &aURI, &aLUID, &aType );

	TError status( CSmlDmAdapter::EOk );

	if(iStreamedURI)
	{
		delete iStreamedURI;
		iStreamedURI = NULL;		
	}
	iStreamedURI = aURI.AllocL();

	if(iStreamedLuid)
	{
		delete iStreamedLuid;
		iStreamedLuid = NULL;
	}
	iStreamedLuid = aLUID.AllocL();

	if(iStreamedType)
	{
		delete iStreamedType;
		iStreamedType = NULL;		
	}
	iStreamedType = aType.AllocL();

	if(!iStreamOpen)
		{
		RApplicationManagement &session = SessionL();
		TDeplCompAttrType attrType;
		TPtrC8 mapping( NSmlDmURI::LastURISeg( aURI ) );
		TDeplCompAttrType fl( UpdateFlagFromMapping( mapping ) );
		if ( mapping == KAMDataNodeName )
		{
			attrType = EDCData;
		}
		else if( mapping == KAMDescriptorNodeName )
		{
			attrType = EDCMetaData;
		}
		else
		{
			RDEBUG8_2("CAmAdapter::UpdateLeafObjectL(): Unknown mapping: (%S)", &mapping);
			status = CSmlDmAdapter::EError;
		}
		
		if( status == CSmlDmAdapter::EOk )
			{
			TInt iluid ( DesToIntL( aLUID ) );
			session.DeploymentComponentTempInstFileL( iluid, iStreamFile, attrType );
			iStream.Attach( iStreamFile );
			aStream = &iStream;
			iStreamOpen = ETrue;
			iIsStreamedContent = ETrue;
			}
		}
	if ( !iInAtomic )
		{
        iStatusRef = aStatusRef;		
		Callback().SetStatusL( aStatusRef, status );		
		}
	else
		{
		iBuffer.Append( TAMCommandBufferElement( aStatusRef, aURI ) );
		}	

	RDEBUG("CAmAdapter::UpdateLeafObjectL() aStream : end");
}		 						
	
// ------------------------------------------------------------------------------------------------
// CAmAdapter::UpdateFlagFromMapping( const TDesC8& aMapping )
// ------------------------------------------------------------------------------------------------
TDeplCompAttrType CAmAdapter::UpdateFlagFromMapping( const TDesC8& aMapping )
	{

	TDeplCompAttrType res ;
	if ( aMapping == KAMIDNodeName )
		{
		res = EDCId;
		}
	else if ( aMapping == KAMNameNodeName )
		{
		res = EDCName;
		}
	else if ( aMapping == KAMVersionNodeName )
		{
		res = EDCVersion;
		}
	else if ( aMapping == KAMStateValueNodeName )
		{
		res = EDCStateValue;
		}
	else if ( aMapping == KAMInstallOptsNodeName )
		{
		res = EDCInstallOptions;
		}
	else if ( aMapping == KAMDescriptorNodeName )
		{
		res = EDCMetaData;
		}
	else if ( aMapping == KAMDataNodeName )
		{
		res = EDCData;
		}
	else if( aMapping == KAMURINodeName )
		{
		res = EDCDownloadURI;
		}
	else if ( aMapping == KAMStatusNodeName )
		{
		res = EDCStatus;
		}
	else if ( aMapping == KAMConRefNodeName )
		{
		res = EDCConRef;
		}
	else
		{
		//User::Leave( KErrArgument );
		res = EDCNone;
		}
	return res;
	}
	
// ------------------------------------------------------------------------------------------------
// CAmAdapter::GetLuidForUserIdL( const TDesC8 &aUserId, 
//		const TDeploymentComponentState aState )
// ------------------------------------------------------------------------------------------------	
TUint32 CAmAdapter::GetLuidForUserIdL( const TDesC8 &aUserId, const TDeploymentComponentState aState )
	{
	TUint32 ret( 0 );
	if ( aState != EDCSNone )
		{
		RElementIdArray array;
		TInt err( SessionL().DeploymentComponentIdsL( array, aState ) );
		if ( err == KErrNone )
			{
			CleanupClosePushL( array );
			TInt count( array.Count() );
			for( TInt i( 0 ); i < count && ret == 0; i++ )
				{
				TUint32 el = array[i];
				TDCUserId dc;
				SessionL().DeploymentComponentUserIdL( el, dc );
				if ( dc == aUserId )
					{
					ret = el;
					}
				}
			CleanupStack::PopAndDestroy( &array );
			}
		else
			{
			}
		}
	return ret;	
	}
	
// ------------------------------------------------------------------------------------------------
// CAmAdapter::GetLuid2L( const TDesC8 &aDMLuid, const TDesC8 &aUserId, 
//		const TDeploymentComponentState aState, TError &aStatus )
// ------------------------------------------------------------------------------------------------	
TUint32 CAmAdapter::GetLuid2L( const TDesC8 &aDMLuid, const TDesC8 &aUserId, 
	const TDeploymentComponentState aState, TError &aStatus )
	{
	TUint32 ret( GetLuidL(aDMLuid, aUserId, aState ) );
	if ( (TInt32)ret <= 0 )
		{
		aStatus = ENotFound;
		}
	return ret;	
	}
	
// ------------------------------------------------------------------------------------------------
// CAmAdapter::GetLuidL( const TDesC8 &aDMLuid, const TDesC8 &aUserId, 
//		const TDeploymentComponentState aState )
// ------------------------------------------------------------------------------------------------	
TUint32 CAmAdapter::GetLuidL( const TDesC8 &aDMLuid, const TDesC8 &aUserId, 
	const TDeploymentComponentState aState )
	{
	TUint32 ret( 0 );
	if ( aDMLuid == KNullDesC8 )
		{
		ret = GetLuidForUserIdL( aUserId, aState );
		}
	else
		{
		ret = DesToIntL( aDMLuid ) ;
		}
	
	return ret;	
	}
	
// ------------------------------------------------------------------------------------------------
// CAmAdapter::DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TInt aStatusRef )
	{

	RDEBUG8_4("CAmAdapter::DeleteObjectL(): begin (%S, %S, %d)" , &aURI, &aLUID, aStatusRef );
	SetAdapterValue(KAMSCMAdapter);
	CheckStateChangesL();
	
	TError ret( EError );

#ifdef __TARM_SYMBIAN_CONVERGENCY	
	TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);	
	TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
	TPtrC8 mapping( NSmlDmURI::LastURISeg( uriPtrc ) );
#else
	TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) );
	TPtrC8 mapping( NSmlDmURI::LastURISeg( aURI ) );
#endif
	
	switch ( cnt )
		{
		case 3:
		case 4:
			{
			
#ifdef __TARM_SYMBIAN_CONVERGENCY
			TPtrC8 which( NSmlDmURI::URISeg( uriPtrc, cnt == 4 ? 3 : 2 ) );
#else
			TPtrC8 which( NSmlDmURI::URISeg( aURI, cnt == 4 ? 3 : 2 ) );
#endif
			if ( which == KAMDeliveredNodeName || which == KAMDownloadNodeName )
				{
				TUint32 luid( GetLuidL( aLUID, mapping, (which == KAMDeliveredNodeName ? EDCSDelivered : EDCSDownload) ) );
				if ( luid > 0 )
					{
					TRAPD( err, SessionL().RemoveL( luid ) );
					if ( err == KErrNone || err == KErrNotFound )
						{
						ret = EOk;
						Callback().SetMappingL( aURI, KNullDesC8 );
						if ( err == KErrNone && iInAtomic )
			    			{
			    			iBuffer.Append( TAMCommandBufferElement( aStatusRef, aURI ) );
			    			}
						}
					else
						{
						MAPERROR( err, ret, _L8("Delete %d") );	
						}
					}
				else
					{
					RDEBUG( "CAmAdapter::DeleteObjectL(): WARNING Object not found" );	
					ret = ENotFound;
					}
				}
			else
				{
				RDEBUG8_2( "CAmAdapter::DeleteObjectL(): ERROR Removing node %S is not supported", &aURI );	
				}
			break;
			}
		default:
			{
			RDEBUG8_3( "CAmAdapter::DeleteObjectL(): ILLEGAL LEVEL %d NODE %S", cnt, &aURI );
			break;
			}
		}
	SetStatusL( aStatusRef, ret );
	RDEBUG( "CAmAdapter::DeleteObjectL(): end" );
	}


// ------------------------------------------------------------------------------------------------
// CAmAdapter::DeactivateIfInactive( const TUint aLuidi, const TDeploymentComponentState aTargetState )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::DeactivateIfInactive( const TUint aLuidi, const TDeploymentComponentState aTargetState )
	{
	if ( aTargetState == EDCSInactive )
		{
		RDEBUG( "CAmAdapter::DeactivateIfInactive():  deactivating!" );
		TRAPD( derr, iManagement.DeactivateL( aLuidi ) );
		if ( derr != KErrNone )
			{
			RDEBUG_2("CAmAdapter::DeactivateIfInactive(): ERROR deactivating failed %d", derr );
			}
		}
	}

void CAmAdapter::InstallL( 
	TUint aLuidi, 
	const TDesC8& aURI, 
	const TDesC8& aLUID, 
	const TDeploymentComponentState aTargetState,
	TError &aRet
	 ) 
	{


	#ifdef __TARM_SYMBIAN_CONVERGENCY	
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);	
	#else
		//nothing
	#endif

	TRAPD( err, SessionL().InstallL( aLuidi, aTargetState ) );
	if ( err == KErrNone )
		{
		// Dont Update the mappings in client to reflect new
		//		position in the management tree ( Inactive or Active..)
		RDEBUG( "CAmAdapter::InstallL(): Install success!" );
		
		//TPtrC8 aclmapping( URISegsUpTo( aURI, 4) );
		//SetMappingL( aclmapping, aTargetState, aLUID);
		//iManagement.StateChangeComplete( aLuidi );			
						
		
		
		DeactivateIfInactive( aLuidi, aTargetState );
		aRet = EOk;
		}
	else
		{
		RDEBUG_2( "CAmAdapter::InstallL(): INSTALL FAILED '%d'" , err);	
		MAPERROR( err, aRet, _L8("Install %d") );
		}
	}
	
	
// ------------------------------------------------------------------------------------------------
// CAmAdapter::DefaultMapError( const TInt aErr, TError &aRet )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::DefaultMapError( const TInt aErr, TError &aRet, const TDesC8& 
	#ifdef __AM_LASTERROR_NODE
	aDes 
	#endif
	)
	{
	if ( aErr == KErrNone )
		{
		aRet = EOk;
		}
	else if ( aErr == KErrNotFound )
		{
		aRet = ENotFound;
		}
	else if ( aErr == KErrArgument )
		{
		aRet = EInvalidObject ;
		}
	else if( aErr == KErrAlreadyExists )
		{
		aRet = EAlreadyExists;
		}
	else
		{
		aRet = EError;
		}
	#ifdef __AM_LASTERROR_NODE
	TInt err = KErrNone;
	TRAP( err, SetErrorL( aDes, aErr ) );
	#endif
	}

#ifdef __AM_LASTERROR_NODE	
void CAmAdapter::SetErrorL( const TDesC8& aDes, const TInt aErr )
	{
	if ( aDes != KNullDesC8 )
		{
		SetLastErrorL( aDes, aErr );
		}
	else
		{
		SetLastErrorL(KStdError, aErr);
		}
	}
#endif	
	
	
// ------------------------------------------------------------------------------------------------
// CAmAdapter::ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aArgument, 
//	const TDesC8& aType, 
//	TInt aStatusRef )
// ------------------------------------------------------------------------------------------------
TDownloadTarget CAmAdapter::DownloadTargetL( const TDesC8& aCommand )
	{
	if ( aCommand == KAMDownloadOperationNodeName )
		{
		return EDeliver;
		}
	else if ( aCommand == KAMDownloadAndInstallNodeName )
		{
		return EInstall;
		}
	else if ( aCommand == KAMDownloadAndInstallAndActivateNodeName )
		{
		return EInstallAndActivate;
		}
	else if ( aCommand == KAMDownloadAndUpdateNodeName )
		{
		return EUpdate;
		}
	else if ( aCommand == KAMDownloadAndUpdateAndActivateNodeName )
		{
		return EUpdateAndActivate;
		}

	RDEBUG8_2( "CAmAdapter::DownloadTargetL(): ASKED OPERATION NOT SUPPORTED Download '%S'", &aCommand );
	User::Leave( KErrArgument );
	// NOTE! will not ever come here! Only to get rid of compiler warning !!
	return EDeliver;
	}
	
// ------------------------------------------------------------------------------------------------
// CAmAdapter::ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aArgument, 
//	const TDesC8& aType, 
//	TInt aStatusRef )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::ExecuteCommandL( const TDesC8& aURI, 
	const TDesC8& aLUID, 
	const TDesC8& /*aArgument*/, 
	const TDesC8& /*aType*/, 
	TInt aStatusRef ) 
	{


	#ifdef __TARM_SYMBIAN_CONVERGENCY	
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);	
	#else
		//nothing
	#endif

	RDEBUG8_3("CAmAdapter::ExecuteCommandL(): begin, '%S', '%S'", &aURI, &aLUID );
	SetAdapterValue(KAMSCMAdapter);
	CheckStateChangesL();
	TError ret( EError );
	
	if( aLUID == KNullDesC8 )
		{
		RDEBUG("CAmAdapter::ExecuteCommandL(): WARNING no valid luid provided" );
		}
	
	#ifdef __TARM_SYMBIAN_CONVERGENCY	
		TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
	#else
		TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) );
	#endif	
	
	TPtrC8 mapping( NSmlDmURI::LastURISeg( aURI ) );
	
	const TInt KDownloadMappingURILoc = 3;
	switch ( cnt )
		{
		case 5:
			{
			TUint32 iluid( GetLuidL( aLUID, NSmlDmURI::URISeg(aURI, KDownloadMappingURILoc), EDCSDownload  ) ) ;	
			
			// Track the Newly added delivered  node to make sure that this reference
			// of delivered node is removed from Tree DB.
			// This delivered node is removed only if Execute command is successful
			// by either adding Install, Install& activate etc.
			// This Fix would be applicable in case of DM 1.2. 
				


			TPtrC8 urival(URISegsUpTo(aURI, KDownloadMappingURILoc));
			
			if(iUriDel)
			{
				delete iUriDel;
				iUriDel = NULL;
			}
	
			
			
			iUriDel = urival.AllocL();
			
			RDEBUG_2("CAmAdapter::ExecuteCommandL(): luid is %d", iluid );
			if ( iluid > 0 )
				{
				TDownloadTarget target = DownloadTargetL( mapping );
		
				TRAPD( errx, SessionL().StartDownloadL( iluid, target ) );
			
		
				/*if(target == EInstall || target == EInstallAndActivate || target == EUpdate || target == EUpdateAndActivate)
				{
				TInt err( iDbSession.RemoveMappingInfoL( KAMAdapterImplUid, *iUriDel, ETrue ) );
				}*/
				if ( errx == KErrNone )
					{
					RDEBUG( "CAmAdapter::ExecuteCommandL(): StartDownloadL Successful " );	
					ret = EOk;
					}
				else
					{
					RDEBUG_2( "CAmAdapter::ExecuteCommandL(): StartDownloadL FAILED '%d'", errx );	
					if ( errx == KErrNotFound )
						{
						ret = ENotFound;
						}
					else if ( errx == KErrArgument )
						{
						ret = EInvalidObject ;
						}
					else
						{
						ret = EError;
						}
					}
		
				}
			else
				{
				ret = ENotFound;
				RDEBUG( "CAmAdapter::ExecuteCommandL(): case 5 Not found node" );
				}
			break;
			}
		case 6:
			{
			const TInt KInventoryMappingURILoc = 4 ;
			
	
			// Track the Newly added delivered  node to make sure that this reference
			// of delivered node is removed from Tree DB.
			// This delivered node is removed only if Execute command is successful
			// by either adding Install, Install& activate etc.
			// This Fix would be applicable in case of DM 1.2. 


			TPtrC8 urival(URISegsUpTo(aURI, KInventoryMappingURILoc));
			
			if(iUriDel)
			{
				delete iUriDel;
				iUriDel = NULL;
			}
	
			
			
			iUriDel = urival.AllocL();
			
			if ( mapping == KAMInstallNodeName || mapping == KAMInstallAndActivateNodeName )
				{
				TUint32 iluid( GetLuidL( aLUID, NSmlDmURI::URISeg(aURI, KInventoryMappingURILoc), EDCSDelivered  ) ) ;
				if( iluid > 0 )
					{
					TDeploymentComponentState targetstate = ((mapping == KAMInstallNodeName) ? EDCSInactive : EDCSActive);
					InstallL( iluid, aURI, aLUID, targetstate, ret );	
					
					//TInt err( iDbSession.RemoveMappingInfoL( KAMAdapterImplUid, *iUriDel, iluid ) );
					}
				else
					{
					ret = ENotFound;
					RDEBUG( "CAmAdapter::ExecuteCommandL(): case 6 install Not found node" );
					}
				}
			else if ( mapping == KAMRemoveNodeName )	
				{
				TUint32 iluid( GetLuidL( aLUID, NSmlDmURI::URISeg(aURI, KInventoryMappingURILoc), EDCSDelivered  ) ) ;
				if ( (TInt32)iluid <= 0 )
					{
					iluid = GetLuidL( aLUID, NSmlDmURI::URISeg(aURI, KInventoryMappingURILoc), EDCSActive  )  ;
					if ( (TInt32)iluid <= 0 )
						{
						iluid = GetLuidL( aLUID, NSmlDmURI::URISeg(aURI, KInventoryMappingURILoc), EDCSInactive  )  ;
						}
					}
				if( iluid > 0 )
					{
					TRAPD( err, SessionL().RemoveL( iluid ) );
					if ( err == KErrNone )
						{
						RDEBUG( "CAmAdapter::ExecuteCommandL(): Remove success!" );
						TPtrC8 aclmapping( URISegsUpTo( aURI, 4) );
						
						// "Nullify" the mapping for argument
						DirectSetMappingL( aclmapping, KNullDesC8 );
						ret = EOk;
						if ( iInAtomic )
			    			{
			    			iBuffer.Append( TAMCommandBufferElement( aStatusRef, aURI ) );
			    			}
						}
					else
						{
						RDEBUG_2( "CAmAdapter::ExecuteCommandL(): Remove FAILED '%d'", err);	
						
						MAPERROR( err, ret, _L8("Execute Remove %d") );
						}
					}
				else
					{
					ret = ENotFound;
					RDEBUG( "CAmAdapter::ExecuteCommandL(): case 6 remove Not found node" );
					}
				}
			else if ( mapping == KAMUpdateNodeName )
				{
				RDEBUG8_2( "CAmAdapter::ExecuteCommandL(): Update: '%S'", &mapping );
				TUint32 iluid( GetLuidL( aLUID, NSmlDmURI::URISeg(aURI, KInventoryMappingURILoc), EDCSDelivered  ) ) ;
				if( iluid > 0 )
					{
					UpdateL( iluid, aURI, aLUID, EDCSInactive, ret );
					//TInt err( iDbSession.RemoveMappingInfoL( KAMAdapterImplUid, *iUriDel, iluid ) );
					}
				else
					{
					ret = ENotFound;
					RDEBUG( "CAmAdapter::ExecuteCommandL(): case 6 Update Not found node" );
					}
				}
			else if ( mapping == KAMUpdateAndActivateNodeName )
				{
				RDEBUG8_2( "CAmAdapter::ExecuteCommandL(): UpdateAndActivate: '%S'", &mapping );
				TUint32 iluid( GetLuidL( aLUID, NSmlDmURI::URISeg(aURI, KInventoryMappingURILoc), EDCSDelivered  ) ) ;
				if( iluid > 0 )
					{
					UpdateL( iluid, aURI, aLUID, EDCSActive, ret );	
					//TInt err( iDbSession.RemoveMappingInfoL( KAMAdapterImplUid, *iUriDel, iluid ) );	
					}
				else
					{
					ret = ENotFound;
					RDEBUG( "CAmAdapter::ExecuteCommandL(): case 6 UpdateAndActivate Not found node" );
					}
				}
			else if ( mapping == KAMActivateNodeName )
				{
				//DBG_ARGS8( "CAmAdapter::ExecuteCommandL(): Activating " );
				TUint32 iluid( GetLuidL( aLUID, NSmlDmURI::URISeg(aURI, KInventoryMappingURILoc), EDCSActive ) );
				if ( (TInt32)iluid <= 0 )
					{
					iluid = GetLuidL( aLUID, NSmlDmURI::URISeg(aURI, KInventoryMappingURILoc), EDCSInactive  )  ;
					}
				if( iluid > 0 )
					{
					TRAPD( erx, SessionL().ActivateL( iluid ) );
					
					MAPERROR( erx, ret, _L8("Activate %d") );
					}
				else
					{
					ret = ENotFound;
					RDEBUG( "CAmAdapter::ExecuteCommandL(): case 6 Activate Not found node" );
					}
				}
			else if ( mapping == KAMDeActivateNodeName )
				{
				//DBG_ARGS8( "CAmAdapter::ExecuteCommandL(): Inactivating " );
				TUint32 iluid( GetLuidL( aLUID, NSmlDmURI::URISeg(aURI, KInventoryMappingURILoc), EDCSActive ) );
				if ( (TInt32)iluid <= 0 )
					{
					iluid = GetLuidL( aLUID, NSmlDmURI::URISeg(aURI, KInventoryMappingURILoc), EDCSInactive  )  ;
					}
				if( iluid > 0 )
					{
					TRAPD( erx, SessionL().DeactivateL( iluid ) );
					
					MAPERROR( erx, ret,_L8("Dectivate %d") );
					}
				else
					{
					ret = ENotFound;
					RDEBUG( "CAmAdapter::ExecuteCommandL(): case 6 DeActivate Not found node" );
					}
				}	
			else
				{
				RDEBUG8_2( "CAmAdapter::ExecuteCommandL(): ASKED NOT SUPPORTED OPERATION '%S'", &mapping );
				User::Leave( KErrArgument );
				}
			break;
			}
		
		default:
			{
			RDEBUG8_3( "CAmAdapter::ExecuteCommandL(): ILLEGAL LEVEL %d NODE %S", cnt, &aURI );
			User::Leave( KErrArgument );			
			break;
			}
		}
	SetStatusL( aStatusRef, ret ) ;
	}



// ------------------------------------------------------------------------------------------------
// CAmAdapter::UpdateL( 
// 	const TUint aLuidi, 
// 	const TDesC8& aURI, 
// 	const TDesC8& aSourceLUID,
// 	const TDeploymentComponentState aTargetState,
// 	TError &aRet
// ------------------------------------------------------------------------------------------------
void CAmAdapter::UpdateL( 
	const TUint aLuidi, 
	const TDesC8& aURI, 
	const TDesC8& aSourceLUID,
	const TDeploymentComponentState aTargetState,
	TError &aRet
	 ) 
	{	


	#ifdef __TARM_SYMBIAN_CONVERGENCY	
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);	
	#else
		//nothing
	#endif

	CBufBase *delivered = CBufFlat::NewL( 128 );
	CleanupStack::PushL( delivered );
	
	#ifdef __TARM_SYMBIAN_CONVERGENCY	
		TPtrC8 aclmapping( NSmlDmURI::URISeg( uriPtrc, 4 ) );
	#else
		TPtrC8 aclmapping( NSmlDmURI::URISeg( aURI, 4 ) );
	#endif

	
	HBufC8 *targetURI = HBufC8::NewLC( KDeployedURI().Length() + aclmapping.Length() );
	
	TPtr8 targetPtr( targetURI->Des() );
	targetPtr.Copy( KDeployedURI );
	targetPtr.Append( aclmapping );
	
	HBufC8 *deployed = Callback().GetLuidAllocL( targetPtr ) ;
	CleanupStack::PushL( deployed );
	
	//DBG_ARGS8( "CAmAdapter::UpdateL(): Updating '%S' luid: '%S'" ), &targetPtr, &(*deployed));
	if ( *deployed == KNullDesC8 )
		{
		//DBG_ARGS8( "CAmAdapter::UpdateL(): Failed to find to update target '%S'" ), &targetPtr );
		aRet = EError;
		}
	else
		{
		// Found delivered & active or inactive components! Do update!
		TInt depluid( DesToIntL( *deployed ) );
		TRAPD( erx, SessionL().UpdateL( aLuidi, depluid ) );
		MAPERROR( erx, aRet,_L8("Update %d") );
		if ( erx == KErrNone )
			{
			//SetMappingLC( aclmapping, aTargetState, aSourceLUID, targetURI );
			DeactivateIfInactive( aLuidi, aTargetState ); 
			}
		}
	CleanupStack::PopAndDestroy( deployed ) ;
	CleanupStack::PopAndDestroy( targetURI ) ;
	CleanupStack::PopAndDestroy( delivered ) ;
	}


// ------------------------------------------------------------------------------------------------
//  CAmAdapter::CheckStateL( const TDeploymentComponent &aComp, const TDesC8& aURI
// ------------------------------------------------------------------------------------------------
TBool CAmAdapter::CheckStateL( const TDeploymentComponent &aComp, const TDesC8& aURI )
	{
	// Scenarios like "./SCM/Download/Node1/Operations/Install" NumOfURISegs won't work.
	
	TBool ret;
	if ((aComp.iState ==  EDCSDelivered) || (aComp.iState == EDCSActive) || ( aComp.iState == EDCSInactive) || (aComp.iState == EDCSDownload))
	{
        // Check for map & __TARM_SYMBIAN_CONVERGENCY also ?
        ret = ETrue;
    }
	else
	    ret = EFalse;
	return ret;
#if 0
	#ifdef __TARM_SYMBIAN_CONVERGENCY
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);	
		TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) ) ;
	#else
		TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) ) ;
	#endif

	TBool ret( EFalse );
	//TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) ) ;
	if ( cnt == 4 )
		{
		#ifdef __TARM_SYMBIAN_CONVERGENCY	
			TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);	
			TPtrC8 map( NSmlDmURI::URISeg( uriPtrc, 3 ) );
		#else
			TPtrC8 map( NSmlDmURI::URISeg( aURI, 3 ) );
		#endif		
		switch ( aComp.iState )
			{
			case EDCSDelivered:
				ret = (map == KAMDeliveredNodeName);
				break;
			case EDCSActive:
			case EDCSInactive:
				ret = (map == KAMDeployedNodeName);
				break;
			default:
				ret = EFalse;
				break;
			}
		}
	else if ( cnt == 3 )
		{
		if ( NSmlDmURI::URISeg( aURI, 2 ) == KAMDownloadNodeName && aComp.iState == EDCSDownload) 
			{
			ret = ETrue; 
			}
		}
	return ret;
#endif
	}


// ------------------------------------------------------------------------------------------------
//  CAmAdapter::StateL( const TDeploymentComponent &aComp, const TDesC8& aURI
// ------------------------------------------------------------------------------------------------
TDeploymentComponentState CAmAdapter::StateL( const TDesC8& aURI )
	{


	#ifdef __TARM_SYMBIAN_CONVERGENCY	
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);	
	#else
		//nothing
	#endif

	TDeploymentComponentState ret( EDCSNone );
	#ifdef __TARM_SYMBIAN_CONVERGENCY	
		TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) ) ;	
	#else
		TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) ) ;
	#endif	
	if ( cnt > 2)
		{
		#ifdef __TARM_SYMBIAN_CONVERGENCY	
			TPtrC8 map( NSmlDmURI::URISeg( uriPtrc, 2 ) );	
		#else
			TPtrC8 map( NSmlDmURI::URISeg( aURI, 2 ) );
		#endif	
		if ( map == KAMInventoryNodeName )
			{
			#ifdef __TARM_SYMBIAN_CONVERGENCY
				TPtrC8 map( NSmlDmURI::URISeg( uriPtrc, 2 ) );	
			#else
				TPtrC8 map( NSmlDmURI::URISeg( aURI, 2 ) );
			#endif	
			map.Set( NSmlDmURI::URISeg( aURI, 3 ) );
			if ( map == KAMDeployedNodeName )
				{
					
				}
			else if ( map == KAMDeliveredNodeName )
				{
				ret = EDCSDelivered;
				}
			else
				{
				User::Leave( KErrArgument );
				}
			}
		else if( map == KAMDownloadNodeName )
			{
			ret = EDCSDownload;
			}
		else
			{
			User::Leave( KErrArgument );
			}
		}
	return ret;
	}
	

// ------------------------------------------------------------------------------------------------
//  CAmAdapter::CheckStateL( const TDesC8& aURI, const TDesC8& aLUID )
// ------------------------------------------------------------------------------------------------
TBool CAmAdapter::CheckStateL( const TDesC8& aURI, const TDesC8& aLUID )
	{


	TUint32 iluid( DesToIntL( aLUID ) );
    TDeploymentComponent comp ;
    TInt err( SessionL().DeploymentComponent( iluid, comp ) );
    TBool ret( EFalse );
    if ( err == KErrNone )
    	{
		ret = CheckStateL( comp, aURI );
    	}
	return ret;
	}

// ------------------------------------------------------------------------------------------------
//  CAmAdapter::InstallOptionsDataL(const TUint32 iluid, const TDeplCompAttrType& aDataType, 
//		CBufBase &aBuf, CBufBase *aRaw/*= NULL*/)
// ------------------------------------------------------------------------------------------------
void CAmAdapter::InstallOptionsDataL(const TUint32 iluid, const TDeplCompAttrType& aDataType, 
		CBufBase &aBuf, CBufBase *aRaw/*= NULL*/)
	{
	TAMInstallOptionsPckgBuf iop;
	TBool pushed( EFalse );
	if ( !aRaw ) 
		{
		aRaw = CBufFlat::NewL(32);
		CleanupStack::PushL( aRaw );
		pushed = ETrue;
		SessionL().DeploymentComponentDataL( iluid, aDataType, *aRaw );
		}
	
	iop.Copy( aRaw->Ptr( 0 ) );
	if ( pushed )
		{
		CleanupStack::PopAndDestroy( aRaw );
		}
	InstallOptionsParser::SerializeOptionsL( iop(), aBuf);
	}

	
// ------------------------------------------------------------------------------------------------
//  CAmAdapter::GetComponentDataL( const TDesC8& parent, const TDesC8& mapping, 
//		const TUint32 iluid, CBufBase *currentList, CBufBase *currentMime, TError &status )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::GetComponentDataL( const TDesC8& parent, const TDesC8& mapping, 
		const TUint32 iluid, CBufBase &currentList, CBufBase &currentMime, TError &status )
	{
	TDeploymentComponent comp ;
    TInt err( SessionL().DeploymentComponent( iluid, comp ) );
    if ( err == KErrNone )
    	{
    	RDEBUG_3( "CAmAdapter::GetComponentDataL(): DeploymentComponent response: %d, state: %d", err, comp.iState );	
    	if ( CheckStateL( comp, parent ) )
    		{
			if ( mapping == KAMIDNodeName )
				{
				currentList.InsertL( 0, comp.iId );
				status = EOk;
				}
			else if ( mapping == KAMNameNodeName )
				{
				currentList.InsertL( 0, comp.iName );
				status = EOk;
				}
			else if ( mapping == KAMVersionNodeName )
				{
				currentList.InsertL( 0, comp.iVersion );
				status = EOk;
				}
			else
				{
				TDeplCompAttrType dataType( UpdateFlagFromMapping( mapping ) );
				if ( comp.iState == EDCSDownload )
					{
					if ( dataType == EDCDownloadURI || dataType == EDCStatus)
						{
						SessionL().DeploymentComponentDataL( iluid, dataType, currentList );
						status = EOk ;
						}
					else if (  dataType == EDCInstallOptions )
						{
						InstallOptionsDataL( iluid, dataType, currentList );
						status = EOk;
						}
					else if ( dataType == EDCConRef )
						{
						CBufBase *b = CBufFlat::NewL(4);
						CleanupStack::PushL( b );
						SessionL().DeploymentComponentDataL( iluid, dataType, *b);
						TPckgBuf<TInt> iap;
						iap.Copy( b->Ptr( 0 ) );
						CleanupStack::PopAndDestroy( b );
#ifdef __TARM_SYMBIAN_CONVERGENCY

			            CArrayFix<TSmlDmMappingInfo>* mapArray = new CArrayFixFlat<TSmlDmMappingInfo>(4);
			            CleanupStack::PushL(mapArray);
			        
				        Callback().GetMappingInfoListL( KNSmlDMMMSSlashIAPUri,*mapArray );

						//TInt iap = KErrNotFound; 
				        TSmlDmMappingInfo mapInfo;
				        HBufC8 * uri = NULL;
				        TPtr8 tempURI(uri->Des());
			        
				        for(TInt i = 0;i<mapArray->Count();i++)
					        {
						        if(iap()==DesToInt(mapArray->At(i).iURISegLUID))
						        {
									tempURI=mapArray->At(i).iURISeg;      	
						        }
						        
					        }
					    mapArray->Reset();
				        CleanupStack::PopAndDestroy(); //mapArray
		    			if ( uri != NULL )
		    				{
		    				CleanupStack::PushL( uri ) ;
			    			currentList.InsertL( 0, *uri );
			    			CleanupStack::PopAndDestroy( uri ) ;
		    				}				        
				        
#else
						
						CNSmlDMIAPMatcher* iapfinder = CNSmlDMIAPMatcher::NewLC( &Callback() );
		    			HBufC8 * uri = iapfinder->URIFromIAPIdL( iap() ) ;
		    			CleanupStack::PopAndDestroy( iapfinder );
		    			if ( uri != NULL )
		    				{
		    				CleanupStack::PushL( uri ) ;
			    			currentList.InsertL( 0, *uri );
			    			CleanupStack::PopAndDestroy( uri ) ;
		    				}
		    			
#endif
		    			status = EOk;
						}
					else
						{
						RDEBUG8_2( "CAmAdapter::GetComponentDataL(): ASKED NOT SUPPORTED LEAF '%S'", &mapping );
						}
					}
				else if ( comp.iState == EDCSDelivered )
					{
					if ( dataType == EDCData || dataType == EDCMetaData || dataType == EDCInstallOptions)
						{
						CBufBase *raw = CBufFlat::NewL( 128 );
						CleanupStack::PushL( raw );
						SessionL().DeploymentComponentDataL( iluid, dataType, *raw );
						TPtrC8 source( raw->Ptr(0) );
						if ( source.Length() > 0 )
							{
		    				if ( dataType == EDCData )
		    					{
								currentList.InsertL( 0, source );
								SessionL().DeploymentComponentDataL( iluid, EDCDataMime, currentMime );
						
		    					}
		    				else if ( dataType == EDCInstallOptions )
		    					{
		    					InstallOptionsDataL( iluid, dataType, currentList, raw );
		    					}
		    				else
		    					{
		    					currentList.InsertL( 0, source );
		    					SessionL().DeploymentComponentDataL( iluid, EDCMetaDataMime, currentMime );
		    					}
							}
						else
							{
							// 0 data length...
							}
						status = EOk;
						CleanupStack::PopAndDestroy( raw );
						RDEBUG8_2( "CAmAdapter::GetComponentDataL(): DeploymentComponentDataL called '%S'", &mapping );
						}
					}
				else if ( dataType == EDCStateValue )
					{
					if ( comp.iState == EDCSActive )
						{
						currentList.InsertL( 0, KAMStateValueActive() );
						}
					else 
						{
						currentList.InsertL( 0, KAMStateValueInactive() );
						}
					status = EOk;
					}
				else
					{
					RDEBUG8_2( "CAmAdapter::GetComponentDataL(): ERROR ASKED NOT SUPPORTED LEAF '%S'", &mapping );
					}	    	
				}
			}
    	else
    		{
    		RDEBUG8_3( "CAmAdapter::GetComponentDataL(): WARNING Asked illegal state leaf '%S', %d", &mapping, comp.iState );
	    	status = ENotFound;
    		}
    	}
	else
		{
		RDEBUG_3( "CAmAdapter::GetComponentDataL(): ERROR FAILED TO GET COMPOMENT OF ID %d: %d", iluid, err );
		MAPERROR( err, status,_L8("Get Component %d") );
		}
	}
	
// ------------------------------------------------------------------------------------------------
//  CAmAdapter::GetLeafLuidL(const TDesC8 &aURI, const TDesC8 & aParentMapping ) 
// ------------------------------------------------------------------------------------------------
TInt CAmAdapter::GetLeafLuidL(const TDesC8 &aURI, const TDesC8 & aParentMapping ) 
	{


	#ifdef __TARM_SYMBIAN_CONVERGENCY	
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
		TInt numSeqs( NSmlDmURI::NumOfURISegs( uriPtrc ) );	
	#else
		TInt numSeqs( NSmlDmURI::NumOfURISegs( aURI ) );
	#endif

	TUint32 iluid( 0 );
	//TInt numSeqs( NSmlDmURI::NumOfURISegs( aURI ) );
		
	switch ( numSeqs )
		{
		case 4: // must be a download leaf
			{
			iluid = GetLuidForUserIdL( aParentMapping, EDCSDownload ) ;
			break;
			}
		case 5: // download operation or delivered or deployed leaf
			{
			if ( URISegsUpTo( aURI, 2, ETrue ) == KDownloadURI )
				{
				RDEBUG8_2( "CAmAdapter::GetLeafLuidL(): WARNING download operation leaf '%S'", &aURI );
				//status = EOk; 
				}
			else 
				{
				TPtrC8 start( URISegsUpTo( aURI, 3, ETrue ) );
				if ( start == KDeliveredURI )	
					{
					iluid = GetLuidForUserIdL( aParentMapping, EDCSDelivered ) ;
					}
				else if ( start == KDeployedURI )
					{
					iluid = GetLuidForUserIdL( aParentMapping, EDCSActive ) ;
					if( (TInt32)iluid <= 0 )
						{
						iluid = GetLuidForUserIdL( aParentMapping, EDCSInactive ) ;
						}
					}
				else
					{
					RDEBUG8_2( "CAmAdapter::GetLeafLuidL(): WARNING strange uri '%S'", &aURI );
					}
				}
			break;
			}
		case 6: // delivered or deployed operation leaf
			{
			//iluid = GetLuidForUserIdL( parentMapping, EDCSDownload ) ;
			//status = EOk;
			RDEBUG8_2( "CAmAdapter::GetLeafLuidL(): WARNING delivered or deployed operation leaf '%S'", &aURI );
			break;
			}
		default: // illegal uri
			{
			RDEBUG8_2( "CAmAdapter::GetLeafLuidL(): ERROR requested luid of illegal URI '%S'", &aURI );
			break;
			}
		}
	return iluid;
	}
	
// ------------------------------------------------------------------------------------------------
//  CAmAdapter::FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, 
//			CBufBase& aObject )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, 
		const TInt aResultsRef, TInt aStatusRef )
	{

	#ifdef __TARM_SYMBIAN_CONVERGENCY	
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
	#else
		//Nothing
	#endif


	RDEBUG8_4("CAmAdapter::FetchLeafObjectL('%S', '%S', '%S'): begin", &aURI, &aLUID, &aType );
	SetAdapterValue(KAMSCMAdapter);
	CheckStateChangesL();
	TError status( EError );
	
	CBufBase *currentList = CBufFlat::NewL( 128 );
	CleanupStack::PushL( currentList );
	
	CBufBase *currentMime = CBufFlat::NewL( 128 );
	CleanupStack::PushL( currentMime );
	
	TPtrC8 mapping( NSmlDmURI::LastURISeg( aURI ) );
	TPtrC8 parent( NSmlDmURI::RemoveLastSeg( aURI ) );
	TPtrC8 parentMapping( NSmlDmURI::LastURISeg( parent ) );

	HBufC8 *luid = NULL;
	if ( aLUID != KNullDesC8 )
		{
		luid = aLUID.AllocLC();
		}
	else 
		{ // try if we find it from download path
		TBuf8<256> down;
		down = KDownloadURI;
		down.Append ( parentMapping );
		luid = Callback().GetLuidAllocL( down );
		CleanupStack::PushL( luid );
		if ( *luid != KNullDesC8 )
			{ 
			// make it to found next time
			DirectSetMappingL( parent, *luid );
			MoveAclL( down, parent );
			
			// remove old
			DirectSetMappingL( down, KNullDesC8 );
			}
		}
	if ( *luid != KNullDesC8 )
		{
		TUint32 iluid ( DesToIntL( *luid ) );
		RDEBUG8_2( "CAmAdapter::FetchLeafObjectL(): Luid is : %d", iluid );
	    GetComponentDataL( parent, mapping, iluid, *currentList, *currentMime, status );
		}
	else
		{
		TUint32 iluid( GetLeafLuidL( aURI, parentMapping ) );
		if ( iluid > 0 )
			{
			HBufC8 *l = IntToDes8LC( iluid );
			DirectSetMappingL( parent, *l );
			CleanupStack::PopAndDestroy( l );
			GetComponentDataL( parent, mapping, iluid, *currentList, *currentMime, status );
			}
		else
			{
			RDEBUG8_2( "CAmAdapter::FetchLeafObjectL(): WARNING Faild to get luid allocation for '%S'", &parent );
			if ( aLUID == KNullDesC8 )
				{
#ifdef __AM_LASTERROR_NODE
				if ( aURI == KAMLastErrorURI ) 
					{
					const TDesC8& error= LastError();
					RDEBUG8_2( "CAmAdapter::FetchLeafObjectL(): Returning last error '%S'", &error );
					currentList->InsertL(0, error );
					status = EOk;
					}
				else 
					{
					status = ENotFound;
					}
#else
				status = ENotFound ;
#endif
				}
			}
		}
	
	SetStatusL( aStatusRef, status );
	if( status == EOk )
		{
		if ( iInAtomic )
			{
			iBuffer.Append( TAMCommandBufferElement( aStatusRef, aURI ) );
			}
		Callback().SetResultsL( aResultsRef, *currentList, 
			( (currentMime->Size() > 0) ? currentMime->Ptr(0) : aType ) );
		}
	CleanupStack::PopAndDestroy( luid );
	CleanupStack::PopAndDestroy( currentMime ); //currentMime
	CleanupStack::PopAndDestroy( currentList ); //currentList
	RDEBUG("CAmAdapter::FetchLeafObjectL: end" );
	}


// ------------------------------------------------------------------------------------------------
//  CAmAdapter::IdListL( TDeploymentComponentState aState, 
//					CBufBase *aCurrentList, TBool aAppend /* = EFalse */)
// ------------------------------------------------------------------------------------------------
CAmAdapter::TError CAmAdapter::IdListL( 
	TDeploymentComponentState aState, 
	CBufBase &aCurrentList, TBool aAppend /* = EFalse */)
	{
	CAmAdapter::TError ret( CAmAdapter::EError );
	RElementIdArray array;
	TPtrC8 uriStart;
	TBuf8<256> uri;
	switch ( aState )
		{
		case EDCSDelivered:
			uriStart.Set( KDeliveredURI );
			break;
		case EDCSActive:
		case EDCSInactive:
			uriStart.Set( KDeployedURI ) ;
			break;
		case EDCSDownload:
			uriStart.Set( KDownloadURI ) ;
			break;
		default:
			break;
			
		}
	TInt err( SessionL().DeploymentComponentIdsL( array, aState ) );
	if ( err == KErrNone )
		{
		TInt count( array.Count() );
		if ( count && aAppend && aCurrentList.Size() )
			{
			aCurrentList.InsertL( aCurrentList.Size(), KAMSeparator8 );
			}
		for( TInt i( 0 ); i < count; i++ )
			{
			TUint32 el = array[i];
			TDCUserId dc;
			SessionL().DeploymentComponentUserIdL( el, dc );
			
			if ( uriStart == KNullDesC8() )
				{
				TDeploymentComponent comp ;
			    err = SessionL().DeploymentComponent( el, comp ) ;
			    if ( err == KErrNone )
			    	{
			    	switch ( comp.iState )
			    		{
			    		case EDCSDelivered:
							uriStart.Set( KDeliveredURI );
							break;
						case EDCSActive:
						case EDCSInactive:
							uriStart.Set( KDeployedURI ) ;
							break;
						case EDCSDownload:
							uriStart.Set( KDownloadURI ) ;
							break;
						default:
							break;
			    		}
			    	}
				}
			if ( uriStart != KNullDesC8() )
				{
				uri.Copy( uriStart );
				uri.Append( dc );
				HBufC8 *luid = DirectGetLuidAllocLC( uri ); //Callback().GetLuidAllocL( uri );
				if ( KNullDesC8() == *luid )				
					{
					RDEBUG8_3( "CAmAdapter::IdListL(): WARNING updating lost luid of '%S' %d", &uri, el );
					HBufC8 *l = IntToDes8LC( el );
					if ( *luid == *l )
						{
						RDEBUG( "CAmAdapter::IdListL(): WARNING CANCEL (not really updating, they're the same already" );
						}
					else
						{
						DirectSetMappingL( uri, *l );
						}
					CleanupStack::PopAndDestroy( l );
					}
				CleanupStack::PopAndDestroy( luid );
			
				}
			else
				{
				RDEBUG_2( "CAmAdapter::IdListL(): WARNING failed to get component state of id %d, cannot check mappings ", el );
				}
			aCurrentList.InsertL( aCurrentList.Size(), dc);
			if ( i + 1 < count )
				{
				aCurrentList.InsertL( aCurrentList.Size(), KAMSeparator8 );
				}
			}
		ret = EOk;
		}
	else
		{
		RDEBUG_2( "CAmAdapter::IdListL(): Error %d", err );
		}
	array.Reset();
	return ret; 	
	}
		
// ------------------------------------------------------------------------------------------------
//  CAmAdapter::ChildURIListL( const TDesC8& aURI, const TDesC8& aParentLUID, 
//			const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/, const TInt aResultsRef, 
//			TInt aStatusRef );
// ------------------------------------------------------------------------------------------------
void CAmAdapter::ChildURIListL( const TDesC8& aURI, const TDesC8& aParentLUID, 
	const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/, const TInt aResultsRef, 
	TInt aStatusRef  )
	{

	
	#ifdef __TARM_SYMBIAN_CONVERGENCY	
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
	#else
		//Nothing
	#endif

	RDEBUG8_3 ( "CAmAdapter::ChildURIListL(): begin ('%S', '%S')" , &aURI, &aParentLUID );
	CheckStateChangesL();
	
	TError ret( EError );
		
	CBufBase *currentList = CBufFlat::NewL( 128 );
	CleanupStack::PushL( currentList );

	#ifdef __TARM_SYMBIAN_CONVERGENCY	
		TInt numSeqs( NSmlDmURI::NumOfURISegs( uriPtrc ) );
	#else
		TInt numSeqs( NSmlDmURI::NumOfURISegs( aURI ) );
	#endif
	
	TPtrC8 mapping( NSmlDmURI::LastURISeg( aURI ) );
	if ( numSeqs == 1 ) 
		{
		currentList->InsertL( 0, KAMRootChilds );
		ret = EOk;
		}
	else if ( numSeqs == 2 )
		{
		if ( mapping == KAMInventoryNodeName)
			{
			currentList->InsertL( 0, KAMInventoryChilds );
			ret = EOk;
			}
		else if ( mapping == KAMDownloadNodeName )
			{
			RDEBUG8_2( "CAmAdapter::ChildURIListL(): Level 2 NODE %S", &aURI );
			TDeploymentComponentState state ( EDCSDownload );
			ret = IdListL( state, *currentList );
			}
		else
			{
			RDEBUG8_2( "CAmAdapter::ChildURIListL(): ILLEGAL LEVEL 2 NODE %S", &aURI );
			User::Leave( KErrArgument );
			}	
		}
	else if ( numSeqs == 3 )
		{
		if ( mapping == KAMDeliveredNodeName )
			{
			RDEBUG8_2( "CAmAdapter::ChildURIListL(): Level 3 NODE %S", &aURI );
			TDeploymentComponentState state ( EDCSDelivered );			
			ret = IdListL( state, *currentList );	
			}
		else if ( mapping == KAMDeployedNodeName )
			{
			TDeploymentComponentState state ( EDCSActive );
			ret = IdListL( state, *currentList );
			if ( ret == EOk )
				{
				state = EDCSInactive ;
				ret = IdListL( state, *currentList, ETrue );
				}
			}
		else
			{
			if ( aParentLUID != KNullDesC8 )
				{
				if ( CheckStateL( aURI, aParentLUID ) )
					{
					TPtrC8 parent( NSmlDmURI::RemoveLastSeg( aURI ) );
					TPtrC8 parentMapping( NSmlDmURI::LastURISeg( parent ) );
					if ( parentMapping == KAMDownloadNodeName )
						{
						currentList->InsertL( 0, KAMDownloadDynaChilds );
						ret = EOk;
						}
					else
						{
						RDEBUG8_2( "CAmAdapter::ChildURIListL(): ILLEGAL LEVEL 3 NODE %S", &aURI );
						User::Leave( KErrArgument );
						}
					}
				else
					{
					RDEBUG8_2( "CAmAdapter::ChildURIListL(): DC has changed state %S", &aURI );
					ret = ENotFound;	
					}
				}
			else
				{
				RDEBUG8_2( "CAmAdapter::ChildURIListL(): Level3 Node not found %S", &aURI );
				ret = ENotFound;
				}
			}
		}
	else if ( numSeqs == 4 )
		{
		TPtrC8 parent( NSmlDmURI::RemoveLastSeg( aURI ) );
		TPtrC8 parentMapping( NSmlDmURI::LastURISeg( parent ) );
		if ( aParentLUID == KNullDesC8 )
			{
			RDEBUG8_2( "CAmAdapter::ChildURIListL(): WARNING possibly a illegal level 4 node %S", &aURI );
			ret = ENotFound;
			}
				
		if ( parentMapping == KAMDeliveredNodeName )
			{
			if ( GetLuid2L( aParentLUID, mapping, EDCSDelivered, ret ) > 0 )
				{
				currentList->InsertL( 0, KAMInventoryDeliveredDynaChilds );
				ret = EOk;	
				}
			}
		else if ( parentMapping == KAMDeployedNodeName )
			{
			if ( GetLuid2L( aParentLUID, mapping, EDCSActive, ret ) > 0 )
				{
				currentList->InsertL( 0, KAMInventoryDynaChilds );
				ret = EOk;
				}
			else if ( GetLuid2L( aParentLUID, mapping, EDCSInactive, ret ) > 0 )
				{
				currentList->InsertL( 0, KAMInventoryDynaChilds );
				ret = EOk;
				}
			}
		else if ( mapping == KAMOperationsNodeName )
			{
		    if ( GetLuid2L( aParentLUID, parentMapping, EDCSDownload, ret ) > 0 )
		    	{
		    	if ( CheckStateL( aURI, aParentLUID ) )
		    		{
		    		currentList->InsertL( 0, KAMDownloadOperationChilds );
					ret = EOk;	
		    		}
		    	else
		    		{
		    		RDEBUG8_2( "CAmAdapter::ChildURIListL(): Level3 operations DC changed state '%S'", &aURI );
					ret = ENotFound;
		    		}
		    	}
			}
		else
			{
			RDEBUG8_2( "CAmAdapter::ChildURIListL(): Level4 Node not found '%S'", &aURI );
			ret = ENotFound;
			}
		}
	else if ( numSeqs == 5 )
		{
		if ( aParentLUID == KNullDesC8 )
			{
			RDEBUG8_2( "CAmAdapter::ChildURIListL(): WARNING possibly a illegal level 5 node %S", &aURI );
			ret = ENotFound;
			}
		if ( mapping == KAMOperationsNodeName )
			{
			TPtrC8 parent( NSmlDmURI::RemoveLastSeg( aURI ) );
			TPtrC8 parentMapping( NSmlDmURI::LastURISeg( parent ) );
			TPtrC8 gparent( NSmlDmURI::RemoveLastSeg( parent ) );
			TPtrC8 gparentMapping( NSmlDmURI::LastURISeg( gparent ) );
			if ( gparentMapping == KAMDeliveredNodeName )
				{
				if ( GetLuid2L( aParentLUID, parentMapping, EDCSDelivered, ret ) > 0 )
					{
					currentList->InsertL( 0, KAMDeliveredOperationChilds );
					ret = EOk;	
					}
				}
			else if ( gparentMapping == KAMDeployedNodeName )
				{
				if ( GetLuid2L( aParentLUID, parentMapping, EDCSActive, ret ) > 0 )
					{
					currentList->InsertL( 0, KAMDeployedOperationChilds );
					ret = EOk;
					}
				else if ( GetLuid2L( aParentLUID, parentMapping, EDCSInactive, ret ) > 0 )
					{
					currentList->InsertL( 0, KAMDeployedOperationChilds );
					ret = EOk;
					}
				}
			else 
				{
				RDEBUG8_2( "CAmAdapter::ChildURIListL(): Level5 Node not found wrong parent '%S'", &aURI );
				ret = ENotFound;
				}
			}
		else
			{
			RDEBUG8_2( "CAmAdapter::ChildURIListL(): Level5 Node not found '%S'", &aURI );
			ret = ENotFound;
			}
		}
	else 
		{
		RDEBUG8_3( "CAmAdapter::ChildURIListL(): ILLEGAL LEVEL %d NODE %S", numSeqs, &aURI );
		ret = EError;
		}
	
	SetStatusL( aStatusRef, ret );
	
	if( ret == EOk )
		{
		if ( iInAtomic )
			{
			iBuffer.Append( TAMCommandBufferElement( aStatusRef, aURI ) );
			}
		Callback().SetResultsL( aResultsRef, *currentList, KNullDesC8 );
		}
	CleanupStack::PopAndDestroy(); //currentList
	}


// ------------------------------------------------------------------------------------------------
//  CAmAdapter::DeliverOrDownloadL( const TDesC8 &aUserId, const TDesC8 &aURI, TInt aCnt, 
//		const TDesC8 &aParentMapping, MSmlDmAdapter::TError &aStatus )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::DeliverOrDownloadL( const TDesC8 &aUserId, const TDesC8 &aURI, TInt aCnt, 
	const TDesC8 &aParentMapping, MSmlDmAdapter::TError &aStatus )
	{


	#ifdef __TARM_SYMBIAN_CONVERGENCY	
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
	#else
		//Nothing
	#endif

	switch ( aCnt )
		{
		case 3:
			{
			if ( aParentMapping == KAMDownloadNodeName )
				{	
				TUint32 luid( SessionL().DownloadL( aUserId ) );
				Callback().SetMappingL( aURI, *IntToDes8LC( luid ) );
				CleanupStack::PopAndDestroy();
				aStatus = EOk;
				}
			else
				{
				RDEBUG( "CAmAdapter::DeliverOrDownloadL(): ERROR Illegal parent");
				}
			break;
			} 	
		case 4:
			{
			if ( aParentMapping == KAMDeliveredNodeName )
				{	
				TUint32 luid( SessionL().DeliverL( aUserId ) );
				Callback().SetMappingL( aURI, *IntToDes8LC( luid ) );
				CleanupStack::PopAndDestroy();
				aStatus = EOk;
				}
			else
				{
				RDEBUG( "CAmAdapter::DeliverOrDownloadL(): ERROR Illegal parent");
				}
			break;
			} 	
		default:
			{
			RDEBUG( "CAmAdapter::DeliverOrDownloadL(): ERROR Illegal URI");
			break;
			}
		}
	}			


// ------------------------------------------------------------------------------------------------
// CAmAdapter::AddNodeObjectL( const TDesC8& aURI, const TDesC& aParentLUID )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::AddNodeObjectL( const TDesC8& aURI, const TDesC8& aParentLUID, TInt aStatusRef )
	{


	#ifdef __TARM_SYMBIAN_CONVERGENCY	
		TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
		TPtrC8 mapping( NSmlDmURI::LastURISeg( uriPtrc ) );
		TPtrC8 parent( NSmlDmURI::RemoveLastSeg( uriPtrc ) );
		TPtrC8 parentMapping( NSmlDmURI::LastURISeg( parent ) );
		TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
	#else
		TPtrC8 mapping( NSmlDmURI::LastURISeg( aURI ) );
		TPtrC8 parent( NSmlDmURI::RemoveLastSeg( aURI ) );
		TPtrC8 parentMapping( NSmlDmURI::LastURISeg( parent ) );
		TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) );
	#endif

	RDEBUG8_3( "CAmAdapter::AddNodeObjectL(): begin '%S', '%S'", &aURI, &aParentLUID );
	SetAdapterValue(KAMSCMAdapter);
	CheckStateChangesL();
	TError status ( EError );
//	TPtrC8 mapping( NSmlDmURI::LastURISeg( aURI ) );
//	TPtrC8 parent( NSmlDmURI::RemoveLastSeg( aURI ) );
//	TPtrC8 parentMapping( NSmlDmURI::LastURISeg( parent ) );
//	TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) );
	TUint32 iluid( GetLuidL( aParentLUID, mapping, (parentMapping == KAMDeliveredNodeName ? EDCSDelivered : ( parentMapping == KAMDownloadNodeName ? EDCSDownload : EDCSNone ) ) ) ) ;
	if( !iluid && parentMapping == KAMDownloadNodeName )
	    {
            // This is added for avoiding duplicate node creation in Delivered/Download only.
            iluid = GetLuidL( aParentLUID, mapping, EDCSDelivered );        
	    }
	if ( iluid == 0 )
		{
		// Not found, add it ... 
		DeliverOrDownloadL( mapping, aURI, cnt, parentMapping, status );
		}
	else
		{
		RDEBUG_2( "CAmAdapter::AddNodeObjectL(): WARNING May Already exists '%d'", iluid );
		TDeploymentComponent comp ;
		TInt err( SessionL().DeploymentComponent( iluid, comp ) );
		if ( err == KErrNone )
			{
			RDEBUG_2( "CAmAdapter::AddNodeObjectL(): ERROR Already exists '%d'", iluid );
			if ( comp.iState == StateL( aURI ) )
				{
				RDEBUG( "CAmAdapter::AddNodeObjectL():    (state is ok) " );
				}
			else
				{
				RDEBUG( "CAmAdapter::AddNodeObjectL():    (state is NOK) " );
				}
			status = EAlreadyExists;
			}
		else
			{
			if ( err == KErrNotFound )
				{
				RDEBUG_2( "CAmAdapter::AddNodeObjectL(): WARNING Already exists but not found '%d'", iluid );
				DeliverOrDownloadL( mapping, aURI, cnt, parentMapping, status );
				}
			else
				{
				
				RDEBUG_2( "CAmAdapter::AddNodeObjectL(): ERROR May exists, but failed to fetch get one? '%d'", err );
				}
			}
		}
	if ( status == EOk )
		{
		if ( iInAtomic )
    		{
			iBuffer.Append( TAMCommandBufferElement( aStatusRef, aURI ) );
			}
		}
	
	
	
	SetStatusL( aStatusRef, status );
	RDEBUG( "CAmAdapter::AddNodeObject(): end" );
	}


	
	
// ------------------------------------------------------------------------------------------------
// CAmAdapter::GetSizeL( const TDesC8& aURI, const TDesC& aParentLUID )
// ------------------------------------------------------------------------------------------------
TInt CAmAdapter::GetSizeL( const TDeplCompAttrType aDataType, const TInt aLuid, TError &aRet )
	{
	TInt size( 0 );
	if ( aDataType == EDCStateValue )
		{
		TDeploymentComponent comp;
		TInt err( SessionL().DeploymentComponent( aLuid, comp ) );
		if ( err == KErrNone )
			{
			if ( comp.iState == EDCSActive )
				{
				size = KAMStateValueActive().Length();
				}
			else 
				{
				size = KAMStateValueInactive().Length() ;
				}
			aRet = EOk;
			}
		}
	else 
		{
		size = SessionL().DeploymentComponentDataSizeL( aLuid, aDataType );
		aRet = EOk;
		}
	return size;
	}


// ------------------------------------------------------------------------------------------------
// CAmAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC& aParentLUID )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID,
									   const TDesC8& aType, TInt aResultsRef,
									   TInt aStatusRef )
	{



	RDEBUG8_4("CAmAdapter::FetchLeafObjectSizeL('%S', '%S', '%S'): begin", &aURI, &aLUID, &aType );
	CheckStateChangesL();
	TError status( EError );
	CBufBase *currentList = CBufFlat::NewL( 128 );
	CleanupStack::PushL( currentList );
	
	TPtrC8 mapping( NSmlDmURI::LastURISeg( aURI ) );
	TPtrC8 parent( NSmlDmURI::RemoveLastSeg( aURI ) );
	TPtrC8 parentMapping( NSmlDmURI::LastURISeg( parent ) );
	TInt size( 0 );
	HBufC8 *luid = NULL;
	if ( aLUID != KNullDesC8 )
		{
		luid = aLUID.AllocLC();
		}
	else 
		{ // try if we find it from download path
		TBuf8<256> down;
		down = KDownloadURI;
		down.Append ( parentMapping );
		luid = Callback().GetLuidAllocL( down );
		CleanupStack::PushL( luid );
		if ( *luid != KNullDesC8 )
			{ 
			// make it to found next time
			DirectSetMappingL( parent, *luid );
			MoveAclL( down, parent );
			
			// remove old
			DirectSetMappingL( down, KNullDesC8 );
			}
		}
	TDeplCompAttrType dataType( UpdateFlagFromMapping( mapping ) );
	if ( *luid != KNullDesC8 )
		{
		TUint32 iluid ( DesToIntL( *luid ) );
		RDEBUG_2( "CAmAdapter::FetchLeafObjectSizeL(): Luid is : %d", iluid );
	    size = GetSizeL( dataType, iluid, status );
		}
	else
		{
		TUint32 iluid( GetLeafLuidL( aURI, parentMapping ) );
		if ( iluid > 0 )
			{
			HBufC8 *l = IntToDes8LC( iluid );
			DirectSetMappingL( parent, *l );
			CleanupStack::PopAndDestroy( l );
			size = GetSizeL( dataType, iluid, status );
			}
		else
			{
			RDEBUG8_2( "CAmAdapter::FetchLeafObjectSizeL(): WARNING Faild to get luid allocation for '%S'", &parent );
			if ( aLUID == KNullDesC8 )
				{
				status = ENotFound ;
				}
			}
		}
	SetStatusL( aStatusRef, status );
	if( status == EOk && size > 0 )
		{
		HBufC8 *hsize = IntToDes8LC( size );
		currentList->InsertL( 0, *hsize );
		if ( iInAtomic )
			{
			iBuffer.Append( TAMCommandBufferElement( aStatusRef, aURI ) );
			}
		Callback().SetResultsL( aResultsRef, *currentList, aType );
		CleanupStack::PopAndDestroy( hsize );
		}
	CleanupStack::PopAndDestroy( luid );
	CleanupStack::PopAndDestroy( currentList ); //currentList	
	}
	
	
// ------------------------------------------------------------------------------------------------
// CAmAdapter::ExecuteCommandL( const TDesC8& aURI, const TDesC& aParentLUID )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::ExecuteCommandL( const TDesC8& /*aURI*/, const TDesC8& /*aLUID*/,
								  RWriteStream*& /*aStream*/, const TDesC8& /*aType*/,
								  TInt /*aStatusRef*/ )
	{
	RDEBUG("CAmAdapter::ExecuteCommandL(stream): begin (NOT SUPPORTED)");
	User::Leave( KErrNotSupported );
	}
	

// ------------------------------------------------------------------------------------------------
// CAmAdapter::CopyCommandL( const TDesC8& aURI, const TDesC& aParentLUID )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::CopyCommandL( const TDesC8& /*aTargetURI*/, const TDesC8& /*aTargetLUID*/,
							   const TDesC8& /*aSourceURI*/, const TDesC8& /*aSourceLUID*/,
							   const TDesC8& /*aType*/, TInt aStatusRef )
	{
	RDEBUG("CAmAdapter::CopyCommandL(): begin (NOT SUPPORTED)");
	SetStatusL( aStatusRef, EError );
	
	}
	

// ------------------------------------------------------------------------------------------------
// CAmAdapter::StartAtomicL(  )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::StartAtomicL()
	{
	RDEBUG("CAmAdapter::StartAtomicL(): begin (NOT properly SUPPORTED)");
	iInAtomic = ETrue ;
	}
	

// ------------------------------------------------------------------------------------------------
// CAmAdapter::CommitAtomicL(  )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::CommitAtomicL()
	{
	RDEBUG("CAmAdapter::CommitAtomicL(): begin (NOT SUPPORTED)");
	iInAtomic = EFalse ; // the initial statuses are already set
	}
	

// ------------------------------------------------------------------------------------------------
// CAmAdapter::RollbackAtomicL(  )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::RollbackAtomicL()
	{
	RDEBUG("CAmAdapter::RollbackAtomicL(): begin (NOT properly SUPPORTED)");
	TInt count( iBuffer.Count() );
	for ( TInt i( 0 ); i < count; i++ )
		{
		TAMCommandBufferElement &pm( iBuffer[i] );
		SetStatusL( pm.iStatusRef, ERollbackFailed );
		}
	}


// ------------------------------------------------------------------------------------------------
// CAmAdapter::StreamingSupport( )
// ------------------------------------------------------------------------------------------------
TBool CAmAdapter::StreamingSupport( TInt& aItemSize ) 
	{
	RDEBUG("CAmAdapter::StreamingSupport(): begin");
	aItemSize = 1024;
	RDEBUG("CAmAdapter::StreamingSupport(): end");
	return ETrue;
	}
	

// ------------------------------------------------------------------------------------------------
// CAmAdapter::StreamCommittedL( )
// ------------------------------------------------------------------------------------------------
#ifdef __TARM_SYMBIAN_CONVERGENCY
void CAmAdapter::StreamCommittedL( RWriteStream& /*aStream*/ )
#else
void CAmAdapter::StreamCommittedL()
#endif
	{
	RDEBUG("CAmAdapter::StreamCommittedL(): begin");

	if(iStreamOpen)
		{
		iStream.CommitL();
		}

	MSmlDmAdapter::TError status = EOk;

	TRAPD( err,	_UpdateLeafObjectL( *iStreamedURI, *iStreamedLuid, KNullDesC8(), *iStreamedType, iStatusRef, status ) );

	if(err != KErrNone)
	{
		RDEBUG_2("CAmAdapter::StreamCommittedL(): _UpdateLeafObjectL returned error (%d)", err);
	}
	
	iIsStreamedContent = EFalse;
	iStreamOpen = EFalse;

	Callback().SetStatusL( iStatusRef, status );		

	RDEBUG("CAmAdapter::StreamCommittedL(): end");
	}
	
// ------------------------------------------------------------------------------------------------
// CAmAdapter::CloseStreaming( )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::CloseStreaming()
{
	if( iStreamOpen )
	{
		RDEBUG("CAmAdapter::CloseStreaming(): closing");
		iStreamFile.Close();  // !!! Closing this will also close the iStreamRFs because it is adopted from server
		iStreamOpen = EFalse;
	}
}

	
// ------------------------------------------------------------------------------------------------
// CAmAdapter::CompleteOutstandingCmdsL()
// ------------------------------------------------------------------------------------------------
void CAmAdapter::CompleteOutstandingCmdsL()
	{
	RDEBUG("CAmAdapter::CompleteOutstandingCmdsL(): begin");
/*	iManagement.Close();
	iSessionOpened = EFalse ;*/
	iBuffer.Reset();
	RDEBUG("CAmAdapter::CompleteOutstandingCmdsL(): Management session closed");
	}



// ------------------
//
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] = 
	{
	//
	IMPLEMENTATION_PROXY_ENTRY( KAMAdapterImplUid, CAmAdapter::NewL )
	};

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	return ImplementationTable;
	}


// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
TInt CAmAdapter::DesToIntL( const TDesC8& aLuid )
	{
	TLex8 lex( aLuid );
	TInt value = 0;
	User::LeaveIfError( lex.Val( value ) );
	return value;
	}


// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
HBufC8* CAmAdapter::IntToDes8LC( const TInt aLuid )
	{
	HBufC8* buf = HBufC8::NewLC( 10 ); //10 = max length of 32bit integer
	TPtr8 ptrBuf = buf->Des();
	ptrBuf.Num( aLuid );
	return buf;
	}



// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CAmAdapter::ConstructL()
	{
#ifdef __TARM_SYMBIAN_CONVERGENCY
	iDbSession.ConnectL();
#else
	User::LeaveIfError( iDbSession.Connect());
#endif
	}

// ------------------------------------------------------------------------------------------------
// CAmAdapter::SetStatusL( TInt aStatusRef, MSmlDmAdapter::TError aErrorCode ) 
// ------------------------------------------------------------------------------------------------
void CAmAdapter::SetStatusL( TInt aStatusRef,
						 MSmlDmAdapter::TError aErrorCode ) 
	{
	RDEBUG_3("CAmAdapter::SetStatusL( %d, %d )", aStatusRef, aErrorCode );
	Callback().SetStatusL( aStatusRef, aErrorCode );
	}
							 
// ------------------------------------------------------------------------------------------------
// CAmAdapter::SetResultsL( TInt aResultsRef, CBufBase& aObject, const TDesC8& aType )
// ------------------------------------------------------------------------------------------------
void CAmAdapter::SetResultsL( TInt aResultsRef, CBufBase& aObject,
						  const TDesC8& aType )
	{
	Callback().SetResultsL( aResultsRef, aObject, aType );
	
	}
							  
// -------------------------------------------------------------------------------------
// CAmAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -------------------------------------------------------------------------------------
void CAmAdapter::FillNodeInfoL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,
		MSmlDmDDFObject::TOccurence aOccurrence, MSmlDmDDFObject::TScope aScope, 
		MSmlDmDDFObject::TDFFormat aFormat,const TDesC8& aDescription)
	{
	FillNodeInfoNoDefaultMimeL( aNode, aAccTypes, aOccurrence, aScope, aFormat, aDescription );
	if(aFormat!=MSmlDmDDFObject::ENode)
		{
		aNode.AddDFTypeMimeTypeL(KAMTextPlain);
		}
	
	}


// -------------------------------------------------------------------------------------
// CAmAdapter::FillNodeInfoNoDefaultMimeL()
// Fills the node info in ddf structure, without default mime type
// -------------------------------------------------------------------------------------
void CAmAdapter::FillNodeInfoNoDefaultMimeL( MSmlDmDDFObject& aNode,TSmlDmAccessTypes aAccTypes,
		MSmlDmDDFObject::TOccurence aOccurrence, MSmlDmDDFObject::TScope aScope, 
		MSmlDmDDFObject::TDFFormat aFormat,const TDesC8& aDescription)
	{
	aNode.SetAccessTypesL(aAccTypes);
	aNode.SetOccurenceL(aOccurrence);
	aNode.SetScopeL(aScope);
	aNode.SetDFFormatL(aFormat);
	
	aNode.SetDescriptionL(aDescription);
	}
	
// -------------------------------------------------------------------------------------
// CAmAdapter::CheckStateChangesInternalL()
// Updates the changed DC states to DM client mappings and ACLs
// -------------------------------------------------------------------------------------
void CAmAdapter::CheckStateChangesInternalL()
	{


#ifdef __AM_CHECKSTATE_CHANGES
	RDEBUG( "CAmAdapter::CheckStateChangesLInternal(): begin" );	
	if ( !iUpdated )
		{
		RDEBUG( "CAmAdapter::CheckStateChangesLInternal(): !iUpdated" );
		RElementIdArray arr;
		CleanupClosePushL( arr );
		TInt err( SessionL().StateChangingDCIdsL( arr ) );
		if ( err == KErrNone )
			{
			TInt c( arr.Count() );
			for( TInt i( 0 ); i < c; i++ )
				{
				TDeploymentComponent comp ;
				err = iManagement.DeploymentComponent( arr[i], comp );
				if ( err == KErrNone )
					{
					TDCUserId dc;
					SessionL().DeploymentComponentUserIdL( arr[i], dc );
					const TInt KNSmlDmMaxUriLength = 256;
					HBufC8 *uri = HBufC8::NewLC( KNSmlDmMaxUriLength );
					TPtr8 puri( uri->Des() );
#ifdef __TARM_SYMBIAN_CONVERGENCY					
					RDmTreeNode uriNode;
					CleanupClosePushL(uriNode);

					RBuf8 acl;
					acl.CreateL(128);
					CleanupClosePushL(acl);
					
					HBufC8 *newUri = HBufC8::NewLC( KNSmlDmMaxUriLength );
					TPtr8 pnewUri( newUri->Des() );
								
					RDmTreeNode newUriNode;
					CleanupClosePushL(newUriNode);
#else
					CBufBase* acl = CBufFlat::NewL( 128 );
					CleanupStack::PushL( acl );
				
					HBufC8 *newUri = HBufC8::NewLC( KNSmlDmMaxUriLength );
					TPtr8 pnewUri( newUri->Des() );

#endif
					
					switch ( comp.iOldState ) 
						{
						case EDCSDelivered:
							{
							RDEBUG( "CAmAdapter::CheckStateChangesLInternal(): Old State = EDCSDelivered" );
							puri.Copy( KDeliveredURI() );
							puri.Append( dc );
					
							HBufC8 *luid = DirectGetLuidAllocLC( puri );
							
							if ( *luid == KNullDesC8 )
								{ 
                                        // try if we find it from download path
                                        puri.Zero();
                                        puri.Copy( KDownloadURI() );
                                        puri.Append( dc );                                       
                                        luid = DirectGetLuidAllocLC( puri );     
								}
							
							// check if the luid is still null
							
							if ( *luid != KNullDesC8 )
								{
								// "Nullify" the old mapping 
								//DirectSetMappingL( puri, KNullDesC8 );
								DirectRemoveMappingL(puri);
#ifdef __TARM_SYMBIAN_CONVERGENCY								
								iDbSession.OpenNodeL(puri, uriNode, RDmTree::EReadOnly);
								uriNode.DmPropertyL( DevMan::EACL, acl );
#else
								err = iDbSession.GetAclL(puri, *acl, EFalse ); // no need for inherited
#endif										
								// Determine new uri 
								switch( comp.iState )
									{
									case EDCSActive:
									case EDCSInactive: // both have same node
										{
										pnewUri.Copy( KDeployedURI() );
										pnewUri.Append( dc );
										DirectSetMappingL( pnewUri, *luid );	
										if ( err == KErrNone )
											{
#ifdef __TARM_SYMBIAN_CONVERGENCY
											iDbSession.OpenNodeL(pnewUri, newUriNode, RDmTree::EReadOnly);
											uriNode.RemoveDmPropertyL( DevMan::EACL );
											newUriNode.SetDmPropertyL( DevMan::EACL, acl );
#else
											err = iDbSession.DeleteAclL( puri );
											
											err = iDbSession.UpdateAclL( pnewUri, acl->Ptr( 0 ) );
#endif
											}
										else
											{
											RDEBUG_2("CAmAdapter::CheckStateChangesL: ERROR Failed to get acl from %d ", err );
											}
										iManagement.StateChangeComplete( arr[i] );							
										break;
										}
									default:
										{
										
										RDEBUG_2("CAmAdapter::CheckStateChangesL: WARNING Illegal state change from delivered to %d ", comp.iState );
			
										break;
										}
									}	
								}
							else
                                {
                                // probably moved already
                                iManagement.StateChangeComplete( arr[i] );
                                }
							
							CleanupStack::PopAndDestroy( luid );		
							break;
							}
						case EDCSDownload:
							{
							RDEBUG( "CAmAdapter::CheckStateChangesLInternal(): Old State = EDCSDownload" );
							puri.Copy( KDownloadURI() );
							puri.Append( dc );
							HBufC8 *luid = DirectGetLuidAllocLC( puri ); //Callback().GetLuidAllocL( puri );
							//CleanupStack::PushL( luid );
							if ( *luid == KNullDesC8 )
								{ // probably moved already
								CleanupStack::PopAndDestroy( luid );
								puri.Copy( KDeliveredURI() );
								puri.Append( dc );
								luid = DirectGetLuidAllocLC( puri ); //Callback().GetLuidAllocL( puri );
								if ( *luid == KNullDesC8 )
								{ // probably moved already
							
									iManagement.StateChangeComplete( arr[i] );
									
								}
								}
							else
								{
#ifdef __TARM_SYMBIAN_CONVERGENCY
								iDbSession.OpenNodeL(puri, uriNode, RDmTree::EReadOnly);
								uriNode.DmPropertyL( DevMan::EACL, acl );
#else
								err = iDbSession.GetAclL(puri, *acl, EFalse ); // no need for inherited
#endif							
								// "Nullify" the old mapping 
								//DirectSetMappingL( puri, KNullDesC8 );
								DirectRemoveMappingL(puri);
								switch( comp.iState )
									{
									case EDCSActive:
									case EDCSInactive: // both have same node
										{
										pnewUri.Copy( KDeployedURI() );
										pnewUri.Append( dc );
										DirectSetMappingL( pnewUri, *luid );	
										if ( err == KErrNone )
											{
#ifdef __TARM_SYMBIAN_CONVERGENCY
											iDbSession.OpenNodeL(pnewUri, newUriNode, RDmTree::EReadOnly);
											uriNode.RemoveDmPropertyL( DevMan::EACL );
											newUriNode.SetDmPropertyL( DevMan::EACL, acl );
#else											
											err = iDbSession.DeleteAclL( puri );
											err = iDbSession.UpdateAclL( pnewUri, acl->Ptr( 0 ) );
#endif
											}
										else
											{
											RDEBUG_2("CAmAdapter::CheckStateChangesL: ERROR Failed to get acl from %d ", err );
											}
										iManagement.StateChangeComplete( arr[i] );							
										break;
										}
									case EDCSDelivered:
										{
										RDEBUG( "CAmAdapter::CheckStateChangesLInternal(): Current State = EDCSDelivered" );
										pnewUri.Copy( KDeliveredURI() );
										pnewUri.Append( dc );
										DirectSetMappingL( pnewUri, *luid );
										if ( err == KErrNone )
											{
#ifdef __TARM_SYMBIAN_CONVERGENCY
											iDbSession.OpenNodeL(pnewUri, newUriNode, RDmTree::EReadOnly);
											uriNode.RemoveDmPropertyL( DevMan::EACL );
											newUriNode.SetDmPropertyL( DevMan::EACL, acl );
#else
											
											err = iDbSession.DeleteAclL( puri );
											err = iDbSession.UpdateAclL( pnewUri, acl->Ptr( 0 ) );
#endif
											}
										else
											{
											RDEBUG_2("CAmAdapter::CheckStateChangesL: ERROR Failed to get acl from %d ", err );
											}
										iManagement.StateChangeComplete( arr[i] );								
										break;
										}
									default:
										{
										
										RDEBUG_2("CAmAdapter::CheckStateChangesL: WARNING Illegal state change from delivered to %d ", comp.iState );
										break;
										}
									}
							
								}
							CleanupStack::PopAndDestroy( luid );		
							break;
							}
						case EDCSActive:
						case EDCSInactive: // these both are in same node
							{
							if (comp.iState == EDCSDelete)
							    {
		                         puri.Copy( KDeployedURI());
		                         puri.Append( dc ); 
		                         HBufC8 *bufLuid = DirectGetLuidAllocLC( puri );
		                         TUint32 luid = DesToIntL( *bufLuid ) ;
		                         TRAPD(err,SessionL().RemoveL( luid ));
		                         if ( err == KErrNone || err == KErrNotFound )
		                             {}
		                         DirectRemoveMappingL(puri);
		                         iManagement.StateChangeComplete( arr[i] );
		                         CleanupStack::PopAndDestroy(bufLuid);
		                         RDEBUG("CAmAdapter::CheckStateChangesL:DirectRemoveMappingL for comp with istate EDCSDelete " );
		                         break;
							    }
							RDEBUG_3("CAmAdapter::CheckStateChangesL: No need to move anything in change from %d to %d ", comp.iOldState, comp.iState );
							iManagement.StateChangeComplete( arr[i] );								
							break;
							}
						}
#ifdef __TARM_SYMBIAN_CONVERGENCY
					CleanupStack::PopAndDestroy( &newUriNode );
					CleanupStack::PopAndDestroy( newUri );
					CleanupStack::PopAndDestroy( &uriNode );
					CleanupStack::PopAndDestroy( &acl );
					CleanupStack::PopAndDestroy( uri );
#else
					CleanupStack::PopAndDestroy( newUri );	
					CleanupStack::PopAndDestroy( acl );
					CleanupStack::PopAndDestroy( uri );			
#endif
					}
				else
					{
					RDEBUG_2("CAmAdapter::CheckStateChangesL: ERROR Failed to get component: %d ", err );				
					}
				}
			iUpdated = ETrue;
			}
		else
			{
			RDEBUG_2("CAmAdapter::CheckStateChangesL: ERROR Failed to get state change dcs: %d ", err );				
			}
		CleanupStack::PopAndDestroy( &arr );
		}
	else
		{
		RDEBUG("CAmAdapter::CheckStateChangesL: Already updated! ");				
		}
	RDEBUG( "CAmAdapter::CheckStateChangesL(): end" );	
#else
	DBG_ARGS8( "CAmAdapter::CheckStateChangesL(): disabled" );	
#endif
	}


// -------------------------------------------------------------------------------------
// CAmAdapter::CheckStateChangesL()
// Updates acl and mapping trees to match state changes
// -------------------------------------------------------------------------------------
void CAmAdapter::CheckStateChangesL()	
	{
	iCertRequired = EFalse ;
	RDEBUG( "CAmAdapter::CheckStateChangesL(): iCertRequired = EFalse" );
	TRAPD( err, CheckStateChangesInternalL() );
	if ( err != KErrNone )
		{
		RDEBUG_2("CAmAdapter::CheckStateChangesL(): warning failed to check state changes %d", err );
		}
	iCertRequired = ETrue ;
	RDEBUG( "CAmAdapter::CheckStateChangesL(): iCertRequired = ETrue" );
	if ( !iTrustAdded )
		{
			RDEBUG( "CAmAdapter::CheckStateChangesL(): Adding Trust Closing Session" );
			iManagement.Close();
			iSessionOpened = EFalse;
			RApplicationManagement &session = SessionL();
			RDEBUG( "CAmAdapter::CheckStateChangesL(): Adding Trust new session started" );
		}
	}

		
// -------------------------------------------------------------------------------------
// CAmAdapter::URISegsUpTo(const TDesC8& aURI, TInt aUpTo)
// -------------------------------------------------------------------------------------
TPtrC8 CAmAdapter::URISegsUpTo( const TDesC8& aURI, TInt aUpTo, TBool aInclKeno/*=EFalse*/)
	{



	TPtrC8 ret;
	TInt numOfURISegs( 0 );
	for( TInt i( 0 ); i < aURI.Length() && ret.Ptr() == NULL; i++ )
		{
		if( aURI[i] == KNSmlDMUriSeparator )
			{
			numOfURISegs++;
			if( aUpTo == numOfURISegs )
				{
				if( aInclKeno && aURI.Length() > i+1)
					{
					ret.Set( aURI.Left( i + 1 ) );
					}
				else
					{
					ret.Set( aURI.Left( i ) );
					}
				}
			}
		}
	return ret;
	}
	
		
// -------------------------------------------------------------------------------------
// CAmAdapter::DirectGetLuidAllocLC( const TDesC8 &aURI )
// -------------------------------------------------------------------------------------	
HBufC8 *CAmAdapter::DirectGetLuidAllocLC( const TDesC8 &aURI )
	{


#ifdef __TARM_SYMBIAN_CONVERGENCY
	HBufC8 *ret = Callback().GetLuidAllocL( aURI );
#else
	HBufC8* ret = iDbSession.GetLuidAllocL( KAMAdapterImplUid, aURI );
#endif
	
	CleanupStack::PushL( ret );
	return ret;
	}
	
	
// -------------------------------------------------------------------------------------
// CAmAdapter::DirectSetMappingL( const TDesC8 &aURI, const TDesC8 &aLUID )
// -------------------------------------------------------------------------------------		
TInt CAmAdapter::DirectSetMappingL( const TDesC8 &aURI, const TDesC8 &aLUID )
	{

#ifdef __TARM_SYMBIAN_CONVERGENCY
	TRAPD( err, Callback().SetMappingL( aURI, aLUID ) );
#else
	TInt err( iDbSession.AddMappingInfoL( KAMAdapterImplUid, aURI, aLUID ) );
#endif
	return err;
	}
	
// -------------------------------------------------------------------------------------
// CAmAdapter::DirectRemoveMappingL( const TDesC8 &aURI )
// -------------------------------------------------------------------------------------        
TInt CAmAdapter::DirectRemoveMappingL( const TDesC8 &aURI )
    {

#ifdef __TARM_SYMBIAN_CONVERGENCY
    TRAPD( err, Callback().SetMappingL( aURI, aLUID ) );
#else
    TInt err( iDbSession.RemoveMappingInfoL( KAMAdapterImplUid, aURI, ETrue ) );
#endif
    return err;
    }
	
// -------------------------------------------------------------------------------------
// CAmAdapter::MoveAclL( const TDesC8 &aFrom, const TDesC8 &aTo )
// -------------------------------------------------------------------------------------		
void CAmAdapter::MoveAclL( const TDesC8 &aFrom, const TDesC8 &aTo )
	{	

#ifdef __TARM_SYMBIAN_CONVERGENCY
	RDmTreeNode from;
	CleanupClosePushL(from);

	RDmTreeNode to;
	CleanupClosePushL(to);

	RBuf8 acl;
	acl.CreateL(128);
	CleanupClosePushL(acl);

	iDbSession.OpenNodeL(aFrom, from, RDmTree::EReadOnly);
	TRAPD( err, from.DmPropertyL( DevMan::EACL, acl ) );
	if ( err == KErrNone )
		{
		iDbSession.OpenNodeL(aTo, to, RDmTree::EReadOnly);
		from.RemoveDmPropertyL( DevMan::EACL );
		to.SetDmPropertyL( DevMan::EACL, acl );
		}
	CleanupStack::PopAndDestroy( 3, &from );
#else
	CBufBase *acl = CBufFlat::NewL( 128 );
	CleanupStack::PushL( acl );
	TInt err  (iDbSession.GetAclL(aFrom, *acl, EFalse ) ); // no need for inherited
	if ( err == KErrNone )
		{
		err = iDbSession.DeleteAclL( aFrom );
		err = iDbSession.UpdateAclL( aTo, acl->Ptr( 0 ) );
		}
	CleanupStack::PopAndDestroy( acl );
#endif
	}


// -------------------------------------------------------------------------------------
// CAmAdapter::SetMappingL( const TDesC8 &aOriginalURI, 
//	const TDeploymentComponentState aTargetState, const TDesC8 &aLUID )
// -------------------------------------------------------------------------------------		
void CAmAdapter::SetMappingL( const TDesC8 &aOriginalURI, 
	const TDeploymentComponentState aTargetState, const TDesC8 &aLUID )
	{


	HBufC8 * targeturi = NULL;
	SetMappingLC( aOriginalURI, aTargetState, aLUID, targeturi );
	CleanupStack::PopAndDestroy( targeturi );
	}


// -------------------------------------------------------------------------------------
// CAmAdapter::SetMappingLC( const TDesC8 &aOriginalURI, 
//	const TDeploymentComponentState aTargetState, const TDesC8 &aLUID, 
//	HBufC8 *&aNewUri )
// -------------------------------------------------------------------------------------		
void CAmAdapter::SetMappingLC( const TDesC8 &aOriginalURI, 
	const TDeploymentComponentState aTargetState, const TDesC8 &aLUID, HBufC8 *&aNewUri )
	{

	
	// "Nullify" the mapping for argument
	DirectSetMappingL( aOriginalURI, KNullDesC8 );
	
	// Determine new uri for aArgument
	TPtrC8 puri;
	if ( aNewUri == NULL )	
		{
		TPtrC8 argMapping( NSmlDmURI::LastURISeg( aOriginalURI ) );
		TPtrC8 target;
		if ( aTargetState == EDCSInactive || aTargetState == EDCSActive )
			{
			target.Set( KDeployedURI() );
			}
		else if ( aTargetState == EDCSDelivered )
			{
			target.Set( KDeliveredURI() );
			}
		else
			{
			
			User::Leave( KErrArgument );
			}
	
		aNewUri = HBufC8::NewLC( target.Length() + argMapping.Length() );
		TPtr8 p( aNewUri->Des() );
		p.Copy( target );
		p.Append( argMapping );
		puri.Set( p );
		}
	else
		{
		puri.Set( *aNewUri );
		}
	DirectSetMappingL( puri, aLUID );
	MoveAclL( aOriginalURI, puri );
	}


#ifdef __AM_LASTERROR_NODE
// ------------------------------------------------------------------------------------------------
// CAmAdapter::SetLastErrorL( const TDesC8& aLastError, const TInt aError )
// ------------------------------------------------------------------------------------------------	
void CAmAdapter::SetLastErrorL( const TDesC8& aLastError, const TInt aError )
	{
	if ( iLastError )
		{
		TPtrC8 p(*iLastError);
		RDEBUG8_2( "CAmAdapter::SetLastErrorL: clearing last error %S", &p);
		delete iLastError;
		iLastError = NULL;
		}
	iLastError = HBufC8::NewL( aLastError.Length() + 10);
	iLastError->Des().Format( aLastError, aError );
	}
	
const TDesC8& CAmAdapter::LastError()
	{
	return iLastError == NULL ? KNullDesC8() : *iLastError;
	}
	
#endif

TPtrC8 CAmAdapter::LastURISeg(const TDesC8& aURI)
    {
    TInt i;
    for(i=aURI.Length()-1;i>=0;i--)
        {
        if(aURI[i]==KNSmlDMAMSeparatorDef)
            {
            break;
            }
        }
    if(i==0)
        {
        return aURI;
        }
    else
        {
        return aURI.Mid(i+1);
        }
    }

TInt CAmAdapter::DesToInt(const TDesC8& aLuid)
    {
    TLex8 lex(aLuid);
    TInt value = 0;
    lex.Val(value);
    return value;
    }
TInt CAmAdapter::GetAdapterValue()
    {
    TInt adapterValue;
    CRepository *repository = NULL;
    TRAPD( err, repository = CRepository::NewL ( KCRUidPrivateApplicationManagementKeys ) );
    if (err == KErrNone)
        {
        err = repository->Get(KAMAdapterValue, adapterValue);
        }
    delete repository;
    repository = NULL;
    return adapterValue;
    }

void CAmAdapter::SetAdapterValue(TInt aAdapterValue)
    {
    
    CRepository *repository = NULL;
    TRAPD( err, repository = CRepository::NewL ( KCRUidPrivateApplicationManagementKeys ) );
    if (err == KErrNone)
        {
        err = repository->Set(KAMAdapterValue, aAdapterValue);
        }
    delete repository;
    repository = NULL;
    }

// End of File

