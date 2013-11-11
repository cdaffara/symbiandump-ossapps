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


#ifndef __OMASCOMOADAPTER_H__
#define __OMASCOMOADAPTER_H__

// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <e32base.h>
#include <smldmadapter.h>
#include "omascomoadapterdb.h"

#ifdef __TARM_SYMBIAN_CONVERGENCY
#include <dmtree.h>
#else
#include "nsmldmtreedbclient.h"
#endif

#include "ApplicationManagementClient.h"

const TInt KMaxUrlLength = 2048;

namespace NApplicationManagement
    {

    const TUint KAMAdapterImplUid = 0x20021336;
    
    const TInt KMaxSizeString = 256;

    // the DDF version must be changed if any changes in DDF structure ( built in DDFStructureL() function )
    _LIT8( KAMDDFVersion, "1.0" );

    _LIT8( KAMTextPlain, "text/plain" );

#ifdef __TARM_SYMBIAN_CONVERGENCY
    _LIT8( KAMNodeName, "./SCOMO" );
#else
    _LIT8( KAMNodeName, "SCOMO" );
#endif
    _LIT8( KAMNodeDescription, "This node is the root node for all application management functionality" );

    _LIT8( KAMInventoryNodeName, "Inventory" );
    _LIT8( KAMInventoryNodeDescription, "This node is start node of application inventory" );

    _LIT8( KAMDeliveredNodeName, "Delivered" );
    _LIT8( KAMDeliveredNodeDescription, "This is node under which applications that are delivered but not installed are found." );

    _LIT8( KAMDeployedNodeName, "Deployed" );
    _LIT8( KAMDeployedNodeDescription, "This is a node under which deployed applications are found" );

    _LIT8( KAMDeliveredDynaNodeDescription, "This dynamic node is placeholder applications that are in Delivered state" );

    _LIT8( KAMPkgIDNodeName, "PkgID" );
    _LIT8( KAMPkgIDNodeDescription, "This leaf node holds an package identifier for an application" );


    _LIT8( KAMIDNodeName, "ID" );
    _LIT8( KAMIDNodeDescription, "This leaf node holds an identifier for an deployment component" );
    
    _LIT8( KAMPkgIDRefNodeName, "PkgIDRef" );
    _LIT8( KAMPkgIDRefNodeDescription, "This leaf node holds an identifier for an deployment component" );

    _LIT8( KAMNameNodeName, "Name" );
    _LIT8( KAMNameNodeDescription, "This leaf node holds name of an application" );

    _LIT8( KAMVersionNodeName, "Version" );
    _LIT8( KAMVersionNodeDescription, "This leaf node holds the version of an application" );

    _LIT8( KAMStateValueNodeName, "State" );
    _LIT8( KAMStateValueNodeDescription, "This leaf node holds the state value of an application (active/inactive)" );

    _LIT8( KAMDataNodeName, "Data" );
    _LIT8( KAMDataNodeDescription, "This leaf node holds the data of an application" );

    _LIT8( KAMDescriptorNodeName, "Descriptor" );
    _LIT8( KAMDescriptorNodeDescription, "This leaf node holds the possible metadata of an application" );

    _LIT8( KAMOperationsNodeName, "Operations" );
    _LIT8( KAMOperationsNodeDescription, "This is a node under which operations are found" );

    _LIT8( KAMInstallOptsNodeName, "InstallOpts" );
    _LIT8( KAMInstallOptsNodeDescription, "This is a node that holds the installation options of an application" );

    _LIT8( KAMDeployedDynaNodeDescription, "This dynamic node is placeholder applications that are in Inactive state" );

    _LIT8( KAMDownloadOperationNodeName, "Download" );
    _LIT8( KAMDownloadOperationNodeDescription, "Starts the download" );

    _LIT8( KAMConRefNodeName, "ConRef" );
    _LIT8( KAMConRefNodeDescription, "Holds reference to IAP" );

    _LIT8( KAMLocalOperationsNodeDescription, "This is the root node for local operations" );

    _LIT8( KAMActivateNodeName, "Activate" );
    _LIT8( KAMActivateNodeDescription, "Exec command causes device to activate an inactive application" );

    _LIT8( KAMDeActivateNodeName, "Deactivate" );
    _LIT8( KAMDeActivateNodeDescription, "Exec command causes device to stop and deactivate an active application" );

    _LIT8( KAMInstallNodeName, "Install" );
    _LIT8( KAMInstallNodeDescription, "Exec command causes device to install a delivered application" );

    _LIT8( KAMInstallInactiveNodeName, "InstallInactive" );
    _LIT8( KAMInstallInactivateNodeDescription, "Exec command causes device to install a delivered application and activate it" );

    _LIT8( KAMUpdateNodeName, "Update" );
    _LIT8( KAMUpdateNodeDescription, "Exec command causes device to update an application" );

    _LIT8( KAMUpdateAndActivateNodeName, "UpdateAndActivate" );
    _LIT8( KAMUpdateAndActivateNodeDescription, "Exec command causes device to update an application and activate it" );

    _LIT8( KAMRemoveNodeName, "Remove" );
    _LIT8( KAMRemoveNodeDescription, "Exec command causes device to remove an application" );

    _LIT8( KAMDownloadNodeName, "Download" );
    _LIT8( KAMDownloadNodeDescription, "This node is a root node for application download functionality" );

    _LIT8( KAMDownloadAndInstallNodeName, "DownloadInstall" );
    _LIT8( KAMDownloadAndInstallOperationNodeDescription, "Starts the download and installs component when complete" );

    _LIT8( KAMDownloadAndInstallAndInActivateNodeName, "DownloadInstallInactive" );
    _LIT8( KAMDownloadAndInstallAndInActivateNodeDescription, "Starts the download and installs and activates component when complete" );

    _LIT8( KAMDownloadAndUpdateNodeName, "DownloadAndUpdate" );
    _LIT8( KAMDownloadAndUpdateNodeDescription, "Starts the download and installs component when complete" );

    _LIT8( KAMDownloadAndUpdateAndActivateNodeName, "DownloadAndUpdateAndActivate" );
    _LIT8( KAMDownloadAndUpdateAndActivateNodeDescription, "Starts the download and updates and activates component when complete" );

    _LIT8( KAMDownloadDynaNodeDescription, "This node is a placeholder for identifier of an application that is to be downloaded" );

    _LIT8( KAMURINodeName, "PkgURL" );
    _LIT8( KAMURINodeDescription, "This leaf node holds the URL from which the application should be downloaded" );

    _LIT8( KAMStatusNodeName, "Status" );
    _LIT8( KAMStatusNodeDescription, "This leaf node holds the status of the download" );

    _LIT8( KAMDescriptionNodeName, "Description" );
    _LIT8( KAMDescriptionNodeDescription, "This leaf node holds the description of the download" );
    
    _LIT8( KAMPkgTypeNodeName, "PkgType" );
    _LIT8( KAMPkgTypeNodeDescription, "This leaf node holds the PkgType of the download or delivered" );

#ifdef __AM_LASTERROR_NODE
    _LIT8( KAMExtNodeName, "Ext" );
    _LIT8( KAMExtNodeDescription, "Non std extentions are found under this leaf" );
    _LIT8( KAMLastErrorNodeName, "LastError" );
    _LIT8( KAMLastErrorDescription, "This leaf node may hold the error" );
#endif

    _LIT8( KAMDynamicNodeName, "" );
    _LIT( KAMSeparator16, "/" );

    _LIT8( KAMSeparator8, "/" );

    _LIT8( KAMRootChilds, "Inventory/Download" );
    _LIT8( KAMInventoryChilds, "Delivered/Deployed" );
    //_LIT8( KAMLocalOperationsChilds, "Activate/DeActivate/Install/Update/Remove" );

    _LIT8( KAMDownloadDynaChilds, "PkgID/Name/PkgURL/Description/Status/PkgType/Operations" );
_LIT8( KAMInventoryDynaChilds, "ID/PkgIDRef/Name/Description/Version/State/Operations" );
_LIT8( KAMDownloadOperationChilds, "Download/DownloadInstall/DownloadInstallInactivate" );
_LIT8( KAMDeployedOperationChilds, "Activate/Deactivate/Remove" );
_LIT8( KAMDeliveredOperationChilds, "Install/InstallInactive/Remove" );
_LIT8( KAMInventoryDeliveredDynaChilds, "PkgID/Data/Name/Description/Status/State/PkgType/Operations" );

    _LIT8( KNSmlDMSCOMOMetaType,"org.openmobilealliance.dm.softwarecomponent.OperationComplete" );
    _LIT8( KNSmlDMSCOMOMetaFormat, "text/plain" );
    
    _LIT8(KDataStart, "<![CDATA[");
            _LIT8(KDataEnd, "]]>");
            _LIT8(KResultCodeStart, "<ResultCode>");
            _LIT8(KResultCodeEnd, "</ResultCode>");
            _LIT8(KIdentifierStart, "<Identifier>");
            _LIT8(KIdentifierEnd, "</Identifier>");
                    
            
            _LIT8(KMark, "warning");

    struct TAMCommandBufferElement
        {
        TInt iStatusRef;
        TBuf8<256> iURI;
        TAMCommandBufferElement(TInt aRef, const TDesC8 &aURI) :
            iStatusRef(aRef), iURI(aURI)
            {
            }
        };

    typedef RArray<TAMCommandBufferElement> TAMCommandBuffer;

    // ------------------------------------------------------------------------------------------------
    // CSCOMOAdapter 
    // ------------------------------------------------------------------------------------------------
    class CSCOMOAdapter : public CSmlDmAdapter
        {
public:
        static CSCOMOAdapter* NewL(MSmlDmCallback* aDmCallback);
        static CSCOMOAdapter* NewLC(MSmlDmCallback* aDmCallback);

        virtual ~CSCOMOAdapter();

        /**
         The function returns current version of the DDF.
         By asking current DDF versions from adapters DM Module can control
         possible changes in the data structure and send the changed DDF
         description to a management server.
         This function is always called after DDFStructureL.
         @param aVersion DDF version of the adapter. (filled by the adapter)
         @publishedPartner
         @prototype
         */
        virtual void DDFVersionL(CBufBase& aVersion);

        /**
         The function for filling the DDF structure of the adapter
         This function is only called once, immediately after the adapter is created.
         @param aDDFObject	Reference to root object. A DM adapter starts filling
         the data structure by calling AddChildObjectL to the root object and
         so describes the DDF of the adapter. 
         @publishedPartner
         @prototype
         */
        virtual void DDFStructureL(MSmlDmDDFObject& aDDF);

        /**
         The function creates new leaf objects, or replaces data in existing leaf
         objects. The information about the success of the command should be
         returned by calling SetStatusL function of MSmlDmCallback callback
         interface. This makes it possible to buffer the commands.  However, all
         the status codes for buffered commands must be returned at the latest when
         the adapter's CompleteOutstandingCmdsL() is called.
         @param aURI			URI of the object
         @param aLUID		LUID of the object (if the adapter has earlier returned a
         LUID to the DM Module). For new objects, this is the LUID
         inherited through the parent node.
         @param aObject		Data of the object.
         @param aType		MIME type of the object
         @param aStatusRef	Reference to correct command, i.e. this reference
         must be used when calling the SetStatusL of this command
         @publishedPartner
         @prototype
         */
        virtual void UpdateLeafObjectL(const TDesC8& aURI,
                const TDesC8& aLUID, const TDesC8& aObject,
                const TDesC8& aType, TInt aStatusRef);

        /**
         The function creates new leaf objects, or replaces data in existing leaf
         objects, in the case where data is large enough to be streamed. The
         information about the success of the command should be returned by calling
         SetStatusL function of MSmlDmCallback callback interface. This makes it
         possible to buffer the commands.  However, all the status codes for buffered
         commands must be returned at the latest when the CompleteOutstandingCmdsL()
         of adapter is called.
         @param aURI		URI of the object
         @param aLUID		LUID of the object (if the adapter has earlier returned a
         LUID to the DM Module). For new objects, this is the LUID
         inherited through the parent node.
         @param aStream	Data of the object. Adapter should create write stream
         and return, when data is written to stream by DM agent,
         StreamCommittedL() is called by DM engine
         @param aType		MIME type of the object
         @param aStatusRef	Reference to correct command, i.e. this reference
         must be used when calling the SetStatusL of this
         command.
         @publishedPartner
         @prototype
         */
        virtual void UpdateLeafObjectL(const TDesC8& aURI,
                const TDesC8& aLUID, RWriteStream*& aStream,
                const TDesC8& aType, TInt aStatusRef);

        /**
         The function deletes an object and its child objects. The SetStatusL
         should be used as described in UpdateLeafObjectL()
         @param aURI		URI of the object
         @param aLUID		LUID of the object (if the adapter have earlier returned
         LUID to the DM Module).
         @param aStatusRef	Reference to correct command, i.e. this reference must
         be used when calling the SetStatusL of this command.
         @publishedPartner
         @prototype
         */
        virtual void DeleteObjectL(const TDesC8& aURI, const TDesC8& aLUID,
                TInt aStatusRef);

        /**
         The function fetches data of a leaf object. The SetStatusL should be used
         as described in UpdateLeafObjectL(). The data is returned by using the
         SetResultsL function of MSmlCallback callback interface, and may be streamed.
         @param aURI			URI of the object
         @param aLUID			LUID of the object (if the adapter have earlier
         returned LUID to the DM Module).   
         @param aType 			MIME type of the object
         @param aResultsRef	Reference to correct results, i.e. this reference
         must be used when returning the result by calling
         the SetResultsL.
         @param aStatusRef		Reference to correct command, i.e. this reference
         must be used when calling the SetStatusL of this
         command.
         @publishedPartner
         @prototype
         */
        virtual void FetchLeafObjectL(const TDesC8& aURI,
                const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef,
                TInt aStatusRef);

        /**
         The function fetches the size of the data of a leaf object. The size is
         in bytes, and must reflect the number of bytes that will be transferred
         when the framework calls FetchLeafObjectL. The SetStatusL should be used
         as described in FetchLeafObjectL(). The size value is returned by using
         the SetResultsL function of MSmlCallback callback interface, and must be
         a decimal integer expressed as a string, eg. "1234".
         Results from this call MUST NOT be streamed.
         @param aURI			URI of the object
         @param aLUID			LUID of the object (if the adapter have earlier
         returned LUID to the DM Module).   
         @param aType 			MIME type of the object
         @param aResultsRef	Reference to correct results, i.e. this reference
         must be used when returning the result by calling
         the SetResultsL.
         @param aStatusRef		Reference to correct command, i.e. this reference
         must be used when calling the SetStatusL of this
         command.
         @publishedPartner
         @prototype
         */
        virtual void FetchLeafObjectSizeL(const TDesC8& aURI,
                const TDesC8& aLUID, const TDesC8& aType, TInt aResultsRef,
                TInt aStatusRef);
        /**
         The function fetches URI list. An adapter returns the list of URI segments
         under the given URI be separated by slash ("/"). The URI segment names for
         new objects must be given by the adapter.
         The list is returned by calling the SetResultsL function of MSmlCallback
         callback interface.	Results from this call MUST NOT be streamed.
         @param aParentURI					URI of the parent object
         @param aParentLUID				LUID of the parent object (if the
         adapter have earlier returned LUID to
         the DM Module).   
         @param aPreviousURISegmentList	URI list with mapping LUID information,
         which is known by DM engine. An adapter
         can use this information when verifying
         if old objects still exists. An adapter
         also knows what objects are new to DM
         engine and can provide LUID mapping for
         them. aPreviousURISegmentList parameter
         (see above) helps to recognise new
         objects.
         @param aResultsRef				Reference to correct results, i.e. this
         reference must be used when returning
         the result by calling the SetResultsL.
         @param aStatusRef					Reference to correct command, i.e. this
         reference must be used when calling the
         SetStatusL of this command.
         @publishedPartner
         @prototype
         */
        virtual void ChildURIListL(const TDesC8& aURI, const TDesC8& aLUID,
                const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                TInt aResultsRef, TInt aStatusRef);

        /**
         The function adds node object. In some cases an implementation of the
         function may be empty function, if the node object does not need concrete
         database update. Still this function may be helpful to an adapter, i.e. in
         passing mapping LUID of the node to DM Module. The SetStatusL should be
         used as described in UpdateLeafObjectL()
         @param aURI			URI of the object
         @param aParentLUID	LUID of the parent object (if the adapter have
         earlier returned LUID to the DM Module).   
         @param aStatusRef		Reference to correct command, i.e. this reference
         must be used when calling the SetStatusL of this
         command.
         @publishedPartner
         @prototype
         */
        virtual void AddNodeObjectL(const TDesC8& aURI,
                const TDesC8& aParentLUID, TInt aStatusRef);

        /**
         The function implements execute command. The information about the success
         of the command should be returned by calling SetStatusL function of
         MSmlDmCallback callback interface. This makes it possible to buffer the
         commands.
         However, all the status codes for buffered commands must be returned at
         the latest when the CompleteOutstandingCmdsL() of adapter is called.
         @param aURI			URI of the command
         @param aLUID			LUID of the object (if the adapter have earlier
         returned LUID to the DM Module).   
         @param aArgument		Argument for the command
         @param aType			MIME type of the object 
         @param aStatusRef		Reference to correct command, i.e. this reference
         must be used when calling the SetStatusL of this
         command.
         @publishedPartner
         @prototype
         */
        virtual void
                ExecuteCommandL(const TDesC8& aURI, const TDesC8& aLUID,
                        const TDesC8& aArgument, const TDesC8& aType,
                        TInt aStatusRef);

        /**
         The function implements execute command. The information about the
         success of the command should be returned by calling SetStatusL function
         of MSmlDmCallback callback interface. This makes it possible to buffer the
         commands.
         However, all the status codes for buffered commands must be returned at
         the latest when the CompleteOutstandingCmdsL() of adapter is called.
         @param aURI			URI of the command
         @param aLUID			LUID of the object (if the adapter have earlier
         returned LUID to the DM Module).   
         @param aStream		Argument for the command. Adapter should create
         write stream and return, when data is written to
         stream by DM agent, StreamCommittedL() is called by
         DM engine
         @param aType			MIME type of the object 
         @param aStatusRef		Reference to correct command, i.e. this reference
         must be used when calling the SetStatusL of this
         command.
         @publishedPartner
         @prototype
         */
        virtual void ExecuteCommandL(const TDesC8& aURI, const TDesC8& aLUID,
                RWriteStream*& aStream, const TDesC8& aType, TInt aStatusRef);

        /**
         The function implements copy command. The information about the success of
         the command should be returned by calling SetStatusL function of
         MSmlDmCallback callback interface. This makes it possible to buffer the
         commands.
         However, all the status codes for buffered commands must be returned at
         the latest when the CompleteOutstandingCmdsL() of adapter is called.
         @param aTargetURI		Target URI for the command
         @param aSourceLUID	LUID of the target object (if one exists, and if the adapter
         has	earlier returned a LUID to the DM Module).   
         @param aSourceURI		Source URI for the command
         @param aSourceLUID	LUID of the source object (if the adapter has
         earlier returned a LUID to the DM Module).   
         @param aType			MIME type of the objects
         @param aStatusRef		Reference to correct command, i.e. this reference
         must be used when calling the SetStatusL of this
         command.
         @publishedPartner
         @prototype
         */
        virtual void CopyCommandL(const TDesC8& aTargetURI,
                const TDesC8& aTargetLUID, const TDesC8& aSourceURI,
                const TDesC8& aSourceLUID, const TDesC8& aType,
                TInt aStatusRef);

        /**
         The function indicates start of Atomic command.
         @publishedPartner
         @prototype
         */
        virtual void StartAtomicL();

        /**
         The function indicates successful end of Atomic command. The adapter
         should commit all changes issued between StartAtomicL() and
         CommitAtomicL()
         @publishedPartner
         @prototype
         */
        virtual void CommitAtomicL();

        /**
         The function indicates unsuccessful end of Atomic command. The adapter
         should rollback all changes issued between StartAtomicL() and
         RollbackAtomicL(). If rollback fails for a command, adapter should use
         SetStatusL() to indicate it.
         @publishedPartner
         @prototype
         */
        virtual void RollbackAtomicL();

        /**
         Returns ETrue if adapter supports streaming otherwise EFalse.
         @param aItemSize size limit for stream usage
         @return TBool ETrue for streaming support
         @publishedPartner
         @prototype
         */
        virtual TBool StreamingSupport(TInt& aItemSize);

        /**
         Called when stream returned from UpdateLeafObjectL or ExecuteCommandL has
         been written to and committed. Not called when fetching item.
         @publishedPartner
         @prototype
         */
#ifdef __TARM_SYMBIAN_CONVERGENCY	
        virtual void StreamCommittedL( RWriteStream& aStream );
#else
        virtual void StreamCommittedL();
#endif	
        /**
         The function tells the adapter that all the commands of the message that
         can be passed to the adapter have now been passed.  This indciates that
         the adapter must supply status codes and results to any buffered commands.
         This must be done at latest by the time this function returns.
         This function is used at the end of SyncML messages, and during processing
         of Atomic.   In the case of Atomic processing, the function will be
         followed by a call to CommitAtomicL or RollbackAtomicL.
         @publishedPartner
         @prototype
         */
        virtual void CompleteOutstandingCmdsL();

private:
        CSCOMOAdapter(TAny* aDmCallback);
        void ConstructL();
        TInt DesToIntL(const TDesC8& aLuid);
        HBufC8* IntToDes8LC(const TInt aLuid);

        TBool CheckStateL(const TDeploymentComponent &aComp,
                const TDesC8& aURI);
        TBool CheckStateL(const TDesC8& aURI, const TDesC8& aLUID);
        /**
         * UpdateFlagFromMapping Maps a node name to internal attribute flag that
         * 	is used by server to identify to data 
         * @param aMapping	The SyncML node name
         * @return  TDeplCompAttrType the attribute type
         */
        TDeplCompAttrType UpdateFlagFromMapping(const TDesC8& aMapping);

        /**
         * IdListL Looks up the given state user ids of deployment components,
         * 	and places them to currentList in the format specified by SyncML spec 
         * @param aState	The deployment component state in which we are interested
         * @param aCurrentList The buffer where the results are placed
         * @param aAppend Does the list possibly contain some items already
         * @return  TError EOk if successful
         */
        TError IdListL(TDeploymentComponentState aState,
                CBufBase &aCurrentList, TBool aAppend = EFalse);

        /**
         * DeliverOrDownloadL Depending on aURI, creates new delivery or download component
         *
         * @param aUserId	The name of dynamic node.
         * @param aURI		The full URI
         * @param aCnt		Count of URI segments
         * @param aParentMapping	The parent data
         * @param aStatus A reference to the error which will be set the status of the operation
         * @return  TError EOk if successful
         */
        void DeliverOrDownloadL(const TDesC8 &aUserId, const TDesC8 &aURI,
                TInt aCnt, const TDesC8 &aParentMapping,
                MSmlDmAdapter::TError &aStatus);

        /**
         * Fills the node info in ddf structure
         */
        void
                FillNodeInfoL(MSmlDmDDFObject& aNode,
                        TSmlDmAccessTypes aAccTypes,
                        MSmlDmDDFObject::TOccurence aOccurrence,
                        MSmlDmDDFObject::TScope aScope,
                        MSmlDmDDFObject::TDFFormat aFormat,
                        const TDesC8& aDescription);

        /**
         * Fills the node info in ddf structure, without default mime type
         */
        void
                FillNodeInfoNoDefaultMimeL(MSmlDmDDFObject& aNode,
                        TSmlDmAccessTypes aAccTypes,
                        MSmlDmDDFObject::TOccurence aOccurrence,
                        MSmlDmDDFObject::TScope aScope,
                        MSmlDmDDFObject::TDFFormat aFormat,
                        const TDesC8& aDescription);

        TBool IsImage(const TDesC8& aMime);
        /**
         Calls Callback().SetResultsL()
         */
        void SetResultsL(TInt aResultsRef, CBufBase& aObject,
                const TDesC8& aType);
        /**
         Calls Callback().SetStatusL()
         */
        void SetStatusL(TInt aStatusRef, MSmlDmAdapter::TError aErrorCode);
        //	TInt FindBuffered( const TDesC8 &aURI ) ;
        RApplicationManagement &SessionL();
        TDeploymentComponentState StateL(const TDesC8& aURI);

        void CheckStateChangesL();
        void CheckStateChangesInternalL();

        TPtrC8 URISegsUpTo(const TDesC8& aURI, TInt aUpTo,
                TBool aInclKeno=EFalse);

        /**
         * Helper method to move acl. Copies acl of node aFrom to acl of aTo,
         * and removes acl from aFrom.
         *
         * @param aFrom The source uri
         * @param aTo	The target uri
         */
        void MoveAclL(const TDesC8 &aFrom, const TDesC8 &aTo);

        /**
         * Helper method to move nodes in SCOMO tree
         *
         * 1. Sets mapping of aOriginal to null.
         * 2. If aNewURI is null, constructs new uri depending on aTargetstate.
         * 3. Sets mapping of aNewURI to aLUID
         * 4. Moves acls of aOriginal to aNewURI (using MoveAclL)
         *
         * @param aOriginal The uri of original node to be moved
         * @param aTargetstate The target state the node is moving to
         * @param aLUID The luid of the original node
         * @param aNewUri The new uri of the node (will be constructed and left to stack if null)
         */
        void SetMappingLC(const TDesC8 &aOriginal,
                const TDeploymentComponentState aTargetstate,
                const TDesC8 &aLUID, HBufC8 *&aNewUri);

        /**
         * Helper method to move nodes in SCOMO tree
         *
         * 1. Sets mapping of aOriginal to null.
         * 2. Sets mapping of a uri constructed using aTargetState to aLUID
         * 2. Moves acls of aOriginal to a uri constructed using aTargetState (using MoveAclL)
         *
         * @param aTargetstate The target state the node is moving to
         * @param aLUID The luid of the original node
         */
        void SetMappingL(const TDesC8 &aOriginalURI,
                const TDeploymentComponentState aTargetState,
                const TDesC8 &aLUID);

        /**
         * Helper method to set DM LUID mappings directly database.
         *
         * @param aURI The uri of node whose mapping is to be set
         * @param aLUID The new mapping value
         * @return Possible database error - KErrNone if successful
         */
        TInt DirectSetMappingL(const TDesC8 &aURI, const TDesC8 &aLUID);

	TInt DirectRemoveMappingL(const TDesC8 &aURI);

        /**
         * Helper method to get DM LUID mappings directly from database.
         *
         * @param aURI The uri of node whose mapping is to be found
         * @return The luid mapping - Empty string if not found.
         */
        HBufC8 *DirectGetLuidAllocLC(const TDesC8 &aURI);

        /**
         * Helper method to perform install
         *
         * @param aLuidi The mapping value of component as integer
         * @param aURI The uri of node that is to be installed
         * @param aLUID The mapping value of component as text
         * @param aTargetstate The target state the node is moving to (EDCSActive or EDCSInactive)
         * @param aRet	The SyncML Error code - value will be updated according to success
         */
        void InstallL(const TUint aLuidi, const TDesC8& aURI,
                const TDesC8& aLUID,
                const TDeploymentComponentState aTargetState, TError &aRet);

        /**
         * Helper method to perform update
         *
         * @param aLuidi The mapping value of component as integer
         * @param aURI The uri of node that is to be installed
         * @param aLUID The mapping value of component as text
         * @param aTargetstate The target state the node is moving to (EDCSActive or EDCSInactive)
         * @param aRet	The SyncML Error code - value will be updated according to success
         */
        void UpdateL(const TUint aLuidi, const TDesC8& aURI,
                const TDesC8& aSourceLUID,
                const TDeploymentComponentState aTargetState, TError &aRet);
        /**
         maps symbian error code to DM error code as follows:
         Kerrnone-> EOk
         Kerrnotfound ->Enotfound
         all oether -> EError
         */
        void DefaultMapError(const TInt aErr, TError &aRet,
                const TDesC8& aDes = KNullDesC8);

        /**
         * Deactivates given component if given state is inactive
         */
        void DeactivateIfInactive(const TUint aLuidi,
                const TDeploymentComponentState aTargetState);

        /**
         * Finds luid from app mgmt server to given userid
         */
        TUint32 GetLuidForUserIdL(const TDesC8 &aUserId,
                const TDeploymentComponentState aState);

        TUint32 GetLuidL(const TDesC8 &aDMLuid, const TDesC8 &aUserId,
                const TDeploymentComponentState aState);

        /**
         * places result in current
         * @param aParent The deployment component identifying uri 
         * @param aMapping The user id part of the deployment component uri
         * @param aLuid Internal id of the deployment compoennt
         * @param aCurrentList The list that will get the data
         * @param aStatus The syncml status
         */
        void GetComponentDataL(const TDesC8& aParent, const TDesC8& aMapping,
                const TUint32 aLuid, CBufBase &aCurrentList,
                CBufBase &currentMime, TError &aStatus);
        /**
         * returns luid of leaf node, or 0
         */
        TInt GetLeafLuidL(const TDesC8 &aURI, const TDesC8 & aParentMapping);
        /**
         * returns length of result
         */
        TInt GetSizeL(const TDeplCompAttrType aDataType, const TInt aLuid,
                TError &aRet);

        /**
         * places installoptions of given aLuid to aBuf
         */
        void InstallOptionsDataL(const TUint32 aLuid,
                const TDeplCompAttrType& aDataType, CBufBase &aBuf,
                CBufBase *aRaw = NULL);

        /**
         * Gets the luid using GetLuidL, and updates aStatus to ENotFound if not found...
         */
        TUint32 GetLuid2L(const TDesC8 &aDMLuid, const TDesC8 &aUserId,
                const TDeploymentComponentState aState, TError &aStatus);

        /**
         * Finds out correct download target based on download operation 
         */
        TDownloadTarget DownloadTargetL(const TDesC8& aOperation);

        TPtrC8 LastURISeg(const TDesC8& aURI);
        TInt DesToInt(const TDesC8& aLuid);

#ifdef __AM_LASTERROR_NODE
        void SetLastErrorL(const TDesC8& aLastError, const TInt aErr);
        const TDesC8& LastError();
        void SetErrorL(const TDesC8& aDes, const TInt aErr);
#endif

        /**
         The function creates new leaf objects, or replaces data in existing leaf
         objects. The information about the success of the command should be
         returned by calling SetStatusL function of MSmlDmCallback callback
         interface. This makes it possible to buffer the commands.  However, all
         the status codes for buffered commands must be returned at the latest when
         the adapter's CompleteOutstandingCmdsL() is called.
         @param aURI			URI of the object
         @param aLUID		LUID of the object (if the adapter has earlier returned a
         LUID to the DM Module). For new objects, this is the LUID
         inherited through the parent node.
         @param aObject		Data of the object.
         @param aType		MIME type of the object
         @param aStatusRef	Reference to correct command, i.e. this reference
         must be used when calling the SetStatusL of this command
         @publishedPartner
         @prototype
         */
        void _UpdateLeafObjectL(const TDesC8& aURI, const TDesC8& aLUID,
                const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef,
                MSmlDmAdapter::TError& aStatus);
        /**
         * The function creates new leaf objects, or replaces data in existing leaf
         */
        void CloseStreaming();

        void GetServerInfoL(TSmlProfileId& aProfId, HBufC8*& aServerId) const;

        void SendPendingGenericAlertL();

        void ASyncReportL(TUint32 aLuid, const TDesC8& aArgument,
                const TDownloadTarget aTarget, const TDesC8& aURI);

        void SyncReportL(TUint32 aLuid, const TDesC8& aArgument,
                const TDownloadTarget aTarget, const TDesC8& aURI, TError &aRet);
	/**
	 Checks if predefined node for Download object is in tree if not add to the tree 
	*/
	void CheckAndAddPredefinedNodeL();
	/**
	 Gets  predefined node for Download object from cenrep 
	 */
	void GetPredefinedNodeL(TDes8& aNode);
	TInt GetAdapterValue();
	void SetAdapterValue(TInt aAdapterValue);
	TBool RecognizeMimeType(const TDesC8& aMimeType);

private:

#ifdef __TARM_SYMBIAN_CONVERGENCY
        RDmTree iDbSession;
#else
        RNSmlDMCallbackSession iDbSession; // to change acl location
#endif

        RApplicationManagement iManagement;
        TBool iSessionOpened;
        TAMCommandBuffer iBuffer;
        TBool iInAtomic;
        TBool iUpdated; // have we checked changed state DCs?
        // Streaming API support
        TBool iStreamOpen;
        RFileWriteStream iStream;
        TInt iStatusRef;
        RFs iStreamRFs;
        RFile iStreamFile;
        TBool iIsStreamedContent;
        HBufC8* iStreamedURI;
        HBufC8* iStreamedLuid;
        HBufC8* iStreamedType;
        TBool iCertRequired;
        TBool iTrustAdded;
        CSCOMOAdapterDb* iAMdb;
        TUint iInternalId;
#ifdef __AM_LASTERROR_NODE
        HBufC8 *iLastError;
#endif

        HBufC8 *iUriDel;
        };

    } // namespace
#endif // __AMDMADAPTER_H__
// End of File
