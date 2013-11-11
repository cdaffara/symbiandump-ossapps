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
 * Description:  Implementation of applicationmanagement components
 *
*/



/**

 1. Remember the applications managed by Corporate Admin Role certificate
 1.1 They must be removed during rfs
 1.2 Others must be left as is (applications at least must be left)

 */


#define __AM_CHECKSTATE_CHANGES // uncomment to enable dynamic acl
#include <badesca.h>
#include <fbs.h>
#include <s32file.h> 
#include "omascomoadapter.h"
#include <implementationproxy.h> // For TImplementationProxy definition
#include <f32file.h>
#include "nsmldmuri.h"
#include "nsmldmiapmatcher.h"
#include "debug.h"
#include "amprivateCRKeys.h"
#include <centralrepository.h>
#include "aminstalloptions.h"
#include <featmgr.h>
#include <apgtask.h>
#include <w32std.h>

#include <SyncMLClient.h>
#include <SyncMLClientDM.h>
#include <e32property.h>
#include <centralrepository.h>
#include <DevManInternalCRKeys.h>

#include "NSmlPrivateAPI.h"

#include "nsmldmconst.h"
#include "ampskeys.h"

#include "nsmldmtreedbclient.h"
//#include "SyncService.h"
#include "amprivateCRKeys.h"
#include <e32cmn.h>
#include <SWInstDefs.h>

#ifdef __TARM_SYMBIAN_CONVERGENCY
#include <dmtreenode.h>
#include <devman.h>
#include "nsmldmuri.h"

// Constants
_LIT8( KDeployedURI, "./SCOMO/Inventory/Deployed/" );
_LIT8( KDeliveredURI, "./SCOMO/Inventory/Delivered/" );
_LIT8( KDownloadURI, "./SCOMO/Download/" );
#else
#include <nsmldmuri.h>

// Constants
_LIT8( KDeployedURI, "SCOMO/Inventory/Deployed/" );
_LIT8( KDeliveredURI, "SCOMO/Inventory/Delivered/" );
_LIT8( KDownloadURI, "SCOMO/Download/" );
#endif

#ifdef __NSML_DEBUG__
#pragma message("NSml Debugging is ON!")
#else
#endif

using namespace NApplicationManagement;

_LIT8( KAMStateValueActive, "20" );
_LIT8( KAMStateValueInactive, "10" );
_LIT8( KAMStateValueDelivered, "10" );

_LIT8( KSisxMimeType, "x-epoc/x-sisx-app" );
_LIT8( KSisMimeType, "application/vnd.symbian.install" );
_LIT8( KPipMimeType, "application/x-pip" );
_LIT8( KJadMIMEType, "text/vnd.sun.j2me.app-descriptor" );
_LIT8( KJarMIMEType, "application/java-archive" );
_LIT8( KJavaMIMEType, "application/java" );



const TUint8 KNSmlDMAMSeparatorDef = 0x2f; //forward slash
#ifdef __AM_LASTERROR_NODE

#ifdef __TARM_SYMBIAN_CONVERGENCY
_LIT8( KAMLastErrorURI, "./SCOMO/Ext/LastError" );
#else
_LIT8( KAMLastErrorURI, "SCOMO/Ext/LastError" );
#endif
_LIT8( KStdError, "Error %d" );

#define MAPERROR(x,y,z) DefaultMapError(x,y,z)
#else

#define MAPERROR(x,y,z) DefaultMapError(x,y)
#endif


// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter* CSCOMOAdapter::NewL( )
// ------------------------------------------------------------------------------------------------
CSCOMOAdapter* CSCOMOAdapter::NewL(MSmlDmCallback* aDmCallback)
    {
    RDEBUG("CSCOMOAdapter::NewL(): begin");

    CSCOMOAdapter* self = NewLC(aDmCallback);
    CleanupStack::Pop();
    return self;
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter* CSCOMOAdapter::NewLC( )
// ------------------------------------------------------------------------------------------------
CSCOMOAdapter* CSCOMOAdapter::NewLC(MSmlDmCallback* aDmCallback)
    {
    CSCOMOAdapter* self = new( ELeave ) CSCOMOAdapter( aDmCallback );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::CSCOMOAdapter()
// ------------------------------------------------------------------------------------------------
CSCOMOAdapter::CSCOMOAdapter(TAny* aEcomArguments) :
    CSmlDmAdapter(aEcomArguments), iSessionOpened(EFalse), iBuffer( 8),
            iInAtomic(EFalse), iUpdated(EFalse), iIsStreamedContent(EFalse)
    {
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::~CSCOMOAdapter()
// ------------------------------------------------------------------------------------------------
CSCOMOAdapter::~CSCOMOAdapter()
    {
    RDEBUG( "CSCOMOAdapter::~CSCOMOAdapter" );
    iDbSession.Close();
    iManagement.Close(); // make sure we're closed
    iSessionOpened = EFalse;
    iBuffer.Close();

    CloseStreaming(); // Ensures that no streams are left open and that the temporary file is removed

    delete iStreamedURI;
    delete iStreamedLuid;
    delete iStreamedType;

#ifdef __AM_LASTERROR_NODE
    delete iLastError;
#endif

    if (iAMdb)
        delete iAMdb;

    if (iUriDel)
        delete iUriDel;
    RDEBUG( "CSCOMOAdapter::~CSCOMOAdapter end" );
    }

// ------------------------------------------------------------------------------------------------
//  CSCOMOAdapter::DDFVersionL()
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::DDFVersionL(CBufBase& aDDFVersion)
    {
    aDDFVersion.InsertL( 0, KAMDDFVersion);
    }

// ------------------------------------------------------------------------------------------------
//  CSCOMOAdapter::DDFStructureL()
//
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::DDFStructureL(MSmlDmDDFObject& aDDF)
    {
    RDEBUG( "CSCOMOAdapter::DDFStructureL(): begin" );

    TSmlDmAccessTypes accessTypesGet;
    accessTypesGet.SetGet();

    TSmlDmAccessTypes accessTypesExec;
    accessTypesExec.SetExec();

    TSmlDmAccessTypes accessTypesAdd;
    accessTypesAdd.SetAdd();

    TSmlDmAccessTypes accessTypesExecReplace = accessTypesExec;
    accessTypesExecReplace.SetReplace();

    TSmlDmAccessTypes accessTypesGetAdd = accessTypesGet;
    accessTypesGetAdd.SetAdd();

    TSmlDmAccessTypes accessTypesGetReplace = accessTypesGet;
    accessTypesGetReplace.SetGet();

    TSmlDmAccessTypes accessTypesGetAddReplace = accessTypesGetAdd;
    accessTypesGetAddReplace.SetReplace();

    TSmlDmAccessTypes accessTypesGetAddReplaceDelete =
            accessTypesGetAddReplace;
    accessTypesGetAddReplaceDelete.SetDelete();

    TSmlDmAccessTypes accessTypesAddReplace = 
	    accessTypesAdd;
    accessTypesAddReplace.SetReplace();

	RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step1" );

    /*
     Node: ./SCOMO
     AM node is the common parent to all application management functionality nodes. 
     Support: Mandatory
     Occurs: One
     Format: Node
     Access Types: Get
     Values: N/A
     */
    //MSmlDmDDFObject& root = aDDF.AddChildObjectGroupL();
    MSmlDmDDFObject& am = aDDF.AddChildObjectL(KAMNodeName);
    FillNodeInfoL(am, accessTypesGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMNodeDescription);

	RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 2" );

    /*
     Node: ./SCOMO/Inventory
     This node is start node to application inventory. 
     Support: Mandatory
     Occurs: One
     Format: Node
     Access Types: Get
     Values: N/A
     */
    MSmlDmDDFObject& inv = am.AddChildObjectL(KAMInventoryNodeName);
    FillNodeInfoL(inv, accessTypesGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMInventoryNodeDescription);

	RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 3" );

    /*
     Node: ./SCOMO/Inventory/Delivered
     This is node under which applications that are delivered but not installed are found.
     Support: Mandatory
     Occurs: One
     Format: Node
     Access Types: Get, Add
     Values: N/A
     */
    MSmlDmDDFObject& del = inv.AddChildObjectL(KAMDeliveredNodeName);
    FillNodeInfoL(del, accessTypesGetAdd, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMDeliveredNodeDescription);

	RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 4" );

    /*
     Node: ./SCOMO/Inventory/Deployed
     This is a node under which active applications are found.
     The child node descriptions are same as Delivered sibling node, except that <X> cannot be created, data node does not exist and there's no InstallOpts leaf.
     Support: Mandatory
     Occurs: One
     Format: Node
     Access Types: Get
     Values: N/A
     */

    MSmlDmDDFObject& ac = inv.AddChildObjectL(KAMDeployedNodeName);
    FillNodeInfoL(ac, accessTypesGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMDeployedNodeDescription);


	RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 5" );
    /*
     Node: ./SCOMO/Inventory/Delivered/<X>
     This dynamic node is placeholder applications that are in Delivered state. 
     Support: Mandatory
     Occurs: ZeroOrMore
     Format: Node
     Access Types: Get, Add, Replace, Delete
     Values: N/A
     */
    MSmlDmDDFObject& deldyna = del.AddChildObjectGroupL();
    FillNodeInfoL(deldyna, accessTypesGetAddReplaceDelete,
            MSmlDmDDFObject::EZeroOrMore, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KAMDeliveredDynaNodeDescription);


	RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 6" );
    /*
     Node: ./SCOMO/Inventory/Delivered/<X>/PkgID
     This leaf node holds an identifier for an application.
     Support: Mandatory
     Occurs: One
     Format: Chr
     Access Types: Get, Add, Replace
     Values: N/A
     */
    MSmlDmDDFObject& delid = deldyna.AddChildObjectL(KAMPkgIDNodeName);
    FillNodeInfoL(delid, accessTypesGetAddReplace, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EChr,
            KAMPkgIDNodeDescription);

	RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 7" );
    /*
     Node: ./SCOMO/Inventory/Delivered/<X>/Name
     This leaf node holds name of an application. 
     Support: Mandatory
     Occurs: One
     Format: Chr
     Access Types: Get ( + Add, Replace access within Delivered node only)
     Values: N/A
     */
    MSmlDmDDFObject& delname = deldyna.AddChildObjectL(KAMNameNodeName);
    FillNodeInfoL(delname, accessTypesGetAddReplace,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
            MSmlDmDDFObject::EChr, KAMNameNodeDescription);

    RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 8" );
    /*
     Node: ./SCM/Inventory/Delivered/<X>/Status
     This leaf node holds the status of the Delivered. The client updates the node value. 
     The values are typically error codes of Operations done on delivered node
     Support: Mandatory 
     Occurs: One
     Format: Int
     Access Types: Get
     Values: N/A
     */
    MSmlDmDDFObject& delstatus = deldyna.AddChildObjectL(KAMStatusNodeName);
    FillNodeInfoL(delstatus, accessTypesGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EInt,
            KAMStatusNodeDescription);


	RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 9" );
    /*
     Node: ./SCOMO/Inventory/Delivered/<X>/PkgType
     This leaf node holds an identifier for an application.
     Support: Mandatory
     Occurs: One
     Format: Chr
     Access Types: Get 
     Values: N/A
     */
    MSmlDmDDFObject& delpkgtype = deldyna.AddChildObjectL(KAMPkgTypeNodeName);
    FillNodeInfoL(delpkgtype, accessTypesGetAddReplace, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EChr,
            KAMPkgTypeNodeDescription);
    
    RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 10" );
    /*
     Node: ./SCOMO/Inventory/Delivered/<X>/Data
     This leaf node holds the data of an application. 
     Support: Mandatory
     Occurs: One
     Format: Bin
     Access Types: Add, Replace
     Values: N/A
     */
    MSmlDmDDFObject& delda = deldyna.AddChildObjectL(KAMDataNodeName);
    FillNodeInfoNoDefaultMimeL(delda, accessTypesAddReplace,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
            MSmlDmDDFObject::EBin, KAMDataNodeDescription);

	RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 11" );

    delda.AddDFTypeMimeTypeL(KSisxMimeType);
    delda.AddDFTypeMimeTypeL(KSisMimeType);
    delda.AddDFTypeMimeTypeL(KPipMimeType);

	RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 12" );

    FeatureManager::InitializeLibL();
    if (FeatureManager::FeatureSupported(KFeatureIdJavaMIDP20) )
        {
	    RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 13" );
        delda.AddDFTypeMimeTypeL(KJadMIMEType);
        delda.AddDFTypeMimeTypeL(KJarMIMEType);
        delda.AddDFTypeMimeTypeL(KJavaMIMEType);
        }

	RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 14" );
    FeatureManager::UnInitializeLib();

    /*
     Node: ./SCOMO/Inventory/Delivered/<X>/Description
     This leaf node holds the possible metadata of an application. Descriptor can be for example such a data that is required by the actual data in the Data leaf, but for some reason they cannot be bundled into same package. An example is Java JAR and JAD file combination, in which JAD file could be placed in MetaData and JAR in Data leaf.
     Support: Optional
     Occurs: One
     Format: Bin
     Access Types: Add, Get, Replace
     Values: N/A
     */
    MSmlDmDDFObject& delmd = deldyna.AddChildObjectL(KAMDescriptionNodeName);
    FillNodeInfoL(delmd, accessTypesGetAddReplace,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
            MSmlDmDDFObject::EBin, KAMDescriptorNodeDescription);

	RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 15" );
     /*
     Node: ./SCOMO/Inventory/Delivered/<X>/State
     This leaf node holds state value of an application. 
     Support: ?
     Occurs: One
     Format: Chr
     Access Types: Get 
     Values: N/A
     */
    MSmlDmDDFObject& delstate =
            deldyna.AddChildObjectL(KAMStateValueNodeName);
    FillNodeInfoL(delstate, accessTypesGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EChr,
            KAMStateValueNodeDescription);
    RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 16" );
    /*
     Node: ./SCOMO/Inventory/Delivered/<X>/Operations
     This is a node that allows vendors to extend functionality.
     Support: Optional
     Occurs: One
     Format: Node
     Access Types: Get, Replace, Add, Delete
     Values: N/A
     */
    MSmlDmDDFObject& delop = deldyna.AddChildObjectL(KAMOperationsNodeName);
    FillNodeInfoL(delop, accessTypesGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMOperationsNodeDescription);

    /*
     Node: ./SCOMO/Inventory/Delivered/<X>/Operations/Install
     Exec command causes device to install a delivered application. The data inside 
     exec command refers to the application to be installed. 
     Support: Mandatory
     Occurs: One
     Format: Node
     Access Types: Exec, Replace
     Values: N/A
     */
    MSmlDmDDFObject& loinst = delop.AddChildObjectL(KAMInstallNodeName);
    FillNodeInfoL(loinst, accessTypesExecReplace, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMInstallNodeDescription);

    /*
     Node: ./SCOMO/Inventory/Delivered/<X>/Operations/InstallInactive
     Exec command causes device to install a delivered application. The data inside 
     exec command refers to the application to be installed. 
     Support: Mandatory
     Occurs: One
     Format: Node
     Access Types: Exec, Replace
     Values: N/A
     */
    MSmlDmDDFObject& loinstact =
            delop.AddChildObjectL(KAMInstallInactiveNodeName);
    FillNodeInfoL(loinstact, accessTypesExecReplace, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMInstallInactivateNodeDescription);

    
    /*
     Node: ./SCOMO/Inventory/Delivered/<X>/Operations/Remove
     Exec command causes device to remove an application. The data inside exec command refers to the application to be removed. The application must be in Inactive state in order for remove to be successful.
     Support: Mandatory
     Occurs: One
     Format: Node
     Access Types: Exec, Replace
     Values: N/A
     */
    MSmlDmDDFObject& lorem = delop.AddChildObjectL(KAMRemoveNodeName);
    FillNodeInfoL(lorem, accessTypesExecReplace, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMRemoveNodeDescription);

    /*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
     DEPLOYED
     *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/
    RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 17" );
    /*
     Node: ./SCOMO/Inventory/Deployed/<X>
     This dynamic node is placeholder applications that are in Inactive state. 
     Support: Mandatory
     Occurs: ZeroOrMore
     Format: Node
     Access Types: Get, Replace
     Values: N/A
     */
    MSmlDmDDFObject& depldyna = ac.AddChildObjectGroupL();
    FillNodeInfoL(depldyna, accessTypesGetReplace,
            MSmlDmDDFObject::EZeroOrMore, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KAMDeployedDynaNodeDescription);

    /*
     Node: ./SCOMO/Inventory/Deployed/<X>/ID
     This leaf node holds an identifier for an application. This cannot be changed.
     Support: Mandatory
     Occurs: One
     Format: Chr
     Access Types: Get
     Values: N/A
     */
    MSmlDmDDFObject& deplid = depldyna.AddChildObjectL(KAMIDNodeName);
    FillNodeInfoL(deplid, accessTypesGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EChr,
            KAMIDNodeDescription);

    /*
     Node: ./SCOMO/Inventory/Deployed/<X>/Name
     This leaf node holds name of an application.
     Support: Mandatory
     Occurs: One
     Format: Chr
     Access Types: Get, Add, Replace
     Values: N/A
     */
    MSmlDmDDFObject& deplname = depldyna.AddChildObjectL(KAMNameNodeName);
    FillNodeInfoL(deplname, accessTypesGetAddReplace,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
            MSmlDmDDFObject::EChr, KAMNameNodeDescription);

    /*
     Node: ./SCOMO/Inventory/Deployed/<X>/Version
     This leaf node holds the version of an application.
     Support: Mandatory
     Occurs: One
     Format: Chr
     Access Types: Get, Add, Replace
     Values: N/A
     */
    MSmlDmDDFObject& deplver = depldyna.AddChildObjectL(KAMVersionNodeName);
    FillNodeInfoL(deplver, accessTypesGetAddReplace,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
            MSmlDmDDFObject::EChr, KAMVersionNodeDescription);
    RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 18" );
    /*
     Node: ./SCOMO/Inventory/Deployed/<X>/Description
     This leaf node holds the version of an application.
     Support: Mandatory
     Occurs: One
     Format: Chr
     Access Types: Get, Add, Replace
     Values: N/A
     */
    MSmlDmDDFObject& depldesc = depldyna.AddChildObjectL(KAMDescriptionNodeName);
    FillNodeInfoL(depldesc, accessTypesGetAddReplace,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
            MSmlDmDDFObject::EChr, KAMDescriptionNodeDescription);        
            
    /*
     Node: ./SCOMO/Inventory/Deployed/<X>/PkgIDRef
     This leaf node holds an identifier for an application. This cannot be changed.
     Support: Mandatory
     Occurs: One
     Format: Chr
     Access Types: Get
     Values: N/A
     */
    MSmlDmDDFObject& deplidref = depldyna.AddChildObjectL(KAMPkgIDRefNodeName);
    FillNodeInfoL(deplidref, accessTypesGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EChr,
            KAMPkgIDRefNodeDescription);

    /*
     Node: ./SCOMO/Inventory/Deployed/<X>/Status
     This leaf node holds the status of the Deployed. The client updates the node value. 
     The values are typically error codes of Operations done on deployed node
     Support: Mandatory 
     Occurs: One
     Format: Int
     Access Types: Get
     Values: N/A
     */
    MSmlDmDDFObject& deplstatus = depldyna.AddChildObjectL(KAMStatusNodeName);
    FillNodeInfoL(deplstatus, accessTypesGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EInt,
            KAMStatusNodeDescription);

    /*
     Node: ./SCOMO/Inventory/Deployed/<X>/State
     This leaf node holds state value of an application. 
     Support: ?
     Occurs: One
     Format: Chr
     Access Types: Get 
     Values: N/A
     */
    MSmlDmDDFObject& deplstate =
            depldyna.AddChildObjectL(KAMStateValueNodeName);
    FillNodeInfoL(deplstate, accessTypesGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EChr,
            KAMStateValueNodeDescription);
    RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 19" );
    /*
     Node: ./SCOMO/Inventory/Deployed/<X>/Operations
     This is a node that allows vendors to extend functionality.
     Support: Optional
     Occurs: One
     Format: Node
     Access Types: Get, Replace, Add, Delete
     Values: N/A

     */
    MSmlDmDDFObject& deplop = depldyna.AddChildObjectL(KAMOperationsNodeName);
    FillNodeInfoL(deplop, accessTypesGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMOperationsNodeDescription);

    /*
     Node: ./SCOMO/Inventory/Deployed/<X>/Operations/Remove
     Exec command causes device to remove an application. The data inside exec command 
     refers to the application to be removed. The application must be in Inactive state 
     in order for remove to be successful.
     Support: Mandatory
     Occurs: One
     Format: Node
     Access Types: Exec, Replace
     Values: N/A
     */
    MSmlDmDDFObject& deplrem = deplop.AddChildObjectL(KAMRemoveNodeName);
    FillNodeInfoL(deplrem, accessTypesExecReplace, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMRemoveNodeDescription);
    RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 20" );
    /*
     Node: ./SCOMO/Inventory/Deployed/<X>/Operations/Activate
     Exec command causes device to activate an application. 
     Support: Mandatory
     Occurs: One
     Format: Node
     Access Types: Exec, Replace
     Values: N/A
     */
    MSmlDmDDFObject& deplac = deplop.AddChildObjectL(KAMActivateNodeName);
    FillNodeInfoL(deplac, accessTypesExecReplace, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMActivateNodeDescription);

    /*
     Node: ./SCOMO/Inventory/Deployed/<X>/Operations/Deactivate
     Exec command causes device to deactivate an application. 
     Support: Mandatory
     Occurs: One
     Format: Node
     Access Types: Exec, Replace
     Values: N/A
     */
    MSmlDmDDFObject& depldeac = deplop.AddChildObjectL(KAMDeActivateNodeName);
    FillNodeInfoL(depldeac, accessTypesExecReplace, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMDeActivateNodeDescription);

    /*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
     DOWNLOAD
     *=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*/

    /*
     Node: ./SCOMO/Download
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
    MSmlDmDDFObject& dow = am.AddChildObjectL(KAMDownloadNodeName);
    FillNodeInfoL(dow, accessTypesGetAddReplace, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMDownloadNodeDescription);

    /*
     Node: ./SCOMO/Download/<X>
     This node is a placeholder for identifier of an application that is to be downloaded. 
     Support: Mandatory 
     Occurs: One
     Format: Node
     Access Types: Get, Add, Replace, Delete
     Values: N/A
     */
    MSmlDmDDFObject& dowdyna = dow.AddChildObjectGroupL();
    FillNodeInfoL(dowdyna, accessTypesGetAddReplaceDelete,
            MSmlDmDDFObject::EZeroOrMore, MSmlDmDDFObject::EDynamic,
            MSmlDmDDFObject::ENode, KAMDownloadDynaNodeDescription);

    /*
     Node: ./SCOMO/Download/<X>/PkgID
     This leaf node holds an identifier for an application. 
     Support: Mandatory
     Occurs: One
     Format: Chr
     Access Types: Get
     Values: N/A
     */
    MSmlDmDDFObject& dowid = dowdyna.AddChildObjectL(KAMPkgIDNodeName);
    FillNodeInfoL(dowid, accessTypesGetAddReplace, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EChr,
            KAMPkgIDNodeDescription);
    RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 21" );
    /*
     Node: ./SCOMO/Download/<X>/Name
     This leaf node holds name of an application. 
     Support: Mandatory
     Occurs: One
     Format: Chr
     Access Types: Add, Get, Replace
     Values: N/A
     */
    MSmlDmDDFObject& downame = dowdyna.AddChildObjectL(KAMNameNodeName);
    FillNodeInfoL(downame, accessTypesGetAddReplace,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
            MSmlDmDDFObject::EChr, KAMNameNodeDescription);

    
    /*
     Node: ./SCOMO/Download/<X>/PkgURL
     This leaf node holds the URL from which the application should be downloaded.
     Support: Mandatory 
     Occurs: One
     Format: Chr
     Access Types: Add, Get, Replace
     Values: N/A
     */
    MSmlDmDDFObject& dowuri = dowdyna.AddChildObjectL(KAMURINodeName);
    FillNodeInfoL(dowuri, accessTypesGetAddReplace,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
            MSmlDmDDFObject::EChr, KAMURINodeDescription);
            
    /*
     Node: /SCOMO/Download/<X>/PkgType
     This leaf node holds an identifier for an application.
     Support: Mandatory
     Occurs: One
     Format: Chr
     Access Types: Add, Get, Replace
     Values: N/A
     */
    MSmlDmDDFObject& dowpkgtype = dowdyna.AddChildObjectL(KAMPkgTypeNodeName);
    FillNodeInfoL(dowpkgtype, accessTypesGetAddReplace, MSmlDmDDFObject::EZeroOrOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EChr,
            KAMPkgTypeNodeDescription);

    /*
     Node: ./SCOMO/Download/<X>/Status
     This leaf node holds the status of the download. The client updates the node value. The values are typically error codes of download protocol. Initially after the URI leaf is filled, this will get value 100, and will be replaced by one.
     Support: Optional 
     Occurs: One
     Format: Chr
     Access Types: Get
     Values: N/A
     */
    MSmlDmDDFObject& dowstat = dowdyna.AddChildObjectL(KAMStatusNodeName);
    FillNodeInfoL(dowstat, accessTypesGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EChr,
            KAMStatusNodeDescription);

    /*
     Node: ./SCOMO/Download/<X>/Description
     This leaf node holds the version of an application. 
     Support: Mandatory
     Occurs: One
     Format: Chr
     Access Types: Add, Get, Replace
     Values: N/A
     */
    MSmlDmDDFObject& dowver = dowdyna.AddChildObjectL(KAMDescriptionNodeName);
    FillNodeInfoL(dowver, accessTypesGetAddReplace,
            MSmlDmDDFObject::EZeroOrOne, MSmlDmDDFObject::EPermanent,
            MSmlDmDDFObject::EChr, KAMDescriptionNodeDescription);
    
    
    /*
     Node: ./SCOMO/Download/<X>/Operations
     This is a node that holds the operation nodes.
     Support: Optional
     Occurs: One
     Format: Node
     Access Types: Get, Replace, Add, Delete
     Values: N/A

     */
    MSmlDmDDFObject& dowop = dowdyna.AddChildObjectL(KAMOperationsNodeName);
    FillNodeInfoL(dowop, accessTypesGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMOperationsNodeDescription);

    /*
     Node: ./SCOMO/Download/<X>/Operations/Download
     Exec command causes device to download an application. The dynamic node specifies the application to be downloaded. 
     Support: Mandatory
     Occurs: One
     Format: Node
     Access Types: Exec
     Values: N/A
     */
    MSmlDmDDFObject& dowdown =
            dowop.AddChildObjectL(KAMDownloadOperationNodeName);
    FillNodeInfoL(dowdown, accessTypesExecReplace, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMDownloadOperationNodeDescription);

    /*
     Node: ./SCOMO/Download/<X>/Operations/DownloadInstall
     Exec command causes device to download and install an application. The dynamic node specifies the application to be downloaded. 
     Support: Mandatory
     Occurs: One
     Format: Node
     Access Types: Exec
     Values: N/A
     */
    MSmlDmDDFObject& dowdowni =
            dowop.AddChildObjectL(KAMDownloadAndInstallNodeName);
    FillNodeInfoL(dowdowni, accessTypesExec, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMDownloadAndInstallOperationNodeDescription);

    /*
     Node: ./SCOMO/Download/<X>/Operations/DownloadInstallInactive
     Exec command causes device to download and install and activate an application. The dynamic node specifies the application to be downloaded. 
     Support: Mandatory
     Occurs: One
     Format: Node
     Access Types: Exec
     Values: N/A
     */
    MSmlDmDDFObject& dowdownia =
            dowop.AddChildObjectL(KAMDownloadAndInstallAndInActivateNodeName);
    FillNodeInfoL(dowdownia, accessTypesExec, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMDownloadAndInstallAndInActivateNodeName);

    RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 22" );
#ifdef __AM_LASTERROR_NODE
    MSmlDmDDFObject& amext = am.AddChildObjectL(KAMExtNodeName);
    FillNodeInfoL(amext, accessTypesGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::ENode,
            KAMExtNodeDescription);

    MSmlDmDDFObject& amerr = amext.AddChildObjectL(KAMLastErrorNodeName);
    FillNodeInfoL(amerr, accessTypesGet, MSmlDmDDFObject::EOne,
            MSmlDmDDFObject::EPermanent, MSmlDmDDFObject::EChr,
            KAMLastErrorDescription);
#endif

    // Ignore generic alert error status if any otherwise adapter fails to load 
    RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 23" );
    TRAP_IGNORE(SendPendingGenericAlertL());
	RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 24" );
    //if (GetAdapterValue() == KAMSCOMOAdapter)
        {
		RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 25" );
        CheckStateChangesL();
        SetAdapterValue(KAMDefaultValue);
        }
	RDEBUG( "CSCOMOAdapter::DDFStructureL(): Step 26" );
    CheckAndAddPredefinedNodeL();
    RDEBUG( "CSCOMOAdapter::DDFStructureL(): end" );
    }

void CSCOMOAdapter::SendPendingGenericAlertL()
    {

    // get information of the remote DM server
    TSmlProfileId profId = 0;
    HBufC8* serverId= NULL;
    TBuf8<KMaxSizeString> scomometatype;
    TBuf8<KMaxSizeString> scomometaformat;
    TBuf8<KMaxSizeString> scomomark;

    TBuf8<512> targetURIGet;

    TUint32 internalid = KErrNone;
    TInt finalresult = KErrNone;
    TInt i =0;

    // Get all the generic alert parameters from cenrep
    // to construct generic alert message
    CRepository *repository=CRepository::NewLC ( KCRUidPrivateApplicationManagementKeys );
    repository->Get(KSCOMOMetaTypeKey,scomometatype);
    repository->Get(KSCOMOMetaFormatKey,scomometaformat);
    repository->Get(KSCOMOMark,scomomark);
    CleanupStack::PopAndDestroy();
    
    // Retrive server info of current DM Session

    GetServerInfoL(profId, serverId);
    CleanupStack::PushL(serverId);

    RPointerArray<CAMDbItem> genericalerts;
    

    // Get generic alert entry from Database for particular server ID.
    TRAPD(err, iAMdb->GetEntryForServerIDL(genericalerts, *serverId));
    
    
    // Send Pending generic alerts for server
    
    for(i=0; i<genericalerts.Count(); i++)
    {
    	
    if (genericalerts.Count()!= 0 && err == KErrNone)
        {  

	
       
            internalid = genericalerts[i]->iLUID;
           
            // Get Target URI and Final Generic Alert result from App. Mgmt. Server 
            // database.

            TRAPD(err, SessionL().GetFinalResultsForGAL(internalid, finalresult,
                    targetURIGet));
                    
            // If data in SCOMO DB is inconsistent with AM Server DB or
            // if it exceeds retry count then delete that entry from adapter
            // and server database
            
            if(finalresult <= KErrNone || err!=KErrNone)
            {
            	iAMdb->DeleteFromDatabaseL(internalid);
            	User::Leave(KErrGeneral);
            }
	         
            
            TBuf8<6> dataresult;
            dataresult.Num(finalresult);
            
            TPtrC8 mapping(NSmlDmURI::LastURISeg(targetURIGet) );
            
            
            TInt retryupdate = iAMdb->GetRetryCountForLuidL(internalid);
            
            // If retryupdate in Zero then delete its entry from 
            // database.
            
            if(retryupdate == KErrNone)
            {
            iAMdb->DeleteFromDatabaseL(internalid);
            SessionL().GenericAlertSentL(internalid);
            User::Leave(KErrCompletion);	
            }
            
            // Decrement the retry count and update in database for
            // that internalid.
            iAMdb->UpdateRetryCountForLuidL(internalid, --retryupdate);    
            
            

            // open handle to private api

            RNSmlPrivateAPI privateAPI;
            privateAPI.OpenL();
            CleanupClosePushL(privateAPI);
            
            RArray<CNSmlDMAlertItem> iItemArray;
            

    		CNSmlDMAlertItem* item = new (ELeave) CNSmlDMAlertItem ;
    		
    		HBufC8 *data = HBufC8::NewL(1024);
            
            TPtr8 ptr = data->Des();
            //ptr.Append(KDataStart);
            ptr.Append(KResultCodeStart);
            ptr.Append(dataresult);
            ptr.Append(KResultCodeEnd);
            ptr.Append(KIdentifierStart);
            ptr.Append(mapping);
            ptr.Append(KIdentifierEnd);
            //ptr.Append(KDataEnd);

            HBufC8 *sourceuri = genericalerts[i]->iSourceURI;
    		
    		item->iSource = sourceuri;

			HBufC8 *targeturi = targetURIGet.AllocL();
    		
    		item->iTarget = targeturi;
    		
    		HBufC8 *metatype = scomometatype.AllocL();
    		            
    		item->iMetaType = metatype;
    		            
    		HBufC8 *metaformat = scomometaformat.AllocL();         

    		item->iMetaFormat = metaformat;

    		HBufC8 *metamark = scomomark.AllocL();
    		            
    		item->iMetaMark = metamark;
    
    		item->iData = data;
    
    		iItemArray.AppendL(*item);
            
            TRAP_IGNORE(privateAPI.AddDMGenericAlertRequestL(              
                    *genericalerts[i]->iCorrelator,iItemArray ));
                    
                    
            delete data;
            
            delete metamark;
            
            delete metaformat;
            
            delete metatype;
            
            delete targeturi;
            delete item;
                    
            iItemArray.Reset();			
			iItemArray.Close();
            CleanupStack::PopAndDestroy( &privateAPI);
            }
        }
    genericalerts.ResetAndDestroy();
    CleanupStack::PopAndDestroy(); //serverid


    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::SessionL()
// ------------------------------------------------------------------------------------------------
RApplicationManagement &CSCOMOAdapter::SessionL()
    {
    if ( !iSessionOpened)
        {
        TCertInfo info;

        const TUid KCentralRepositoryUid =
            {
            0x10207843
            };
        const TInt KCertKey = 0x01;
        CRepository *re= NULL;
        TRAPD( errx, re = CRepository::NewL ( KCentralRepositoryUid ) );
        if (errx == KErrNone && re)
            {
            TPckg<TCertInfo> pcert(info);
            errx = re->Get(KCertKey, pcert) ;

            RDEBUG_2("aCertInfo.iFingerprint.Length() is  %u",info.iFingerprint.Length() );
            if (errx == KErrNone)
                {
                RDEBUG("CSCOMOAdapter::SessionL() errx == KErrNone");
                }
            else
                {
                if (errx != KErrNotFound)
                    {
                    RDEBUG("CSCOMOAdapter::SessionL() errx != KErrNotFound");
                    }
                else
                    {
                    RDEBUG("CSCOMOAdapter::SessionL() errx == KErrNotFound");
                    }

                }
            delete re;
            }

        if (errx == KErrNotFound || !iCertRequired)
            {
            RDEBUG("CSCOMOAdapter::SessionL() calling iManagement.Connect()");
            User::LeaveIfError(iManagement.Connect() );
            iSessionOpened = ETrue;
            iTrustAdded = EFalse;
            }
        else
            {
            RDEBUG("CSCOMOAdapter::SessionL() calling iManagement.Connect( info )");
            User::LeaveIfError(iManagement.Connect(info) );
            iSessionOpened = ETrue;
            iTrustAdded = ETrue;
            }
        }
    return iManagement;
    }

// ------------------------------------------------------------------------------------------------
//  CSCOMOAdapter::UpdateLeafObjectL()
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::UpdateLeafObjectL(const TDesC8& aURI,
        const TDesC8& aLUID, const TDesC8& aObject, const TDesC8& aType,
        TInt aStatusRef)
    {
    RDEBUG8_4("CSCOMOAdapter::UpdateLeafObjectL(): begin, '%S', '%S' '%S'", &aURI, &aLUID, &aType );

    MSmlDmAdapter::TError status = EOk;
    SetAdapterValue(KAMSCOMOAdapter);
    _UpdateLeafObjectL(aURI, aLUID, aObject, aType, aStatusRef, status);

    SetStatusL(aStatusRef, status);
    if (status == EOk)
        {
        if (iInAtomic)
            {
            iBuffer.Append(TAMCommandBufferElement(aStatusRef, aURI) );
            }
        }
    RDEBUG_2("CSCOMOAdapter::UpdateLeafObjectL(): end (%d)", status);
    }

// ------------------------------------------------------------------------------------------------
//  CSCOMOAdapter::UpdateLeafObjectL()
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::_UpdateLeafObjectL(const TDesC8& aURI,
        const TDesC8& aLUID, const TDesC8& aObject, const TDesC8& aType,
        TInt /*aStatusRef*/, MSmlDmAdapter::TError& aStatus)
    {
    RDEBUG("CSCOMOAdapter::_UpdateLeafObjectL(): begin");

    CheckStateChangesL();

    TError status(EError);

#ifdef __TARM_SYMBIAN_CONVERGENCY
    TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash( aURI );
    TInt numSeqs( NSmlDmURI::NumOfURISegs( uriPtrc ) );
    TPtrC8 mapping( NSmlDmURI::LastURISeg( uriPtrc ) );
    TPtrC8 parent( NSmlDmURI::RemoveLastSeg( uriPtrc ) );
#else
    TInt numSeqs(NSmlDmURI::NumOfURISegs(aURI) );
    TPtrC8 mapping(NSmlDmURI::LastURISeg(aURI) );
    TPtrC8 parent(NSmlDmURI::RemoveLastSeg(aURI) );
#endif

    if (mapping == KAMNameNodeName)
        {
        if (aObject.Size() > KDeploymentComponentNameMaxLength)
            {
            aStatus = ETooLargeObject;
            return;
            }
        }
    else
        if (mapping == KAMVersionNodeName)
            {
            if (aObject.Size() > KDeploymentComponentVersionMaxLength)
                {
                aStatus = ETooLargeObject;
                return;
                }
            }
        else
	if (mapping == KAMURINodeName)
	  {
	   if (aObject.Size() > KMaxUrlLength)
                {
                aStatus = ETooLargeObject;
                return;
                }

	  }
        else
            if (mapping == KAMConRefNodeName)
                {
                if (aObject.Size() > KMaxConRef)
                    {
                    aStatus = ETooLargeObject;
                    return;
                    }
                }
            else
                if (mapping == KAMDescriptionNodeName)
                    {
                    if (aObject.Size() > 512)
                        {
                        aStatus = ETooLargeObject;
                        return;
                        }
                    }
            else
               if(mapping == KAMPkgIDNodeName)
               {
               if(aObject.Size() > 256)
               {
               	aStatus = ETooLargeObject;
                        return;
               }
               }
            else
              if(mapping == KAMPkgTypeNodeName)
              {
                // ADD check for PkgType MIME Type if not supported 
                // error status should be sent and operation should stop
                
              	if(aObject.Size() >100)
              	{
              		aStatus = ETooLargeObject;
                        return;
              	}
              }
             

    TPtrC8 grandParent(NSmlDmURI::RemoveLastSeg(parent) );
    TPtrC8 grandParentMapping(NSmlDmURI::LastURISeg(grandParent) );
    RApplicationManagement &session = SessionL();
    TDeploymentComponentState state(EDCSNone);
    if (numSeqs == 4 || numSeqs == 5)
        {
        if (numSeqs == 4)
            {
            state = EDCSDownload;
            }
        else
            {
            if (grandParentMapping == KAMDeliveredNodeName)
                {
                state = EDCSDelivered;
                }
            else
                if (grandParentMapping == KAMDeployedNodeName)
                    {
                    state = EDCSActive;
                    }
                else
                    {
                    RDEBUG8_3( "CSCOMOAdapter::UpdateLeafObjectL(): ILLEGAL LEVEL %d NODE %S", numSeqs, &aURI );
                    User::Leave(KErrArgument);
                    }
            }
        }
    else
        {
        RDEBUG8_3("CSCOMOAdapter::UpdateLeafObjectL(): ILLEGAL LEVEL %d NODE %S", numSeqs, &aURI );
        }
    if (state == EDCSDelivered || state == EDCSDownload ||state == EDCSActive
            ||state == EDCSInactive)
        {
        if (aLUID != KNullDesC8)
            {
            TInt iluid(DesToIntL(aLUID) );
            TDeploymentComponent comp;
            TInt err(session.DeploymentComponent(iluid, comp) );
            if (err == KErrNone)
                {
                
                // Set default component install options for particular iluid.
                TPtrC8 obj;
                TAMInstallOptions opts; 
                TAMInstallOptionsPckg optsb(opts);
                
                TBuf<KMaxSizeString> drive;
                
                CRepository *rep = CRepository::NewLC(KCRUidPrivateApplicationManagementKeys );
                rep->Get(KDefaultDriveForInstall,drive);
                CleanupStack::PopAndDestroy();
                
                //TChar KDefaultDrive = 'c';
                TChar KDefaultDrive = drive[0]; 
                
	 			opts.iOptions.iUpgrade = SwiUI::EPolicyAllowed;
     			opts.iOptions.iOptionalItems = SwiUI::EPolicyAllowed;
     			opts.iOptions.iOCSP = SwiUI::EPolicyAllowed;
     			opts.iOptions.iIgnoreOCSPWarnings = SwiUI::EPolicyAllowed;
					
     			opts.iOptions.iUntrusted = SwiUI::EPolicyAllowed;
     			opts.iOptions.iPackageInfo = SwiUI::EPolicyAllowed;
     			opts.iOptions.iCapabilities = SwiUI::EPolicyAllowed;
     			opts.iOptions.iKillApp = SwiUI::EPolicyAllowed;
     			opts.iOptions.iOverwrite = SwiUI::EPolicyAllowed;
     			opts.iOptions.iDownload = SwiUI::EPolicyAllowed;
     			opts.iOptions.iDrive = KDefaultDrive;
     			opts.iOptions.iLang = ELangNone;
     			opts.iOptions.iUsePhoneLang = EFalse ;
     			
     			obj.Set(optsb);
                
                TRAP_IGNORE( session.UpdateDeploymentComponentL( iluid, EDCInstallOptions, obj ) );
                
                
                TDeplCompAttrType fl(UpdateFlagFromMapping(mapping) );

                if (mapping == KAMDataNodeName || mapping
                        == KAMDescriptorNodeName)
                    {
                    TInt erx(KErrNone);
                    if ((mapping == KAMDataNodeName) &&(RecognizeMimeType(aType)== EFalse))
                        {
                        User::Leave(EInvalidObject);
                        }
                    if (iIsStreamedContent)
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
                    if (fl != EDCNone)
                        {
                        TPtrC8 obj;
                        TAMInstallOptions opts;
                        TAMInstallOptionsPckg optsb(opts);
                        TPckgBuf<TInt> iap;
                        if (fl == EDCInstallOptions)
                            {
                            }
                        else
                            if (fl == EDCConRef)
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
                                CNSmlDMIAPMatcher
                                        * iapfinder =
                                                CNSmlDMIAPMatcher::NewLC( &Callback() );
                                TInt i(iapfinder->IAPIdFromURIL(aObject) );
                                CleanupStack::PopAndDestroy(iapfinder);
                                iap() = i;
#endif
                                obj.Set(iap);
                                }
                            else
                                {
                                obj.Set(aObject);
                                }
                        TRAPD( erx, session.UpdateDeploymentComponentL( iluid, fl, obj ) )
                        ;
                        MAPERROR( erx, status, _L8("Update some attribute %d") );
                        }
                    else
                        {
                        status = ENotFound;
                        RDEBUG( "CSCOMOAdapter::UpdateLeafObjectL(): Unknown Target leaf" );
                        }
                    }
                }
            else
                {
                RDEBUG_3( "CSCOMOAdapter::UpdateLeafObjectL(): FAILED TO GET COMPONENT OF ID %d: %d" , iluid, err );
                status = ENotFound;
                }
            }
        else
            {
            RDEBUG8_2( "CSCOMOAdapter::UpdateLeafObjectL(): Faild to get luid allocation and find buffered for '%S'", &parent );
            status = ENotFound;
            }
        }
    else
        {
        RDEBUG8_2( "CSCOMOAdapter::UpdateLeafObjectL(): WARNING Tried to update illegal state leaf! '%S'", &grandParentMapping );
        }

    aStatus = status;

    RDEBUG("CSCOMOAdapter::_UpdateLeafObjectL(): end");
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::UpdateLeafObjectL
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::UpdateLeafObjectL(const TDesC8& aURI,
        const TDesC8& aLUID, RWriteStream*& aStream, const TDesC8& aType,
        TInt aStatusRef)
    {
    RDEBUG8_4("CSCOMOAdapter::UpdateLeafObjectL() aStream: begin, '%S', '%S' '%S'", &aURI, &aLUID, &aType );

    TError status(CSmlDmAdapter::EOk);

    if (iStreamedURI)
        {
        delete iStreamedURI;
        iStreamedURI = NULL;
        }
    iStreamedURI = aURI.AllocL();

    if (iStreamedLuid)
        {
        delete iStreamedLuid;
        iStreamedLuid = NULL;
        }
    iStreamedLuid = aLUID.AllocL();

    if (iStreamedType)
        {
        delete iStreamedType;
        iStreamedType = NULL;
        }
    iStreamedType = aType.AllocL();

    if (!iStreamOpen)
        {
        RApplicationManagement &session = SessionL();
        TDeplCompAttrType attrType;
        TPtrC8 mapping(NSmlDmURI::LastURISeg(aURI) );
        TDeplCompAttrType fl(UpdateFlagFromMapping(mapping) );
        if (mapping == KAMDataNodeName)
            {
            attrType = EDCData;
            }
        else
            if (mapping == KAMDescriptorNodeName)
                {
                attrType = EDCMetaData;
                }
            else
                {
                RDEBUG8_2("CSCOMOAdapter::UpdateLeafObjectL(): Unknown mapping: (%S)", &mapping);
                status = CSmlDmAdapter::EError;
                }

        if (status == CSmlDmAdapter::EOk)
            {
            TInt iluid(DesToIntL(aLUID) );
            session.DeploymentComponentTempInstFileL(iluid, iStreamFile,
                    attrType);
            iStream.Attach(iStreamFile);
            aStream = &iStream;
            iStreamOpen = ETrue;
            iIsStreamedContent = ETrue;
            }
        }
    if ( !iInAtomic)
        {
        iStatusRef = aStatusRef;
        Callback().SetStatusL(aStatusRef, status);
        }
    else
        {
        iBuffer.Append(TAMCommandBufferElement(aStatusRef, aURI) );
        }

    RDEBUG("CSCOMOAdapter::UpdateLeafObjectL() aStream : end");
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::UpdateFlagFromMapping( const TDesC8& aMapping )
// ------------------------------------------------------------------------------------------------
TDeplCompAttrType CSCOMOAdapter::UpdateFlagFromMapping(const TDesC8& aMapping)
    {

    TDeplCompAttrType res;
    if (aMapping == KAMIDNodeName)
        {
        res = EDCId;
        }
    else
        if (aMapping == KAMNameNodeName)
            {
            res = EDCName;
            }
        else
            if (aMapping == KAMVersionNodeName)
                {
                res = EDCVersion;
                }
            else
                if (aMapping == KAMStateValueNodeName)
                    {
                    res = EDCStateValue;
                    }
                else
                    if (aMapping == KAMInstallOptsNodeName)
                        {
                        res = EDCInstallOptions;
                        }
                    else
                        if (aMapping == KAMDescriptorNodeName)
                            {
                            res = EDCMetaData;
                            }
                        else
                            if (aMapping == KAMDataNodeName)
                                {
                                res = EDCData;
                                }
                            else
                                if (aMapping == KAMURINodeName)
                                    {
                                    res = EDCDownloadURI;
                                    }
                                else
                                    if (aMapping == KAMStatusNodeName)
                                        {
                                         res = EDCStatus;
                                        }
                                    else
                                        if (aMapping == KAMConRefNodeName)
                                            {
                                            res = EDCConRef;
                                            }
                                        else
                                            if (aMapping
                                                    == KAMDescriptionNodeName)
                                                {
                                                res = EDCDescriptionRef;
                                                }
                                              else
                                                if(aMapping == KAMPkgIDNodeName)
                                                {
                                                	res = EDCPkgID;
                                                }
                                                else
                                                if(aMapping == KAMPkgTypeNodeName)
                                                {
                                                	res = EDCPkgType;
                                                }
                                                

                                            else
                                                {
                                                //User::Leave( KErrArgument );
                                                res = EDCNone;
                                                }
    return res;
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::GetLuidForUserIdL( const TDesC8 &aUserId, 
//		const TDeploymentComponentState aState )
// ------------------------------------------------------------------------------------------------	
TUint32 CSCOMOAdapter::GetLuidForUserIdL(const TDesC8 &aUserId,
        const TDeploymentComponentState aState)
    {
    TUint32 ret( 0);
    if (aState != EDCSNone)
        {
        RElementIdArray array;
        TInt err(SessionL().DeploymentComponentIdsL(array, aState) );
        if (err == KErrNone)
            {
            CleanupClosePushL(array);
            TInt count(array.Count() );
            for (TInt i( 0); i < count && ret == 0; i++)
                {
                TUint32 el = array[i];
                TDCUserId dc;
                SessionL().DeploymentComponentUserIdL(el, dc);
                if (dc == aUserId)
                    {
                    ret = el;
                    }
                }
            CleanupStack::PopAndDestroy( &array);
            }
        else
            {
            }
        }
    return ret;
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::GetLuid2L( const TDesC8 &aDMLuid, const TDesC8 &aUserId, 
//		const TDeploymentComponentState aState, TError &aStatus )
// ------------------------------------------------------------------------------------------------	
TUint32 CSCOMOAdapter::GetLuid2L(const TDesC8 &aDMLuid,
        const TDesC8 &aUserId, const TDeploymentComponentState aState,
        TError &aStatus)
    {
    TUint32 ret(GetLuidL(aDMLuid, aUserId, aState) );
    if ( (TInt32)ret <= 0)
        {
        aStatus = ENotFound;
        }
    return ret;
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::GetLuidL( const TDesC8 &aDMLuid, const TDesC8 &aUserId, 
//		const TDeploymentComponentState aState )
// ------------------------------------------------------------------------------------------------	
TUint32 CSCOMOAdapter::GetLuidL(const TDesC8 &aDMLuid, const TDesC8 &aUserId,
        const TDeploymentComponentState aState)
    {
    TUint32 ret( 0);
    if (aDMLuid == KNullDesC8)
        {
        ret = GetLuidForUserIdL(aUserId, aState);
        }
    else
        {
        ret = DesToIntL(aDMLuid) ;
        }

    return ret;
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::DeleteObjectL(const TDesC8& aURI, const TDesC8& aLUID,
        const TInt aStatusRef)
    {

    RDEBUG8_4("CSCOMOAdapter::DeleteObjectL(): begin (%S, %S, %d)" , &aURI, &aLUID, aStatusRef );
    SetAdapterValue(KAMSCOMOAdapter);
    CheckStateChangesL();

    TError ret(EError);

#ifdef __TARM_SYMBIAN_CONVERGENCY	
    TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
    TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
    TPtrC8 mapping( NSmlDmURI::LastURISeg( uriPtrc ) );
#else
    TInt cnt(NSmlDmURI::NumOfURISegs(aURI) );
    TPtrC8 mapping(NSmlDmURI::LastURISeg(aURI) );
#endif
	
	TBuf8<KMaxFullName> temp;
	GetPredefinedNodeL(temp);
	TInt comp = mapping.Compare( temp );
		if (temp.Length() && mapping.Compare( temp ) == 0)
            {
            MSmlDmAdapter::TError status = MSmlDmAdapter::ENotAllowed;
            Callback().SetStatusL( aStatusRef, status );
            return;
            }
    switch (cnt)
        {
        case 3:
        case 4:
            {

#ifdef __TARM_SYMBIAN_CONVERGENCY
            TPtrC8 which( NSmlDmURI::URISeg( uriPtrc, cnt == 4 ? 3 : 2 ) );
#else
            TPtrC8 which(NSmlDmURI::URISeg(aURI, cnt == 4 ? 3 : 2) );
#endif
            if (which == KAMDeliveredNodeName || which == KAMDownloadNodeName)
                {
                TUint32 luid(GetLuidL(aLUID, mapping, (which
                        == KAMDeliveredNodeName
                                                ? EDCSDelivered
                                                   : EDCSDownload)) );
                if (luid > 0)
                    {
                    TRAPD( err, SessionL().RemoveL( luid ) )
                    ;
                    if (err == KErrNone || err == KErrNotFound)
                        {
                        ret = EOk;
                        Callback().SetMappingL(aURI, KNullDesC8);
                        if (err == KErrNone && iInAtomic)
                            {
                            iBuffer.Append(TAMCommandBufferElement(
                                    aStatusRef, aURI) );
                            }
                        }
                    else
                        {
                        MAPERROR( err, ret, _L8("Delete %d") );
                        }
                    }
                else
                    {
                    RDEBUG( "CSCOMOAdapter::DeleteObjectL(): WARNING Object not found" );
                    ret = ENotFound;
                    }
                }
            else
                {
                RDEBUG8_2( "CSCOMOAdapter::DeleteObjectL(): ERROR Removing node %S is not supported", &aURI );
                }
            break;
            }
        default:
            {
            RDEBUG8_3( "CSCOMOAdapter::DeleteObjectL(): ILLEGAL LEVEL %d NODE %S", cnt, &aURI );
            break;
            }
        }
    SetStatusL(aStatusRef, ret);
    RDEBUG( "CSCOMOAdapter::DeleteObjectL(): end" );
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::DeactivateIfInactive( const TUint aLuidi, const TDeploymentComponentState aTargetState )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::DeactivateIfInactive(const TUint aLuidi,
        const TDeploymentComponentState aTargetState)
    {
    if (aTargetState == EDCSInactive)
        {
        RDEBUG( "CSCOMOAdapter::DeactivateIfInactive():  deactivating!" );
        TRAPD( derr, iManagement.DeactivateL( aLuidi ) )
        ;
        if (derr != KErrNone)
            {
            RDEBUG_2("CSCOMOAdapter::DeactivateIfInactive(): ERROR deactivating failed %d", derr );
            }
        }
    }

void CSCOMOAdapter::InstallL(TUint aLuidi, const TDesC8& aURI,
        const TDesC8& aLUID, const TDeploymentComponentState aTargetState,
        TError &aRet)
    {

#ifdef __TARM_SYMBIAN_CONVERGENCY	
    TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
#else
    //nothing
#endif

    TRAPD( err, SessionL().InstallL( aLuidi, aTargetState ) )
    ;
    if (err == KErrNone)
        {
        // Update the mappings in client to reflect new
        //		position in the management tree ( Inactive or Active..)
        RDEBUG( "CSCOMOAdapter::InstallL(): Install success!" );

        TPtrC8 aclmapping(URISegsUpTo(aURI, 4) );
        SetMappingL(aclmapping, aTargetState, aLUID);
        iManagement.StateChangeComplete(aLuidi);

        DeactivateIfInactive(aLuidi, aTargetState);
        aRet = EOk;
        }
    else
        {
        RDEBUG_2( "CSCOMOAdapter::InstallL(): INSTALL FAILED '%d'" , err);
        MAPERROR( err, aRet, _L8("Install %d") );
        }
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::DefaultMapError( const TInt aErr, TError &aRet )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::DefaultMapError(const TInt aErr, TError &aRet,
        const TDesC8&
#ifdef __AM_LASTERROR_NODE
        aDes
#endif
)
    {
    if (aErr == KErrNone)
        {
        aRet = EOk;
        }
    else
        if (aErr == KErrNotFound)
            {
            aRet = ENotFound;
            }
        else
            if (aErr == KErrArgument)
                {
                aRet = EInvalidObject;
                }
            else
                if (aErr == KErrAlreadyExists)
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
void CSCOMOAdapter::SetErrorL(const TDesC8& aDes, const TInt aErr)
    {
    if (aDes != KNullDesC8)
        {
        SetLastErrorL(aDes, aErr);
        }
    else
        {
        SetLastErrorL(KStdError, aErr);
        }
    }
#endif	

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aArgument, 
//	const TDesC8& aType, 
//	TInt aStatusRef )
// ------------------------------------------------------------------------------------------------
TDownloadTarget CSCOMOAdapter::DownloadTargetL(const TDesC8& aCommand)
    {
    if (aCommand == KAMDownloadOperationNodeName)
        {
        return EDeliver;
        }
    else
        if (aCommand == KAMDownloadAndInstallNodeName)
            {
            return EInstallAndActivate;
            }
        else
            if (aCommand == KAMDownloadAndInstallAndInActivateNodeName)
                {
                return EInstall;
                }

    RDEBUG8_2( "CSCOMOAdapter::DownloadTargetL(): ASKED OPERATION NOT SUPPORTED Download '%S'", &aCommand );
    User::Leave(KErrArgument);
    // NOTE! will not ever come here! Only to get rid of compiler warning !!
    return EDeliver;
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aArgument, 
//	const TDesC8& aType, 
//	TInt aStatusRef )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::ExecuteCommandL(const TDesC8& aURI, const TDesC8& aLUID,
        const TDesC8& aArgument, const TDesC8& /*aType*/, TInt aStatusRef)
    {

#ifdef __TARM_SYMBIAN_CONVERGENCY	//Done by Dipak
    TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
#else
    //nothing
#endif

    RDEBUG8_3("CSCOMOAdapter::ExecuteCommandL(): begin, '%S', '%S'", &aURI, &aLUID );
    SetAdapterValue(KAMSCOMOAdapter);
    CheckStateChangesL();
    TError ret(EError);

    if (aLUID == KNullDesC8)
        {
        RDEBUG("CSCOMOAdapter::ExecuteCommandL(): WARNING no valid luid provided" );
        }

#ifdef __TARM_SYMBIAN_CONVERGENCY	//Done by Dipak
    TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
#else
    TInt cnt(NSmlDmURI::NumOfURISegs(aURI) );
#endif	

    TPtrC8 mapping(NSmlDmURI::LastURISeg(aURI) );

    const TInt KDownloadMappingURILoc = 3;
    switch (cnt)
        {
        case 5:
            {
            TUint32 iluid(GetLuidL(aLUID, NSmlDmURI::URISeg(aURI,
                    KDownloadMappingURILoc), EDCSDownload) );

            // Track the Newly added delivered  node to make sure that this reference
            // of delivered node is removed from Tree DB.
            // This delivered node is removed only if Execute command is successful
            // by either adding Install, Install& activate etc.
            // This Fix would be applicable in case of DM 1.2. 


            TPtrC8 urival(URISegsUpTo(aURI, KDownloadMappingURILoc));

            if (iUriDel)
                {
                delete iUriDel;
                iUriDel = NULL;
                }

            iUriDel = urival.AllocL();

            RDEBUG_2("CSCOMOAdapter::ExecuteCommandL(): luid is %d", iluid );
            if (iluid > 0)
                {
                TDownloadTarget target = DownloadTargetL(mapping);

                TRAPD( errx, SessionL().StartDownloadL( iluid, target ) );
                

                //Check If Sync is supported. Default is ASyncReporting.
                //If ASyncSupported is 0 mean Sync reporting is supported.  
                TInt ASyncSupported = -1;

                CRepository *repository=CRepository::NewLC ( KCRUidPrivateApplicationManagementKeys );
                repository->Get(KAsyncEnabled,ASyncSupported);
                CleanupStack::PopAndDestroy();
                    
				if(errx == KErrNone)
				{
                if (ASyncSupported)
                    ASyncReportL(iluid, aArgument, target, aURI);
                else
                    SyncReportL(iluid, aArgument, target, aURI, ret);
				}

                if (target == EInstall || target == EInstallAndActivate)
                    {
                    TInt err(iDbSession.RemoveMappingInfoL(KAMAdapterImplUid,
                            *iUriDel, iluid) );
                    }
                if (errx == KErrNone)
                    {
                    RDEBUG( "CSCOMOAdapter::ExecuteCommandL(): StartDownloadL Successful " );
                    if (ASyncSupported)
                    {
                       
                       ret = EAcceptedForProcessing;
                    }
                        
                    
                    }
                else
                    {
                    RDEBUG_2( "CSCOMOAdapter::ExecuteCommandL(): StartDownloadL FAILED '%d'", errx );
                    if (errx == KErrNotFound)
                        {
                        ret = ENotFound;
                        }
                    else
                        if (errx == KErrArgument)
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
                RDEBUG( "CSCOMOAdapter::ExecuteCommandL(): case 5 Not found node" );
                }
            break;
            }
        case 6:
            {
            const TInt KInventoryMappingURILoc = 4;

            // Track the Newly added delivered  node to make sure that this reference
            // of delivered node is removed from Tree DB.
            // This delivered node is removed only if Execute command is successful
            // by either adding Install, Install& activate etc.
            // This Fix would be applicable in case of DM 1.2. 


            TPtrC8 urival(URISegsUpTo(aURI, KInventoryMappingURILoc));

            if (iUriDel)
                {
                delete iUriDel;
                iUriDel = NULL;
                }

            iUriDel = urival.AllocL();

            if (mapping == KAMInstallNodeName || mapping
                    == KAMInstallInactiveNodeName)
                {
                TUint32 iluid(GetLuidL(aLUID, NSmlDmURI::URISeg(aURI,
                        KInventoryMappingURILoc), EDCSDelivered) );
                if (iluid > 0)
                    {
                    TDeploymentComponentState targetstate = ((mapping
                            == KAMInstallNodeName)
                                                   ? EDCSActive
                                                      : EDCSInactive);
                    InstallL(iluid, aURI, aLUID, targetstate, ret);

                    TInt err(iDbSession.RemoveMappingInfoL(KAMAdapterImplUid,
                            *iUriDel, iluid) );
                    }
                else
                    {
                    ret = ENotFound;
                    RDEBUG( "CSCOMOAdapter::ExecuteCommandL(): case 6 install Not found node" );
                    }
                }
            else
                if (mapping == KAMRemoveNodeName)
                    {
                    TPtrC8 parent( NSmlDmURI::RemoveLastSeg( urival ) );
                    TPtrC8 ParentMapping( NSmlDmURI::LastURISeg( parent ) );
                    TUint32 iluid(0);
                    if ( ParentMapping == KAMDeliveredNodeName )
                        {
                        iluid = GetLuidL(aLUID, NSmlDmURI::URISeg(aURI,
                                                    KInventoryMappingURILoc), EDCSDelivered) ;
                        }
                    
                    else if ( ParentMapping == KAMDeployedNodeName)
                        {
                        iluid = GetLuidL(aLUID, NSmlDmURI::URISeg(aURI,
                                KInventoryMappingURILoc), EDCSActive) ;
                        if ( (TInt32)iluid <= 0)
                            {
                            iluid = GetLuidL(aLUID, NSmlDmURI::URISeg(aURI,
                                    KInventoryMappingURILoc), EDCSInactive) ;
                            }
                        }
                    if (iluid > 0)
                        {
                        TRAPD( err, SessionL().RemoveL( iluid ) )
                        ;
                        if (err == KErrNone)
                            {
                            RDEBUG( "CSCOMOAdapter::ExecuteCommandL(): Remove success!" );
                            TPtrC8 aclmapping(URISegsUpTo(aURI, 4) );

                            // "Nullify" the mapping for argument
                           err = iDbSession.RemoveMappingInfoL(KAMAdapterImplUid, aclmapping);
                            if (err == KErrNone)
                                {
                                ret = EOk;
                                }
                            if (iInAtomic)
                                {
                                iBuffer.Append(TAMCommandBufferElement(
                                        aStatusRef, aURI) );
                                }
                            }
                        else
                            {
                            RDEBUG_2( "CSCOMOAdapter::ExecuteCommandL(): Remove FAILED '%d'", err);

                            if (err == SwiUI::KSWInstErrUserCancel)
                                ret = EError;
                            else
                                MAPERROR( err, ret, _L8("Execute Remove %d") );
                            }
                        }
                    else
                        {
                        ret = ENotFound;
                        RDEBUG( "CSCOMOAdapter::ExecuteCommandL(): case 6 remove Not found node" );
                        }
                    }
                        else
                            if (mapping == KAMActivateNodeName)
                                {
                                //DBG_ARGS8( "CSCOMOAdapter::ExecuteCommandL(): Activating " );
                                TUint32 iluid(GetLuidL(aLUID,
                                        NSmlDmURI::URISeg(aURI,
                                                KInventoryMappingURILoc),
                                        EDCSActive) );
                                if ( (TInt32)iluid <= 0)
                                    {
                                    iluid = GetLuidL(aLUID,
                                            NSmlDmURI::URISeg(aURI,
                                                    KInventoryMappingURILoc),
                                            EDCSInactive) ;
                                    }
                                if (iluid > 0)
                                    {
                                    TRAP_IGNORE( SessionL().ActivateL( iluid ) );
                                    ret = EOk;
								
                                    //MAPERROR( erx, ret, _L8("Activate %d") );
                                    }
                                else
                                    {
                                    ret = ENotFound;
                                    RDEBUG( "CSCOMOAdapter::ExecuteCommandL(): case 6 Activate Not found node" );
                                    }
                                }
                            else
                                if (mapping == KAMDeActivateNodeName)
                                    {
                                    //DBG_ARGS8( "CSCOMOAdapter::ExecuteCommandL(): Inactivating " );
                                    TUint32 iluid(GetLuidL(aLUID,
                                            NSmlDmURI::URISeg(aURI,
                                                    KInventoryMappingURILoc),
                                            EDCSActive) );
                                    if ( (TInt32)iluid <= 0)
                                        {
                                        iluid
                                                = GetLuidL(
                                                        aLUID,
                                                        NSmlDmURI::URISeg(
                                                                aURI,
                                                                KInventoryMappingURILoc),
                                                        EDCSInactive) ;
                                        }
                                    if (iluid > 0)
                                        {
                                        TRAP_IGNORE(SessionL().DeactivateL( iluid ) )
                                        ;
										ret = EOk;
                                        //MAPERROR( erx, ret,_L8("Dectivate %d") );
                                        }
                                    else
                                        {
                                        ret = ENotFound;
                                        RDEBUG( "CSCOMOAdapter::ExecuteCommandL(): case 6 DeActivate Not found node" );
                                        }
                                    }
                                else
                                    {
                                    RDEBUG8_2( "CSCOMOAdapter::ExecuteCommandL(): ASKED NOT SUPPORTED OPERATION '%S'", &mapping );
                                    
                                    ret = EError;
                                    }
            break;
            }

        default:
            {
            RDEBUG8_3( "CSCOMOAdapter::ExecuteCommandL(): ILLEGAL LEVEL %d NODE %S", cnt, &aURI );
            
            ret = EInvalidObject;
            break;
            }
        }
    SetStatusL(aStatusRef, ret) ;
    }

void CSCOMOAdapter::ASyncReportL(TUint32 aLuid, const TDesC8& aArgument,
        const TDownloadTarget aTarget,const TDesC8& aURI)
    {
    // while sending generic alert
    // adapter uses entry stored in database and constructs generic alert
    // using SyncML framework API.
    TInt index = 0;
	// Default max retry count is 5. It is also cenep configurable
	// Cenrep configuration responsible can change the max retry 
	// count
    TInt retrycount = 5;
    
    CRepository *repository = CRepository::NewLC ( KCRUidPrivateApplicationManagementKeys );
    repository->Get(KMaxRetryCount,retrycount);
    CleanupStack::PopAndDestroy(); //repository
      
    TSmlProfileId profId = KErrNotFound;
    HBufC8* serverId= NULL;

    //Retrive Server Information

    GetServerInfoL(profId, serverId);

    CleanupStack::PushL(serverId);

    //Add Download entry to database

    iAMdb->SaveToDatabaseL(index, aLuid, profId, retrycount, aURI,
           *serverId, aArgument);

    CleanupStack::PopAndDestroy(serverId);

    }

void CSCOMOAdapter::SyncReportL(TUint32 aLuid, const TDesC8& aArgument,
        const TDownloadTarget aTarget,const TDesC8& aURI, TError &aRet )
    {
    TInt status = KStatusSuccess;

    RProperty counter;
    TInt r=counter.Attach(KUidPSApplicationManagementKeys, KSyncNotifier,
            EOwnerThread);
    User::LeaveIfError(r);

    /*CSyncService *syncService = CSyncService::NewL(NULL, KDevManServiceStart);
    if (syncService)
        {
        syncService->EnableProgressNoteL(EFalse);
        }

    delete syncService;*/

    // wait for the previously attached ‘counter’ property to be updated
    TRequestStatus s;
    counter.Subscribe(s);
    User::WaitForRequest(s);
    
    CRepository *repository = CRepository::NewLC ( KCRUidPrivateApplicationManagementKeys );
    repository->Get(KAMSyncStatus,status);
    CleanupStack::PopAndDestroy();
                		   
		switch(status)
		{
			case KStatusUserCancelled:
			{
				aRet = MSmlDmAdapter::EError;
				break;
			}
			               
			case KStatusDowloadFailedOOM:
			{
				aRet = MSmlDmAdapter::EError;
				break;
			}

			case KStatusAlternateDownldAuthFail:
			{
			    aRet = MSmlDmAdapter::EError;
			    break;	
			}

			case KStatusDownloadFailed:
			{
				aRet = MSmlDmAdapter::EError;
				break;
			}

			case KStatusPkgValidationFailed:
			{
				aRet = MSmlDmAdapter::EError;
				break;
			}

			case KStatusInstallFailedOOM:
			{
				aRet = MSmlDmAdapter::EError;
				break;
			}

			case KStatusInstallFailed:
			{
				aRet = MSmlDmAdapter::EError;
				break;
			}
		
		  case KStatusUnSupportedEnvironment:
			{
				aRet = MSmlDmAdapter::EError;
				break;
			}
			
			case KStatusAltDowldUnavailable:
			{
				aRet = MSmlDmAdapter::EError;
				break;
			}
		
			default:
			{
			    aRet = MSmlDmAdapter::EOk;
				break;
			}
		}                
    

    }

void CSCOMOAdapter::GetServerInfoL(TSmlProfileId& aProfId, HBufC8*& aServerId) const
    {
    // Open session
    RSyncMLSession session;
    session.OpenL();
    CleanupClosePushL(session);

    // get current job's id (and usage type)
    TSmlJobId jobId = KErrNotFound;
    TSmlUsageType type = ESmlDevMan;
    session.CurrentJobL(jobId, type);

    // open currently running job
    RSyncMLDevManJob dmJob;
    dmJob.OpenL(session, jobId);
    CleanupClosePushL(dmJob);

    // get job's profile id
    aProfId = dmJob.Profile();

    // open profile using id in read-only mode
    RSyncMLDevManProfile dmProf;
    dmProf.OpenL(session, aProfId, ESmlOpenRead);
    CleanupClosePushL(dmProf);

    // get profile's server id
    aServerId = dmProf.ServerId().AllocL();

    // close handles
    CleanupStack::PopAndDestroy( &dmProf);
    CleanupStack::PopAndDestroy( &dmJob);
    CleanupStack::PopAndDestroy( &session);
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::UpdateL( 
// 	const TUint aLuidi, 
// 	const TDesC8& aURI, 
// 	const TDesC8& aSourceLUID,
// 	const TDeploymentComponentState aTargetState,
// 	TError &aRet
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::UpdateL(const TUint aLuidi, const TDesC8& aURI,
        const TDesC8& aSourceLUID,
        const TDeploymentComponentState aTargetState, TError &aRet)
    {

#ifdef __TARM_SYMBIAN_CONVERGENCY	//Done by Dipak
    TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
#else
    //nothing
#endif

    CBufBase *delivered = CBufFlat::NewL( 128);
    CleanupStack::PushL(delivered);

#ifdef __TARM_SYMBIAN_CONVERGENCY	//Done by Dipak
    TPtrC8 aclmapping( NSmlDmURI::URISeg( uriPtrc, 4 ) );
#else
    TPtrC8 aclmapping(NSmlDmURI::URISeg(aURI, 4) );
#endif

    HBufC8 *targetURI = HBufC8::NewLC(KDeployedURI().Length() + aclmapping.Length() );

    TPtr8 targetPtr(targetURI->Des() );
    targetPtr.Copy(KDeployedURI);
    targetPtr.Append(aclmapping);

    HBufC8 *deployed = Callback().GetLuidAllocL(targetPtr) ;
    CleanupStack::PushL(deployed);

    //DBG_ARGS8( "CSCOMOAdapter::UpdateL(): Updating '%S' luid: '%S'" ), &targetPtr, &(*deployed));
    if ( *deployed == KNullDesC8)
        {
        //DBG_ARGS8( "CSCOMOAdapter::UpdateL(): Failed to find to update target '%S'" ), &targetPtr );
        aRet = EError;
        }
    else
        {
        // Found delivered & active or inactive components! Do update!
        TInt depluid(DesToIntL( *deployed ));
        TRAPD( erx, SessionL().UpdateL( aLuidi, depluid ) )
        ;
        MAPERROR( erx, aRet,_L8("Update %d") );
        if (erx == KErrNone)
            {
            SetMappingLC(aclmapping, aTargetState, aSourceLUID, targetURI);
            DeactivateIfInactive(aLuidi, aTargetState);
            }
        }
    CleanupStack::PopAndDestroy(deployed) ;
    CleanupStack::PopAndDestroy(targetURI) ;
    CleanupStack::PopAndDestroy(delivered) ;
    }

// ------------------------------------------------------------------------------------------------
//  CSCOMOAdapter::CheckStateL( const TDeploymentComponent &aComp, const TDesC8& aURI
// ------------------------------------------------------------------------------------------------
TBool CSCOMOAdapter::CheckStateL(const TDeploymentComponent &aComp,
        const TDesC8& aURI)
    {
	TBool ret;
	if ((aComp.iState ==  EDCSDelivered) || (aComp.iState == EDCSActive) || ( aComp.iState == EDCSInactive) || (aComp.iState == EDCSDownload))
	{
        // Check for map & __TARM_SYMBIAN_CONVERGENCY also ?
        ret = ETrue;
    }
	else
	    ret = EFalse;
	return ret;
    }

// ------------------------------------------------------------------------------------------------
//  CSCOMOAdapter::StateL( const TDeploymentComponent &aComp, const TDesC8& aURI
// ------------------------------------------------------------------------------------------------
TDeploymentComponentState CSCOMOAdapter::StateL(const TDesC8& aURI)
    {

#ifdef __TARM_SYMBIAN_CONVERGENCY	//Done by Dipak
    TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
#else
    //nothing
#endif

    TDeploymentComponentState ret(EDCSNone);
#ifdef __TARM_SYMBIAN_CONVERGENCY	//Done by Dipak
    TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
#else
    TInt cnt(NSmlDmURI::NumOfURISegs(aURI) );
#endif	
    if (cnt > 2)
        {
#ifdef __TARM_SYMBIAN_CONVERGENCY	//Done by Dipak
        TPtrC8 map( NSmlDmURI::URISeg( uriPtrc, 2 ) );
#else
        TPtrC8 map(NSmlDmURI::URISeg(aURI, 2) );
#endif	
        if (map == KAMInventoryNodeName)
            {
#ifdef __TARM_SYMBIAN_CONVERGENCY	//Done by Dipak
            TPtrC8 map( NSmlDmURI::URISeg( uriPtrc, 2 ) );
#else
            TPtrC8 map(NSmlDmURI::URISeg(aURI, 2) );
#endif	
            map.Set(NSmlDmURI::URISeg(aURI, 3) );
            if (map == KAMDeployedNodeName)
                {

                }
            else
                if (map == KAMDeliveredNodeName)
                    {
                    ret = EDCSDelivered;
                    }
                else
                    {
                    User::Leave(KErrArgument);
                    }
            }
        else
            if (map == KAMDownloadNodeName)
                {
                ret = EDCSDownload;
                }
            else
                {
                User::Leave(KErrArgument);
                }
        }
    return ret;
    }

// ------------------------------------------------------------------------------------------------
//  CSCOMOAdapter::CheckStateL( const TDesC8& aURI, const TDesC8& aLUID )
// ------------------------------------------------------------------------------------------------
TBool CSCOMOAdapter::CheckStateL(const TDesC8& aURI, const TDesC8& aLUID)
    {

    TUint32 iluid(DesToIntL(aLUID) );
    TDeploymentComponent comp;
    TInt err(SessionL().DeploymentComponent(iluid, comp) );
    TBool ret(EFalse);
    if (err == KErrNone)
        {
        ret = CheckStateL(comp, aURI);
        }
    return ret;
    }

// ------------------------------------------------------------------------------------------------
//  CSCOMOAdapter::InstallOptionsDataL(const TUint32 iluid, const TDeplCompAttrType& aDataType, 
//		CBufBase &aBuf, CBufBase *aRaw/*= NULL*/)
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::InstallOptionsDataL(const TUint32 iluid,
        const TDeplCompAttrType& aDataType, CBufBase &aBuf, CBufBase *aRaw/*= NULL*/)
    {
    TAMInstallOptionsPckgBuf iop;
    TBool pushed(EFalse);
    if ( !aRaw)
        {
        aRaw = CBufFlat::NewL(32);
        CleanupStack::PushL(aRaw);
        pushed = ETrue;
        SessionL().DeploymentComponentDataL(iluid, aDataType, *aRaw);
        }

    iop.Copy(aRaw->Ptr( 0) );
    if (pushed)
        {
        CleanupStack::PopAndDestroy(aRaw);
        }
    InstallOptionsParser::SerializeOptionsL(iop(), aBuf);
    }

// ------------------------------------------------------------------------------------------------
//  CSCOMOAdapter::GetComponentDataL( const TDesC8& parent, const TDesC8& mapping, 
//		const TUint32 iluid, CBufBase *currentList, CBufBase *currentMime, TError &status )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::GetComponentDataL(const TDesC8& parent,
        const TDesC8& mapping, const TUint32 iluid, CBufBase &currentList,
        CBufBase &currentMime, TError &status)
    {
    TDeploymentComponent comp;
    TInt err(SessionL().DeploymentComponent(iluid, comp) );
    if (err == KErrNone)
        {
        RDEBUG_3( "CSCOMOAdapter::GetComponentDataL(): DeploymentComponent response: %d, state: %d", err, comp.iState );
        if (CheckStateL(comp, parent) )
            {
            if (mapping == KAMPkgIDNodeName)
                {
                currentList.InsertL( 0, comp.iPkgID);
                status = EOk;
                }
            else
                if (mapping == KAMNameNodeName)
                    {
                    currentList.InsertL( 0, comp.iName);
                    status = EOk;
                    }
                else
                    if (mapping == KAMVersionNodeName)
                        {
                        currentList.InsertL( 0, comp.iVersion);
                        status = EOk;
                        }
                else
                   if(mapping == KAMIDNodeName)
                   {
                   	
                        currentList.InsertL( 0, comp.iId);
                        status = EOk;
                        
                   }
                else
                   if(mapping == KAMPkgIDRefNodeName)
                   {
                   	
                        currentList.InsertL( 0, comp.iPkgIDRef);
                        status = EOk;
                        
                   }
				else
                   if(mapping == KAMPkgIDNodeName)
                   {
                   	
                        currentList.InsertL( 0, comp.iPkgID);
                        status = EOk;
                        
                   }               
				else
                   if(mapping == KAMPkgTypeNodeName)
                   {
                   	
                        currentList.InsertL( 0, comp.iPkgType);
                        status = EOk;
                        
                   }             
                else 
                   if ( mapping == KAMStatusNodeName )
                {
                TDeplCompAttrType dataType( UpdateFlagFromMapping( mapping ) );
                SessionL().DeploymentComponentDataL( iluid, dataType, currentList );
                status = EOk;
                }			

                    else
                        if (mapping == KAMDescriptionNodeName)
                            {
                            currentList.InsertL(0, comp.iDescription);
                            status = EOk;
                            }
                        else
                            {
                            TDeplCompAttrType
                                    dataType(UpdateFlagFromMapping(mapping) );
                            if (comp.iState == EDCSDownload)
                                {
					if ( dataType == EDCDownloadURI )//|| dataType == EDCDownloadStatus )
                                    {
                                    SessionL().DeploymentComponentDataL(iluid,
                                            dataType, currentList);
                                    status = EOk;
                                    }
                                else
                                    if (dataType == EDCInstallOptions)
                                        {
                                        InstallOptionsDataL(iluid, dataType,
                                                currentList);
                                        status = EOk;
                                        }
                                    else
                                        if (dataType == EDCConRef)
                                            {
                                            CBufBase *b = CBufFlat::NewL(4);
                                            CleanupStack::PushL(b);
                                            SessionL().DeploymentComponentDataL(
                                                    iluid, dataType, *b);
                                            TPckgBuf<TInt> iap;
                                            iap.Copy(b->Ptr( 0) );
                                            CleanupStack::PopAndDestroy(b);
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
                                                CleanupStack::PushL( uri );
                                                currentList.InsertL( 0, *uri );
                                                CleanupStack::PopAndDestroy( uri );
                                                }

#else

                                            CNSmlDMIAPMatcher
                                                    * iapfinder =
                                                            CNSmlDMIAPMatcher::NewLC( &Callback() );
                                            HBufC8
                                                    * uri =
                                                            iapfinder->URIFromIAPIdL(iap() ) ;
                                            CleanupStack::PopAndDestroy(iapfinder);
                                            if (uri != NULL)
                                                {
                                                CleanupStack::PushL(uri) ;
                                                currentList.InsertL( 0, *uri);
                                                CleanupStack::PopAndDestroy(uri) ;
                                                }

#endif
                                            status = EOk;
                                            }
                                        else
                                            {
                                            RDEBUG8_2( "CSCOMOAdapter::GetComponentDataL(): ASKED NOT SUPPORTED LEAF '%S'", &mapping );
                                            }
                                }
                            else
                                if (comp.iState == EDCSDelivered)
                                    {
                                    if(mapping == KAMStateValueNodeName)
                                        {
                                      currentList.InsertL( 0,KAMStateValueDelivered() );
                                      status = EOk;
                                        }
                                    if (dataType == EDCData || dataType
                                            == EDCMetaData || dataType
                                            == EDCInstallOptions)
                                        {
                                        CBufBase *raw = CBufFlat::NewL( 128);
                                        CleanupStack::PushL(raw);
                                        SessionL().DeploymentComponentDataL(
                                                iluid, dataType, *raw);
                                        TPtrC8 source(raw->Ptr(0) );
                                        if (source.Length() > 0)
                                            {
                                            if (dataType == EDCData)
                                                {
                                                currentList.InsertL( 0,
                                                        source);
                                                SessionL().DeploymentComponentDataL(
                                                        iluid, EDCDataMime,
                                                        currentMime);

                                                }
                                            else
                                                if (dataType
                                                        == EDCInstallOptions)
                                                    {
                                                    InstallOptionsDataL(
                                                            iluid, dataType,
                                                            currentList, raw);
                                                    }
                                                else
                                                    {
                                                    currentList.InsertL( 0,
                                                            source);
                                                    SessionL().DeploymentComponentDataL(
                                                            iluid,
                                                            EDCMetaDataMime,
                                                            currentMime);
                                                    }
                                            }
                                        else
                                            {
                                            // 0 data length...
                                            }
                                        status = EOk;
                                        CleanupStack::PopAndDestroy(raw);
                                        RDEBUG8_2( "CSCOMOAdapter::GetComponentDataL(): DeploymentComponentDataL called '%S'", &mapping );
                                        }
                                    }
                                else
                                    if (dataType == EDCStateValue)
                                        {
                                        if (comp.iState == EDCSActive)
                                            {
                                            currentList.InsertL( 0,
                                                    KAMStateValueActive() );
                                            }
                                        else
                                            {
                                            currentList.InsertL( 0,
                                                    KAMStateValueInactive() );
                                            }
                                        status = EOk;
                                        }
                                    else
                                        {
                                        RDEBUG8_2( "CSCOMOAdapter::GetComponentDataL(): ERROR ASKED NOT SUPPORTED LEAF '%S'", &mapping );
                                        }
                            }
            }
        else
            {
            RDEBUG8_3( "CSCOMOAdapter::GetComponentDataL(): WARNING Asked illegal state leaf '%S', %d", &mapping, comp.iState );
            status = ENotFound;
            }
        }
    else
        {
        RDEBUG_3( "CSCOMOAdapter::GetComponentDataL(): ERROR FAILED TO GET COMPOMENT OF ID %d: %d", iluid, err );
        MAPERROR( err, status,_L8("Get Component %d") );
        }
    }

// ------------------------------------------------------------------------------------------------
//  CSCOMOAdapter::GetLeafLuidL(const TDesC8 &aURI, const TDesC8 & aParentMapping ) 
// ------------------------------------------------------------------------------------------------
TInt CSCOMOAdapter::GetLeafLuidL(const TDesC8 &aURI,
        const TDesC8 & aParentMapping)
    {

#ifdef __TARM_SYMBIAN_CONVERGENCY	//Done by Dipak
    TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
    TInt numSeqs( NSmlDmURI::NumOfURISegs( uriPtrc ) );
#else
    TInt numSeqs(NSmlDmURI::NumOfURISegs(aURI) );
#endif

    TUint32 iluid( 0);
    //TInt numSeqs( NSmlDmURI::NumOfURISegs( aURI ) );//Dipak

    switch (numSeqs)
        {
        case 4: // must be a download leaf
            {
            iluid = GetLuidForUserIdL(aParentMapping, EDCSDownload) ;
            break;
            }
        case 5: // download operation or delivered or deployed leaf
            {
            if (URISegsUpTo(aURI, 2, ETrue) == KDownloadURI)
                {
                RDEBUG8_2( "CSCOMOAdapter::GetLeafLuidL(): WARNING download operation leaf '%S'", &aURI );
                //status = EOk; 
                }
            else
                {
                TPtrC8 start(URISegsUpTo(aURI, 3, ETrue) );
                if (start == KDeliveredURI)
                    {
                    iluid = GetLuidForUserIdL(aParentMapping, EDCSDelivered) ;
                    }
                else
                    if (start == KDeployedURI)
                        {
                        iluid = GetLuidForUserIdL(aParentMapping, EDCSActive) ;
                        if ( (TInt32)iluid <= 0)
                            {
                            iluid = GetLuidForUserIdL(aParentMapping,
                                    EDCSInactive) ;
                            }
                        }
                    else
                        {
                        RDEBUG8_2( "CSCOMOAdapter::GetLeafLuidL(): WARNING strange uri '%S'", &aURI );
                        }
                }
            break;
            }
        case 6: // delivered or deployed operation leaf
            {
            //iluid = GetLuidForUserIdL( parentMapping, EDCSDownload ) ;
            //status = EOk;
            RDEBUG8_2( "CSCOMOAdapter::GetLeafLuidL(): WARNING delivered or deployed operation leaf '%S'", &aURI );
            break;
            }
        default: // illegal uri
            {
            RDEBUG8_2( "CSCOMOAdapter::GetLeafLuidL(): ERROR requested luid of illegal URI '%S'", &aURI );
            break;
            }
        }
    return iluid;
    }

// ------------------------------------------------------------------------------------------------
//  CSCOMOAdapter::FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, const TDesC8& aType, 
//			CBufBase& aObject )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::FetchLeafObjectL(const TDesC8& aURI, const TDesC8& aLUID,
        const TDesC8& aType, const TInt aResultsRef, TInt aStatusRef)
    {

#ifdef __TARM_SYMBIAN_CONVERGENCY	//Done by Dipak
    TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
#else
    //Nothing
#endif

    RDEBUG8_4("CSCOMOAdapter::FetchLeafObjectL('%S', '%S', '%S'): begin", &aURI, &aLUID, &aType );
    SetAdapterValue(KAMSCOMOAdapter);
    CheckStateChangesL();
    TError status(EError);

    CBufBase *currentList = CBufFlat::NewL( 128);
    CleanupStack::PushL(currentList);

    CBufBase *currentMime = CBufFlat::NewL( 128);
    CleanupStack::PushL(currentMime);

    TPtrC8 mapping(NSmlDmURI::LastURISeg(aURI) );
    TPtrC8 parent(NSmlDmURI::RemoveLastSeg(aURI) );
    TPtrC8 parentMapping(NSmlDmURI::LastURISeg(parent) );

    HBufC8 *luid= NULL;
    if (aLUID != KNullDesC8)
        {
        luid = aLUID.AllocLC();
        }
    else
        { // try if we find it from download path
        TBuf8<256> down;
        down = KDownloadURI;
        down.Append(parentMapping);
        luid = Callback().GetLuidAllocL(down);
        CleanupStack::PushL(luid);
        if ( *luid != KNullDesC8)
            {
            // make it to found next time
            DirectSetMappingL(parent, *luid);
            MoveAclL(down, parent);

            // remove old
            DirectSetMappingL(down, KNullDesC8);
            }
        }
    if ( *luid != KNullDesC8)
        {
        TUint32 iluid(DesToIntL( *luid ));
        RDEBUG8_2( "CSCOMOAdapter::FetchLeafObjectL(): Luid is : %d", iluid );
        GetComponentDataL(parent, mapping, iluid, *currentList, *currentMime,
                status);
        }
    else
        {
        TUint32 iluid(GetLeafLuidL(aURI, parentMapping) );
        if (iluid > 0)
            {
            HBufC8 *l = IntToDes8LC(iluid);
            DirectSetMappingL(parent, *l);
            CleanupStack::PopAndDestroy(l);
            GetComponentDataL(parent, mapping, iluid, *currentList,
                    *currentMime, status);
            }
        else
            {
            RDEBUG8_2( "CSCOMOAdapter::FetchLeafObjectL(): WARNING Faild to get luid allocation for '%S'", &parent );
            if (aLUID == KNullDesC8)
                {
#ifdef __AM_LASTERROR_NODE
                if (aURI == KAMLastErrorURI)
                    {
                    const TDesC8& error= LastError();
                    RDEBUG8_2( "CSCOMOAdapter::FetchLeafObjectL(): Returning last error '%S'", &error );
                    currentList->InsertL(0, error);
                    status = EOk;
                    }
                else
                    {
                    status = ENotFound;
                    }
#else
                status = ENotFound;
#endif
                }
            }
        }

    SetStatusL(aStatusRef, status);
    if (status == EOk)
        {
        if (iInAtomic)
            {
            iBuffer.Append(TAMCommandBufferElement(aStatusRef, aURI) );
            }
        Callback().SetResultsL(aResultsRef, *currentList,
                ( (currentMime->Size() > 0) ? currentMime->Ptr(0) : aType ));
        }
    CleanupStack::PopAndDestroy(luid);
    CleanupStack::PopAndDestroy(currentMime); //currentMime
    CleanupStack::PopAndDestroy(currentList); //currentList
    RDEBUG("CSCOMOAdapter::FetchLeafObjectL: end" );
    }

// ------------------------------------------------------------------------------------------------
//  CSCOMOAdapter::IdListL( TDeploymentComponentState aState, 
//					CBufBase *aCurrentList, TBool aAppend /* = EFalse */)
// ------------------------------------------------------------------------------------------------
CSCOMOAdapter::TError CSCOMOAdapter::IdListL(
        TDeploymentComponentState aState, CBufBase &aCurrentList,
        TBool aAppend /* = EFalse */)
    {
	RDEBUG_2( "CSCOMOAdapter::IdListL:Begin aState is %d", aState);
    CSCOMOAdapter::TError ret(CSCOMOAdapter::EError);
    RElementIdArray array;
    TPtrC8 uriStart;
    TBuf8<256> uri;
	
    switch (aState)
        {
        case EDCSDelivered:
            uriStart.Set(KDeliveredURI);
            break;
        case EDCSActive:
        case EDCSInactive:
            uriStart.Set(KDeployedURI) ;
            break;
        case EDCSDownload:
            uriStart.Set(KDownloadURI) ;
            break;
        default:
            break;

        }
    TInt err(SessionL().DeploymentComponentIdsL(array, aState) );
    if (err == KErrNone)
        {
        TInt count(array.Count() );
		RDEBUG_2( "CSCOMOAdapter::IdListL: count is %d", count);
        if (count && aAppend && aCurrentList.Size() )
            {
				RDEBUG("CSCOMOAdapter::IdListL: Inserting list size" );
            aCurrentList.InsertL(aCurrentList.Size(), KAMSeparator8);
            }
        for (TInt i( 0); i < count; i++)
            {
            TUint32 el = array[i];
            TDCUserId dc;
            SessionL().DeploymentComponentUserIdL(el, dc);
			RDEBUG_2("CSCOMOAdapter::IdListL: Node name is %S", &dc);
            if (uriStart == KNullDesC8() )
                {
					RDEBUG("CSCOMOAdapter::IdListL: uriStart == KNullDesC8()" );
                TDeploymentComponent comp;
                err = SessionL().DeploymentComponent(el, comp) ;
                if (err == KErrNone)
                    {
                    switch (comp.iState)
                        {
                        case EDCSDelivered:
                            uriStart.Set(KDeliveredURI);
                            break;
                        case EDCSActive:
                        case EDCSInactive:
                            uriStart.Set(KDeployedURI) ;
                            break;
                        case EDCSDownload:
                            uriStart.Set(KDownloadURI) ;
                            break;
                        default:
                            break;
                        }
                    }
                }
            if (uriStart != KNullDesC8() )
                {
                uri.Copy(uriStart);
                uri.Append(dc);
                HBufC8 *luid = DirectGetLuidAllocLC(uri); //Callback().GetLuidAllocL( uri );
				RDEBUG_2( "CSCOMOAdapter::IdListL: luid is %S", luid);
                if (KNullDesC8() == *luid)
                    {
                    RDEBUG8_3( "CSCOMOAdapter::IdListL(): WARNING updating lost luid of '%S' %d", &uri, el );
                    HBufC8 *l = IntToDes8LC(el);
                    if ( *luid == *l)
                        {
                        RDEBUG( "CSCOMOAdapter::IdListL(): WARNING CANCEL (not really updating, they're the same already" );
                        }
                    else
                        {
							RDEBUG("CSCOMOAdapter::IdListL: DirectSetMappingL" );
                        DirectSetMappingL(uri, *l);
                        }
                    CleanupStack::PopAndDestroy(l);
                    }
                CleanupStack::PopAndDestroy(luid);

                }
            else
                {
                RDEBUG_2( "CSCOMOAdapter::IdListL(): WARNING failed to get component state of id %d, cannot check mappings ", el );
                }
            aCurrentList.InsertL(aCurrentList.Size(), dc);
            if (i + 1 < count)
                {
					RDEBUG("CSCOMOAdapter::IdListL: Adding Separator" );
                aCurrentList.InsertL(aCurrentList.Size(), KAMSeparator8);
                }
            }
        ret = EOk;
        }
    else
        {
        RDEBUG_2( "CSCOMOAdapter::IdListL(): Error %d", err );
        }
    array.Reset();
	RDEBUG("CSCOMOAdapter::IdListL: end" );
    return ret;
    }

// ------------------------------------------------------------------------------------------------
//  CSCOMOAdapter::ChildURIListL( const TDesC8& aURI, const TDesC8& aParentLUID, 
//			const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/, const TInt aResultsRef, 
//			TInt aStatusRef );
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::ChildURIListL(const TDesC8& aURI,
        const TDesC8& aParentLUID,
        const CArrayFix<TSmlDmMappingInfo>& /*aPreviousURISegmentList*/,
        const TInt aResultsRef, TInt aStatusRef)
    {

#ifdef __TARM_SYMBIAN_CONVERGENCY	//Done by Dipak
    TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
#else
    //Nothing
#endif

    RDEBUG8_3 ( "CSCOMOAdapter::ChildURIListL(): begin ('%S', '%S')" , &aURI, &aParentLUID );
    CheckStateChangesL();

    TError ret(EError);

    CBufBase *currentList = CBufFlat::NewL( 128);
    CleanupStack::PushL(currentList);

#ifdef __TARM_SYMBIAN_CONVERGENCY	
    TInt numSeqs( NSmlDmURI::NumOfURISegs( uriPtrc ) );
#else
    TInt numSeqs(NSmlDmURI::NumOfURISegs(aURI) );
#endif

    TPtrC8 mapping(NSmlDmURI::LastURISeg(aURI) );
    if (numSeqs == 1)
        {
        currentList->InsertL( 0, KAMRootChilds);
        ret = EOk;
        }
    else
        if (numSeqs == 2)
            {
            if (mapping == KAMInventoryNodeName)
                {
                currentList->InsertL( 0, KAMInventoryChilds);
                ret = EOk;
                }
            else
                if (mapping == KAMDownloadNodeName)
                    {
                    RDEBUG8_2( "CSCOMOAdapter::ChildURIListL(): Level 2 NODE %S", &aURI );
                    TDeploymentComponentState state(EDCSDownload);
                    ret = IdListL(state, *currentList);
                    }
                else
                    {
                    RDEBUG8_2( "CSCOMOAdapter::ChildURIListL(): ILLEGAL LEVEL 2 NODE %S", &aURI );
                    User::Leave(KErrArgument);
                    }
            }
        else
            if (numSeqs == 3)
                {
                if (mapping == KAMDeliveredNodeName)
                    {
                    RDEBUG8_2( "CSCOMOAdapter::ChildURIListL(): Level 3 NODE %S", &aURI );
                    TDeploymentComponentState state(EDCSDelivered);
                    ret = IdListL(state, *currentList);
                    }
                else
                    if (mapping == KAMDeployedNodeName)
                        {
                        TDeploymentComponentState state(EDCSActive);
                        ret = IdListL(state, *currentList);
                        if (ret == EOk)
                            {
                            state = EDCSInactive;
                            ret = IdListL(state, *currentList, ETrue);
                            }
                        }
                    else
                        {
                        if (aParentLUID != KNullDesC8)
                            {
                            if (CheckStateL(aURI, aParentLUID) )
                                {
                                TPtrC8
                                        parent(NSmlDmURI::RemoveLastSeg(aURI) );
                                TPtrC8
                                        parentMapping(NSmlDmURI::LastURISeg(parent) );
                                if (parentMapping == KAMDownloadNodeName)
                                    {
                                    currentList->InsertL( 0,
                                            KAMDownloadDynaChilds);
                                    ret = EOk;
                                    }
                                else
                                    {
                                    RDEBUG8_2( "CSCOMOAdapter::ChildURIListL(): ILLEGAL LEVEL 3 NODE %S", &aURI );
                                    User::Leave(KErrArgument);
                                    }
                                }
                            else
                                {
                                RDEBUG8_2( "CSCOMOAdapter::ChildURIListL(): DC has changed state %S", &aURI );
                                ret = ENotFound;
                                }
                            }
                        else
                            {
                            RDEBUG8_2( "CSCOMOAdapter::ChildURIListL(): Level3 Node not found %S", &aURI );
                            ret = ENotFound;
                            }
                        }
                }
            else
                if (numSeqs == 4)
                    {
                    TPtrC8 parent(NSmlDmURI::RemoveLastSeg(aURI) );
                    TPtrC8 parentMapping(NSmlDmURI::LastURISeg(parent) );
                    if (aParentLUID == KNullDesC8)
                        {
                        RDEBUG8_2( "CSCOMOAdapter::ChildURIListL(): WARNING possibly a illegal level 4 node %S", &aURI );
                        ret = ENotFound;
                        }

                    if (parentMapping == KAMDeliveredNodeName)
                        {
                        if (GetLuid2L(aParentLUID, mapping, EDCSDelivered,
                                ret) > 0)
                            {
                            currentList->InsertL( 0,
                                    KAMInventoryDeliveredDynaChilds);
                            ret = EOk;
                            }
                        }
                    else
                        if (parentMapping == KAMDeployedNodeName)
                            {
                            if (GetLuid2L(aParentLUID, mapping, EDCSActive,
                                    ret) > 0)
                                {
                                currentList->InsertL( 0,
                                        KAMInventoryDynaChilds);
                                ret = EOk;
                                }
                            else
                                if (GetLuid2L(aParentLUID, mapping,
                                        EDCSInactive, ret) > 0)
                                    {
                                    currentList->InsertL( 0,
                                            KAMInventoryDynaChilds);
                                    ret = EOk;
                                    }
                            }
                        else
                            if (mapping == KAMOperationsNodeName)
                                {
                                if (GetLuid2L(aParentLUID, parentMapping,
                                        EDCSDownload, ret) > 0)
                                    {
                                    if (CheckStateL(aURI, aParentLUID) )
                                        {
                                        currentList->InsertL( 0,
                                                KAMDownloadOperationChilds);
                                        ret = EOk;
                                        }
                                    else
                                        {
                                        RDEBUG8_2( "CSCOMOAdapter::ChildURIListL(): Level3 operations DC changed state '%S'", &aURI );
                                        ret = ENotFound;
                                        }
                                    }
                                }
                            else
                                {
                                RDEBUG8_2( "CSCOMOAdapter::ChildURIListL(): Level4 Node not found '%S'", &aURI );
                                ret = ENotFound;
                                }
                    }
                else
                    if (numSeqs == 5)
                        {
                        if (aParentLUID == KNullDesC8)
                            {
                            RDEBUG8_2( "CSCOMOAdapter::ChildURIListL(): WARNING possibly a illegal level 5 node %S", &aURI );
                            ret = ENotFound;
                            }
                        if (mapping == KAMOperationsNodeName)
                            {
                            TPtrC8 parent(NSmlDmURI::RemoveLastSeg(aURI) );
                            TPtrC8
                                    parentMapping(NSmlDmURI::LastURISeg(parent) );
                            TPtrC8 gparent(NSmlDmURI::RemoveLastSeg(parent) );
                            TPtrC8
                                    gparentMapping(NSmlDmURI::LastURISeg(gparent) );
                            if (gparentMapping == KAMDeliveredNodeName)
                                {
                                if (GetLuid2L(aParentLUID, parentMapping,
                                        EDCSDelivered, ret) > 0)
                                    {
                                    currentList->InsertL( 0,
                                            KAMDeliveredOperationChilds);
                                    ret = EOk;
                                    }
                                }
                            else
                                if (gparentMapping == KAMDeployedNodeName)
                                    {
                                    if (GetLuid2L(aParentLUID, parentMapping,
                                            EDCSActive, ret) > 0)
                                        {
                                        currentList->InsertL( 0,
                                                KAMDeployedOperationChilds);
                                        ret = EOk;
                                        }
                                    else
                                        if (GetLuid2L(aParentLUID,
                                                parentMapping, EDCSInactive,
                                                ret) > 0)
                                            {
                                            currentList->InsertL( 0,
                                                    KAMDeployedOperationChilds);
                                            ret = EOk;
                                            }
                                    }
                                else
                                    {
                                    RDEBUG8_2( "CSCOMOAdapter::ChildURIListL(): Level5 Node not found wrong parent '%S'", &aURI );
                                    ret = ENotFound;
                                    }
                            }
                        else
                            {
                            RDEBUG8_2( "CSCOMOAdapter::ChildURIListL(): Level5 Node not found '%S'", &aURI );
                            ret = ENotFound;
                            }
                        }
                    else
                        {
                        RDEBUG8_3( "CSCOMOAdapter::ChildURIListL(): ILLEGAL LEVEL %d NODE %S", numSeqs, &aURI );
                        ret = EError;
                        }

    SetStatusL(aStatusRef, ret);

    if (ret == EOk)
        {
        if (iInAtomic)
            {
            iBuffer.Append(TAMCommandBufferElement(aStatusRef, aURI) );
            }
        Callback().SetResultsL(aResultsRef, *currentList, KNullDesC8);
        }
    CleanupStack::PopAndDestroy(); //currentList
    }

// ------------------------------------------------------------------------------------------------
//  CSCOMOAdapter::DeliverOrDownloadL( const TDesC8 &aUserId, const TDesC8 &aURI, TInt aCnt, 
//		const TDesC8 &aParentMapping, MSmlDmAdapter::TError &aStatus )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::DeliverOrDownloadL(const TDesC8 &aUserId,
        const TDesC8 &aURI, TInt aCnt, const TDesC8 &aParentMapping,
        MSmlDmAdapter::TError &aStatus)
    {

#ifdef __TARM_SYMBIAN_CONVERGENCY	//Done by Dipak
    TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
#else
    //Nothing
#endif

    switch (aCnt)
        {
        case 3:
            {
            if (aParentMapping == KAMDownloadNodeName)
                {
                TUint32 luid(SessionL().DownloadL(aUserId) );
                Callback().SetMappingL(aURI, *IntToDes8LC(luid) );
                CleanupStack::PopAndDestroy();
                aStatus = EOk;
                }
            else
                {
                RDEBUG( "CSCOMOAdapter::DeliverOrDownloadL(): ERROR Illegal parent");
                }
            break;
            }
        case 4:
            {
            if (aParentMapping == KAMDeliveredNodeName)
                {
                TUint32 luid(SessionL().DeliverL(aUserId) );
                Callback().SetMappingL(aURI, *IntToDes8LC(luid) );
                CleanupStack::PopAndDestroy();
                aStatus = EOk;
                }
            else
                {
                RDEBUG( "CSCOMOAdapter::DeliverOrDownloadL(): ERROR Illegal parent");
                }
            break;
            }
        default:
            {
            RDEBUG( "CSCOMOAdapter::DeliverOrDownloadL(): ERROR Illegal URI");
            break;
            }
        }
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::AddNodeObjectL( const TDesC8& aURI, const TDesC& aParentLUID )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::AddNodeObjectL(const TDesC8& aURI,
        const TDesC8& aParentLUID, TInt aStatusRef)
    {

#ifdef __TARM_SYMBIAN_CONVERGENCY	//Done by Dipak
    TPtrC8 uriPtrc = NSmlDmURI::RemoveDotSlash(aURI);
    TPtrC8 mapping( NSmlDmURI::LastURISeg( uriPtrc ) );
    TPtrC8 parent( NSmlDmURI::RemoveLastSeg( uriPtrc ) );
    TPtrC8 parentMapping( NSmlDmURI::LastURISeg( parent ) );
    TInt cnt( NSmlDmURI::NumOfURISegs( uriPtrc ) );
#else
    TPtrC8 mapping(NSmlDmURI::LastURISeg(aURI) );
    TPtrC8 parent(NSmlDmURI::RemoveLastSeg(aURI) );
    TPtrC8 parentMapping(NSmlDmURI::LastURISeg(parent) );
    TInt cnt(NSmlDmURI::NumOfURISegs(aURI) );
#endif

    RDEBUG8_3( "CSCOMOAdapter::AddNodeObjectL(): begin '%S', '%S'", &aURI, &aParentLUID );
    SetAdapterValue(KAMSCOMOAdapter);
    CheckStateChangesL();
    TError status(EError);
    //	TPtrC8 mapping( NSmlDmURI::LastURISeg( aURI ) );//Dipak
    //	TPtrC8 parent( NSmlDmURI::RemoveLastSeg( aURI ) );
    //	TPtrC8 parentMapping( NSmlDmURI::LastURISeg( parent ) );
    //	TInt cnt( NSmlDmURI::NumOfURISegs( aURI ) );
	TUint32 iluid( GetLuidL( aParentLUID, mapping, (parentMapping == KAMDeliveredNodeName ? EDCSDelivered : ( parentMapping == KAMDownloadNodeName ? EDCSDownload : EDCSNone ) ) ) ) ;
	if( !iluid)
	    {
            // This is added for avoiding duplicate node creation in Delivered/Download only.
	    iluid = GetLuidL( aParentLUID, mapping, (parentMapping == KAMDeliveredNodeName ? EDCSDownload:EDCSDelivered ));
	    }
    if (iluid == 0)
        {
        // Not found, add it ... 
        DeliverOrDownloadL(mapping, aURI, cnt, parentMapping, status);
        }
    else
        {
        RDEBUG_2( "CSCOMOAdapter::AddNodeObjectL(): WARNING May Already exists '%d'", iluid );
        TDeploymentComponent comp;
        TInt err(SessionL().DeploymentComponent(iluid, comp) );
        if (err == KErrNone)
            {
            RDEBUG_2( "CSCOMOAdapter::AddNodeObjectL(): ERROR Already exists '%d'", iluid );
            if (comp.iState == StateL(aURI) )
                {
                RDEBUG( "CSCOMOAdapter::AddNodeObjectL():    (state is ok) " );
                }
            else
                {
                RDEBUG( "CSCOMOAdapter::AddNodeObjectL():    (state is NOK) " );
                }
            status = EAlreadyExists;
            }
        else
            {
            if (err == KErrNotFound)
                {
                RDEBUG_2( "CSCOMOAdapter::AddNodeObjectL(): WARNING Already exists but not found '%d'", iluid );
                DeliverOrDownloadL(mapping, aURI, cnt, parentMapping, status);
                }
            else
                {

                RDEBUG_2( "CSCOMOAdapter::AddNodeObjectL(): ERROR May exists, but failed to fetch get one? '%d'", err );
                }
            }
        }
    if (status == EOk)
        {
        if (iInAtomic)
            {
            iBuffer.Append(TAMCommandBufferElement(aStatusRef, aURI) );
            }
        }

    //status = EAcceptedForProcessing;
    //TInt error = 202;
    SetStatusL(aStatusRef, status);
    RDEBUG( "CSCOMOAdapter::AddNodeObject(): end" );
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::GetSizeL( const TDesC8& aURI, const TDesC& aParentLUID )
// ------------------------------------------------------------------------------------------------
TInt CSCOMOAdapter::GetSizeL(const TDeplCompAttrType aDataType,
        const TInt aLuid, TError &aRet)
    {
    TInt size( 0);
    if (aDataType == EDCStateValue)
        {
        TDeploymentComponent comp;
        TInt err(SessionL().DeploymentComponent(aLuid, comp) );
        if (err == KErrNone)
            {
            if (comp.iState == EDCSActive)
                {
                size = KAMStateValueActive().Length();
                }
            else if (comp.iState == EDCSDelivered)
                {
                size = KAMStateValueDelivered().Length();
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
        size = SessionL().DeploymentComponentDataSizeL(aLuid, aDataType);
        aRet = EOk;
        }
    return size;
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC& aParentLUID )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::FetchLeafObjectSizeL(const TDesC8& aURI,
        const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef,
        TInt aStatusRef)
    {

    RDEBUG8_4("CSCOMOAdapter::FetchLeafObjectSizeL('%S', '%S', '%S'): begin", &aURI, &aLUID, &aType );
    CheckStateChangesL();
    TError status(EError);
    CBufBase *currentList = CBufFlat::NewL( 128);
    CleanupStack::PushL(currentList);

    TPtrC8 mapping(NSmlDmURI::LastURISeg(aURI) );
    TPtrC8 parent(NSmlDmURI::RemoveLastSeg(aURI) );
    TPtrC8 parentMapping(NSmlDmURI::LastURISeg(parent) );
    TInt size( 0);
    HBufC8 *luid= NULL;
    if (aLUID != KNullDesC8)
        {
        luid = aLUID.AllocLC();
        }
    else
        { // try if we find it from download path
        TBuf8<256> down;
        down = KDownloadURI;
        down.Append(parentMapping);
        luid = Callback().GetLuidAllocL(down);
        CleanupStack::PushL(luid);
        if ( *luid != KNullDesC8)
            {
            // make it to found next time
            DirectSetMappingL(parent, *luid);
            MoveAclL(down, parent);

            // remove old
            DirectSetMappingL(down, KNullDesC8);
            }
        }
    TDeplCompAttrType dataType(UpdateFlagFromMapping(mapping) );
    if ( *luid != KNullDesC8)
        {
        TUint32 iluid(DesToIntL( *luid ));
        RDEBUG_2( "CSCOMOAdapter::FetchLeafObjectSizeL(): Luid is : %d", iluid );
        size = GetSizeL(dataType, iluid, status);
        }
    else
        {
        TUint32 iluid(GetLeafLuidL(aURI, parentMapping) );
        if (iluid > 0)
            {
            HBufC8 *l = IntToDes8LC(iluid);
            DirectSetMappingL(parent, *l);
            CleanupStack::PopAndDestroy(l);
            size = GetSizeL(dataType, iluid, status);
            }
        else
            {
            RDEBUG8_2( "CSCOMOAdapter::FetchLeafObjectSizeL(): WARNING Faild to get luid allocation for '%S'", &parent );
            if (aLUID == KNullDesC8)
                {
                status = ENotFound;
                }
            }
        }
    SetStatusL(aStatusRef, status);
    if (status == EOk && size > 0)
        {
        HBufC8 *hsize = IntToDes8LC(size);
        currentList->InsertL( 0, *hsize);
        if (iInAtomic)
            {
            iBuffer.Append(TAMCommandBufferElement(aStatusRef, aURI) );
            }
        Callback().SetResultsL(aResultsRef, *currentList, aType);
        CleanupStack::PopAndDestroy(hsize);
        }
    CleanupStack::PopAndDestroy(luid);
    CleanupStack::PopAndDestroy(currentList); //currentList	
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::ExecuteCommandL( const TDesC8& aURI, const TDesC& aParentLUID )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::ExecuteCommandL(const TDesC8& /*aURI*/,
        const TDesC8& /*aLUID*/, RWriteStream*& /*aStream*/,
        const TDesC8& /*aType*/, TInt /*aStatusRef*/)
    {
    RDEBUG("CSCOMOAdapter::ExecuteCommandL(stream): begin (NOT SUPPORTED)");
    User::Leave(KErrNotSupported);
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::CopyCommandL( const TDesC8& aURI, const TDesC& aParentLUID )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::CopyCommandL(const TDesC8& /*aTargetURI*/,
        const TDesC8& /*aTargetLUID*/, const TDesC8& /*aSourceURI*/,
        const TDesC8& /*aSourceLUID*/, const TDesC8& /*aType*/,
        TInt aStatusRef)
    {
    RDEBUG("CSCOMOAdapter::CopyCommandL(): begin (NOT SUPPORTED)");
    SetStatusL(aStatusRef, EError);

    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::StartAtomicL(  )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::StartAtomicL()
    {
    RDEBUG("CSCOMOAdapter::StartAtomicL(): begin (NOT properly SUPPORTED)");
    iInAtomic = ETrue;
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::CommitAtomicL(  )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::CommitAtomicL()
    {
    RDEBUG("CSCOMOAdapter::CommitAtomicL(): begin (NOT SUPPORTED)");
    iInAtomic = EFalse; // the initial statuses are already set
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::RollbackAtomicL(  )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::RollbackAtomicL()
    {
    RDEBUG("CSCOMOAdapter::RollbackAtomicL(): begin (NOT properly SUPPORTED)");
    TInt count(iBuffer.Count() );
    for (TInt i( 0); i < count; i++)
        {
        TAMCommandBufferElement &pm(iBuffer[i]);
        SetStatusL(pm.iStatusRef, ERollbackFailed);
        }
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::StreamingSupport( )
// ------------------------------------------------------------------------------------------------
TBool CSCOMOAdapter::StreamingSupport(TInt& aItemSize)
    {
    RDEBUG("CSCOMOAdapter::StreamingSupport(): begin");
    //aItemSize = 1024;
    
    // After Generic alert is sent delete that entry from database and notify
    // AM server to delete the appropriate entry in its database.
    
    if(aItemSize == KNSmlDMResetGenAlerts)
    {
      TSmlProfileId profId = 0;
      HBufC8* serverId = NULL;
      TRAP_IGNORE(GetServerInfoL( profId, serverId ));
      
    
    TUint32 internalid;
    RPointerArray<CAMDbItem> genericalerts;
    

    // Get generic alert entry from Database for particular server ID.
    TRAPD(err, iAMdb->GetEntryForServerIDL(genericalerts, *serverId));

    if (genericalerts.Count()!= KErrNone && err == KErrNone)
        {
          	for(TInt i=0; i<genericalerts.Count(); i++)
    		{
			internalid = genericalerts[i]->iLUID;
			// Delete the all the generic alert details from SCOMO adapeter DB
			// Also delete the appropriate entry from AM Server DB as well.
            TRAP_IGNORE(iAMdb->DeleteFromDatabaseL(internalid));
            TRAP_IGNORE(SessionL().GenericAlertSentL(internalid));
    		}
        }
    

    genericalerts.ResetAndDestroy();
    delete serverId;

    _LIT(KNull,"");
    // No monitoring required after generic alert is sent
    TInt err_rep = KErrNone;
    CRepository *repository1 = NULL;

    TRAP(err_rep, repository1 =CRepository::NewL ( KCRUidDeviceManagementInternalKeys ));
  
    if(err_rep == KErrNone)  
    {
    repository1 ->Set(KDevManServerIdKey ,KNull);
    repository1 ->Set(KDevManIapIdKey, KErrNone);
    repository1 ->Set(KDevManEnableDMNetworkMon, KErrNone);
    }
     
    if(repository1)
     delete repository1;
      
    }
    aItemSize = 1024;
    RDEBUG("CSCOMOAdapter::StreamingSupport(): end");
    return ETrue;
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::StreamCommittedL( )
// ------------------------------------------------------------------------------------------------
#ifdef __TARM_SYMBIAN_CONVERGENCY
void CSCOMOAdapter::StreamCommittedL( RWriteStream& /*aStream*/)
#else
void CSCOMOAdapter::StreamCommittedL()
#endif
    {
    RDEBUG("CSCOMOAdapter::StreamCommittedL(): begin");

    if (iStreamOpen)
        {
        iStream.CommitL();
        }

    MSmlDmAdapter::TError status = EOk;

    TRAPD( err, _UpdateLeafObjectL( *iStreamedURI, *iStreamedLuid, KNullDesC8(), *iStreamedType, iStatusRef, status ) )
    ;

    if (err != KErrNone)
        {
        RDEBUG_2("CSCOMOAdapter::StreamCommittedL(): _UpdateLeafObjectL returned error (%d)", err);
        }

    iIsStreamedContent = EFalse;
    iStreamOpen = EFalse;

    Callback().SetStatusL(iStatusRef, status);

    RDEBUG("CSCOMOAdapter::StreamCommittedL(): end");
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::CloseStreaming( )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::CloseStreaming()
    {
    if (iStreamOpen)
        {
        RDEBUG("CSCOMOAdapter::CloseStreaming(): closing");
        iStreamFile.Close(); // !!! Closing this will also close the iStreamRFs because it is adopted from server
        iStreamOpen = EFalse;
        }
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::CompleteOutstandingCmdsL()
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::CompleteOutstandingCmdsL()
    {
    RDEBUG("CSCOMOAdapter::CompleteOutstandingCmdsL(): begin");
    /*	iManagement.Close();
     iSessionOpened = EFalse ;*/
    iBuffer.Reset();
    RDEBUG("CSCOMOAdapter::CompleteOutstandingCmdsL(): Management session closed");
    }

// ------------------
//
// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] =
    {
    //
            IMPLEMENTATION_PROXY_ENTRY( KAMAdapterImplUid, CSCOMOAdapter::NewL )
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
TInt CSCOMOAdapter::DesToIntL(const TDesC8& aLuid)
    {
    TLex8 lex(aLuid);
    TInt value = 0;
    User::LeaveIfError(lex.Val(value) );
    return value;
    }

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
HBufC8* CSCOMOAdapter::IntToDes8LC(const TInt aLuid)
    {
    HBufC8* buf = HBufC8::NewLC( 10); //10 = max length of 32bit integer
    TPtr8 ptrBuf = buf->Des();
    ptrBuf.Num(aLuid);
    return buf;
    }

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::ConstructL()
    {
    iAMdb = CSCOMOAdapterDb::NewL();

#ifdef __TARM_SYMBIAN_CONVERGENCY
    iDbSession.ConnectL();
#else
    User::LeaveIfError(iDbSession.Connect());
#endif
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::SetStatusL( TInt aStatusRef, MSmlDmAdapter::TError aErrorCode ) 
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::SetStatusL(TInt aStatusRef,
        MSmlDmAdapter::TError aErrorCode)
    {
    RDEBUG_3("CSCOMOAdapter::SetStatusL( %d, %d )", aStatusRef, aErrorCode );
    Callback().SetStatusL(aStatusRef, aErrorCode);
    }

// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::SetResultsL( TInt aResultsRef, CBufBase& aObject, const TDesC8& aType )
// ------------------------------------------------------------------------------------------------
void CSCOMOAdapter::SetResultsL(TInt aResultsRef, CBufBase& aObject,
        const TDesC8& aType)
    {
    Callback().SetResultsL(aResultsRef, aObject, aType);

    }

// -------------------------------------------------------------------------------------
// CSCOMOAdapter::FillNodeInfoL()
// Fills the node info in ddf structure
// -------------------------------------------------------------------------------------
void CSCOMOAdapter::FillNodeInfoL(MSmlDmDDFObject& aNode,
        TSmlDmAccessTypes aAccTypes, MSmlDmDDFObject::TOccurence aOccurrence,
        MSmlDmDDFObject::TScope aScope, MSmlDmDDFObject::TDFFormat aFormat,
        const TDesC8& aDescription)
    {
    FillNodeInfoNoDefaultMimeL(aNode, aAccTypes, aOccurrence, aScope,
            aFormat, aDescription);
    if (aFormat!=MSmlDmDDFObject::ENode)
        {
        aNode.AddDFTypeMimeTypeL(KAMTextPlain);
        }

    }

// -------------------------------------------------------------------------------------
// CSCOMOAdapter::FillNodeInfoNoDefaultMimeL()
// Fills the node info in ddf structure, without default mime type
// -------------------------------------------------------------------------------------
void CSCOMOAdapter::FillNodeInfoNoDefaultMimeL(MSmlDmDDFObject& aNode,
        TSmlDmAccessTypes aAccTypes, MSmlDmDDFObject::TOccurence aOccurrence,
        MSmlDmDDFObject::TScope aScope, MSmlDmDDFObject::TDFFormat aFormat,
        const TDesC8& aDescription)
    {
    aNode.SetAccessTypesL(aAccTypes);
    aNode.SetOccurenceL(aOccurrence);
    aNode.SetScopeL(aScope);
    aNode.SetDFFormatL(aFormat);

    aNode.SetDescriptionL(aDescription);
    }

// -------------------------------------------------------------------------------------
// CSCOMOAdapter::CheckStateChangesInternalL()
// Updates the changed DC states to DM client mappings and ACLs
// -------------------------------------------------------------------------------------
void CSCOMOAdapter::CheckStateChangesInternalL()
    {

#ifdef __AM_CHECKSTATE_CHANGES
    RDEBUG( "CSCOMOAdapter::CheckStateChangesLInternal(): begin" );
    if ( !iUpdated)
        {
        RDEBUG( "CSCOMOAdapter::CheckStateChangesLInternal(): !iUpdated" );
        RElementIdArray arr;
        CleanupClosePushL(arr);
		SessionL().CheckStatusNodesL();
		RElementIdArray array;
		TInt err( SessionL().DeploymentComponentIdsL( array, EDCSActive ) ); //to sync with SIS/Java registry
		err = SessionL().DeploymentComponentIdsL( array, EDCSInactive ); //to sync with SIS/Java registry
		CleanupClosePushL(array);
        err = SessionL().StateChangingDCIdsL(arr);
        if (err == KErrNone)
            {
            TInt c(arr.Count() );
            for (TInt i( 0); i < c; i++)
                {
                TDeploymentComponent comp;
                err = iManagement.DeploymentComponent(arr[i], comp);
                if (err == KErrNone)
                    {
                    TDCUserId dc;
                    SessionL().DeploymentComponentUserIdL(arr[i], dc);
                    const TInt KNSmlDmMaxUriLength = 256;
                    HBufC8 *uri = HBufC8::NewLC(KNSmlDmMaxUriLength);
                    TPtr8 puri(uri->Des() );
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
                    CBufBase* acl = CBufFlat::NewL( 128);
                    CleanupStack::PushL(acl);

                    HBufC8 *newUri = HBufC8::NewLC(KNSmlDmMaxUriLength);
                    TPtr8 pnewUri(newUri->Des() );

#endif

                    switch (comp.iOldState)
                        {
                        case EDCSDelivered:
                            {
                            RDEBUG( "CSCOMOAdapter::CheckStateChangesLInternal(): Old State = EDCSDelivered" );
                            puri.Copy(KDeliveredURI() );
                            puri.Append(dc);

                            HBufC8 *luid = DirectGetLuidAllocLC(puri);

                            if ( *luid == KNullDesC8)
                                { // probably moved already
                                iManagement.StateChangeComplete(arr[i]);
                                }
                            else
                                {
                                // "Nullify" the old mapping 
								//DirectSetMappingL( puri, KNullDesC8 );
								DirectRemoveMappingL(puri);
#ifdef __TARM_SYMBIAN_CONVERGENCY								
                                iDbSession.OpenNodeL(puri, uriNode, RDmTree::EReadOnly);
                                uriNode.DmPropertyL( DevMan::EACL, acl );
#else
                                err = iDbSession.GetAclL(puri, *acl, EFalse); // no need for inherited
#endif										
                                // Determine new uri 
                                switch (comp.iState)
                                    {
                                    case EDCSActive:
                                    case EDCSInactive: // both have same node
                                        {
                                        pnewUri.Copy(KDeployedURI() );
                                        pnewUri.Append(dc);
                                        DirectSetMappingL(pnewUri, *luid);
                                        if (err == KErrNone)
                                            {
#ifdef __TARM_SYMBIAN_CONVERGENCY
                                            iDbSession.OpenNodeL(pnewUri, newUriNode, RDmTree::EReadOnly);
                                            uriNode.RemoveDmPropertyL( DevMan::EACL );
                                            newUriNode.SetDmPropertyL( DevMan::EACL, acl );
#else
                                            err = iDbSession.DeleteAclL(puri);

                                            err = iDbSession.UpdateAclL(
                                                    pnewUri, acl->Ptr( 0) );
#endif
                                            }
                                        else
                                            {
                                            RDEBUG_2("CSCOMOAdapter::CheckStateChangesL: ERROR Failed to get acl from %d ", err );
                                            }
                                        iManagement.StateChangeComplete(arr[i]);
                                        break;
                                        }
                                    default:
                                        {

                                        RDEBUG_2("CSCOMOAdapter::CheckStateChangesL: WARNING Illegal state change from delivered to %d ", comp.iState );

                                        break;
                                        }
                                    }
                                }
                            CleanupStack::PopAndDestroy(luid);
                            break;
                            }
                        case EDCSDownload:
                            {
                            RDEBUG( "CSCOMOAdapter::CheckStateChangesLInternal(): Old State = EDCSDownload" );
                            puri.Copy(KDownloadURI() );
                            //check DC contains SCOMODefault.
                            TBuf8<256> staticnode;
                            TInt staticnodeenabled = 0;
                            CRepository* cenrep = CRepository::NewLC( KCRUidPrivateApplicationManagementKeys );
                            cenrep->Get( KAMDownloadPredefinedNode, staticnode ); 
                            cenrep->Get( KAMStaticNodeEnabled, staticnodeenabled );
                            CleanupStack::PopAndDestroy(); //cenrep
                            if (dc.Find(staticnode) != KErrNotFound && staticnodeenabled)
                                {
                                puri.Append(staticnode);
                                }
                            else
                                {
                                puri.Append(dc);
                                }


                            HBufC8 *luid = DirectGetLuidAllocLC(puri); //Callback().GetLuidAllocL( puri );
                            //CleanupStack::PushL( luid );
                            if ( *luid == KNullDesC8)
                                { // probably moved already
                                CleanupStack::PopAndDestroy(luid);
                                puri.Copy(KDeliveredURI() );
                                puri.Append(dc);
                                luid = DirectGetLuidAllocLC(puri); //Callback().GetLuidAllocL( puri );
                                if ( *luid == KNullDesC8)
                                    { // probably moved already

                                    iManagement.StateChangeComplete(arr[i]);

                                    }
                                }
                            else
                                {
#ifdef __TARM_SYMBIAN_CONVERGENCY
                                iDbSession.OpenNodeL(puri, uriNode, RDmTree::EReadOnly);
                                uriNode.DmPropertyL( DevMan::EACL, acl );
#else
                                err = iDbSession.GetAclL(puri, *acl, EFalse); // no need for inherited
#endif							
                                // "Nullify" the old mapping 
								//DirectSetMappingL( puri, KNullDesC8 );
								DirectRemoveMappingL(puri);
                                switch (comp.iState)
                                    {
                                    case EDCSActive:
                                    case EDCSInactive: // both have same node
                                        {
                                        pnewUri.Copy(KDeployedURI() );
                                        pnewUri.Append(dc);
                                        DirectSetMappingL(pnewUri, *luid);
                                        if (err == KErrNone)
                                            {
#ifdef __TARM_SYMBIAN_CONVERGENCY
                                            iDbSession.OpenNodeL(pnewUri, newUriNode, RDmTree::EReadOnly);
                                            uriNode.RemoveDmPropertyL( DevMan::EACL );
                                            newUriNode.SetDmPropertyL( DevMan::EACL, acl );
#else											
                                            err = iDbSession.DeleteAclL(puri);
                                            err = iDbSession.UpdateAclL(
                                                    pnewUri, acl->Ptr( 0) );
#endif
                                            }
                                        else
                                            {
                                            RDEBUG_2("CSCOMOAdapter::CheckStateChangesL: ERROR Failed to get acl from %d ", err );
                                            }
                                        iManagement.StateChangeComplete(arr[i]);
                                        break;
                                        }
                                    case EDCSDelivered:
                                        {
                                        RDEBUG( "CSCOMOAdapter::CheckStateChangesLInternal(): Current State = EDCSDelivered" );
                                        pnewUri.Copy(KDeliveredURI() );
                                        pnewUri.Append(dc);
                                        DirectSetMappingL(pnewUri, *luid);
                                        if (err == KErrNone)
                                            {
#ifdef __TARM_SYMBIAN_CONVERGENCY
                                            iDbSession.OpenNodeL(pnewUri, newUriNode, RDmTree::EReadOnly);
                                            uriNode.RemoveDmPropertyL( DevMan::EACL );
                                            newUriNode.SetDmPropertyL( DevMan::EACL, acl );
#else

                                            err = iDbSession.DeleteAclL(puri);
                                            err = iDbSession.UpdateAclL(
                                                    pnewUri, acl->Ptr( 0) );
#endif
                                            }
                                        else
                                            {
                                            RDEBUG_2("CSCOMOAdapter::CheckStateChangesL: ERROR Failed to get acl from %d ", err );
                                            }
                                        iManagement.StateChangeComplete(arr[i]);
                                        break;
                                        }
                                    default:
                                        {

                                        RDEBUG_2("CSCOMOAdapter::CheckStateChangesL: WARNING Illegal state change from delivered to %d ", comp.iState );
                                        break;
                                        }
                                    }

                                }
                            CleanupStack::PopAndDestroy(luid);
                            break;
                            }
                        case EDCSActive:
                        case EDCSInactive: // these both are in same node
                            {

                            RDEBUG_3("CSCOMOAdapter::CheckStateChangesL: No need to move anything in change from %d to %d ", comp.iOldState, comp.iState );
                            iManagement.StateChangeComplete(arr[i]);
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
                    CleanupStack::PopAndDestroy(newUri);
                    CleanupStack::PopAndDestroy(acl);
                    CleanupStack::PopAndDestroy(uri);
#endif
                    }
                else
                    {
                    RDEBUG_2("CSCOMOAdapter::CheckStateChangesL: ERROR Failed to get component: %d ", err );
                    }
                }
            iUpdated = ETrue;
            }
        else
            {
            RDEBUG_2("CSCOMOAdapter::CheckStateChangesL: ERROR Failed to get state change dcs: %d ", err );
            }
        CleanupStack::PopAndDestroy( &array);
        CleanupStack::PopAndDestroy( &arr);
        }
    else
        {
        RDEBUG("CSCOMOAdapter::CheckStateChangesL: Already updated! ");
        }
    RDEBUG( "CSCOMOAdapter::CheckStateChangesL(): end" );
#else
    DBG_ARGS8( "CSCOMOAdapter::CheckStateChangesL(): disabled" );
#endif
    }

// -------------------------------------------------------------------------------------
// CSCOMOAdapter::CheckStateChangesL()
// Updates acl and mapping trees to match state changes
// -------------------------------------------------------------------------------------
void CSCOMOAdapter::CheckStateChangesL()
    {
    iCertRequired = EFalse;
    RDEBUG( "CSCOMOAdapter::CheckStateChangesL(): iCertRequired = EFalse" );
    TRAPD( err, CheckStateChangesInternalL() )
    ;
    if (err != KErrNone)
        {
        RDEBUG_2("CSCOMOAdapter::CheckStateChangesL(): warning failed to check state changes %d", err );
        }
    iCertRequired = ETrue;
    RDEBUG( "CSCOMOAdapter::CheckStateChangesL(): iCertRequired = ETrue" );
    if ( !iTrustAdded)
        {
        RDEBUG( "CSCOMOAdapter::CheckStateChangesL(): Adding Trust Closing Session" );
        iManagement.Close();
        iSessionOpened = EFalse;
        RApplicationManagement &session = SessionL();
        RDEBUG( "CSCOMOAdapter::CheckStateChangesL(): Adding Trust new session started" );
        }
    }

// -------------------------------------------------------------------------------------
// CSCOMOAdapter::URISegsUpTo(const TDesC8& aURI, TInt aUpTo)
// -------------------------------------------------------------------------------------
TPtrC8 CSCOMOAdapter::URISegsUpTo(const TDesC8& aURI, TInt aUpTo,
        TBool aInclKeno/*=EFalse*/)
    {

    TPtrC8 ret;
    TInt numOfURISegs( 0);
    for (TInt i( 0); i < aURI.Length() && ret.Ptr() == NULL; i++)
        {
        if (aURI[i] == KNSmlDMUriSeparator)
            {
            numOfURISegs++;
            if (aUpTo == numOfURISegs)
                {
                if (aInclKeno && aURI.Length() > i+1)
                    {
                    ret.Set(aURI.Left(i + 1) );
                    }
                else
                    {
                    ret.Set(aURI.Left(i) );
                    }
                }
            }
        }
    return ret;
    }

// -------------------------------------------------------------------------------------
// CSCOMOAdapter::DirectGetLuidAllocLC( const TDesC8 &aURI )
// -------------------------------------------------------------------------------------	
HBufC8 *CSCOMOAdapter::DirectGetLuidAllocLC(const TDesC8 &aURI)
    {

#ifdef __TARM_SYMBIAN_CONVERGENCY
    HBufC8 *ret = Callback().GetLuidAllocL( aURI );
#else
    HBufC8* ret = iDbSession.GetLuidAllocL(KAMAdapterImplUid, aURI);
#endif

    CleanupStack::PushL(ret);
    return ret;
    }

// -------------------------------------------------------------------------------------
// CSCOMOAdapter::DirectSetMappingL( const TDesC8 &aURI, const TDesC8 &aLUID )
// -------------------------------------------------------------------------------------		
TInt CSCOMOAdapter::DirectSetMappingL(const TDesC8 &aURI, const TDesC8 &aLUID)
    {

#ifdef __TARM_SYMBIAN_CONVERGENCY
    TRAPD( err, Callback().SetMappingL( aURI, aLUID ) );
#else
    TInt err(iDbSession.AddMappingInfoL(KAMAdapterImplUid, aURI, aLUID) );
#endif
    return err;
    }

// -------------------------------------------------------------------------------------
// CSCOMOAdapter::MoveAclL( const TDesC8 &aFrom, const TDesC8 &aTo )
// -------------------------------------------------------------------------------------		
void CSCOMOAdapter::MoveAclL(const TDesC8 &aFrom, const TDesC8 &aTo)
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
    CBufBase *acl = CBufFlat::NewL( 128);
    CleanupStack::PushL(acl);
    TInt err(iDbSession.GetAclL(aFrom, *acl, EFalse) ); // no need for inherited
    if (err == KErrNone)
        {
        err = iDbSession.DeleteAclL(aFrom);
        err = iDbSession.UpdateAclL(aTo, acl->Ptr( 0) );
        }
    CleanupStack::PopAndDestroy(acl);
#endif
    }

// -------------------------------------------------------------------------------------
// CSCOMOAdapter::SetMappingL( const TDesC8 &aOriginalURI, 
//	const TDeploymentComponentState aTargetState, const TDesC8 &aLUID )
// -------------------------------------------------------------------------------------		
void CSCOMOAdapter::SetMappingL(const TDesC8 &aOriginalURI,
        const TDeploymentComponentState aTargetState, const TDesC8 &aLUID)
    {

    HBufC8 * targeturi= NULL;
    SetMappingLC(aOriginalURI, aTargetState, aLUID, targeturi);
    CleanupStack::PopAndDestroy(targeturi);
    }

// -------------------------------------------------------------------------------------
// CSCOMOAdapter::SetMappingLC( const TDesC8 &aOriginalURI, 
//	const TDeploymentComponentState aTargetState, const TDesC8 &aLUID, 
//	HBufC8 *&aNewUri )
// -------------------------------------------------------------------------------------		
void CSCOMOAdapter::SetMappingLC(const TDesC8 &aOriginalURI,
        const TDeploymentComponentState aTargetState, const TDesC8 &aLUID,
        HBufC8 *&aNewUri)
    {

    // "Nullify" the mapping for argument
    DirectSetMappingL(aOriginalURI, KNullDesC8);

    // Determine new uri for aArgument
    TPtrC8 puri;
    if (aNewUri == NULL)
        {
        TPtrC8 argMapping(NSmlDmURI::LastURISeg(aOriginalURI) );
        TPtrC8 target;
        if (aTargetState == EDCSInactive || aTargetState == EDCSActive)
            {
            target.Set(KDeployedURI() );
            }
        else
            if (aTargetState == EDCSDelivered)
                {
                target.Set(KDeliveredURI() );
                }
            else
                {

                User::Leave(KErrArgument);
                }

        aNewUri = HBufC8::NewLC(target.Length() + argMapping.Length() );
        TPtr8 p(aNewUri->Des() );
        p.Copy(target);
        p.Append(argMapping);
        puri.Set(p);
        }
    else
        {
        puri.Set( *aNewUri);
        }
    DirectSetMappingL(puri, aLUID);
    MoveAclL(aOriginalURI, puri);
    }

#ifdef __AM_LASTERROR_NODE
// ------------------------------------------------------------------------------------------------
// CSCOMOAdapter::SetLastErrorL( const TDesC8& aLastError, const TInt aError )
// ------------------------------------------------------------------------------------------------	
void CSCOMOAdapter::SetLastErrorL(const TDesC8& aLastError, const TInt aError)
    {
    if (iLastError)
        {
        TPtrC8 p(*iLastError);
        RDEBUG8_2( "CSCOMOAdapter::SetLastErrorL: clearing last error %S", &p);
        delete iLastError;
        iLastError = NULL;
        }
    iLastError = HBufC8::NewL(aLastError.Length() + 10);
    iLastError->Des().Format(aLastError, aError);
    }

const TDesC8& CSCOMOAdapter::LastError()
    {
    return iLastError == NULL ? KNullDesC8() : *iLastError;
    }

#endif

TPtrC8 CSCOMOAdapter::LastURISeg(const TDesC8& aURI)
    {
    TInt i;
    for (i=aURI.Length()-1; i>=0; i--)
        {
        if (aURI[i]==KNSmlDMAMSeparatorDef)
            {
            break;
            }
        }
    if (i==0)
        {
        return aURI;
        }
    else
        {
        return aURI.Mid(i+1);
        }
    }

TInt CSCOMOAdapter::DesToInt(const TDesC8& aLuid)
    {
    TLex8 lex(aLuid);
    TInt value = 0;
    lex.Val(value);
    return value;
    }
void CSCOMOAdapter::CheckAndAddPredefinedNodeL()
    {

    RDEBUG("CSCOMOAdapter::CheckAndAddPredefinedNodeL(): begin");

    // Check if SCOMODefault node addition is enabled.
    TInt staticnodeenabled = 0;
    CRepository* cenrep = CRepository::NewLC( KCRUidPrivateApplicationManagementKeys );
    cenrep->Get( KAMStaticNodeEnabled, staticnodeenabled ); 
    CleanupStack::PopAndDestroy(); //cenrep
        
    RDEBUG("CSCOMOAdapter::CheckAndAddPredefinedNodeL(): step1");

    if(staticnodeenabled)
    {
      TBuf8<KMaxFullName> temp;

      RDEBUG("CSCOMOAdapter::CheckAndAddPredefinedNodeL(): step2");
      GetPredefinedNodeL(temp);

      RDEBUG("CSCOMOAdapter::CheckAndAddPredefinedNodeL(): step3");
      if (temp.Length())
        {


        RDEBUG("CSCOMOAdapter::CheckAndAddPredefinedNodeL(): step4");
        TBuf8<KMaxFullName> temp1;
        temp1.Zero();
        temp1.Copy(KAMNodeName);
        temp1.Append(KAMSeparator8);
        temp1.Append(KAMDownloadNodeName);
        temp1.Append(KAMSeparator8);
        temp1.Append(temp);
       
        HBufC8* luid = DirectGetLuidAllocLC(temp1);

         RDEBUG("CSCOMOAdapter::CheckAndAddPredefinedNodeL(): step5");

        if (luid->Length() > 0)
            {
            CleanupStack::PopAndDestroy(luid);
            return;
            }
        else
            {
            RDEBUG("CSCOMOAdapter::CheckAndAddPredefinedNodeL(): step6");
            // Not found, add node and mapping ... 
            //TError status(EError);
            TPtrC8 mapping(NSmlDmURI::LastURISeg(temp1) );
            TPtrC8 parent(NSmlDmURI::RemoveLastSeg(temp1) );
            TPtrC8 parentMapping(NSmlDmURI::LastURISeg(parent) );
            TInt cnt(NSmlDmURI::NumOfURISegs(temp1) );
             RDEBUG("CSCOMOAdapter::CheckAndAddPredefinedNodeL(): step7");
            TUint32 newLuid(SessionL().DownloadL(mapping));
            //Callback().SetMappingL( temp1, *IntToDes8LC( newLuid ) );
            //CleanupStack::PopAndDestroy();
            RDEBUG("CSCOMOAdapter::CheckAndAddPredefinedNodeL(): step8");
            DirectSetMappingL(temp1, *IntToDes8LC(newLuid) );
            RDEBUG("CSCOMOAdapter::CheckAndAddPredefinedNodeL(): step9");
            CleanupStack::PopAndDestroy(); // for IntToDes8LC()
            CleanupStack::PopAndDestroy(luid);
            }
        }
    }
    }
// ---------------------------------------------------------------------------
// CAmAdapter::GetPredefinedNodeL()
// Gets  predefined node for Download object from cenrep 
// ---------------------------------------------------------------------------
//
void CSCOMOAdapter::GetPredefinedNodeL(TDes8& aNode)
	{
	CRepository* centrep( NULL);
	aNode.Zero();
   	centrep = CRepository::NewLC( KCRUidPrivateApplicationManagementKeys );

    if ( centrep )
	    {
        TBuf8<256> temp;
		TInt err = centrep->Get( KAMDownloadPredefinedNode, temp );
    	if (centrep->Get( KAMDownloadPredefinedNode, temp )==KErrNone && temp.Length() )
			{
			temp.Trim();
			aNode.Copy(temp);
			}
		CleanupStack::PopAndDestroy(centrep);
	    }
	}
// -------------------------------------------------------------------------------------
// CAmAdapter::DirectRemoveMappingL( const TDesC8 &aURI )
// -------------------------------------------------------------------------------------        
TInt CSCOMOAdapter::DirectRemoveMappingL( const TDesC8 &aURI )
    {

#ifdef __TARM_SYMBIAN_CONVERGENCY
    TRAPD( err, Callback().SetMappingL( aURI, aLUID ) );
#else
    TInt err( iDbSession.RemoveMappingInfoL( KAMAdapterImplUid, aURI, ETrue ) );
#endif
    return err;
    }
TInt CSCOMOAdapter::GetAdapterValue()
    {
    TInt adapterValue;
    CRepository *repository = NULL;
    TRAPD( err, repository = CRepository::NewL ( KCRUidPrivateApplicationManagementKeys ) );

    RDEBUG_2("error reading cenrep %d", err);
    if (err == KErrNone)
        {
        err = repository->Get(KAMAdapterValue, adapterValue);
        }
    delete repository;
    repository = NULL;
    return adapterValue;
    }

void CSCOMOAdapter::SetAdapterValue(TInt aAdapterValue)
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
TBool CSCOMOAdapter::RecognizeMimeType(const TDesC8& aMimeType)
    {
    TBool isSupportedMimeType = EFalse;
    if ((aMimeType == KSisxMimeType )||(aMimeType == KSisMimeType )||(aMimeType == KPipMimeType )||
            (aMimeType == KJadMIMEType )||(aMimeType == KJarMIMEType )||(aMimeType == KJavaMIMEType ))
        {
        isSupportedMimeType = ETrue;
        }
    return isSupportedMimeType;
    }
// End of File

