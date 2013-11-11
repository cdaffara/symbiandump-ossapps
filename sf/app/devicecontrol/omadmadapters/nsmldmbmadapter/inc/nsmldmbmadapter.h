/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Device management adapter handling bearer management related settings
*
*/




#ifndef NSMLDMBMADAPTER_H
#define NSMLDMBMADAPTER_H

#include <smldmadapter.h>

#include "nsmldmbmsettingstore.h"

class CSmlDmBmCommandElement;
class CSmlDmBmSettingsElement;
class CNSmlDmBmSettingStore;

/**  The DDF version must be changed if any changes in DDF structure 
( built in DDFStructureL() function ) */
_LIT8( KNSmlDmBMAdapterDDFVersion, "1.1" ); 
_LIT8( KNSmlDmBMAdapterTextPlain, "text/plain" );

/** Names of the nodes */
/** When updating KNSmlDmBMAdapterBMURI also update 
KNSmlDmBmAdapterURIMaxLength! */
_LIT8(KNSmlDmBMAdapterBMURI, "./BearerManagementSNAP");
_LIT8(KNSmlDmBMAdapterBM, "BearerManagementSNAP");
_LIT8(KNSmlDmBMAdapterName, "Name");
_LIT8(KNSmlDmBMAdapterIAPPriorityList, "IAPPriorityList");
_LIT8(KNSmlDmBMAdapterEmbeddedSNAP, "EmbeddedSNAP");
_LIT8(KNSmlDmBMAdapterMetadata, "Metadata");
_LIT8(KNSmlDmBMAdapterProtected, "Protected");
_LIT8(KNSmlDmBMAdapterHidden, "Hidden");

/** Descriptions of the nodes */
_LIT8( KNSmlDmBMAdapterBMDescription, 
       "BearerManagement provides management of Access Point groups (SNAPs)");
_LIT8( KNSmlDmBMAdapterDynamicDescription, 
       "Node presents a group of Access Points (SNAP)");
_LIT8( KNSmlDmBMAdapterNameDescription, 
       "Name of the Access Point group");
_LIT8( KNSmlDmBMAdapterIAPPriorityListDescription, 
       "Comma separated list of Access Point URIs in priority order.");
_LIT8( KNSmlDmBMAdapterEmbeddedSNAPDescription, 
       "URI of embedded Access Point group. All Access points of the Embedded \
       SNAP are seen as also belonging to this SNAP");
_LIT8( KNSmlDmBMAdapterMetadataDescription, 
       "32 bit integer representing SNAP metadata. Following bitmasks indicate \
       meaning of the individual bits: \n 0x00000001 \t Internet: The SNAP is \
       the Internet SNAP. This may only be set to one SNAP object.\n \
       0x00000002\tHighlight: SNAP is highlighted in certain UI dialog.\n \
       0x00000004\tHidden: SNAP is hidden in certain UI dialog.");
_LIT8( KNSmlDmBMAdapterProtectedDescription, 
       "SNAP is protected with one of the following protection levels. \
       Protected object can not be modified without NetworkControl capability \n \
       0 \t No protection \n 1 \t SNAP and related objects \n \
       2 \t SNAP name is protected.");
_LIT8( KNSmlDmBMAdapterHiddenDescription, 
       "SNAP is set as hidden in the setting store");

/** Leaf nodes of snap node */
_LIT8( KNSmlDmBMAllLeafNodes, 
       "Name/IAPPriorityList/Metadata/Protected/Hidden/EmbeddedSNAP");
_LIT8( KNSmlDmBMAllLeafNodesNoEmbedded, 
       "Name/IAPPriorityList/Metadata/Protected/Hidden");

/** URI of for fetching IAP nodes */
/** When updating KNSmlDmBMAdapterIAPURI also update 
KNSmlDmBmAdapterAPURIMaxLength! */
_LIT8( KNSmlDmBMAdapterIAPURI, "./AP" );
/** Node name of AP adapter root */
_LIT8( KNSmlDmBMAdapterIAPNode, "AP" );
/** URI segment separator */
/** When updating this literal also update 
KNSmlDmBmAdapterAPURIMaxLength and KNSmlDmBmAdapterURIMaxLength! */
_LIT8( KNSmlDmBmSeparator, "/" );
/** URI segment separator */
/** When updating KNSmlDmBMAdapterBMURI also update 
KNSmlDmBmAdapterAPURIMaxLength and KNSmlDmBmAdapterURIMaxLength! */
_LIT8( KNSmlDmBmUriListSeparator, "," );
/** Prefix in URIs (removed for LUID mapping) */
_LIT8( KNSmlDmBmAdapterURIPrefix, "./" );

/** Name prefix for unnamed snap nodes */
_LIT8( KNSmlDmBMSNAPNamePrefix, "SNAP" );
_LIT8( KNSmlDmBMBooleanTrue, "True" );
_LIT8( KNSmlDmBMBooleanFalse, "False" );


const TInt KNSmlDmBMGranularity = 4;
const TInt KNsmlDmBmSNAPNodeDepth = 2;
const TInt KNSmlDmBmInvalidRef = -1;

/** Maximum length of snap URI including a separator character in URI List.
Node is not calculated here. KNSmlDmBMAdapterBMURI + KNSmlDmBmSeparator + 
KNSmlDmBmUriListSeparator
*/
const TInt KNSmlDmBmAdapterURIMaxLength =  24;

/** Maximum length of AP URI including a separator character in URI List.
Node is not calculated here. KNSmlDmBMAdapterIAPURI + KNSmlDmBmSeparator + 
KNSmlDmBmUriListSeparator
*/
const TInt KNSmlDmBmAdapterAPURIMaxLength = 6;

/** Maximum length of 32bit integer */
const TInt KMaxLengthOf32bitInteger = 10;

/**
 *  Bearer Management device management adapter
 *
 *  Bearer Management device management adapter manages settings
 *  related to Service Network Access Points (SNAPs) which are
 *  used for groupin IAPs
 *
 *  @lib nsmldmbmadapter
 *  @since S60 v3.2
 */
class CNSmlDmBmAdapter : public CSmlDmAdapter
    {
    
public:

/** Possible command types */
enum TCommandType
    {
    EAddCmd,
    EGetCmd,
    EGetSizeCmd,
    EDeleteCmd
    };

    /**
     * Two-phased constructor.
     */
    static CNSmlDmBmAdapter* NewL( MSmlDmCallback* aDmCallback );

    /**
     * Destructor.
     */
    virtual ~CNSmlDmBmAdapter();


// from base class CSmlDmAdapter

    /**
     *  The function returns current version of the DDF. 
     *
     *  @since S60 v3.2
     *  @param aDDFVersion DDF version of the
     *  adapter. (filled by the adapter) 
     */
    void DDFVersionL( CBufBase& aDDFVersion );

    /**
     *  The function for filling the DDF structure of the adapter
     *
     *  @since S60 v3.2
     *  @param aDDF Reference to root object.
    */
    void DDFStructureL( MSmlDmDDFObject& aDDF );

    /**
     * The function creates new leaf objects, or replaces data in existing 
     * leaf objects. The information about the success of the command is
     * returned by calling SetStatusL function of MSmlDmCallback callback
     * interface. 
     *     
     * @since S60 v3.2
     * @param aURI         URI of the object
     * @param aLUID        LUID of the object 
     * @param aObject      Data of the object.
     * @param aType        MIME type of the object
     * @param aStatusRef   Reference to correct command, i.e. this reference
     *                     must be used when calling the SetStatusL of this 
     *                     command
    */
    void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID,
                            const TDesC8& aObject, const TDesC8& aType,
                            TInt aStatusRef );
    /**
     * The function deletes an object and its child objects. 
     *
     * @since S60 v3.2
     * @param aURI         URI of the object
     * @param aLUID        LUID of the object (if the adapter have earlier 
     *                     returned LUID to the DM Module).
     * @param aStatusRef   Reference to correct command, i.e. this reference 
     *                     must be used when calling the SetStatusL of this 
     *                     command.
    */
    void DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID,
                        TInt aStatusRef );

    /**
     * The function fetches data of a leaf object. The SetStatusL is used
     * as described in UpdateLeafObjectL(). The data is returned by using the
     * SetResultsL function of MSmlCallback callback interface.
     *
     * @since S60 v3.2
     * @param aURI             URI of the object
     * @param aLUID            LUID of the object (if the adapter have 
     *                         earlier returned LUID to the DM Module). 
     * @param aType            MIME type of the object
     * @param aResultsRef      Reference to correct results, i.e. this 
     *                         reference must be used when returning the 
     *                         result by calling the SetResultsL.
     * @param aStatusRef       Reference to correct command, i.e. this 
     *                         reference must be used when calling the 
     *                         SetStatusL of this command.
    */
    void FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID,
                           const TDesC8& aType, TInt aResultsRef,
                           TInt aStatusRef );

    /**
     * The function fetches the size of the data of a leaf object. The size 
     * is in bytes, and must reflect the number of bytes that will be 
     * transferred when the framework calls FetchLeafObjectL. 
     *
     * @since S60 v3.2
     * @param aURI             URI of the object
     * @param aLUID            LUID of the object (if the adapter have 
     *                         earlier returned LUID to the DM Module).   
     * @param aType            MIME type of the object
     * @param aResultsRef      Reference to correct results, i.e. this 
     *                         reference must be used when returning the 
     *                         result by calling the SetResultsL.
     * @param aStatusRef       Reference to correct command, i.e. this 
     *                         reference must be used when calling the 
     *                         SetStatusL of this command.
    */
    void FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID,
                               const TDesC8& aType, TInt aResultsRef,
                               TInt aStatusRef );

    /**
     * The function fetches URI list. An adapter returns the list of URI 
     * segments under the given URI be separated by slash ("/"). The URI 
     * segment names for new objects must be given by the adapter.
     * The list is returned by calling the SetResultsL function of 
     * MSmlCallback callback interface. 
     *
     * @since S60 v3.2
     * @param aURI                     URI of the parent object
     * @param aLUID                    LUID of the parent object (if the
     *                                 adapter have earlier returned LUID to
     *                                 the DM Module).   
     * @param aPreviousURISegmentList  URI list with mapping LUID 
     *                                 information, which is known by DM
     *                                 engine.  
     * @param aResultsRef              Reference to correct results, i.e. 
     *                                 this reference must be used when 
     *                                 returning the result by calling the 
     *                                 SetResultsL.
     * @param aStatusRef               Reference to correct command, i.e. 
     *                                 this reference must be used when 
     *                                 calling the SetStatusL of this 
     *                                 command.
    */
    void ChildURIListL( const TDesC8& aURI, const TDesC8& aLUID,
                        const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
                        TInt aResultsRef, TInt aStatusRef );

    /**
     * The function adds node object. 
     *
     * @since S60 v3.2
     * @param aURI             URI of the object
     * @param aParentLUID      LUID of the parent object (if the adapter have
     *                         earlier returned LUID to the DM Module).   
     * @param aStatusRef       Reference to correct command, i.e. this 
     *                         reference must be used when calling the 
     *                         SetStatusL of this command.
    */
    void AddNodeObjectL( const TDesC8& aURI, const TDesC8& aParentLUID,
                         TInt aStatusRef );
    /**
     * The adapter does not support streaming and no implementation is 
     * provided for this function.
     *
     * @since S60 v3.2
     * @param aURI         URI of the object
     * @param aLUID        LUID of the object
     * @param aStream      Data of the object. 
     * @param aType        MIME type of the object
     * @param aStatusRef   Reference to correct command, i.e. this reference
     *                     must be used when calling the SetStatusL of this
     *                     command.
    */
    void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID,
                            RWriteStream*& aStream, const TDesC8& aType,
                            TInt aStatusRef );
    /**
     * The adapter does not support execute command and does not 
     * provide implementation for this function. 
     *
     *  @since S60 v3.2
     * @param aURI             URI of the command
     * @param aLUID            LUID of the object
     * @param aArgument        Argument for the command
     * @param aType            MIME type of the object 
     * @param aStatusRef       Reference to correct command, i.e. this 
     *                         reference must be used when calling the 
     *                         SetStatusL of this command.
    */
    void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, 
                          const TDesC8& aArgument, const TDesC8& aType, 
                          TInt aStatusRef );
    /**
     * The adapter does not support execute command and does not 
     * provide implementation for this function. 
     *
     * @since S60 v3.2
     * @param aURI         URI of the command
     * @param aLUID            LUID of the object 
     * @param aStream      Argument for the command. 
     * @param aType            MIME type of the object 
     * @param aStatusRef       Reference to correct command, i.e. this 
     *                         reference must be used when calling the 
     *                         SetStatusL of this command.
    */
    void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID,
                          RWriteStream*& aStream, const TDesC8& aType,
                          TInt aStatusRef );
    /**
     * The adapter does not support copy command and does not 
     * provide implementation for this function. 
     *
     * @since S60 v3.2
     * @param aTargetURI       Target URI for the command
     * @param aTargetLUID      LUID of the target object 
     * @param aSourceURI       Source URI for the command
     * @param aSourceLUID      LUID of the source object 
     * @param aType            MIME type of the objects
     * @param aStatusRef       Reference to correct command, i.e. this 
     *                         reference must be used when calling the 
     *                         SetStatusL of this command.
    */
    void CopyCommandL( const TDesC8& aTargetURI, const TDesC8& aTargetLUID, 
                       const TDesC8& aSourceURI, const TDesC8& aSourceLUID, 
                       const TDesC8& aType, TInt aStatusRef );
    /**
     * Not supported
     *  @since S60 v3.2
    */
    void StartAtomicL();
    /**
     * Not Supported     
     *  @since S60 v3.2
     *
    */
    void CommitAtomicL();
    /**
     * Not supported.
     *  @since S60 v3.2
     */
    void RollbackAtomicL();
    /**
     * Returns EFalse as the adapter does not support streaming
     *
     * @since S60 v3.2
     * @param aItemSize size limit for stream usage
     * @return TBool EFalse as streaming is not supported
    */
    TBool StreamingSupport( TInt& aItemSize );
    /**
     * Not supported
     *
     *  @since S60 v3.2
    */  
    void StreamCommittedL();
    /**
     * The function tells the adapter that all the commands of the message 
     * that can be passed to the adapter have now been passed.  This 
     * indciates that the adapter must supply status codes and results to 
     * any buffered commands. This must be done at latest by the time this 
     * function returns. This function is used at the end of SyncML messages, 
     * and during processing of Atomic.  
     *
     *  @since S60 v3.2
    */
    void CompleteOutstandingCmdsL();
    
    /**
     * Function checks if a SNAP with the argument URI is buffered for 
     * later handling. If so, the function calls iSettingStore object 
     * to store the SNAP. This function is called by the iSettingStore 
     * object to check whether EmbeddedSNAP node referenced by a command is 
     * still in the buffer and needs to be stored first.
     *
     * @since S60 v3.2
     * @param aURI Descriptor containing the SNAP URI
     */    
    void StoreSnapIfBufferedL( const TDesC8& aURI );


    /**
     * Function returns a pointer to a HBufC object containing  a SNAP URI 
     * which correcponds to the argument LUID.
     *
     * @since S60 v3.2
     * @param aSnapId LUID of SNAP object
     * @return Descriptor containing the SNAP URI. 
     */    
    HBufC8* GetSnapUriFromLuidL( const TUint aSnapId );


    /**
     * Function returns a pointer to HBufC object containing a 
     * comma separated list of IAP URIs according to the IAP LUIDs
     * contained in the argument array.
     *
     * @since S60 v3.2
     * @param aIdArray Array of IAP LUIDs for which the URI list is formed.
     * @return HBufC object containing a comma separated list of IAP URIs
     */
    HBufC8* GetIapListL( const RArray<TUint>& aIdArray );


    /**
     * Function returns an array containing the LUIDs of IAP URIs listed in 
     * argument descriptor. 
     *
     *  @since S60 v3.2
     * @param aIAPList TDesC object containing a comma separated list of IAP
     *  URIs
     * @return Array of IAP LUIDs that correspond to the argument URI list. 
     */    
    RArray<TUint> GetIdArrayL( const TDesC8& aIAPList);

    
    /**
     * Converts integer to 8bit descriptor
     *
     *  @since S60 v3.2
     * @param    aLuid The integer to be converted
     * @return    The Integer as a descriptor
     */
    HBufC8* IntToDes8L( TInt aLuid) const;

    /**
     * Converts 8bit descriptor to integer
     *
     *  @since S60 v3.2
     * @param    aLuid The descriptor to be converted
     * @return    Integer value of the descriptor
     */
    TUint DesToIntL(const TDesC8& aLuid) const;

    /**
     * Converts 8bit descriptor to a boolean value
     * @since S60 v3.2
     * @param aBoolDes The descriptor to be converted
     * @param aBool A pointer to a boolean where result is stored
     * @return ETrue if conversion was succesful, otherwise EFalse
     */
    TBool DesToBool(const TDesC8& aBoolDes, TBool& aBool ) const;

    /**
     * Converts boolean value to 8bit descriptor
     * @since S60 v3.2
     * @param    aBool The boolean value to be converted
     * @return    The boolean value as a descriptor
     */
    HBufC8* BoolToDes8LC( TBool aBool) const;


    /**
     * The function calls the GetLuidAllocL function of the MSmlDmCallback to 
     * get the LUID which is mapped to aURI. This function LUID is not found,  
     * the function allocates a null length string, i.e. the function
     * allocates memory in every case.
     * @since S60 v3.2
     * @param aURI  URI of the object. 
     */
    HBufC8* GetLuidAllocL( const TDesC8& aURI );

private:

     /**
     * Constructor
     */
    CNSmlDmBmAdapter();

    /**
     * Constructor
     * @param aDmCallback Callback object to the framework
     */
    CNSmlDmBmAdapter( MSmlDmCallback* aDmCallback );

    /**
     * Second phase constructor
     */
    void ConstructL();
    
    
    /**
     * Searches the buffer for a SNAP with the parameter mapping name 
     * @param aMappingName Mapping Name of this SNAP
     * @return Pointer to the CSmlDmBmSettingsElement object in buffer. 
     * The object is owned by the CNSmlDmBmAdapter.  
     */
     CSmlDmBmSettingsElement* GetSnapIfBuffered( const TDesC8& aMappingName);
     
    /**
     * Returns the first URI from a list of URIs
     * @param  aURIList Comma separated list of URIs
     * @return    The first URI
     */
     TPtrC8 FirstURI(const TDesC8& aURIList) const;
    
    /**
     * Returns the number of URIs in a list of URIs
     * @param aURIList Comma-separated list of URIs
     * @return Number of URIs in the list.  
     */
    TInt NumOfURIs(const TDesC8& aURIList) const;


    /**
     * Removes the first URI from a list of URIs
     * @param  aURIList Comma separated list of URIs
     * @return    The first URI
     */
     TPtrC8 RemoveFirstURI(const TDesC8& aURI) const;

    /**
     * Parses the last URI segment from URI
     * @param    aURI The whole URI
     * @return    The last URI segment
     */
    TPtrC8 LastURISeg(const TDesC8& aURI) const; 

    /**
     * Removes the last URI segment from URI
     * @param    aURI The whole URI
     * @return    The aURI without the last URI segment
     */
    TPtrC8 RemoveLastURISeg(const TDesC8& aURI) const;

    /**
     * Calculates the number of URI segments
     * @param    aURI The whole URI
     * @return    The number of URI segments
     */
    TInt NumOfURISegs(const TDesC8& aURI) const;

    /**
     * Checks if the URI refers to predefined SNAP (of format SNAPx) in which
     * case operations should be successful even if passed LUID is empty.
     * Maps the URI to found id.
     * @param    aURI The whole URI
     * @param    aLUID LUID
     * @return    The SNAP ID if found
     */
    TInt MapPredefinedSnapsL( const TDesC8& aURI, const TDesC8& aLUID );
    
    /**
     * Tries to find predefined URI string (SNAPx) fronm input
     * returns the ID.
     * @param    aURI The whole URI    
     * @return    The SNAP ID if found
     */
    TInt GetPredefinedSnapIdFromUri( const TDesC8& aURI ) const;

private: //data

    /**
     * Buffered commands. Contains a CSmlDmBmSettingsElement object 
     * for each SNAP. THese in turn contain commands for the SNAP.
     */
    RPointerArray<CSmlDmBmSettingsElement> iBuffer;
    
    /**
     * Setting store object, which is called for managing settings 
     * in CommsDat. Own.
     */
     CNSmlDmBmSettingStore * iSettingStore;
     
     /**
      * CmManager object for SNAP existence checking for
      * direct accesses to pre-defined SNAPs    
      */
     RCmManagerExt iCmManagerExt;
        
    };


/**
 *  CSmlDmBmCommandElement 
 *
 * Helper class, which stores a single buffered command for a SNAP. 
 *  @lib nsmldmbmadapter
 *  @since S60 v3.2
 */

class CSmlDmBmCommandElement : public CBase
    {

public:

    /**
     * Two-phased constructor.
     */
    static CSmlDmBmCommandElement* NewLC( TBool aLeaf, 
                                          TInt aStatusRef, 
                                          TInt aResultRef, 
                                          CNSmlDmBmAdapter::TCommandType  aCmdType, 
                                          const TDesC8& aLastUriSeg, 
                                          const TDesC8& aData );

    /**
     * Destructor.
     */
     ~CSmlDmBmCommandElement();

    /**
     * Returns the iExecuted member value of the object
     *
     * @since S60 v3.2
     * @return The iExecuted member value of the object
     */
    inline TBool Executed();

    /**
     * Sets the iExecuted member value of the object
     *
     * @since S60 v3.2
     * @param aExecuted Executed value for the object. 
     */    
    inline void SetExecuted( TBool aExecuted );
    
    /**
     * Returns the iStatus member value of the object
     *
     * @since S60 v3.2
     * @return The iStatus value of the object
     */
    inline CSmlDmAdapter::TError Status();
    
    /**
     * Sets the iStatus member value of the object
     *
     * @since S60 v3.2
     * @param aStatus Status value for the object. 
     */    
    inline void SetStatus( CSmlDmAdapter::TError aStatus ); 
    
    /**
     * Returns the iLeaf member value of the object
     *
     * @since S60 v3.2
     * @return The iLeaf member value of the object
     */
    inline TBool Leaf();
    
    /**
     * Returns the iStatusRef member value of the object
     *
     * @since S60 v3.2
     * @return The iStatusRef member value of the object
     */
    inline TInt StatusRef();
    
    /**
     * Returns the iResultRef member value of the object
     *
     * @since S60 v3.2
     * @return The iResultRef member value of the object
     */
    inline TInt ResultRef();
    
    /**
     * Returns the iCmdType member value of the object
     *
     * @since S60 v3.2
     * @return The iCmdType member value of the object
     */
    inline CNSmlDmBmAdapter::TCommandType  CmdType();
    
    /**
     * Returns the iData member value of the object
     *
     * @since S60 v3.2
     * @return The iData member value of the object
     */
    inline const HBufC8* Data();
    
    /**
     * Returns the iLastUriSeg member value of the object
     *
     * @since S60 v3.2
     * @return The iLastUriSeg member value of the object
     */
    inline const HBufC8* LastUriSeg();
    
    /**
     * Sets the iData member value of the object
     *
     * @since S60 v3.2
     * @param aData Data set to the object. The data will be owned by 
     * the command object. 
     */   
    inline void SetDataL( const TDesC8& aData );

private:

    CSmlDmBmCommandElement( TBool aLeaf, 
                            TInt aStatusRef, 
                            TInt aResultRef, 
                            CNSmlDmBmAdapter::TCommandType aCmdType );

    void ConstructL( const TDesC8& aLastUriSeg, const TDesC8& aData );

    
private: //data

    /**
     * Has command been executed. 
     * Set to ETrue when command is executed.
     */
    TBool iExecuted;
    
    /**
     * The execution status of an exeuted command. 
     * Filled in when command is executed.
     */    
    CSmlDmAdapter::TError iStatus;
    
    
    /**
     * True if commend is for a leaf node, False if it is for a Snap node.
     */    
    const TBool iLeaf;
    
    /**
     * Reference for returning the status to DM framework.
     */    
    const TInt iStatusRef;
    
    /**
     * Reference for returning result of Get command to the DM framework.
     */    
    const TInt iResultRef;
    
    /**
     * Type of command.
     */    
    const CNSmlDmBmAdapter::TCommandType  iCmdType;
    

    /**
     * Data which is either ment to be stored to setting store 
     * or which has been fetched from there. 
     */
    HBufC8* iData;

    /**
     * Last segment in the command URI, which indicates the leaf node 
     * in question. For non leaf command empty string. 
     */    
    HBufC8* iLastUriSeg;

    };


/**
 *  CSmlDmBmSettingsElement
 *
 * Helper class which stores information of a single SNAP object 
 * into buffer. Contains the buffered commands for the 
 * SNAP as a list of CSmlBmCommandElement objects.
 *
 *  @lib nsmldmbmadapter
 *  @since S60 v3.2
 */

class CSmlDmBmSettingsElement : public CBase
    {
 
public:

    /**
     * Two-phased constructor.
     */
     static CSmlDmBmSettingsElement* NewLC( const TDesC8& aMappingName );
    
    /**
     * Destructor.
     */
    ~CSmlDmBmSettingsElement();
     
    /**
     * Returns a reference to iNodeBuf member.  
     * @since S60 v3.2
     * @return Reference to iNodeBuf member of the object
     */
    inline RPointerArray<CSmlDmBmCommandElement>& NodeBuf(); 

    /**
     * Returns the iMappingName member value.  
     * @since S60 v3.2
     * @return The iMappingName member value of the object
     */
    inline const HBufC8* MappingName();

    /**
     * Returns the iExecuted member value.  
     * @since S60 v3.2
     * @return The iExecuted member value of the object
     */
    inline TBool Executed();
    
    /**
     * Sets the iExecuted member value.  
     * @since S60 v3.2
     * @param aExecuted Value for the iExecuted member of the object
     */
    inline void SetExecuted( TBool aExecuted );

    /**
     * Returns the iLuid member value.  
     * @since S60 v3.2
     * @return The iLuid member value of the object
     */
    inline TInt Luid();
    
    /**
     * Sets the iLuid member value.  
     * @since S60 v3.2
     * @param aLuid Value for the iLuid member of the object
     */
    inline void SetLuid( TInt aLuid );
    
        
private:

    CSmlDmBmSettingsElement();

    void ConstructL( const TDesC8& aMappingName );

private: //data

    /**
     * Buffer of commands for this SNAP object. Commands 
     * are added as they come in from the framework and 
     * executed by the setting store.
     */
    RPointerArray<CSmlDmBmCommandElement> iNodeBuf;

    /**
     * Indicates whether commands for this SNAP have been executed. 
     * Set to Erue by CNsmlDmBmSettingStore when object is handled.
     */    
    TBool iExecuted;
    
    /**
     * LUID (Local UID) is the CommsDat id of the SNAP node. This is 
     * filled in when SNAP object is created in CommsDat. 
     */
    TInt iLuid;

    /**
     * Mapping name of this SNAP node. This is mapped to the 
     * iLuid when the Snap is created and has id. 
     */
    HBufC8* iMappingName;
    };

#include "nsmldmbmadapter.inl"

#endif // NSMLDMBMADAPTER_H
