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
* Description:  DM adapter for handling always-on settings.
*
*/


#ifndef NSMLDMALWAYSONADAPTER_H
#define NSMLDMALWAYSONADAPTER_H

#include <smldmadapter.h>

#include "nsmldmalwaysonsettingstore.h"

class CSmlDmAOCommandElement;
class CNSmlDmAOSettingStore;

// The DDF version must be changed if any changes in DDF structure 
// ( built in DDFStructureL() function )
_LIT8( KNSmlDmAOAdapterDDFVersion, "1.0" ); 
_LIT8( KNSmlDmAOAdapterTextPlain, "text/plain" );

// Names of the nodes
// When updating KNSmlDmAOAdapterAOURI also update 
// KNSmlDmAOAdapterURIMaxLength! */
_LIT8(KNSmlDmAOAdapterAOURI, "./VENDORCONFIG");
_LIT8(KNSmlDmAOAdapterAO, "VENDORCONFIG");
_LIT8(KNSmlDmAOAdapterName, "NAME");
_LIT8(KNSmlDmAOAdapterAwonPdpc, "AWON-PDPC");
_LIT8(KNSmlDmAOAdapterTRetry, "T-RETRY");


// Descriptions of the nodes
_LIT8( KNSmlDmAOAdapterAODescription, 
       "Always-on provides management of connections");
_LIT8( KNSmlDmAOAdapterNameDescription, 
       "Name of the VENDORCONFIG");
_LIT8( KNSmlDmAOAdapterAwonPdpcDescription, 
       "Always-on setting in home and visited network");
_LIT8( KNSmlDmAOAdapterTRetryDescription, 
       "T-Retry timer interval");

// Leaf nodes of VENDORCONFIG node
_LIT8( KNSmlDmAOAllLeafNodes, 
       "NAME/AWON-PDPC/T-RETRY");

// URI segment separator
// When updating this literal also update 
// KNSmlDmAOAdapterAPURIMaxLength and KNSmlDmAOAdapterURIMaxLength!
_LIT8( KNSmlDmAOSeparator, "/" );

// URI segment separator
// When updating KNSmlDmAOAdapterAOURI also update 
// KNSmlDmAOAdapterAPURIMaxLength and KNSmlDmAOAdapterURIMaxLength!
_LIT8( KNSmlDmAOUriListSeparator, "," );

// Prefix in URIs (removed for LUID mapping)
_LIT8( KNSmlDmAOAdapterURIPrefix, "./" );

// Name prefix for unnamed VENDORCONFIG nodes
_LIT8( KNSmlDmAONamePrefix, "VENDORCONFIG" );

const TInt KNSmlDmAOGranularity = 4;
const TInt KNSmlDmAOInvalidRef = -1;

// Maximum length of VENDORCONFIG URI including a separator 
// character in URI List. Node is not calculated here.  
// KNSmlDmAOAdapterAOURI + KNSmlDmAOSeparator + KNSmlDmAOUriListSeparator
const TInt KNSmlDmAOAdapterURIMaxLength =  16;
        
/**
 *  Always-on device management adapter
 *
 *  Always-on device management adapter manages settings
 *  related to VENDORCONFIG.
 *
 *  @lib nsmldmalwaysonadapter
 *  @since S60 v3.2
 */
class CNSmlDmAOAdapter : public CSmlDmAdapter
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


    static CNSmlDmAOAdapter* NewL( MSmlDmCallback* aDmCallback );
    
    virtual ~CNSmlDmAOAdapter();


// from base class CSmlDmAdapter

    /**
     *  The function returns current version of the DDF. 
     *
     *  @since S60 v3.2
     *  @param aVersion DDF version of the
     *  adapter. (filled by the adapter) 
     */
    void DDFVersionL( CBufBase& aDDFVersion );

    /**
     *  The function for filling the DDF structure of the adapter
     *
     *  @since S60 v3.2
     *  @param aDDFObject   Reference to root object.
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
     * @param aParentURI               URI of the parent object
     * @param aParentLUID              LUID of the parent object (if the
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
     * @param aSourceLUID      LUID of the target object 
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


private:

     /**
     * Constructor
     */
    CNSmlDmAOAdapter();

    /**
     * Constructor
     * @param aDmCallback Callback object to the framework
     */
    CNSmlDmAOAdapter( MSmlDmCallback* aDmCallback );   

    /**
     * Second phase constructor
     */
    void ConstructL();
     
    /**
     * Parses the last URI segment from URI
     * @param    aURI The whole URI
     * @return    The last URI segment
     */
    TPtrC8 LastURISeg(const TDesC8& aURI) const; 


private: //data

    /**
     * Setting store object, which is called for managing settings 
     * in CommsDat. Own.
     */
     CNSmlDmAOSettingStore * iSettingStore;
        
    };


/**
 *  CSmlDmAOCommandElement 
 *
 * Helper class, which stores a single command for a VENDORCONFIG. 
 *  @lib nsmldmalwaysonadapter
 *  @since S60 v3.2
 */

class CSmlDmAOCommandElement : public CBase
    {

public:

    static CSmlDmAOCommandElement* NewLC( TBool aLeaf, 
                                          TInt aStatusRef, 
                                          TInt aResultRef, 
                                          CNSmlDmAOAdapter::TCommandType  aCmdType, 
                                          const TDesC8& aLastUriSeg, 
                                          const TDesC8& aData );

     ~CSmlDmAOCommandElement();

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
    inline CNSmlDmAOAdapter::TCommandType  CmdType();        
    
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
    inline void SetData( HBufC8* aData );

private:

    CSmlDmAOCommandElement( TBool aLeaf, 
                            TInt aStatusRef, 
                            TInt aResultRef, 
                            CNSmlDmAOAdapter::TCommandType aCmdType );                              

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
     * True if commend is for a leaf node, False if it is for a NAPDEF node.
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
    const CNSmlDmAOAdapter::TCommandType  iCmdType;        
    

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
    
#include "nsmldmalwaysonadapter.inl"    

#endif // NSMLDMALWAYSONADAPTER_H
