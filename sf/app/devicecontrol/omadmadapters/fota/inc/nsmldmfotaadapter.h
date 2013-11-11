/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    DM Fota Adapter
*
*/




#ifndef __NSMLDMFOTAADAPTER_H__
#define __NSMLDMFOTAADAPTER_H__

//  INCLUDES
#include <e32base.h>
#include <smldmadapter.h>
#include <fotaengine.h>
#include <fotaConst.h>

#include "nsmldmfotaadapterdb.h"


// CONSTANTS

const TInt KNSmlDMFotaResultBufferExpandSize = 64;
const TInt KNSmlDMFotaStreamingTreshold = 4096;
const TInt KNSmlDMFotaNullPkgId = -1;

// final result values
const TInt KNSmlDMFotaUndefinedError = 410;
const TInt KNSmlDMFotaSuccessfullDownload = 202;
const TInt KNSmlDMFotaBadUrl = 502;

// P&S keys defined & owned by FotaServer for OMA DM large object
// sets the OMA DM large object download status
const TUint32 KFotaLrgObjDl = 0x00000005;
// sets the profile id used for OMA DM large object download
const TUint32 KFotaLrgObjProfileId = 0x00000006;
// literals for DDF structure
_LIT8( KNSmlDMFotaNode,                     "FUMO" );
_LIT8( KNSmlDMFotaNodeName,                 "PkgName" );
_LIT8( KNSmlDMFotaNodeVersion,              "PkgVersion" );
_LIT8( KNSmlDMFotaNodeDownload,             "Download" );
_LIT8( KNSmlDMFotaNodeDownloadUrl,          "PkgURL" );
_LIT8( KNSmlDMFotaNodeDownloadAndUpdate,    "DownloadAndUpdate" );
_LIT8( KNSmlDMFotaNodeDownloadAndUpdateUrl, "PkgURL" );
_LIT8( KNSmlDMFotaNodeState,                "State" );

// descriptions for DDF nodes
_LIT8( KNSmlDMFotaNodeDescription,
"Placeholder for all firmware management objects" );

_LIT8( KNSmlDMFotaRunTimeNodeDescription,
"Placeholder for a single firmware management object" );

_LIT8( KNSmlDMFotaNodeNameDescription,
"Name of a firmware update package" );

_LIT8( KNSmlDMFotaNodeVersionDescription,
"Version of a firmware update package" );

_LIT8( KNSmlDMFotaNodeDownloadDescription,
"Execution target for firmware update package download" );

_LIT8( KNSmlDMFotaNodeDownloadUrlDescription,
"Url to a location containing binary firmare update package" );

_LIT8( KNSmlDMFotaNodeUpdateDescription,
"Execution target for installing update package to device" );

_LIT8( KNSmlDMFotaNodeUpdateDataDescription,
"Binary data used in installation" );

_LIT8( KNSmlDMFotaNodeDownloadAndUpdateDescription,
"Execution target for downloading firmware update package and installing it" );

_LIT8( KNSmlDMFotaNodeDownloadAndUpdateUrlDescription,
"Url to a location containing binary firmare update package" );

_LIT8( KNSmlDMFotaNodeStateDescription,
"Current state of firmware update" );

// mime types etc
_LIT8( KNSmlDMFotaRunTimeMimeType,
"org.openmobilealliance/1.0/FirmwareUpdateManagementObject" );

_LIT8( KNSmlDMFotaTextPlain,    "text/plain" );
_LIT8( KNSmlDMFotaDDFVersion,   "1.0" ); 

_LIT8( KNSmlDMFotaUpdateMetaType,
"org.openmobilealliance.dm.firmwareupdate.update" );

_LIT8( KNSmlDMFotaDownloadMetaType,
"org.openmobilealliance.dm.firmwareupdate.download" );

_LIT8( KNSmlDMFotaDownloadAndUpdateMetaType,
"org.openmobilealliance.dm.firmwareupdate.downloadandupdate" );

_LIT8( KNSmlDMFotaMetaFormat, "text/plain" );

_LIT8( KNSmlDMFotaRunTimeChildren,
"PkgName/PkgVersion/Download/DownloadAndUpdate/State" );

// uri related
_LIT8( KNSmlDMFotaSeparatorDes, "/" );
_LIT8( KNSmlDMFotaRuntimeMatch, "*FUMO/*" );
_LIT8( KNSmlDMFotaRootMatch,    "*FUMO" );


/**
*  CNSmlDmFotaAdapter, the main adapter class of Fota. Provides methods 
*  to access and modify FUMO objects.
*  
*  @lib nsmldmfotaadapter.lib
*  
*/
class CNSmlDmFotaAdapter : public CSmlDmAdapter
    {

public:

    /**
    * Two-phased constructor.
    * @param aDmCallback A pointer to DM Callback, which is used to 
    * set statuses and results of commands.
    * @return A pointer to the newly created adapter.
    */
    static CNSmlDmFotaAdapter* NewL( MSmlDmCallback* aDmCallback );
    
    /**
    * Two-phased constructor. Pushes the pointer onto the CleanupStack.
    * @param aDmCallback A pointer to DM Callback, which is used to 
    * set statuses and results of commands.
    * @return A pointer to the newly created adapter.
    */
    static CNSmlDmFotaAdapter* NewLC( MSmlDmCallback* aDmCallback );

    /**
    * Destructor.
    */
    virtual ~CNSmlDmFotaAdapter();
    
public: 
    
    // from CSmlDmAdapter
    
    /**
    * Sets current version of Fota adapter's DDF structure to aDDFVersion.
    * @param aVersion Buffer which on return contains the version.
    */
    void DDFVersionL( CBufBase& aDDFVersion );
    
    /**
    * Fills the DDF structure of firmware management object using the given 
    * reference as the root of DDF. Also checks if there are any Generic 
    * Alerts to be sent to current remote DM server. If there are, delegates 
    * the alerts to SOS Server using Private API.
    * @param aDDFObject Reference to root DDF node.
    */
    void DDFStructureL( MSmlDmDDFObject& aDDF );
    
    /**
    * Updates a leaf object in FUMO. Sets ENotFound as status to DM Framework,
    * if aURI and/or aLUID is not valid.
    * @param aURI Uri which spesifies the leaf to be updated in a firmware 
    * object.
    * @param aLUID Identifier used to identify in which firmware object 
    * the leaf should be updated.
    * @param aObject Data used in the update.
    * @param aType Mime type of the data. Ignored in Fota adapter.
    * @param aStatusRef Identifier that is used when setting the completion
    * status to DM Framework.
    */
    void UpdateLeafObjectL( const TDesC8& aURI,
                            const TDesC8& aLUID, 
                            const TDesC8& aObject, 
                            const TDesC8& aType, 
                            TInt aStatusRef );
    
    /**
    * Deletes a firmware object from Fota DB. If aURI does not point to a 
    * runtime node and/or aLUIDis invalid, ENotFound is set as status for 
    * this command.
    * @param aURI Uri which spesifies a firmware object.
    * @param aLUID Identifier used to identify in which firmware object 
    * should be deleted.
    * @param aStatusRef Identifier that is used when setting the completion 
    * status to DM Framework.
    */
    void DeleteObjectL( const TDesC8& aURI,
                        const TDesC8& aLUID, 
                        const TInt aStatusRef ); 
    
    /**
    * Fetches a leaf object in FUMO. Sets ENotFound as status to DM Framework,
    * if aURI and/or aLUID is not valid.
    * @param aURI Uri which spesifies the leaf to be fetched.
    * @param aLUID Identifier used to identify from which firmware object the 
    * leaf should be fetched.
    * @param aType Mime type that server wishes to be used in the returned data.
    * Ignored in Fota adapter (but used when setting the result).
    * @param aResultRef Identifier that is used when setting the result 
    * (fetched data) to DM Framework.
    * @param aStatusRef Identifier that is used when setting the completion
    * status to DM Framework.
    */
    void FetchLeafObjectL( const TDesC8& aURI, 
                           const TDesC8& aLUID, 
                           const TDesC8& aType, 
                           const TInt aResultsRef, 
                           const TInt aStatusRef ); 
    
    /**
    * Forms a list of children of given node (aURI) and sets the list to 
    * DM Framework as result.
    * @param aURI Uri which spesifies the node whose children should be 
    * listed. In Fota this should point to either to the ./FUMO or ./FUMO/<x>.
    * In the first case aPreviousURISegmentList is trusted and the list is 
    * formed entirely based on it. In the latter case, a hard coded list of 
    * run time node's children is returned.
    * @param aLUID Identifier of aURI. Ignored in Fota adapter.
    * @param aPreviousURISegmentList A List of aURI's children formed by 
    * DM Framework.
    * @param aResultRef Identifier that is used when setting the result 
    * (fetched data) to DM Framework.
    * @param aStatusRef Identifier that is used when setting the completion
    * status to DM Framework.
    */
    void ChildURIListL( const TDesC8& aURI, 
                        const TDesC8& aLUID, 
                        const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
                        const TInt aResultsRef, 
                        const TInt aStatusRef );
    
    /**
    * Adds a firmware object to Fota DB. If aURI does not point to a runtime 
    * node and/or aLUID is invalid, ENotFound is set as status for this 
    * command.
    * @param aURI Uri which spesifies the firmware object.
    * @param aParentLUID Identifier of aURI. If this is a valid ID, then 
    * the object has already been added and EAlreadyExists is set as status 
    * for this command.
    * @param aStatusRef Identifier that is used when setting the completion 
    * status to DM Framework.
    */
    void AddNodeObjectL( const TDesC8& aURI, 
                         const TDesC8& aParentLUID, 
                         const TInt aStatusRef );
    
    /**
    * This method is used to update PkgData, which is the only large object 
    * data in FUMO. Adapter opens a stream to a data (using Fota Engine) 
    * and sets this stream to aStream after which this method returns. DM Host
    * Server then writes the data to the stream piece by piece and finally 
    * calls StreamCommittedL() when all data is written.
    * @param aURI Uri which spesifies the leaf to be updated in a firmware 
    * object. If this does not point to ./FUMO/<x>/Update/PkgData, ENotFound 
    * is set as status for this command.
    * @param aLUID Identifier used to identify in which firmware object the 
    * data should be updated.
    * @param aStream Pointer to a stream, where the opened stream is set.
    * @param aType Mime type of the data. Ignored in Fota adapter.
    * @param aStatusRef Identifier that is used when setting the completion 
    * status to DM Framework.
    */
    void UpdateLeafObjectL( const TDesC8& aURI,
                            const TDesC8& aLUID, 
                            RWriteStream*& aStream, 
                            const TDesC8& aType, 
                            const TInt aStatusRef );
    
    /**
    * Fetches the size of leaf object's data in bytes. Sets ENotFound as 
    * status to DM Framework, if aURI and/or aLUID is not valid.
    * @param aURI Uri which spesifies the leaf whose data is measured.
    * @param aLUID Identifier used to identify from which firmware object 
    * the leaf size should be counted.
    * @param aType Mime type that server wishes to be used in the returned 
    * data. Ignored in Fota adapter (but used when setting the result)
    * @param aResultRef Identifier that is used when setting the result 
    * (fetched data) to DM Framework.
    * @param aStatusRef Identifier that is used when setting the completion 
    * status to DM Framework.
    */
    void FetchLeafObjectSizeL( const TDesC8& aURI,
                               const TDesC8& aLUID, 
                               const TDesC8& aType, 
                               const TInt aResultsRef, 
                               const TInt aStatusRef );
    
    /**
    * Executes command targeting Download, DownloadAndUpdate or Update. In any
    * other case, ENotFound is set as status for this command. Saves all 
    * the data needed when reporting the final result of this command in 
    * the beginning of next DM session, fetches needed data for the command 
    * and delegates the command to Fota Engine.
    * @param aURI Uri which spesifies the target node for the exec command.
    * @param aLUID Identifier used to identify the firmware object where the 
    * target node lies.
    * @param aArgument The argument data of the exec command. In Fota this is 
    * an overloaded feature, and this parameter contains the Correlator of 
    * the exec command.
    * @param aType Mime type of aArgument. Ignored in Fota adapter.
    * @param aStatusRef Identifier that is used when setting the completion 
    * status to DM Framework.
    */
    void ExecuteCommandL( const TDesC8& aURI, 
                          const TDesC8& aLUID,
                          const TDesC8& aArgument, 
                          const TDesC8& aType, 
                          const TInt aStatusRef );
    
    /**
    * Not supported.
    */
    void ExecuteCommandL( const TDesC8& aURI, 
                          const TDesC8& aLUID, 
                          RWriteStream*& aStream, 
                          const TDesC8& aType, 
                          const TInt aStatusRef );
    
    /**
    * Not supported.
    */
    void CopyCommandL( const TDesC8& aTargetURI, 
                       const TDesC8& aTargetLUID, 
                       const TDesC8& aSourceURI, 
                       const TDesC8& aSourceLUID, 
                       const TDesC8& aType, 
                       TInt aStatusRef );
    
    /**
    * Not supported.
    */
    void StartAtomicL();
    
    /**
    * Not supported.
    */
    void CommitAtomicL();
    
    /**
    * Not supported.
    */
    void RollbackAtomicL();
    
    /**
    * Returns whether or not streaming is supported in this adapter. In Fota
    * this method is also used to enquire whether or not a large object 
    * whose size is given in aItemSize fits to disk (OOD check using Fota 
    * Engine), and to acknowledge that Generic Alerts have been successfully 
    * sent. If aItemSize is larger that 0, OOD check feature is executed. If 
    * aItemSize equals KNSmlDMResetGenAlerts, Generic Alerts are marked sent.
    * Otherwise aItemSize is set to a treshold value, which is used by DM 
    * Host Server to determine if streaming should be used (large object) 
    * or not.
    * @param aItemSize If this parameter is larger than zero in the beginning,
    * on return it equals KErrNone if the data fits to disk, and KErrNoMemory 
    * if not. If in the beginning this param equals KNSmlDMResetGenAlerts, 
    * Generic Alerts are marked sent. Otherwise if in the beginning this param
    * is less or equal to zero, on return it equals to the treshold value 
    * mentioned above.
    * @return Whether or not streaming is supported in this adapter 
    * (ETrue always).
    */
    TBool StreamingSupport( TInt& aItemSize );
    
    /**
    * DM Host Server notifies Fota adapter using this method, when all data 
    * has been written to the stream opened for streaming in UpdateLeafObjectL 
    * targeting PkgData. Fota adapter forwards this notification to Fota 
    * Engine.
    */
    void StreamCommittedL();
    
    /**
    * Not supported.
    */
    void CompleteOutstandingCmdsL();
    
private:

    /**
    * Check if fota table exists
    * @param aLUID    ID of the node
    * @return  whether table exists
    */
    TBool TableExistsL(const TDesC8& aLUID) const;

    /**
    * Default constructor.
    * @param aEcomArguments A pointer to MSmlDmCallback which is received 
    * through ECom. The pointer is passed on to base class.
    */
    CNSmlDmFotaAdapter( TAny* aEcomArguments );
    
    /**
    * Second phase construction.
    */
    void ConstructL();
    
    /**
    * Fetches the data in firmware object identified by aLUID (object) and 
    * aURI (leaf).
    * @param aURI Identifies the leaf whose data should be fetched.
    * @param aLUID Identifies the FW object in which the leaf object is.
    * @param aObject Reference to CBufBase to which the fetched data is 
    * written.
    * @return Status code according to the success of the fetch.
    */
    CSmlDmAdapter::TError DoFetchObjectL( const TDesC8& aURI, 
                                          const TDesC8& aLUID, 
                                          CBufBase& aObject );
    
    /**
    * Gets data needed for Update execution from Fota DB and notifies 
    * Fota Engine. Note: this method does not wait for Fota Engine to 
    * execute the command, but immediately returns when Fota Engine has been 
    * notified.
    * @param aLUID The id of the firmware object to which this exec is 
    * targeted.
    * @param aProfile The profile id of the currently running DM session.
    * @return Status code according to the success of the method.
    */
    CSmlDmAdapter::TError DoExecUpdateL( const TNSmlDmFwObjectId aLUID, 
                                         const TSmlProfileId aProfile );
    
    /**
    * Gets data needed for Download or DownloadAndUpdate execution from 
    * Fota DB and notifies Fota Engine. Note: this method does not wait 
    * for Fota Engine to execute the command, but immediately returns when 
    * Fota Engine has been notified.
    * @param aLUID The id of the firmware object to which this exec is 
    * targeted.
    * @param aProfile The profile id of the currently running DM session.
    * @param aUpdate If set ETrue, executes DownloadAndUpdate. Otherwise 
    * executes Download.
    * @return Status code according to the success of the method.
    */
    CSmlDmAdapter::TError DoExecDownloadL( const TNSmlDmFwObjectId aLUID, 
                                           const TSmlProfileId aProfile,
                                           TBool aUpdate );
    
    /**
    * Saves all the data needed to save before any exec command and returns 
    * the id of the profile of currently running DM session.
    * @param aURI Management uri, which is the target of the exec command.
    * @param aLUID Identifies the firm3ware object that is the target of the 
    * exec command.
    * @param aCorrelator Correlator received as an argument in the exec 
    * command.
    * @return Profile id of the currently running DM session.
    */
    TSmlProfileId SaveExecInfoL( const TDesC8& aURI, 
                                 const TNSmlDmFwObjectId aLUID,
                                 const TDesC8& aCorrelator );
    
    /**
    * Fetches profile id and server id of the currently running DM session 
    * using Client API.
    * @param aProfId Reference which on succesful return contains the 
    * profile id.
    * @param aServerId Reference which on successful return contains 
    * the server id.
    */
    void GetServerInfoL( TSmlProfileId& aProfId, HBufC8*& aServerId ) const;
    
    /**
    * Checks if there are any firmware objects that have empty final results. 
    * If there are, checks if any their final result should be reported to 
    * currently running DM session's remote server using Generic Alert. 
    * I.e. if the remote server is the same as with any of those firmware 
    * object's whose execution's final result has not been reported yet, 
    * uses Private API to generate Generic Alert about them.
    */
    void MakeGenericAlertsL();
    
    /***
    * Checks existance of predefined node under FUMO and adds it to 
    * DM Tree 
    **/
    
    void CheckAndAddPredefinedNodeL();
    /***
    * Gets predefined node name to be created under FUMO from cenrep 
    * 
    **/
    
    void GetPredefinedNodeL(TDes8& aNode);
    /**
    * Sets final result to all those FW objects that are associated with 
    * current DM session's remote server, have been a target to an exec 
    * command and that exec command has been finished. I.e. Generic Alert 
    * has been successfully sent reporting these final results, and is not 
    * needed to be sent anymore in next DM session.
    */
    void MarkGenericAlertsSentL();
    
    /**
    * Returns correct meta/type acoording to the execution target aURI.
    * @param aURI The target for exec command, e.g. ./FUMO/<X>/Update
    * @return The meta/type
    */
    TPtrC8 GetMetaType( const TDesC8& aURI ) const;
    
    /**
    * Maps a system wide error code to a TError.
    * @param aError A system wide error code.
    * @return A TError value depending on aError.
    */
    CSmlDmAdapter::TError MapErrorToStatus( TInt aError ) const;
    
    /**
    * Parses a numeric value from aLUID.
    * @param aLUID A string representation of a signed integer.
    * @return TInt value.
    */
    TInt DesToInt( const TDesC8& aLUID ) const;
    
    /**
    * Return the last uri segment of the given uri. E.g. in 
    * ./FUMO/<x>/Download last uri segment is "Download"
    * @param aURI the uri to be parsed.
    * @return The last segment.
    */
    TPtrC8 LastURISeg( const TDesC8& aURI ) const;
    
    /**
    * Fills the given information to a DDF Object node.
    * @param aNode The node whose data is filled.
    * @param aAccTypes The access types of the node.
    * @param aOccurance Occurance of the node.
    * @param aScope The scope of the node.
    * @param aFormat The format of the node's data, i.e. node/chr/bin/...
    * @param aDescription Informal description of the node.
    */
    void FillNodeInfoL( MSmlDmDDFObject& aNode, 
                        const TSmlDmAccessTypes& aAccTypes, 
                        MSmlDmDDFObject::TOccurence aOccurrence, 
                        MSmlDmDDFObject::TScope aScope, 
                        MSmlDmDDFObject::TDFFormat aFormat,
                        const TDesC8& aDescription ) const;
    
    RFotaEngineSession& FotaEngineL();

private:

    CNSmlDmFotaAdapterDb* iFotaDb;
    RFotaEngineSession iFotaEngine;
    
    TInt iPkgId;
    
    };

#endif // __NSMLDMFOTAADAPTER_H__
