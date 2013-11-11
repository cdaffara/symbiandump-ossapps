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
* Description:    DM Email Settings Adapter
 *
*/





#ifndef __NSMLDMEMAILADAPTER_H__
#define __NSMLDMEMAILADAPTER_H__

// -----------------------------------------------------------------------------
// Includes
// -----------------------------------------------------------------------------
#include <e32base.h>
#include <smldmadapter.h>
#include <msvapi.h>

// -----------------------------------------------------------------------------
// Class forwards
// -----------------------------------------------------------------------------
class CImSmtpSettings;
class CImPop3Settings;
class CImImap4Settings;
class CImIAPPreferences;
class CMsvSession;
class CEmailAccounts;


// -----------------------------------------------------------------------------
// Definitions
// -----------------------------------------------------------------------------

// the DDF version must be changed if any changes in DDF structure 
// ( built in DDFStructureL() function )
//
_LIT8( KNSmlDMEmailDDFVersion, "1.1" ); 

_LIT8( KNSmlDMEmailTextPlain, "text/plain" );
_LIT8( KNSmlDMEmailNodeName, "Email" );
_LIT8( KNSmlDMEmailDescription, "The interior object holds all email objects" );
_LIT8( KNSmlDMDynamicNodeDescription, "Placeholder for one or more Email objects" );
_LIT8( KNSmlDMNameDescription, "Displayable Account Name for the Email Settings" );
_LIT8( KNSmlDMUIDDescription, "User ID for email account" );
_LIT8( KNSmlDMPWDescription, "Password for email account " );
_LIT8( KNSmlDMUAddrDescription, "User's email address" );
_LIT8( KNSmlDMUNameDescription, "Displayable name for the user" );
_LIT8( KNSmlDMMrcvDescription, "Host name of the receiving  (IMAP/POP3) server" );
_LIT8( KNSmlDMMsndDescription, "Host name of the SMTP server" );
_LIT8( KNSmlDMMproDescription, "Remote mailbox protocol" );
_LIT8( KNSmlDMUseSecConDescription, "Secure connection is used or not" );
_LIT8( KNSmlDMUseSauthDescription, "SMTP authentication is used or not" );
_LIT8( KNSmlDMSauthUIDDescription, "SMTP authenticatio user Id" );
_LIT8( KNSmlDMSauthPWDescription, "SMTP authentication password" );
_LIT8( KNSmlDMPtxtSAuthDescription, "Plain text SMTP authentication is allowed or not" );
_LIT8( KNSmlDMDelFetDescription, "Defines whether to delete fetched mail from remote server" );
_LIT8( KNSmlDMUseAPOPDescription, "Defines whether  the APOP is used or not" );
_LIT8( KNSmlDMRcvPortDescription, "Port for IMAP4 or POP3" );
_LIT8( KNSmlDMSndPortDescription, "Port for SMTP" );
_LIT8( KNSmlDMToNapIDDescription, "Link to connectivity information" );

_LIT8( KNSmlDMSToNapIDDescription, "Link to connectivity information for sending SMTP" );
_LIT8( KNSmlDMSUseSecConDescription, "Defines whether SSL/TLS security is used in SMTP" );
_LIT8( KNSmlDMUseStartTLSDescription, "Defines whether StartTLS is used in IMAP/POP" );
_LIT8( KNSmlDMSUseStartTLSDescription, "Defines whether StartTLS is in SMTP" );
_LIT8( KNSmlDMMrcvPortDescription, "Defines the port number of the IMAP/POP server" );
_LIT8( KNSmlDMMsndPortDescription, "Defines the port number of the SMTP server" );
_LIT8( KNSmlDMFolderPathDescription, "Defines the mailbox folder path" );

_LIT8( KNSmlDMEmailDynamicNode, "" );
_LIT8( KNSmlDMEmailName, "Name" );
_LIT8( KNSmlDMEmailUID, "UID" );
_LIT8( KNSmlDMEmailPW, "PW" );
_LIT8( KNSmlDMEmailUAddr, "UAddr" );
_LIT8( KNSmlDMEmailUName, "UName" );
_LIT8( KNSmlDMEmailMrcv, "Mrcv" );
_LIT8( KNSmlDMEmailMsnd, "Msnd" );
_LIT8( KNSmlDMEmailMpro, "Mpro" );
_LIT8( KNSmlDMEmailUseAPOP, "UseAPOP" );
_LIT8( KNSmlDMEmailUseSecCon, "UseSecCon" );
_LIT8( KNSmlDMEmailUseSauth, "UseSauth" );
_LIT8( KNSmlDMEmailSauthUID, "SauthUID" );
_LIT8( KNSmlDMEmailSauthPW, "SauthPW" );
_LIT8( KNSmlDMEmailPtxtSAuth, "PtxtSAuth" );
//_LIT8( KNSmlDMEmailRcvPort, "RcvPort" );
//_LIT8( KNSmlDMEmailSndPort, "SndPort" );
_LIT8( KNSmlDMEmailToNapID, "ToNapID" );
_LIT8( KNSmlDMEmailIAPUri, "AP" );

_LIT8( KNSmlDMEmailSToNapID, "SToNapID" );
_LIT8( KNSmlDMEmailSUseSecCon, "SUseSecCon" );
_LIT8( KNSmlDMEmailUseStartTLS, "UseStartTLS" );
_LIT8( KNSmlDMEmailSUseStartTLS, "SUseStartTLS" );
_LIT8( KNSmlDMEmailMrcvPort, "MrcvPort" );
_LIT8( KNSmlDMEmailMsndPort, "MsndPort" );
_LIT8( KNSmlDMEmailFolderPath, "FolderPath" );

_LIT8( KNSmlDMEmailMproPOP, "POP" );
_LIT8( KNSmlDMEmailMproIMAP, "IMAP" );
_LIT8( KNSmlDMEmailTrue, "True" );
_LIT8( KNSmlDMEmailFalse, "False" );

_LIT8( KNSmlDMEmailListOfLeafs, "Name/UID/PW/UAddr/UName/Mrcv/Msnd/Mpro/UseSecCon/UseSauth/SauthUID/SauthPW/PtxtSAuth/ToNapID/SToNapID/SUseSecCon/UseStartTLS/SUseStartTLS/MrcvPort/MsndPort/UseAPOP/FolderPath" );
_LIT8( KNSmlDMEmailSeparator8, "/" );
_LIT8( KNSmlEmailUriDotSlash, "./");

const TInt KNSmlDmFatMinimumFileSize = 512;
const TInt KNSmlSmtpMessageTypeValue = 0x10001028;
const TInt KNSmlDefaultMaxLimit = 1024*10; //maximum message size (default)
const TInt KNSmlDmEmailGranularity = 4;
const TUint32 KNSmlDmEmailAlwaysAskIap = 0;

const TInt KNSmlDMEmailNormalSmtpPort = 25;
const TInt KNSmlDMEmailSslSmtpPort = 465;
const TInt KNSmlDMEmailNormalImap4Port = 143;
const TInt KNSmlDMEmailNormalPop3Port = 110;
const TInt KNSmlDMEmailSslWrappedImap4Port = 993;
const TInt KNSmlDMEmailSslWrappedPop3Port = 995;
const TInt KNSmlDMEmailDefaultHeaderCount = 30;

const TInt KNSmlDmEmailImapLuidLow = 50000;
const TInt KNSmlDmEmailImapLuidHigh = 100000;
const TInt KNSmlDmEmailPopLuidHigh = 150000;
const TUint32 KNSmlDMEmailMaxPortNbr = 0xffff;

/**
* The main class of the SyncML DM Email adapter.
*
*  @since 
*/
class CNSmlDmEmailAdapter : public CSmlDmAdapter, public MMsvSessionObserver
    {
public:
    /**
     * The one phase constructor
     *
     */
    static CNSmlDmEmailAdapter* NewL(MSmlDmCallback* aDmCallback );

    /** 
     * The destructor
     *
     */
    virtual ~CNSmlDmEmailAdapter();

    // Adapter interface from CSmlDmAdapter
    /**
     * The function returns current version of the DDF.  By asking
     * current DDF versions from adapters DM Module can control *
     * possible changes in the data structure and send the changed DDF
     * description to a management server.  This function is always
     * called after DDFStructureL.  
     * @param aVersion DDF version of the
     * adapter. (filled by the adapter) 
     * @publishedPartner 
     * @prototype
     */

    void DDFVersionL( CBufBase& aDDFVersion );
    /**
     * The function for filling the DDF structure of the adapter
     * This function is only called once, immediately after the adapter is created.
     * @param aDDFObject   Reference to root object. A DM adapter starts filling
     * the data structure by calling AddChildObjectL to the root object and
     * so describes the DDF of the adapter. 
     * @publishedPartner
     * @prototype
    */
    void DDFStructureL( MSmlDmDDFObject& aDDF );
    /**
     * The function creates new leaf objects, or replaces data in existing leaf
     * objects. The information about the success of the command should be
     * returned by calling SetStatusL function of MSmlDmCallback callback
     * interface. This makes it possible to buffer the commands.  However, all
     * the status codes for buffered commands must be returned at the latest when
     * the adapter's CompleteOutstandingCmdsL() is called.
     * @param aURI         URI of the object
     * @param aLUID        LUID of the object (if the adapter has earlier returned a
     *                    LUID to the DM Module). For new objects, this is the LUID
     *                    inherited through the parent node.
     * @param aObject      Data of the object.
     * @param aType        MIME type of the object
     * @param aStatusRef   Reference to correct command, i.e. this reference
     *                    must be used when calling the SetStatusL of this command
     * @publishedPartner
     * @prototype
    */
    void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                            const TDesC8& aObject, const TDesC8& aType, 
                            TInt aStatusRef );
    /**
     * The function deletes an object and its child objects. The SetStatusL
     * should be used as described in UpdateLeafObjectL()
     * @param aURI     URI of the object
     * @param aLUID        LUID of the object (if the adapter have earlier returned
     *                     LUID to the DM Module).
     * @param aStatusRef   Reference to correct command, i.e. this reference must
     *                     be used when calling the SetStatusL of this command.
     * @publishedPartner
     * @prototype
    */
    void DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                        const TInt aStatusRef );
    /**
     * The function fetches data of a leaf object. The SetStatusL should be used
     * as described in UpdateLeafObjectL(). The data is returned by using the
     * SetResultsL function of MSmlCallback callback interface, and may be streamed.
     * @param aURI         URI of the object
     * @param aLUID            LUID of the object (if the adapter have earlier
     *                       returned LUID to the DM Module).   
     * @param aType            MIME type of the object
     * @param aResultsRef  Reference to correct results, i.e. this reference
     *                         must be used when returning the result by calling
     *                         the SetResultsL.
     * @param aStatusRef       Reference to correct command, i.e. this reference
     *                         must be used when calling the SetStatusL of this
     *                         command.
     * @publishedPartner
     * @prototype
    */
    void FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                           const TDesC8& aType, const TInt aResultsRef, 
                           const TInt aStatusRef );
    /**
     * The function fetches URI list. An adapter returns the list of URI segments
     * under the given URI be separated by slash ("/"). The URI segment names for
     * new objects must be given by the adapter.
     * The list is returned by calling the SetResultsL function of MSmlCallback
     * callback interface. Results from this call MUST NOT be streamed.
     * @param aParentURI                   URI of the parent object
     * @param aParentLUID              LUID of the parent object (if the
     *                                     adapter have earlier returned LUID to
     *                                     the DM Module).   
     * @param aPreviousURISegmentList  URI list with mapping LUID information,
     *                                     which is known by DM engine. An adapter
     *                                     can use this information when verifying
     *                                     if old objects still exists. An adapter
     *                                     also knows what objects are new to DM
     *                                     engine and can provide LUID mapping for
     *                                     them. aPreviousURISegmentList parameter
     *                                     (see above) helps to recognise new
     *                                     objects.
     * @param aResultsRef              Reference to correct results, i.e. this
     *                                     reference must be used when returning
     *                                     the result by calling the SetResultsL.
     * @param aStatusRef                   Reference to correct command, i.e. this
     *                                     reference must be used when calling the
     *                                     SetStatusL of this command.
     * @publishedPartner
     * @prototype
    */
    void ChildURIListL( const TDesC8& aURI, 
                        const TDesC8& aLUID, 
                     const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList,
        const TInt aResultsRef, const TInt aStatusRef );
    /**
     * The function adds node object. In some cases an implementation of the
     * function may be empty function, if the node object does not need concrete
     * database update. Still this function may be helpful to an adapter, i.e. in
     * passing mapping LUID of the node to DM Module. The SetStatusL should be
     * used as described in UpdateLeafObjectL()
     * @param aURI         URI of the object
     * @param aParentLUID  LUID of the parent object (if the adapter have
     *                         earlier returned LUID to the DM Module).   
     * @param aStatusRef       Reference to correct command, i.e. this reference
     *                         must be used when calling the SetStatusL of this
     *                         command.
     * @publishedPartner
     * @prototype
    */
    void AddNodeObjectL( const TDesC8& aURI, const TDesC8& aParentLUID, 
                         const TInt aStatusRef );
    /**
     * The function creates new leaf objects, or replaces data in existing leaf
     * objects, in the case where data is large enough to be streamed. The
     * information about the success of the command should be returned by calling
     * SetStatusL function of MSmlDmCallback callback interface. This makes it
     * possible to buffer the commands.  However, all the status codes for buffered
     * commands must be returned at the latest when the CompleteOutstandingCmdsL()
     * of adapter is called.
     * @param aURI     URI of the object
     * @param aLUID        LUID of the object (if the adapter has earlier returned a
     *                     LUID to the DM Module). For new objects, this is the LUID
     *                     inherited through the parent node.
     * @param aStream  Data of the object. Adapter should create write stream
     *                     and return, when data is written to stream by DM agent,
     *                     StreamCommittedL() is called by DM engine
     * @param aType        MIME type of the object
     * @param aStatusRef   Reference to correct command, i.e. this reference
     *                     must be used when calling the SetStatusL of this
     *                     command.
     * @publishedPartner
     * @prototype
    */
    void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                            RWriteStream*& aStream, const TDesC8& aType, 
                            const TInt aStatusRef );
    /**
     * The function fetches the size of the data of a leaf object. The size is
     * in bytes, and must reflect the number of bytes that will be transferred
     * when the framework calls FetchLeafObjectL. The SetStatusL should be used
     * as described in FetchLeafObjectL(). The size value is returned by using
     * the SetResultsL function of MSmlCallback callback interface, and must be
     * a decimal integer expressed as a string, eg. "1234".
     * Results from this call MUST NOT be streamed.
     * @param aURI         URI of the object
     * @param aLUID            LUID of the object (if the adapter have earlier
     *                         returned LUID to the DM Module).   
     * @param aType            MIME type of the object
     * @param aResultsRef  Reference to correct results, i.e. this reference
     *                         must be used when returning the result by calling
     *                         the SetResultsL.
     * @param aStatusRef       Reference to correct command, i.e. this reference
     *                         must be used when calling the SetStatusL of this
     *                         command.
     * @publishedPartner
     * @prototype
    */
    void FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID, 
                               const TDesC8& aType, const TInt aResultsRef, 
                               const TInt aStatusRef );
    /**
     * The function implements execute command. The information about the success
     * of the command should be returned by calling SetStatusL function of
     * MSmlDmCallback callback interface. This makes it possible to buffer the
     * commands.
     * However, all the status codes for buffered commands must be returned at
     * the latest when the CompleteOutstandingCmdsL() of adapter is called.
     * @param aURI         URI of the command
     * @param aLUID            LUID of the object (if the adapter have earlier
     *                         returned LUID to the DM Module).   
     * @param aArgument        Argument for the command
     * @param aType            MIME type of the object 
     * @param aStatusRef       Reference to correct command, i.e. this reference
     *                         must be used when calling the SetStatusL of this
     *                         command.
     * @publishedPartner
     * @prototype
    */
    void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, 
                          const TDesC8& aArgument, const TDesC8& aType, 
                          const TInt aStatusRef );
    /**
     * The function implements execute command. The information about the
     * success of the command should be returned by calling SetStatusL function
     * of MSmlDmCallback callback interface. This makes it possible to buffer the
     * commands.
     * However, all the status codes for buffered commands must be returned at
     * the latest when the CompleteOutstandingCmdsL() of adapter is called.
     * @param aURI         URI of the command
     * @param aLUID            LUID of the object (if the adapter have earlier
     *                         returned LUID to the DM Module).   
     * @param aStream      Argument for the command. Adapter should create
     *                         write stream and return, when data is written to
     *                         stream by DM agent, StreamCommittedL() is called by
     *                         DM engine
     * @param aType            MIME type of the object 
     * @param aStatusRef       Reference to correct command, i.e. this reference
     *                         must be used when calling the SetStatusL of this
     *                         command.
     * @publishedPartner
     * @prototype
    */
    void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, 
                          RWriteStream*& aStream, const TDesC8& aType, 
                          const TInt aStatusRef );
    /**
     * The function implements copy command. The information about the success of
     * the command should be returned by calling SetStatusL function of
     * MSmlDmCallback callback interface. This makes it possible to buffer the
     * commands.
     * However, all the status codes for buffered commands must be returned at
     * the latest when the CompleteOutstandingCmdsL() of adapter is called.
     * @param aTargetURI       Target URI for the command
     * @param aSourceLUID  LUID of the target object (if one exists, and if the adapter
     *                         has earlier returned a LUID to the DM Module).   
     * @param aSourceURI       Source URI for the command
     * @param aSourceLUID  LUID of the source object (if the adapter has
     *                         earlier returned a LUID to the DM Module).   
     * @param aType            MIME type of the objects
     * @param aStatusRef       Reference to correct command, i.e. this reference
     *                         must be used when calling the SetStatusL of this
     *                         command.
     * @publishedPartner
     * @prototype
    */
    void CopyCommandL( const TDesC8& aTargetURI, const TDesC8& aTargetLUID, 
                       const TDesC8& aSourceURI, const TDesC8& aSourceLUID, 
                       const TDesC8& aType, TInt aStatusRef );
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
     * can be passed to the adapter have now been passed.  This indciates that
     * the adapter must supply status codes and results to any buffered commands.
     * This must be done at latest by the time this function returns.
     * This function is used at the end of SyncML messages, and during processing
     * of Atomic.   In the case of Atomic processing, the function will be
     * followed by a call to CommitAtomicL or RollbackAtomicL.
     * @publishedPartner
     * @prototype
    */
    void CompleteOutstandingCmdsL();

protected:  
    /**
     * Inherited from MMsvSessionObserver
     * Indicates an event has occurred.
     * @param aEvent Indicates the event type. 
     * @param aArg1 Event type-specific argument value 
     * @param aArg2 Event type-specific argument value 
     * @param aArg3 Event type-specific argument value 
     */
    void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2,
                              TAny* aArg3 );

private:

struct TNSmlEmailCommandElement
    {
    HBufC8* iLastUriSeg;
    HBufC8* iData;
    TInt iStatusRef;
    TBool iLeaf;
    CSmlDmAdapter::TError iStatus;
    };

enum TNSmlEmailAccountType
    {
    EImap,
    EPop,
    EUnknown
    };

enum TNSmlEmailSecuritySetting
    {
    EUseSecCon = 1,
    EUseStartTls,
    ESuseSecCon,
    ESuseStartTls,
    };

struct TNSmlEmailSettingsElement
    {
    CArrayFixFlat<TNSmlEmailCommandElement> *iNodeBuf;
    /* iMappingName = Email/xxxx */
    HBufC8* iMappingName;
    HBufC* iName;
    CImSmtpSettings* iSmtpSettings;
    CImPop3Settings* iPop3Settings;
    CImImap4Settings* iImap4Settings;
    CImIAPPreferences* iIapPref;
    CImIAPPreferences* iSmtpIapPref;
    
    TBool iExecuted;
    TInt iLuid;
    TBool iJustFetch;
    CNSmlDmEmailAdapter::TNSmlEmailAccountType iAccType;
    TInt iAccId;
    TMsvId iServiceId;
    };

    /**
    * C++ default constructor.
    */
    void ConstructL();
    CNSmlDmEmailAdapter();
    CNSmlDmEmailAdapter( MSmlDmCallback* aDmCallback );

    /**
     * Initializes the buffer for leaf nodes.
     * @param    aURI URI of the object
     * @param    aObject The data to be updated
     * @param    aStatusRef The status reference
     * @param    aIndex The index to command buffer
     * @return   status of the operation
     */
    TInt FirstAddLeafL(const TDesC8& aURI, 
        const TDesC8& aObject, 
        TInt aStatusRef,
        TInt aIndex);

    /**
     * Updates the value of leaf object
     * @param    aLastUriSeg URI of the object
     * @param    aObject The data to be updated
     * @param    aIndex The index to node buffer
     * @param    aCommand The index to command buffer
     * @return   status of the operation
     */
    TInt DoAddLeafObjectL(const TDesC8& aLastUriSeg, 
        const TDesC8& aObject,
        TInt aIndex, 
        TInt aCommand);

    /**
     * Set the default values to email account
     * @param    aIndex The index to node buffer
     * @return   none
     */
    void SetDefaultSettingsL(TInt aIndex);

    /**
     * Converts 16bit descriptor to integer
     * @param    aLuid The descriptor to be converted
     * @return    Integer value of the descriptor
     */
    TInt DesToInt(const TDesC& aLuid) const;

    /**
     * Converts 8bit descriptor to integer
     * @param    aLuid The descriptor to be converted
     * @return    Integer value of the descriptor
     */
    TInt DesToInt(const TDesC8& aLuid) const;

    /**
     * Converts integer to 16bit descriptor
     * @param    aLuid The integer to be converted
     * @return    The Integer as a descriptor
     */
    HBufC* IntToDesLC(const TInt aLuid) const;

    /**
     * Converts integer to 8bit descriptor
     * @param    aLuid The integer to be converted
     * @return    The Integer as a descriptor
     */
    HBufC8* IntToDes8LC(const TInt aLuid) const;

    /**
     * Converts 16bit descriptor to 8bit descriptor
     * @param    aSource The descriptor to be converted
     * @return    The 8bit descriptor
     */
    HBufC8* ConvertTo8LC( const TDesC& aSource ) const;

    /**
     * Transfers the email settings from memory to phones permanent data store
     * @param    aIndex The index to command buffer
     * @return    Luid of the email account
     */
    TInt ValidateAndStoreSettingsL(TInt aIndex);

    /**
     * Restores the settings to the adapter from phones permanent data store
     * @param    aIndex The index to command buffer
     * @return    Status of the operation
     */
    TInt DoRestoreSettingsL(TInt aIndex);

    /**
     * Opens the CMsvSession if it is not open already
     * @param    none
     * @return    Status of the operation
     */
    TInt OpenSessionL();

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
     * Fills the DDF node with desired properties
     * @param    aNode           Reference to node or leaf which info to be filled.
     * @param    aAccTypes       Access rights for a leaf / node.
     * @param    aOccurrence     Occurance of the node / leaf.
     * @param    aScope          Scope (dynamic / permanent) of the leaf / node.
     * @param    aFormat         Data format of the leaf / node.
     * @param    aDescription    Description of the node / leaf.
     * @param    aDefaultValue   Default value for a leaf or node.
     * @return    none
     */
    void FillNodeInfoL( MSmlDmDDFObject& aNode,
        const TSmlDmAccessTypes& aAccTypes, 
                        MSmlDmDDFObject::TOccurence aOccurrence, 
                        MSmlDmDDFObject::TScope aScope, 
                        MSmlDmDDFObject::TDFFormat aFormat,
                        const TDesC8& aDescription) const;

    /**
     * Parses the dot and slash in start of URI away
     * @param    aURI The whole URI
     * @return    The URI without dot and slash in start
     */
    TPtrC8 RemoveDotSlash(const TDesC8& aURI) const;

    /**
     * Fetches the value of leaf node
     * @param    aURI The whole URI
     * @param    aLUID The LUID
     * @param    aObject The storage for the result
     * @return    Status of the operation
     */
    CSmlDmAdapter::TError FetchObjectL(const TDesC8& aURI, 
        const TDesC8& aLUID,
        CBufBase& aObject);

    /**
     * Calculates correct account id from the LUID
     * @param    aAccType Account Type
     * @param    aAccId Account ID
     * @param    aLuid The LUID
     * @return    none
     */
    void GetIdByLuid( TNSmlEmailAccountType& aAccType, 
                      TInt& aAccId, const TInt aLuid ) const;

    /**
     * Calculates LUID from the account id
     * @param    aAccType Account Type
     * @param    aAccId Account ID
     * @param    aLuid The LUID
     * @return    none
     */
    void SetLuidById( const TNSmlEmailAccountType& aAccType, 
                      const TInt& aAccId, TInt& aLuid ) const;

    /**
     * Checks if the account with given id exists
     * @param    aAccs Pointer to CEmailAccount object
     * @param    aAccType Account Type
     * @param    aId Account ID
     * @param    aIndex The LUIDIndex to the command buffer
     * @return    True if the account exists
     */
    TBool AccountExistsL( CEmailAccounts* aAccs, 
                         const TNSmlEmailAccountType aAccType, 
                         const TInt aId,
                         TInt aIndex );

    /**
     * Sets the parameters related to STARTTLS and SSL/TLS
     * @param    aObject The value(True/False) to be set
     * @param    aThisElem The element of which value is to be set.
     * @param    aSetting The setting which value is to be set
     * @return    none
     */
    void SetDefaultSecurityL(const TDesC8& aObject,
                          const TNSmlEmailSettingsElement* aThisElem,
                          const TNSmlEmailSecuritySetting aSetting
        ) const;

    /**
     * Fetches the parameters related to STARTTLS and SSL/TLS
     * @param    aObject The storage for the value
     * @param    aElem The element of which value is fetched.
     * @param    aSetting The setting which value is fetched.
     * @return    Status of the operation
     */
    CSmlDmAdapter::TError FetchSecurityValueL(CBufBase& aObject, 
        const TNSmlEmailSettingsElement* aElem,
        const TNSmlEmailSecuritySetting aSetting ) const;
		
		/**
     * Creates the mapping for the email account preset
     * @param    aURI The whole URI
     * @return   the luid of the email account specified im aURI, else -1
     */
		TInt ConstructTreeL(const TDesC8& aURI);
		
 /**
        * The function extracts Email/x node from aURI		        
        * @param    aURI     Email aURI.
        * @return   Email/x node.
    */ 
    TPtrC8 GetDynamicEmailNodeUri( const TDesC8& aURI );
		
private:
    // Pointer to CMsvSession
    CMsvSession* iMsvSession;
    // The buffer for received commands
    CArrayFixFlat<TNSmlEmailSettingsElement>* iBuffer;
    };

#endif // __NSMLDMEMAILADAPTER_H__

// End of File
