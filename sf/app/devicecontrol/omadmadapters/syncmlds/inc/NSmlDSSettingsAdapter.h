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
* Description:    Device Management DS Settings adapter header file
*
*/




#ifndef __NSMLDSSETTINGSADAPTER_H__
#define __NSMLDSSETTINGSADAPTER_H__

// INCLUDES
#include <smldmadapter.h>   //adapter interface
#include <SyncMLClientDS.h> //DS Client API

// CONSTANTS
const TInt KNSmlDSGranularity = 4;
const TUint8 KNSmlDMColon = ':';
const TUint8 KNSmlDMUriSeparator = '/';

//DDF accepted DS-fieldnames for DS-profile
_LIT8( KNSmlDdfRoot,            "SyncML");
_LIT8( KNSmlDdfAddr,				"Addr" );
_LIT8( KNSmlDdfAddrType,			"AddrType" );
_LIT8( KNSmlDdfPortNbr,			"PortNbr" );
_LIT8( KNSmlDdfName,				"Name" );

_LIT8( KNSmlDdfDB,				"DB" );
_LIT8( KNSmlDdfCTType,			"CTType" );
_LIT( KNSmlDdfCTVer,			"CTVer" );
_LIT8( KNSmlDdfCTVerL,			"CTVerL" );

_LIT8( KNSmlDdfRDBURI,			"RDBURI" );
_LIT8( KNSmlDdfLDBURI,			"LDBURI" );

_LIT8( KNSmlDdfClientUserName,	"ClientUserName" );
_LIT8( KNSmlDdfClientPW,			"ClientPW" );
_LIT( KNSmlDdfClientNonce,		"ClientNonce" );
_LIT8( KNSmlDdfServerId,			"ServerId" );
_LIT8( KNSmlDdfHidden,			"Hidden" );


_LIT( KNSmlADdfAdapterId,		"AdapterId" );

_LIT8( KNSmlDdfToNAPID,			"ToNapID" );
_LIT( KNSmlDdfAdapterType,		"/DB/" );

//DDF DS-field descriptions
_LIT8( KNSmlDdfRootDescription,     "DS-Settings DDF description");
_LIT8( KNSmlDdfAddrDescription,	    "DS server address" );
_LIT8( KNSmlDdfAddrTypeDescription,	"Type of used DS server address" );
_LIT8( KNSmlDdfPortNbrDescription,	"DS Server port number" );
_LIT8( KNSmlDdfNameDescription,		"Displayable name for the management account" );

_LIT8( KNSmlDdfDBDescription,		"DB node is parent to all Database objects" );
_LIT( KNSmlDdfCTDescription,		"CT node is parent to all Content objects" );
_LIT8( KNSmlDdfCTTypeDescription,	"CTType define supported media content of database" );
_LIT( KNSmlDdfCTVerDescription,		"CTVer node is a parent to all Content Version objects" );
_LIT( KNSmlDdfCTVerLDescription,	"CTVerL define version of supported contenttype" );

_LIT8( KNSmlDdfRDBURIDescription,	"The relative or absolute URI of remote database" );
_LIT8( KNSmlDdfLDBURIDescription,	"The relative or absolute URI of local database" );

_LIT8( KNSmlDdfClientUserNameDescription,	"DS username" );
_LIT8( KNSmlDdfServerIdDescription,     	"Server identifier" );
_LIT8( KNSmlDdfClientPWDescription,			"A password or secret to authenticate to the server" );
_LIT8( KNSmlDdfToNAPIDDescription,			"Logical reference to connectivity information" );
_LIT8( KNSmlDdfHiddenDescription,			"Hidden profile (not shown on UI)" );

_LIT8( KNSmlBufExe,				"BufExe" );

//Default data values
_LIT( KNSmlDefName,				"DSAdapterIns" );
_LIT8( KNSmlDefDSAcc,			"DSAcc" );
_LIT8( KNSmlDSStart,			"SyncML/DSAcc/DSId" );
_LIT( KNSmlDSStartDB,			"/DB/" );
_LIT( KNSmlDefDSIAP,			"AP" );
_LIT8( KNSmlIAPId,			    "NSmlIapId" );
_LIT8( KSegmDSAcc2,              "CTType/RDBURI/LDBURI" );

_LIT8( KNSmlDSSettingsHTTP,  "http://" );
_LIT8( KNSmlDSSettingsHTTPS, "https://" );
const TInt KNSmlDSSettingsHTTPPort = 80;
const TInt KNSmlDSSettingsHTTPSPort = 443;
_LIT8( KNSmlDSUriDotSlash, "./");
_LIT8( KNSmlDSSlash, "/");
_LIT8(KVersion, "1.1");
_LIT8( KMimeType, "text/plain" );
_LIT8( KDSSettingsTitle, "DS-settings title" );

_LIT8( KDBUri1, "SyncML/DSAcc/*/DB/*" );
_LIT8( KDSAccMatch, "SyncML/DSAcc/*" );
_LIT8( KDSAccMatch2, "SyncML/DSAcc/*/*" );
_LIT8( KDSDBMatch, "SyncML/DSAcc/*/DB/*" );
_LIT8( KDSDBMatch2, "SyncML/DSAcc/*/DB" );
_LIT8( KSegmDSAcc,"Addr/AddrType/PortNbr/Name/DB/ClientUserName/ClientPW/ToNapID/ServerId" );
_LIT8( KDSDBAddMatch, "*DSAcc/*/DB/*" );
_LIT8( KDSDBAddMatch2, "*DSAcc/*/DB/*/*" );
_LIT8( KDSDBAddMatch3, "*DSAcc/*/DB" );

_LIT8( KCTTypeMatch, "*/CTType*" );
_LIT8( KRDBUriMatch, "*/RDBURI*" );
_LIT8( KLDBUriMatch, "*/LDBURI*" );

_LIT8( KDummyTxt, "/Dummy" );
_LIT8( KFormat, "%d" );
_LIT8( KDSAcc1, "SyncML/DSAcc" );
_LIT8( Kprev,"CTId" );
_LIT8( Kprev2, "/CTId" );
_LIT8( KDSprev,"DSId" );


// DATA TYPES
enum TNSmlDSFieldType
	{
	EStr,
	EInt,
	EParam,
	EWrong
	};

enum TNSmlDSLeafType
	{
	EDSDelete,
	EDSUnset
	};

enum TNSmlDSProfileDataID
	{
	EProfileName,
	EProfileIAPId,
	EProfileMediumType,
	EProfileSyncServerUsername,
	EProfileSyncServerPassword,
	EProfileURL,
	EProfilePort,
	EProfileServerId,
	EHiddenProfile,
	ESyncAccepted
	};

enum TNSmlDSMediumType
	{
	ENone,
    EHttp,
	EWsp,
    EObex
	};
	
struct TNSmlDSAddElement
	{
	HBufC8  *iUri;
	HBufC8 *iData;
	TInt iStatusRef;
	TBool iLeaf;
	TBool iDone;
	};

struct TNSmlDSBufferElement
	{
	CArrayFixFlat<TNSmlDSAddElement> *iNodeBuf;
	HBufC8* iMappingName;
	HBufC8* iName;
	TBool iExecuted;
	TInt  iLuid;
	};

// FORWARD DECLARATIONS
class CNSmlDSSettingsAdapter;

/**
*  CNSmlDSSettingsAdapter class 
*  Contains the whole implementation of the Device Management DS settings adapter.
*
*  @lib nsmldssettingsadapter.dll
*/
class CNSmlDSSettingsAdapter : public CSmlDmAdapter
	{
    public: // Constructors and destructor
	    /**
        * Two-phased constructor.
        */
        static CNSmlDSSettingsAdapter* NewL(MSmlDmCallback* aDmCallback );
	    static CNSmlDSSettingsAdapter* NewLC(MSmlDmCallback* aDmCallback );

	    void ConstructL();
    
        /**
        * Destructor.
        */
	    virtual ~CNSmlDSSettingsAdapter();

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
                                const TInt aStatusRef );
        
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
	    void FetchLeafObjectL( const TDesC8& aURI, const TDesC8& aLUID, 
                                const TDesC8& aType, const TInt aResultsRef, 
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
        CNSmlDSSettingsAdapter();
	    CNSmlDSSettingsAdapter( TAny* aEcomArguments );
        
        /**
        * The function checks if field to be handled is valid.
        * @param    none
        * @return   ETrue if valid field otherwise EFalse.
        */   
	    TBool AcceptDSField();

        /**
        * The function returns enum value for a field to be handled.
        * @param    none
        * @return   enum value for a field to be handled.
        */ 
        TInt GetDSField() const;

        /**
        * The function checks if field type to be handled is valid.
        * @param    none
        * @return   enum value for a field type.
        */ 
        TInt GetDSFieldTypeL() const;

        /**
        * The function converts LUID to integer.
        * @param    aLUID     LUID data to be converted.
        * @return   Integer value for a LUID.
        */    
        TInt IntLUID(const TDesC8& aLUID);

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
        * The function stores integer value to buffer and returns reference to it.
        * @param    aObject     data to be stored to buffer.
        * @return   reference to data buffer.
        */
        TDesC8& SetIntObjectLC( const TInt& aObject );

        /**
        * The function checks if data length is valid.
        * @param    aProfileItem    Field / Leaf ID to be handled.
        * @param    aSource         Data to be handled.
        * @return   ETrue if data length is not valid otherwise EFalse.
        */ 
        TBool NotValidStrLenght( const TInt& aProfileItem, const TDesC8& aSource );

        /**
        * The function finds out the last element of the uri.
        * @param    aSource     Uri object that contains all elements.
        * @return   KErrNone if successful.
        */
        TInt    SetField( const TDesC8& aSource );

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
        * The function adds new dataprovider for a profile.
        * @param    aIntLUID        Profile integer LUID.
        * @param    aMIMEType       Requested MIME type of the data provider.
        * @param    aDataProviderId Reference to object where new data provider id to be saved.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt AddNewDataProviderL( TInt aIntLUID, const TDesC8& aMIMEType, TInt& aDataProviderId );

        /**
        * The function gets the uri for profile IAP ID.
        * @param    aLUID       Profile integer LUID.
        * @param    aObject     Reference to object where URI to be stored.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt GetToNAPIDL( const TInt aLUID, CBufBase& aObject );

        /**
        * The function sets the profile IAP id based on URI.
        * @param    aLUID       Profile integer LUID.
        * @param    aObject     Access point URI.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetToNAPIDL( const TInt aLUID, const TDesC8& aObject );

        /**
        * The function sets the new node to buffer (in case of content type).
        * @param    aURI            URI of the node.
        * @param    aParentLUID     LUID for the parent object.
        * @param    aStatusRef      Command status reply reference.
        * @return   none
        */ 
        void AddNodeBufferL( const TDesC8& aURI, const TDesC8& aParentLUID, 
                                const TInt aStatusRef );

        /**
        * The function sets new leaf object to buffer (in case DB level object).
        * @param    aURI            URI of the node.
        * @param    aParentLUID     LUID for the parent object.
        * @param    aStatusRef      Command status reply reference.
        * @return   none
        */ 
        void AddLeafBufferL( const TDesC8& aURI,const TDesC8& aParentLUID, 
                                const TDesC8& aObject, const TInt aStatusRef );

        /**
        * The function finds out the last segment of the URI.
        * @param    aURI        URI to be handled.
        * @return   Pointer to last URI segment.
        */ 
        TPtrC8 LastURISeg( const TDesC8& aURI );

        /**
        * The function removes the last segment of the URI.
        * @param    aURI        URI to be handled.
        * @return   Pointer to recontructed URI.
        */ 
        TPtrC8 RemoveLastURISeg( const TDesC8& aURI );

        /**
        * The function executes command from the buffer based on URI.
        * @param    aURI        URI to be handled first.
        * @return   none
        */ 
        void ExecuteBufferL( const TDesC8& aURI );

        /**
        * The function gets the profile Id (LUID) for given URI.
        * @param    aURI        URI which LUID to be fetched.
        * @return   LUID for the URI.
        */ 
        TInt GetProfileIdFromURIL( const TDesC8& aURI );

        /**
        * The function gets the MIME type for given data provider.
        * @param    aDataProviderId Data provider ID.
        * @param    aProfileLUID    Profile LUID.
        * @param    aObject         Reference where fetched MIME type to be stored.
        * @return   KErrNone if successful otherwise error cose.
        */ 
        TInt GetContentTypeL( const TInt aDataProviderId, const TInt aProfileLUID, 
                                CBufBase*& aObject );

	    /**
        * The function find out the buffer index for given URI.
        * @param    aURI        URI which position in the buffer to be solved.
        * @return   Buffer position for given URI. If not found value is -1.
        */ 
        TInt GetBufferIndex( const TDesC8& aURI );

        /**
        * The function sets the LUID to buffer for given URI.
        * @param    aURI        URI which position in the buffer to be solved.
        * @param    aLUID       LUID for an URI.
        * @return   Value > 0 if URI found, otherwise value is 0.
        */ 
	    TInt SetBufferLUID( const TDesC8& aURI, const TInt aLUID );

        /**
        * The function find out the port number from URL and stores it to aPort.
        * @param    aRealURI    Reference data to be containing the whole URI.
        * @param    aPort       Reference variable where port number will be stored.
        * @return   KErrNone if successful otherwise KErrNotFound.
        */ 
	    TInt  ParseUri( const TDesC8& aRealURI, TInt& aPort );
        
        /**
        * The function checks if requested profile ID exits.
        * @param    aIntLUID    Integer value for a LUID of the profile.
        * @return   ETrue if profile exits otherwise EFalse.
        */ 
	    TBool FindProfileIdL( const TInt aIntLUID );

        /**
        * The function gets the profile ID from Client API.
        * @param    aIntLUID    Integer value for a LUID of the profile.
        * @param    aProfileID  Reference to object where profile ID to be stored.
        * @return   KErrNone if successful otherwise KErrNotFound.
        */ 
        TInt GetProfileIdentifierL( const TInt aIntLUID, TInt& aProfileID );

        /**
        * The function gets the profile data acording to URI leaf.
        * @param    aIntLUID    Profile ID.
        * @param    aObject     Reference where fetched data to be stored.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt GetDSFieldDataL( const TInt aIntLUID, CBufBase*& aObject );

        /**
        * The function gets the profile medium type value.
        * @param    aIntLUID     Reference where data to be stored.
        * @return   Medium type for profile connection.
        */ 
        TInt GetProfileConnectiontypeL( const TInt aIntLUID );

        /**
        * The function gets the profile Addr leaf value.
        * @param    aIntLUID    Profile ID.
        * @param    aURL        Reference where fetched data to be stored.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt GetProfileServerURLL( const TInt aIntLUID, CBufBase*& aURL );

        /**
        * The function gets the profile user name value.
        * @param    aIntLUID    Profile ID.
        * @param    aUserName   Reference where fetched data to be stored.
        * @return   KErrNone if successful otherwise error code.
        */ 
	    TInt GetProfileUserNameL( const TInt aIntLUID, CBufBase*& aUserName );

        /**
        * The function gets the profile user name value.
        * @param    aIntLUID    Profile ID.
        * @param    aServerId   Reference where fetched data to be stored.
        * @return   KErrNone if successful otherwise error code.
        */ 
	    TInt GetProfileServerIdL( const TInt aIntLUID, CBufBase*& aServerId );

        /**
        * The function gets the profile IAP Id (ToNAPID) value.
        * @param    aIntLUID    Profile ID.
        * @param    aIAPid      Reference where fetched data to be stored.
        * @return   KErrNone if successful otherwise error code.
        */ 
	    TInt GetProfileIAPIdL( const TInt aIntLUID, TInt& aIAPid );		

        /**
        * The function gets the profile Name value.
        * @param    aIntLUID        Profile ID.
        * @param    aDisplayName    Reference where fetched data to be stored.
        * @return   KErrNone if successful otherwise error code.
        */ 
	    TInt GetProfileDisplayNameL( const TInt aIntLUID, CBufBase*& aDisplayName );

        /**
        * The function creates new DM profile.
        * @param    aPID    Reference to variable where new profile Id will be stored.
        * @return   KErrNone if successful otherwise error code.
        */ 
	    TInt CreateNewProfileL( TInt& aPID );

        /**
        * The function sets the profile Addr leaf value.
        * @param    aPID        Profile ID.
        * @param    aObj        Data to be stored for a profile.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetProfileServerURIL( const TInt aPID, const TDesC8& aObj );

        /**
        * The function sets the profile Name leaf value.
        * @param    aPID        Profile ID.
        * @param    aObj        Data to be stored for a profile.
        * @return   KErrNone if successful otherwise error code.
        */     
        TInt SetProfileDisplayNameL( const TInt aPID, const TDesC8& aObj );

        /**
        * The function sets the profile UserName leaf value.
        * @param    aPID        Profile ID.
        * @param    aObj        Data to be stored for a profile.
        * @return   KErrNone if successful otherwise error code.
        */ 
	    TInt SetProfileUserNameL( const TInt aPID, const TDesC8& aObj );

        /**
        * The function sets the profile UserName leaf value.
        * @param    aPID        Profile ID.
        * @param    aObj        Data to be stored for a profile.
        * @return   KErrNone if successful otherwise error code.
        */ 
	    TInt SetProfileServerIdL( const TInt aPID, const TDesC8& aObj );

        /**
        * The function sets the profile password leaf value.
        * @param    aPID        Profile ID.
        * @param    aObj        Data to be stored for a profile.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetProfilePasswordL( const TInt aPID, const TDesC8& aObj );

        /**
        * General function that checks which leaf to be handled based on field id.
        * @param    aPID        Profile ID.
        * @param    aObj        Data to be stored for a profile.
        * @param    aFieldID    Internal value for a leaf to be handled.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetProfileStrValueL( const TInt aPID, const TDesC8& aObj, 
                                    const TInt aFieldID );

        /**
        * The function deletes the profile data.
        * @param    aPID    Profile ID.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt DeleteProfileL( const TInt aPID );

        /**
        * The function creates list of the content types that profile supports.
        * @param    aURI                    URI that contains content types.
        * @param    aPID                    Profile ID.
        * @param    aCurrentURISegmentList  Reference to list to be filled.
        * @param    aPreviousURISegmentList Previous, existing list for content types.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt CreateURITaskSegmentListL( const TDesC8& aURI, const TInt aPID, 
                                    CBufBase*& aCurrentURISegmentList, 
                                    const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList );

        /**
        * The function creates list of the existing profiles.
        * @param    aCurrentURISegmentList  Reference to list to be filled.
        * @param    aPreviousURISegmentList Previous, existing list for profiles.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt CreateURIProfileSegmentListL( CBufBase*& aCurrentURISegmentList, 
                                    const CArrayFix<TSmlDmMappingInfo>& aPreviousURISegmentList );
        
        /**
        * The function opens the server session and profile via client API.
        * @param    aIntLUID    Integer value for a LUID of the profile.
        * @param    aMode       Read or Read and Write mode for a profile data.
        * @return   KErrNone if successful otherwise KErrNotFound.
        */ 
        TInt OpenSyncSessionAndDSProfileL( const TInt aIntLUID, TSmlOpenMode aMode );

        /**
        * The function closes the DS profile session.
        * @param    none.
        * @return   none.
        */ 
	    void CloseDSProfile();

        /**
        * The function checks if profile has the requested task id.
        * @param    aProfLUID       Integer value for a LUID of the profile.
        * @param    aDataProviderId DataProvider id to be searched.
        * @return   ETrue if task found otherwise EFalse.
        */ 
        TBool FindTaskIDL( const TInt aProfLUID, const TInt aDataProviderId );

        /**
        * The function deletes task for given profile.
        * @param    aLUID           Integer value for a LUID of the profile.
        * @param    aDataProviderId DataProvider id to be searched.
        * @return   KErrNone if successful otherwise error code.
        */ 
	    TInt DeleteTaskByIDL( const TInt aLUID, const TInt aDataProviderId );

        /**
        * The function gets the LDBURI (local datastore) for given profile.
        * @param    aDataProviderId DataProvider id to be searched.
        * @param    aProfileLUID    Profile Id.
        * @param    aObject         Reference to object where fetched value to be saved.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt GetClientDataSourceL( const TInt aDataProviderId, const TInt aProfileLUID, 
                                    CBufBase*& aObject );
        /**
        * The function gets the RDBURI (remote datastore) for given profile.
        * @param    aDataProviderId DataProvider id to be searched.
        * @param    aProfileLUID    Profile Id.
        * @param    aObject         Reference to object where fetched value to be saved.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt GetServerDataSourceL( const TInt aDataProviderId, const TInt aProfileLUID, 
                                    CBufBase*& aObject );

        /**
        * The function replaces the LDBURI or RDBURI values for given profile.
        * @param    aProfileLUID    Profile Id.
        * @param    aDataProviderId DataProvider id to be searched.
        * @param    aObject         Data to be stored.
        * @param    aField          Field Id to be handled.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt UpdateDataSourceL( const TInt aProfileLUID, const TInt aDataProviderId, 
                                    const TDesC8& aObject, const HBufC8& aField );

        /**
        * The function fetches the object (leaf) data based on given URI.
        * @param    aURI            URI of the object
        * @param    aLUID           LUID of the object
        * @param    aObj            Reference to object where fetched data to be saved.
        * @param    aStatusRef      Reference to correct command
        * @return   none
        */
        TInt FetchObjectL( const TDesC8& aURI, const TDesC8& aLUID, CBufBase*& aObj, 
                                    const TInt aStatusRef );
        
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
        * The function set's the address type / medium type value for a profile.
        * @param    aLUID       LUID for a profile.
        * @param    aIntObj     Address type value.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetProfileAddrTypeL( const TInt aLUID, const TInt aIntObj );

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
        * The function checks if MIME type allready exits.
        * @param    aLuid       Profile LUID.
        * @param    aMimeType   MIME type to be searched.
        * @return   ETrue if MIME type found else EFalse.
        */ 
        TBool MimeTypeAllreadyExitsL( const TInt aLuid, const TDesC8& aMimeType );

        /**
        * The function sets the RDBURI (remote datastore) to memeber variable.
        * @param    aStore       Value for a RDBURI.
        * @return   none.
        */ 
        void SetRDBDataStore( const TDesC8& aStore );

        /**
        * The function sets the IAP Id value to profile connection data.
        * @param    aLuid       Profile LUID.
        * @param    aIAPid      Id for an IAP.
        * @return   KErrNone if successful otherwise error code.
        */ 
        TInt SetProfileIAPIdL( const TInt aLUID, const TInt aIAPid );

        /**
        * The function gets UID for the profile medium type.
        * @param    aIntObj         Profile LUID.
        * @param    aMediumType     Requested medium type.
        * @return   none.
        */ 
        void GetMediumTypeL(const TInt aIntObj, TSmlTransportId& aMediumType);
        
        /**
        * The function checks if the URI is of format ./SyncML/DMAcc/DMId<num>
        * Example ./SyncML/DMAcc/DMId000 
        * @param    aURI       aURI.
        * @return   true/false
        */                          
        TBool IsDSAccUriFormatMatchPredefined(const TDesC8 & aURI);
        
         /**
        * The function constructs ./SyncML/DMAcc/x nodes and
        * returns the profile ID matching the aURI.
        * @param    aURI       aURI.
        * @return   KErrNotFound/ProfileID
        */  
        
        TInt ConstructTreeL(const TDesC8& aURI);

   /**
        * The function extracts Syncml/DSAcc/x node from aURI		        
        * @param    aURI     Syncml/DSAcc aURI.
        * @return   Syncml/DSAcc/x node.
    */ 
		TPtrC8 GetDynamicDSNodeUri(const TDesC8& aURI);

    private:    // Data
	    
        // Callback interface for returning result or status
	    MSmlDmCallback*	iCallBack;
        // Client API session class 
        RSyncMLSession iSyncSession;
        // Client API DS profile handling
        RSyncMLDataSyncProfile iDSProfile;
  
        // Buffer for URI leaf element
        HBufC8* iField;
        // Pointer to buffer structure
        CArrayFixFlat<TNSmlDSBufferElement> *iBuffer;
        
        // Buffer for URI leaf element
        HBufC8* iPrevURI;
        // Buffer for profile port number handling
        HBufC8* iPortNbr;
        // Buffer for profile remote data store handling
        HBufC16* iRdbDataStore;
        // Information about leaf command
        TNSmlDSLeafType	  iLeafType;

        // LUID to be handled
	    TInt iLUID;
        // Parent LUID
	    TInt iParentLUID;
        // For handling profile medium type 
	    TInt iObject;
        // Previously handled LUID
        TInt iPrevLUID;
        
        // Status information if session allready opened
        TBool iSyncSessionOpen;
        // Status of command was inside the module from buffer execution
        TBool iBufferExecution;
        
        // buffered data for updating port number
        HBufC8* iPortBuf;
        TInt iPortBufLUID;
        TInt iPortBufStatusRef;
   };

#endif // __NSmlDSSETTINGSADAPTER_H__

// End of File