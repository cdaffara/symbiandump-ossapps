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
* Description:    DM Settings adapter header file
*
*/




#ifndef __NSMLDMSETTINGSADAPTER_H__
#define __NSMLDMSETTINGSADAPTER_H__


// INCLUDES
#include <utf.h>
#include <smldmadapter.h>
#include <SyncMLClientDM.h>
#include "NSmlPrivateAPI.h"

// CONSTANTS
const TInt KNSmlDMGranularity = 4;
const TUint8 KNSmlDMColon = ':';
const TUint8 KNSmlDMUriSeparator = '/';
const TInt KPortMaxSize = 65535;
const TInt KPortMinSize = 0;
//Dynamic Node Posn Const --> SyncML/DMAcc/DMIDxxx
const TInt KDynNodePosn = 3;

//DDF accepted DM-fieldnames
_LIT8( KNSmlDdfId,				"Id" );
_LIT8( KNSmlDdfAddr,			"Addr" );
_LIT8( KNSmlDdfAddrType,		"AddrType" );
_LIT8( KNSmlDdfPortNbr,			"PortNbr" );
_LIT8( KNSmlDdfAuthScheme,		"AuthPref" ); 
_LIT8( KNSmlDdfServerId,		"ServerId" );
_LIT8( KNSmlDdfServerPW,		"ServerPW" );
_LIT8( KNSmlDdfServerNonce,		"ServerNonce" );
_LIT8( KNSmlDdfClientUserName,	"UserName" );
_LIT8( KNSmlDdfClientPW,		"ClientPW" );
_LIT8( KNSmlDdfClientNonce,		"ClientNonce" );
_LIT8( KNSmlDdfName,			"Name" );
_LIT8( KNSmlDdfConRef,			"ConRef" );
_LIT( KNSmlDdfMConRef,			"MConRef" );

//DDF DM-field descriptions
_LIT8( KNSmlDdfAddrDescription,				"Host address of the SyncML server, IP or URL" );
_LIT8( KNSmlDdfAddrTypeDescription,			"The type of address specified in the Addr node" );
_LIT8( KNSmlDdfPortNbrDescription,			"SyncML Server port" );
_LIT8( KNSmlDdfAuthSchemeDescription,		"Scheme of authentication" );
_LIT8( KNSmlDdfServerIdDescription,			"The ServerId value for this server" );
_LIT8( KNSmlDdfServerPWDescription,			"The password or secret that the server will use to authenticate itself to the client" );
_LIT8( KNSmlDdfServerNonceDescription,		"The next nonce that the server will use to authenticate itself to the client" );
_LIT8( KNSmlDdfClientUserNameDescription,	"The username of the device (or user)" );
_LIT8( KNSmlDdfClientPWDescription,			"The password or secret that the client will use to authenticate itself to the server" );
_LIT8( KNSmlDdfClientNonceDescription,		"The next nonce that the client will use to authenticate itself to the server" );
_LIT8( KNSmlDdfNameDescription,				"Displayable name for the SyncML setings" );
_LIT8( KNSmlDdfConRefDescription,			"Logical reference to a connectivity node" );
_LIT( KNSmlDdfMConRefDescription,			"Parent to Multiple Connection Reference objects" );

//Default data values
_LIT( KNSmlDefName,				"DMAdapterIns" );
_LIT8( KNSmlDefDMAcc,			"DMAcc" );
_LIT8( KNSmlDMStart,			"SyncML/DMAcc/DMId" );
_LIT( KNSmlDefDMIAP,			"AP" );
_LIT8( KNSmlDMUriDotSlash,      "./");
_LIT8( KNSmlDMNullDesc8,        "");
_LIT8(KNSmlIAPId,               "NSmlIapId");

// AuthPref text values
_LIT8( KNSmlDMAuthPrefNoAuth,"syncml:auth-none");
_LIT8( KNSmlDMAuthPrefBasic, "syncml:auth-basic");
_LIT8( KNSmlDMAuthPrefMD5,	 "syncml:auth-md5");

_LIT8( KNSmlDMSettingsHTTP,  "http://" );
_LIT8( KNSmlDMSettingsHTTPS, "https://" );

_LIT8( KIAPid, "IAPid" );
_LIT8( KSegmDMAcc,"Addr/AddrType/PortNbr/AuthPref/ServerId/ServerPW/ServerNonce/UserName/ClientPW/ClientNonce/Name/ConRef");
_LIT8( KSmlDmSlash, "/");
_LIT8( KVersion,"1.1" );
_LIT8( KMimeType, "text/plain" );
_LIT8( KSyncMLText, "SyncML" );
_LIT8( KNSmlDMadapterDesc, "DM-Settings DDF description" );
_LIT8( KNSmlDMadapterTitle,"DM-Settings title" );
_LIT8( KDmAccMatch, "SyncML/DMAcc/*" );
_LIT8( KDmAccMatch2, "SyncML/DMAcc" );


// DATA TYPES
enum TNSmlDMFieldType
	{
	EStr,
	EInt,
	EBin,
	EWrong
	};

enum TNSmlDMLeafType
	{
	EDMUpdate,
	EDMDelete,
	EDMUnset
	};

enum TNSmlDMProfileData
	{
	EProfileId,
	EProfileName,
	EProfileIAPId,
	EProfileMediumType,
	EProfileDMServerUsername,
	EProfileDMServerPassword,
	EServerNonce,
	ESharedSecret,
	EClientNonce,
	EProfileURL,
	EProfilePort,
	EServerId,
	EAuthenticationRequired,
	ESyncAccepted
	};

enum TAuthenticationType
	{
	ENoAuth,    //"syncml:auth-none"
	EBasic,     //"syncml:auth-basic"
	EMD5        //"syncml:auth-md5"
	};

enum TNSmlDMMediumType
	{
	ENone,
    EHttp,
	EWsp,
    EObex
	};

struct TNSmlDMLeafElement
	{
	HBufC8 *iUri;
	HBufC8 *iData;
	TInt iStatusRef;
	HBufC8* iLuid;
	};

struct TNSmlDMBufferElement
	{
	CArrayFixFlat<TNSmlDMLeafElement> *iLeafBuf;
	HBufC8* iMappingName; //Holds the Dyn.Node URI
	TBool iExecuted; //Node updated to dB or not
	TInt iDMBuffStatusref; //Statusref for callback
	TBool iAddr;
	TBool iServerId; //Bools for Mandatory fields
	TBool iUName;
	TBool iServerName;
	};
// CLASS DECLARATION
/**
*  CNSmlDMSettingsAdapter class 
*  Contains the whole implementation of the DM settings adapter.
*
*  @lib nsmldmsettingsadapter.dll
*  @since Series 60 Series60_3.0
*/
class CNSmlDMSettingsAdapter : public CSmlDmAdapter
	{
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
	    static CNSmlDMSettingsAdapter* NewL(MSmlDmCallback* aDmCallback );
	    static CNSmlDMSettingsAdapter* NewLC(MSmlDmCallback* aDmCallback );

	    void ConstructL();

        /**
        * Destructor.
        */
	    virtual ~CNSmlDMSettingsAdapter();

    public: // Functions from base classes

	    /**
        * From      CSmlDmAdapter   DDFVersionL returns current version of the DDF.
        * @since    Series60_3.0
        * @param    aVersion        DDF version of the adapter.
        * @return   none
        */
	    void DDFVersionL( CBufBase& aVersion );

        /**
        * From      CSmlDmAdapter   DDFStructureL for filling the DDF structure of the adapter.
        * @since    Series60_3.0
        * @param    aDDF            Reference to root object.
        * @return   none
        */
	    void DDFStructureL( MSmlDmDDFObject& aDDF );

        /**
        * From      CSmlDmAdapter   UpdateLeafObjectL creates new leaf objects, or replaces 
        *                           data in existing leaf objects.
        * @since    Series60_3.0
        * @param    aURI            URI of the object
        * @param    aLUID           LUID of the object
        * @param    aObject         Data of the object
        * @param    aType           MIME type of the object
        * @param    aStatusRef      Reference to correct command
        * @return   none
        */
	    void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                            const TDesC8& aObject, const TDesC8& aType, 
                            TInt aStatusRef );
        
        /**
        * From      CSmlDmAdapter   DeleteObjectL deletes an object and its child objects.
        * @since    Series60_3.0
        * @param    aURI            URI of the object
        * @param    aLUID           LUID of the object
        * @param    aStatusRef      Reference to correct command
        * @return   none
        */
	    void DeleteObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                        const TInt aStatusRef );
        
        /**
        * From      CSmlDmAdapter   FetchLeafObjectL fetches data of a leaf object.
        * @since    Series60_3.0
        * @param    aURI            URI of the object
        * @param    aLUID           LUID of the object
        * @param    aType           MIME type of the object
        * @param    aResultsRef    	Reference to correct results
        * @param    aStatusRef      Reference to correct command
        * @return   none
        */
	    void FetchLeafObjectL(  const TDesC8& aURI, 
                            const TDesC8& aLUID, 
                            const TDesC8& aType, 
                            const TInt aResultsRef, 
                            const TInt aStatusRef ); 
        /**
        * From      CSmlDmAdapter   ChildURIListL fetches URI list.
        * @since    Series60_3.0
        * @param    aURI                    URI of the object
        * @param    aLUID                   LUID of the object
        * @param    aPreviousURISegmentList URI list with mapping LUID information
        * @param    aResultsRef    	        Reference to correct results
        * @param    aStatusRef              Reference to correct command
        * @return   none
        */
	    void ChildURIListL( const TDesC8& aURI, const TDesC8& aLUID, 
                        const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList, 
                        const TInt aResultsRef, const TInt aStatusRef );
        
        /**
        * From      CSmlDmAdapter   AddNodeObjectL adds node object.
        * @since    Series60_3.0
        * @param    aURI            URI of the object
        * @param    aParentLUID     LUID of the parent object
        * @param    aStatusRef      Reference to correct command
        * @return   none
        */
	    void AddNodeObjectL( const TDesC8& aURI, const TDesC8& aParentLUID, 
                            const TInt aStatusRef );

        /**
        * From      CSmlDmAdapter   UpdateLeafObjectL creates new leaf objects, or replaces 
        *                           data in existing leaf objects.
        * @since    Series60_3.0
        * @param    aURI            URI of the object
        * @param    aLUID           LUID of the object
        * @param    aStream         Data of the object
        * @param    aType           MIME type of the object
        * @param    aStatusRef      Reference to correct command
        * @return   none
        */
        void UpdateLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                            RWriteStream*& aStream, const TDesC8& aType, 
                            const TInt aStatusRef );
        
        /**
        * From      CSmlDmAdapter   FetchLeafObjectSizeL fetches the size of a leaf object.
        * @since    Series60_3.0
        * @param    aURI            URI of the object
        * @param    aLUID           LUID of the object
        * @param    aType           MIME type of the object
        * @param    aResultsRef    	Reference to correct results
        * @param    aStatusRef      Reference to correct command
        * @return   none
        */
	    void FetchLeafObjectSizeL( const TDesC8& aURI, const TDesC8& aLUID, 
                                const TDesC8& aType, const TInt aResultsRef, 
                                const TInt aStatusRef );

        /**
        * From      CSmlDmAdapter   The function implements execute command.
        * @since    Series60_3.0
        * @param    aURI            URI of the object
        * @param    aLUID           LUID of the object
        * @param    aArgument       Argument for the command
        * @param    aType       	MIME type of the object
        * @param    aStatusRef      Reference to correct command
        * @return   none
        */
	    void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, 
                            const TDesC8& aArgument, const TDesC8& aType, 
                            const TInt aStatusRef );

        /**
        * From      CSmlDmAdapter   The function implements execute command.
        * @since    Series60_3.0
        * @param    aURI            URI of the object
        * @param    aLUID           LUID of the object
        * @param    aStream         Argument for the command
        * @param    aType       	MIME type of the object
        * @param    aStatusRef      Reference to correct command
        * @return   none
        */
	    void ExecuteCommandL( const TDesC8& aURI, const TDesC8& aLUID, 
                            RWriteStream*& aStream, const TDesC8& aType, 
                            const TInt aStatusRef );
        
        /**
        * From      CSmlDmAdapter   The function implements execute command.
        * @since    Series60_3.0
        * @param    aTargetURI      Target URI for the command
        * @param    aTargetLUID     LUID of the target object
        * @param    aSourceURI      Source URI for the command
        * @param    aSourceLUID    	LUID of the source object
        * @param    aType           MIME type of the object
        * @param    aStatusRef      Reference to correct command
        * @return   none
        */
	    void CopyCommandL( const TDesC8& aTargetURI, const TDesC8& aTargetLUID, 
                        const TDesC8& aSourceURI, const TDesC8& aSourceLUID, 
                        const TDesC8& aType, TInt aStatusRef );
	
        /**
        * From      CSmlDmAdapter   The function indicates start of Atomic command.
        * @since    Series60_3.0
        * @param    none
        * @return   none
        */
        void StartAtomicL();

        /**
        * From      CSmlDmAdapter   The function indicates successful end of Atomic command.
        * @since    Series60_3.0
        * @param    none
        * @return   none
        */
	    void CommitAtomicL();
    
        /**
        * From      CSmlDmAdapter   The function indicates unsuccessful end of Atomic command.
        * @since    Series60_3.0
        * @param    none
        * @return   none
        */
	    void RollbackAtomicL();

        /**
        * From      CSmlDmAdapter   Returns ETrue if adapter supports streaming otherwise EFalse.
        * @since    Series60_3.0
        * @param    aItemSize       Size limit for stream usage.
        * @return   ETrue or EFalse
        */
	    TBool StreamingSupport( TInt& aItemSize );
        
        /**
        * From      CSmlDmAdapter   Called when stream returned from UpdateLeafObjectL or 
        *                           ExecuteCommandL has been written to and committed.
        * @since    Series60_3.0
        * @param    none
        * @return   none
        */
	    void StreamCommittedL();

        /**
        * From      CSmlDmAdapter   The function tells the adapter that all the commands of the message that
	    *                           can be passed to the adapter have now been passed.
        * @since    Series60_3.0
        * @param    none
        * @return   none
        */
	    void CompleteOutstandingCmdsL();
	
    private:
	    
        /**
        * C++ default constructor.
        */
        CNSmlDMSettingsAdapter();
        CNSmlDMSettingsAdapter( TAny* aEcomArguments );
	    
        /**
        * The function stores integer value to buffer and returns reference to it.
        * @param    aObject     data to be stored to buffer.
        * @return   reference to data buffer.
        */
        TDesC8& SetIntObjectLC( const TInt& aObject );
        
        /**
        * The function finds out the last element of the uri.
        * @param    aSource     Uri object that contains all elements.
        * @return   KErrNone if successful.
        */
	    TInt SetField( const TDesC8& aSource );

        /**
        * The function converts data to integer and returns it.
        * @param    aObject     Data object to be converted.
        * @return   Integer value for an object.
        */
	    TInt GetIntObject( const TDesC8& aObject );

        /**
        * The function converts data to integer and returns it.
        * @param    aObject     Data object to be converted.
        * @return   16-bit integer value for an object.
        */
	    TInt GetIntObject16( const TDesC& aObject );

        /**
        * The function converts LUID to integer.
        * @param    aLUID     LUID data to be converted.
        * @return   Integer value for a LUID.
        */    
	    TInt IntLUID( const TDesC8& aLUID );

        /**
        * The function sets the IAP value acording the given URI.
        * @param    aLUID     LUID for a profile.
        * @param    aObject   Data object containing the URI.
        * @return   KErrNone if successfull.
        */    
	    TInt  SetConRefL( const TInt aLUID, const TDesC8& aObject );
    
        /**
        * The function gets the URI for profile IAP.
        * @param    aObject   Object where the result will be stored.
        * @return   KErrNone if successfull.
        */    
	    TInt GetConRefL( CBufBase& aObject );
	
        /**
        * The function checks if field to be handled is valid.
        * @param    none
        * @return   ETrue if valid field otherwise EFalse.
        */    
    	TBool AcceptDMField();

        /**
        * The function checks if field type to be handled is valid.
        * @param    none
        * @return   enum value for a field type.
        */ 
	    TNSmlDMFieldType   GetDMFieldType() const;

        /**
        * The function returns enum value for a field to be handled.
        * @param    none
        * @return   enum value for a field to be handled.
        */ 
	    TNSmlDMProfileData GetDMField() const;

        /**
        * The function checks if data length is valid.
        * @param    aProfileItem    Field / Leaf ID to be handled.
        * @param    aSource         Data to be handled.
        * @return   ETrue if data length is valid otherwise EFalse.
        */ 
	    TBool NotValidStrLenght( const TNSmlDMProfileData& aProfileItem, 
                            const TDesC8& aSource );
        
        /**
        * The function converts 16-bit data to 8-bit and returns reference to it.
        * @param    aSource    Reference data to be converted.
        * @return   Reference to 8-bit data buffer.
        */ 
	    TDesC8&  ConvertTo8LC( const TDesC& aSource );

        /**
        * The function converts 8-bit data to 16-bit and returns reference to it.
        * @param    aSource    Reference data to be converted.
        * @return   Reference to 16-bit data buffer.
        */ 
	    TDesC16& ConvertTo16LC( const TDesC8& aSource );

        /**
        * The function find out the port number from URL and stores it to aPort.
        * @param    aRealURI    Reference data to be containing the whole URI.
        * @param    aPort       Reference variable where port number will be stored.
        * @return   KErrNone if successful otherwise KErrNotFound.
        */ 
	    TInt ParseUri( const TDesC8& aRealURI, TInt& aPort );

        /**
        * The function opens the server session and profile via client API.
        * @param    aIntLUID    Integer value for a LUID of the profile.
        * @param    aMode       Read or Read and Write mode for a profile data.
        * @return   KErrNone if successful otherwise KErrNotFound.
        */ 
        TInt OpenSyncSessionAndProfileL( const TInt aIntLUID, TSmlOpenMode aMode );

        /**
        * The function checks if requested profile ID exits.
        * @param    aIntLUID    Integer value for a LUID of the profile.
        * @return   ETrue if profile exits otherwise EFalse.
        */ 
	    TBool FindProfileIdL( const TInt aIntLUID );

        /**
        * The function creates new DM profile.
        * @param    aPID    Reference to variable where new profile will be stored.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt CreateNewProfileL( TInt& aPID );

        /**
        * The function stores the server URL for a profile.
        * @param    aPID    Profile ID.
        * @param    aObj    Reference to server URL data.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetProfileServerURIL( const TInt aPID, const TDesC8& aObj );

        /**
        * The function stores the display name for a profile.
        * @param    aPID    Profile ID.
        * @param    aObj    Reference to data.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetProfileDisplayNameL( const TInt aPID, const TDesC8& aObj );

        /**
        * The function stores the user name for a profile.
        * @param    aPID    Profile ID.
        * @param    aObj    Reference to data.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetProfileUserNameL( const TInt aPID, const TDesC8& aObj );

        /**
        * The function stores the protocol version for a profile.
        * @param    aPID    Profile ID.
        * @param    aObj    Reference to data.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetProfileProtocolVersionL( const TInt aPID, const TDesC8& aObj );

        /**
        * The function stores the ServerId value for a profile.
        * @param    aPID    Profile ID.
        * @param    aObj    Reference to data.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetProfileServerIdL( const TInt aPID, const TDesC8& aObj );

        /**
        * The function stores the Server password for a profile.
        * @param    aPID    Profile ID.
        * @param    aObj    Reference to data.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetProfileServerPasswordL( const TInt aPID, const TDesC8& aObj );

        /**
        * The function stores the password value for a profile.
        * @param    aPID    Profile ID.
        * @param    aObj    Reference to data.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetProfilePasswordL( const TInt aPID, const TDesC8& aObj );

        /**
        * The function stores the IAP id value for a profile.
        * @param    aLUID   Profile ID.
        * @param    aIAPid  Reference to data.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetProfileConnectionPropertyL( const TInt aLUID, const TInt aIAPid );

        /**
        * The function deleted the profile data.
        * @param    aPID    Profile ID.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt DeleteProfileL( const TInt aPID );
        
        /**
        * The function gets the profile data acoeding to aDMField.
        * @param    aLUID       Profile ID.
        * @param    aDMField    Data field ID to be handled.
        * @param    aObject     Reference where fetched data to be stored.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt GetObjectL( TInt aLUID, TNSmlDMProfileData aDMField, 
                        CBufBase& aObject );

        /**
        * The function adds port number to address URL.
        * @param    aLUID       Profile ID.
        * @param    aPort       Reference to port number data.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetProfileConnectionPortNrL( const TInt aLUID, const TDesC8& aPort );
        
        /**
        * The function adds port number to address URL.
        * @param    aSrvURI     Original address URI.
        * @param    aNewURI     New URI when port number has been added or deleted.
        * @param    aPort       Reference to port number data.
        * @return   KErrNone if successful otherwise error code.
        */ 
        void SetPortNrToAddrStr( const TDesC8& aSrvURI, HBufC8* aNewURI, 
                                const TDesC8& aPort );

        /**
        * General function which selects which leaf to be handled.
        * @param    aLUID       Profile LUID.
        * @param    aObject     Data to be stored.
        * @param    aField      Leaf of field ID to be handled.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetDMFieldDataL( const TInt aLUID, const TDesC8& aObject, 
                            const TNSmlDMProfileData aField );
        /**
        * The function fills the node info.
        * @param    aNode           Reference to node or leaf which info to be filled.
        * @param    aAccTypes       Access rights for a leaf / node.
        * @param    aOccurrence     Occurance of the node / leaf.
        * @param    aScope          Scope (dynamic / permanent) of the leaf / node.
        * @param    aFormat         Data format of the leaf / node.
        * @param    aDescription    Description of the node / leaf.
        * @param    aDefaultValue   Default value for a leaf or node.
        * @param    aMimeType       MIME type for a leaf / node.
        * @return   none
        */ 
        void FillNodeInfoL( MSmlDmDDFObject& aNode, TSmlDmAccessTypes aAccTypes, 
                        MSmlDmDDFObject::TOccurence aOccurrence, 
                        MSmlDmDDFObject::TScope aScope, 
                        MSmlDmDDFObject::TDFFormat aFormat, 
                        const TDesC8& aDescription, 
                        const TDesC8& aDefaultValue, const TDesC8& aMimeType );
        
        /**
        * The function reads the authentication data via private API.
        * @param    aLUID       LUID for a profile.
        * @param    aField      Leaf id to be handled.
        * @param    aObject     Reference where to store the data.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt GetDMAuthInfoL( const TInt aLUID, const TNSmlDMProfileData aField, 
                            CBufBase& aObject );

        /**
        * The function delete's the authentication data via private API.
        * @param    aLUID       LUID for a profile.
        * @param    aField      Leaf id to be handled.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt DeleteDMAuthInfoL( const TInt aLUID, const TNSmlDMProfileData aField );
        
        /**
        * The function set's the address type / medium type value for a profile.
        * @param    aLUID       LUID for a profile.
        * @param    aIntObj     Address type value.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetProfileAddrTypeL( const TInt aLUID, const TInt aIntObj );

        /**
        * The function gets the profile medium type value.
        * @param    aObject     Reference where data to be stored.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt GetProfileConnectiontypeL( CBufBase& aObject );

        /**
        * The function gets the profile IAP value.
        * @param    aIAPid     Reference where data to be stored.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt GetProfileIAPIdL( TInt& aIAPid );

        /**
        * The function gets the profile Server URL value.
        * @param    aURL     Reference where data to be stored.
        * @return   KErrNone if successful otherwise error code.
        */ 
        void GetProfileServerURLL( CBufBase& aURL );

        /**
        * The function checks the requested medium type if supported.
        * @param    aIntObj         Medium type.
        * @param    aMediumType     UID for medium type.
        * @return   none
        */         
        void GetMediumTypeL( const TInt aIntObj, TInt& aMediumType );

        /**
        * The function gets the server id value.
        * @param    aObject     Reference where data to be stored.
        * @return   none
        */       
        void GetServerIdL( CBufBase& aObject );

        /**
        * The function gets the profile name value.
        * @param    aObject     Reference where data to be stored.
        * @return   none
        */       
        void GetProfileNameL( CBufBase& aObject );

        /**
        * The function gets the user name value.
        * @param    aObject     Reference where data to be stored.
        * @return   none
        */ 
        void GetProfileUserNameL( CBufBase& aObject );

        /**
        * The function gets the profile port number value.
        * @param    aObject     Reference where data to be stored.
        * @return   none
        */ 
        void GetProfilePortNumberL( CBufBase& aObject );

        /**
        * The function sets the profile authentication info via private API.
        * @param    aLUID       Profile LUID.
        * @param    aField      Leaf ID to be handled.
        * @param    aObject     Data to be stored.
        * @param    aStatus     Status of the function, KErrNone if successful.
        * @return   none
        */         
        void SetAuthInfoL( const TInt aLUID, const TNSmlDMProfileData aField, 
                        const TDesC8& aObject, TInt& aStatus );
	//Buffers Node Object
	void AddNodeBufferL( const TDesC8& aURI, const TInt aStatusRef);
	//Buffers Leaf Object
	void AddLeafBufferL( const TDesC8& aURI, const TDesC8& aObject,
			const TInt aStatusRef);
	// Executes Buffer
	void ExecuteBufferL();
	//Extracts the ParentURI
	TPtrC8 ParentURI(const TDesC8& aURI);
	// Clears the elements of the passed in LeafElement Structure
	void ClearBuffer( CArrayFixFlat<TNSmlDMLeafElement>* aBuffer);
    private:    // Data
	
        // Client API session class 
        RSyncMLSession iSyncSession;
        // Client API Device Management profile handling
        RSyncMLDevManProfile iProfile;
        // Callback interface for returning result or status
	    MSmlDmCallback* iCallBack;
	    
	    TInt iCurrentProfile;
	    
        // Profile LUID
        TInt iLUID;
        // Parent LUID
	    TInt iParentLUID;
        // For handling profile medium type 
	    TInt iObject;
                  
        // Buffer for URI leaf element
	    HBufC8* iField;
		
        // Information about data type
	    TNSmlDMFieldType  iFieldType;
        // Information about leaf command
	    TNSmlDMLeafType	  iLeafType;
        // Status information if session allready opened
        TBool iSyncSessionOpen;

        // Buffer for storing port number
        HBufC8* iPortNbr;
    
        // Private API interface for handling authebtication data
        RNSmlPrivateAPI iPrivateApi;
        
        struct TPortBuffer
            {
            // buffered data for updating port number
            TBufC8<5> iPortBuf;
            TInt iPortBufLUID;
            TInt iPortBufStatusRef;
            };
        RArray<TPortBuffer> iPortBuffer;    
	TBool iBufOn;
	TBool iComplete;
	TInt iExecutionIndex;
	CArrayFixFlat<TNSmlDMBufferElement> *iBuffer;
    };

#endif // __NSMLDMSETTINGSADAPTER_H__

// End of File

