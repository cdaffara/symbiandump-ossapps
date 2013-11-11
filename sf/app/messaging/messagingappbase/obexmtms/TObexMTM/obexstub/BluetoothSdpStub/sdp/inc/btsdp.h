// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef BTSDP_H
#define BTSDP_H

#include <e32base.h>
#include <bttypes.h>

static const TUid KUidServiceSDP			= {0x100095FA};

typedef TUint16 TSdpAttributeID;

// Universal Attributes
static const TSdpAttributeID KSdpAttrIdServiceRecordHandle = 0x0000;
static const TSdpAttributeID KSdpAttrIdServiceClassIDList = 0x0001;
static const TSdpAttributeID KSdpAttrIdServiceRecordState = 0x0002;
static const TSdpAttributeID KSdpAttrIdServiceID = 0x0003;
static const TSdpAttributeID KSdpAttrIdProtocolDescriptorList = 0x0004;
static const TSdpAttributeID KSdpAttrIdBrowseGroupList = 0x0005;
static const TSdpAttributeID KSdpAttrIdLanguageBaseAttributeIDList = 0x0006;
static const TSdpAttributeID KSdpAttrIdServiceInfoTimeToLive = 0x0007;
static const TSdpAttributeID KSdpAttrIdServiceAvailability = 0x0008;
static const TSdpAttributeID KSdpAttrIdBluetoothProfileDescriptorList = 0x0009;
static const TSdpAttributeID KSdpAttrIdDocumentationURL = 0x000A;
static const TSdpAttributeID KSdpAttrIdClientExecutableURL = 0x000B;
static const TSdpAttributeID KSdpAttrIdIconURL = 0x000C;

// Language specific attributes
static const TSdpAttributeID KSdpAttrIdBasePrimaryLanguage = 0x0100;
static const TInt KSdpAttrIdOffsetServiceName = 0x0000; // + Language offset
static const TInt KSdpAttrIdOffsetServiceDescription = 0x0001; // + Language offset
static const TInt KSdpAttrIdOffsetProviderName = 0x0002; // + Language offset

// SDP server specific attributes
static const TSdpAttributeID KSdpAttrIdSdpServerVersionNumberList = 0x0200;
static const TSdpAttributeID KSdpAttrIdSdpServerServiceDatabaseState = 0x0201;
static const TSdpAttributeID KSdpAttrIdSdpServerGroupID = 0x0200;

// UUIDS (add to these as necessary)
static const TUint32	KSdpContTestUUID = 0x100;
static const TInt KSerialPortUUID = 0x1101;
static const TInt KDialUpNetworkingUUID = 0x1103;
static const TInt KGenericNetworkingUUID = 0x1201;
static const TInt KFaxUUID = 0x1111;
static const TInt KGenericTelephonyUUID = 0x1204;
static const TInt KObexUUID = 0x1105;
static const TInt KProtocolDescriptorListUUID = 0x0004;
static const TInt KPublicBrowseGroupUUID = 0x1002;
static const TInt KBrowseGroupDescriptorServiceClassUUID = 0x1001;
static const TInt KServiceDiscoveryServerServiceClassUUID = 0x1000;

// Language codes, as per "ISO 639:1988 (E/F)"
static const TInt16 KLanguageAfar = 0x6161; // aa
static const TInt16 KLanguageAbkhazian = 0x6162; // ab
static const TInt16 KLanguageAfrikaans = 0x6166; // af
static const TInt16 KLanguageAmharic = 0x616D; // am
static const TInt16 KLanguageArabic = 0x6172; // ar
static const TInt16 KLanguageAssamese = 0x6173; // as
static const TInt16 KLanguageAymara = 0x6179; // ay
static const TInt16 KLanguageAzerbaijani = 0x617A; // az
static const TInt16 KLanguageBashkir = 0x6261; // ba
static const TInt16 KLanguageByelorussian = 0x6265; // be
static const TInt16 KLanguageBulgarian = 0x6267; // bg
static const TInt16 KLanguageBihari = 0x6268; // bh
static const TInt16 KLanguageBislama = 0x6269; // bi
static const TInt16 KLanguageBengali = 0x626E; // bn - also Bangala
static const TInt16 KLanguageTibetan = 0x626F; // bo
static const TInt16 KLanguageBreton = 0x6272; // br
static const TInt16 KLanguageCatalan = 0x6361; // ca
static const TInt16 KLanguageCorsican = 0x636F; // co
static const TInt16 KLanguageCzech = 0x6373; // cs
static const TInt16 KLanguageWelsh = 0x6379; // cy
static const TInt16 KLanguageDanish = 0x6461; // da
static const TInt16 KLanguageGerman = 0x6465; // de
static const TInt16 KLanguageBhutani = 0x647A; // dz
static const TInt16 KLanguageGreek = 0x656C; // el
static const TInt16 KLanguageEnglish = 0x656E; // en
static const TInt16 KLanguageEsperanto = 0x656F; // eo
static const TInt16 KLanguageSpanish = 0x6573; // es
static const TInt16 KLanguageEstonian = 0x6574; // et
static const TInt16 KLanguageBasque = 0x6575; // eu
static const TInt16 KLanguagePersian = 0x6661; // fa
static const TInt16 KLanguageFinnish = 0x6669; // fi
static const TInt16 KLanguageFiji = 0x666A; // fj
static const TInt16 KLanguageFaroese = 0x666F; // fo
static const TInt16 KLanguageFrench = 0x6672; // fr
static const TInt16 KLanguageFrisian = 0x6679; // fy
static const TInt16 KLanguageIrish = 0x6761; // ga
static const TInt16 KLanguageScotsGaelic = 0x6764; // gd
static const TInt16 KLanguageGalician = 0x676C; // gl
static const TInt16 KLanguageGuarani = 0x676E; // gn
static const TInt16 KLanguageGujarati = 0x6775; // gu
static const TInt16 KLanguageHausa = 0x6861; // ha
static const TInt16 KLanguageHebrew = 0x6865; // he - formerly iw
static const TInt16 KLanguageHindi = 0x6869; // hi
static const TInt16 KLanguageCroatian = 0x6872; // hr
static const TInt16 KLanguageHungarian = 0x6875; // hu
static const TInt16 KLanguageArmenian = 0x6879; // hy
static const TInt16 KLanguageInterlingua = 0x6961; // ia
static const TInt16 KLanguageIndonesian = 0x6964; // id - formerly in
static const TInt16 KLanguageInterlingue = 0x6965; // ie
static const TInt16 KLanguageInupiak = 0x696B; // ik
static const TInt16 KLanguageIcelandic = 0x6973; // is
static const TInt16 KLanguageItalian = 0x6974; // it
static const TInt16 KLanguageInuktitut = 0x6975; // iu
static const TInt16 KLanguageJapanese = 0x6A61; // ja
static const TInt16 KLanguageJavanese = 0x6A77; // jw
static const TInt16 KLanguageGeorgian = 0x6B61; // ka
static const TInt16 KLanguageKazakh = 0x6B6B; // kk
static const TInt16 KLanguageGreenlandic = 0x6B6C; // kl
static const TInt16 KLanguageCambodian = 0x6B6D; // km
static const TInt16 KLanguageKannada = 0x6B6E; // kn
static const TInt16 KLanguageKorean = 0x6B6F; // ko
static const TInt16 KLanguageKashmiri = 0x6B73; // ks
static const TInt16 KLanguageKurdish = 0x6B75; // ku
static const TInt16 KLanguageKirghiz = 0x6B79; // ky
static const TInt16 KLanguageLatin = 0x6C61; // la
static const TInt16 KLanguageLingala = 0x6C6E; // ln
static const TInt16 KLanguageLaothian = 0x6C6F; // lo
static const TInt16 KLanguageLithuanian = 0x6C74; // lt
static const TInt16 KLanguageLatvian = 0x6C76; // lv - aslo Lettish
static const TInt16 KLanguageMalagasy = 0x6D67; // mg
static const TInt16 KLanguageMaori = 0x6D69; // mi
static const TInt16 KLanguageMacedonian = 0x6D6B; // mk
static const TInt16 KLanguageMalayalam = 0x6D6C; // ml
static const TInt16 KLanguageMongolian = 0x6D6E; // mn
static const TInt16 KLanguageMoldavian = 0x6D6F; // mo
static const TInt16 KLanguageMarathi = 0x6D72; // mr
static const TInt16 KLanguageMalay = 0x6D73; // ms
static const TInt16 KLanguageMaltese = 0x6D74; // mt
static const TInt16 KLanguageBurmese = 0x6D79; // my
static const TInt16 KLanguageNauru = 0x6E61; // na
static const TInt16 KLanguageNepali = 0x6E65; // ne
static const TInt16 KLanguageDutch = 0x6E6C; // nl
static const TInt16 KLanguageNorwegian = 0x6E6F; // no
static const TInt16 KLanguageOccitan = 0x6F63; // oc
static const TInt16 KLanguageOromo = 0x6F6D; // om - (Afan)
static const TInt16 KLanguageOriya = 0x6F72; // or
static const TInt16 KLanguagePunjabi = 0x7061; // pa
static const TInt16 KLanguagePolish = 0x706C; // pl
static const TInt16 KLanguagePashto = 0x7073; // ps
static const TInt16 KLanguagePortuguese = 0x7074; // pt
static const TInt16 KLanguageQuechua = 0x7175; // qu
static const TInt16 KLanguageRhaetoRomance = 0x726D; // rm
static const TInt16 KLanguageKirundi = 0x726E; // rn
static const TInt16 KLanguageRomanian = 0x726F; // ro
static const TInt16 KLanguageRussian = 0x7275; // ru
static const TInt16 KLanguageKinyarwanda = 0x7277; // rw
static const TInt16 KLanguageSanskrit = 0x7361; // sa
static const TInt16 KLanguageSindhi = 0x7364; // sd
static const TInt16 KLanguageSangho = 0x7367; // sg
static const TInt16 KLanguageSerboCroatian = 0x7368; // sh
static const TInt16 KLanguageSinhalese = 0x7369; // si
static const TInt16 KLanguageSlovak = 0x736B; // sk
static const TInt16 KLanguageSlovenian = 0x736C; // sl
static const TInt16 KLanguageSamoan = 0x736D; // sm
static const TInt16 KLanguageShona = 0x736E; // sn
static const TInt16 KLanguageSomali = 0x736F; // so
static const TInt16 KLanguageAlbanian = 0x7371; // sq
static const TInt16 KLanguageSerbian = 0x7372; // sr
static const TInt16 KLanguageSiswati = 0x7373; // ss
static const TInt16 KLanguageSesotho = 0x7374; // st
static const TInt16 KLanguageSundanese = 0x7375; // su
static const TInt16 KLanguageSwedish = 0x7376; // sv
static const TInt16 KLanguageSwahili = 0x7377; // sw
static const TInt16 KLanguageTamil = 0x7461; // ta
static const TInt16 KLanguageTelugu = 0x7465; // te
static const TInt16 KLanguageTajik = 0x7467; // tg
static const TInt16 KLanguageThai = 0x7468; // th
static const TInt16 KLanguageTigrinya = 0x7469; // ti
static const TInt16 KLanguageTurkmen = 0x746B; // tk
static const TInt16 KLanguageTagalog = 0x746C; // tl
static const TInt16 KLanguageSetswana = 0x746E; // tn
static const TInt16 KLanguageTonga = 0x746F; // to
static const TInt16 KLanguageTurkish = 0x7472; // tr
static const TInt16 KLanguageTsonga = 0x7473; // ts
static const TInt16 KLanguageTatar = 0x7474; // tt
static const TInt16 KLanguageTwi = 0x7477; // tw
static const TInt16 KLanguageUighur = 0x7567; // ug
static const TInt16 KLanguageUkrainian = 0x756B; // uk
static const TInt16 KLanguageUrdu = 0x7572; // ur
static const TInt16 KLanguageUzbek = 0x757A; // uz
static const TInt16 KLanguageVietnamese = 0x7669; // vi
static const TInt16 KLanguageVolapuk = 0x766F; // vo
static const TInt16 KLanguageWolof = 0x776F; // wo
static const TInt16 KLanguageXhosa = 0x7868; // xh
static const TInt16 KLanguageYiddish = 0x7969; // yi - formerly ji
static const TInt16 KLanguageYoruba = 0x796F; // yo
static const TInt16 KLanguageZhuang = 0x7A61; // za
static const TInt16 KLanguageChinese = 0x7A68; // zh
static const TInt16 KLanguageZulu = 0x7A75; // zu

/************************************************************************/
//
//   SDP database server API
//
/************************************************************************/

typedef TPckgBuf<TSdpAttributeID> TSdpAttributeIDPckgBuf;
typedef TUint32 TSdpServRecordHandle;
typedef TPckgBuf<TSdpServRecordHandle> TSdpServRecordHandlePckgBuf;

/**
   The main session class.
   Used to create subsessions to database functionality.
**/
class RSdp : public RSessionBase
	{
public:
	IMPORT_C RSdp();
	IMPORT_C TInt Connect();
	IMPORT_C TVersion Version() const;
	IMPORT_C void ResourceCountMarkStart();
	IMPORT_C void ResourceCountMarkEnd();
	IMPORT_C TInt ResourceCount();
	};

/**
   Subsession base class.
   Contains basic subssession functionality.
**/
class RSdpSubSession : public RSubSessionBase
	{
public:
	IMPORT_C virtual TInt Open(RSdp& aSession) = 0;
	IMPORT_C virtual void Close() = 0;
	};



class CSdpAttrValueDES;
class CSdpAttrValue;

/**
   API for creating, modifying and deleting service record.
   
   Allow service records to be added and deleted, and arbitrary
   attributes to be added, updated and deleted from these records.
**/
class RSdpDatabase : public RSdpSubSession
	{
public:
	IMPORT_C RSdpDatabase();
	IMPORT_C TInt Open(RSdp& aSession);
	IMPORT_C void Close();
	IMPORT_C void CreateServiceRecordL(const TUUID& aUUID, TSdpServRecordHandle& aHandle);
	IMPORT_C void CreateServiceRecordL(CSdpAttrValueDES& aUUIDList, TSdpServRecordHandle& aHandle);
	IMPORT_C void UpdateAttributeL(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue& aAttrValue);
	IMPORT_C void UpdateAttributeL(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, TUint aUintValue);
	IMPORT_C void UpdateAttributeL(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, const TDesC16& aDesCValue);
	IMPORT_C void UpdateAttributeL(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, const TDesC8& aDesCValue);
	IMPORT_C void DeleteAttributeL(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID);
	IMPORT_C void DeleteRecordL(TSdpServRecordHandle aHandle);
private:
	HBufC8* iBuffer; //used for synchronous requests (could have been local)
	};


/**
   Interface class from the element parser.

   The member functions of this interface are called by the
   CElementParser class whenever it decodes an element of a specific
   type.
**/
class MSdpElementBuilder
	{
public:
	IMPORT_C virtual MSdpElementBuilder* BuildUnknownL(TUint8 aType, TUint8 aSizeDesc, const TDesC8& aData);
	IMPORT_C virtual MSdpElementBuilder* BuildNilL();
	IMPORT_C virtual MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	IMPORT_C virtual MSdpElementBuilder* BuildIntL(const TDesC8& aInt);
	IMPORT_C virtual MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
 	IMPORT_C virtual MSdpElementBuilder* BuildBooleanL(TBool aBool);
	IMPORT_C virtual MSdpElementBuilder* BuildStringL(const TDesC8& aString);
	IMPORT_C virtual MSdpElementBuilder* BuildDESL();  // Must not return NULL
	IMPORT_C virtual MSdpElementBuilder* BuildDEAL();  // ditto
	IMPORT_C virtual MSdpElementBuilder* StartListL(); // ditto
	IMPORT_C virtual MSdpElementBuilder* EndListL();
	IMPORT_C virtual MSdpElementBuilder* BuildURLL(const TDesC8& aURL);
	};

enum TSdpElementType
	{
	ETypeNil		=  0,
	ETypeUint		=  1,
	ETypeInt		=  2,
	ETypeUUID		=  3,
	ETypeString		=  4,
	ETypeBoolean	=  5,
	ETypeDES		=  6,
	ETypeDEA		=  7,
	ETypeURL		=  8,
	ETypeEncoded	= 32,	// Outside SDP reserved range
	};

class MSdpAttributeValueVisitor;

/**
   Base class for all SDP values in the database.

   Derived classes from this are used as the values for SDP service
   record attributes.  They can also be built using the parser to
   decode elements.
**/
class CSdpAttrValue : public CBase
	{
public:
	CSdpAttrValue();
	virtual ~CSdpAttrValue();
	virtual TSdpElementType Type() const=0;
	virtual TUint DataSize() const=0;

	// getter interface
	virtual TUint Uint() const;
	virtual TInt Int() const;
	virtual TBool DoesIntFit() const;
	virtual TInt Bool() const;
	virtual const TUUID &UUID() const;
	virtual const TPtrC8 Des() const;

	virtual void AcceptVisitorL(MSdpAttributeValueVisitor& aVisitor);
	};

/**
   This is a list of attr-values => can build its constituents.

   Used for DataElementSequences and DataElementAlternative
**/
class CSdpAttrValueList : public CSdpAttrValue, public MSdpElementBuilder
	{
public:
	IMPORT_C virtual ~CSdpAttrValueList();
	IMPORT_C virtual void AcceptVisitorL(MSdpAttributeValueVisitor& aVisitor);
	IMPORT_C virtual TUint DataSize() const;
	IMPORT_C void AppendValueL(CSdpAttrValue* aValue);

	// MSdpElementBuilder interface
	IMPORT_C virtual MSdpElementBuilder* BuildUnknownL(TUint8 aType, TUint8 aSizeDesc, const TDesC8& aData);
	IMPORT_C virtual MSdpElementBuilder* BuildNilL();
	IMPORT_C virtual MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	IMPORT_C virtual MSdpElementBuilder* BuildIntL(const TDesC8& aInt);
	IMPORT_C virtual MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
	IMPORT_C virtual MSdpElementBuilder* BuildBooleanL(TBool aBool);
	IMPORT_C virtual MSdpElementBuilder* BuildStringL(const TDesC8& aString);
	IMPORT_C virtual MSdpElementBuilder* BuildDESL();
	IMPORT_C virtual MSdpElementBuilder* BuildDEAL();
	IMPORT_C virtual MSdpElementBuilder* StartListL();
	IMPORT_C virtual MSdpElementBuilder* EndListL();
	IMPORT_C virtual MSdpElementBuilder* BuildURLL(const TDesC8& aString);
	IMPORT_C virtual MSdpElementBuilder* BuildEncodedL(const TDesC8& aString);

protected:
	CSdpAttrValueList(MSdpElementBuilder *aBuilder);
	void ConstructL();

private:
	MSdpElementBuilder *iParent;

protected:
	CArrayPtr<CSdpAttrValue> *iList;
	};


/**
   Represents a Nil element value.

   There are no accessors for this type, for obvious reasons.
**/
class CSdpAttrValueNil : public CSdpAttrValue
	{
public:
	IMPORT_C static CSdpAttrValueNil *NewNilL();
	virtual ~CSdpAttrValueNil();
	virtual TSdpElementType Type() const;
	virtual TUint DataSize() const;
private:
	CSdpAttrValueNil();
private:
	};

static const TInt KSdpMaxUintSize = 16;

/**
   Represents a unsigned integer value of an attribute.

   The uint can be up to 128 bits in size.
**/
class CSdpAttrValueUint : public CSdpAttrValue
	{
public:
	IMPORT_C static CSdpAttrValueUint* NewUintL(const TDesC8 &aUint);
	virtual ~CSdpAttrValueUint();
	virtual TSdpElementType Type() const;
	virtual TUint DataSize() const;
	TUint Uint() const;
	virtual TBool DoesIntFit() const;
	virtual const TPtrC8 Des() const;
private:
	CSdpAttrValueUint(const TDesC8 & aUint);
private:
	TBuf8<KSdpMaxUintSize> iUint;
	};

static const TInt KMaxIntSize  = 16;

/**
   Represents a integer value of an attribute.

   The int can be up to 128 bits in size.
**/
class CSdpAttrValueInt : public CSdpAttrValue
	{
public:
	IMPORT_C static CSdpAttrValueInt* NewIntL(const TDesC8 &aInt);
	virtual ~CSdpAttrValueInt();
	virtual TSdpElementType Type() const;
	virtual TUint DataSize() const;
	virtual TInt Int() const;
	virtual TBool DoesIntFit() const;
	virtual const TPtrC8 Des() const;
private:
	CSdpAttrValueInt(const TDesC8 & aInt);
private:
	TBuf8<KMaxIntSize> iInt;
	};


/**
   Represents a UUID value of an attribute.

   The UUID can be up to 128 bits in size.
**/
class CSdpAttrValueUUID : public CSdpAttrValue
	{
public:
	IMPORT_C static CSdpAttrValueUUID* NewUUIDL(const TUUID& aUUID);
	virtual ~CSdpAttrValueUUID();
	virtual TSdpElementType Type() const;
	virtual TUint DataSize() const;
	// getters
	virtual const TUUID &UUID() const;
	virtual const TPtrC8 Des() const;
private:
	CSdpAttrValueUUID(const TUUID& aUUID);
private:
	TUUID iUUID;
	};

/**
   Represents a string value of an attribute.

   The encoding of the string is up to the user.  The interpretation
   of the encoding is enabled using facilities in SDP.
**/
class CSdpAttrValueString : public CSdpAttrValue
	{
public:
	IMPORT_C static CSdpAttrValueString* NewStringL(const TDesC8& aString);
	virtual ~CSdpAttrValueString();
	virtual TSdpElementType Type() const;
	virtual TUint DataSize() const;
	virtual const TPtrC8 Des() const;
private:
	CSdpAttrValueString();
	void ConstructL(const TDesC8& aString);
private:
	HBufC8 *iBuffer;
	};

/**
   Represents a boolean value of an attribute.

**/
class CSdpAttrValueBoolean : public CSdpAttrValue
	{
public:
	IMPORT_C static CSdpAttrValueBoolean *NewBoolL(TBool aBool);
	virtual ~CSdpAttrValueBoolean();
	virtual TSdpElementType Type() const;
	virtual TUint DataSize() const;
	virtual TBool Bool() const;
private:
	CSdpAttrValueBoolean(TBool aBool);
private:
	TBool iBool;
	};

/**
   Represents a data element sequence value of an attribute.

   A DES can contain other values within it, including other
   sequences.  This enables arbitrary tree structures to be created.
**/

class CSdpAttrValueDES : public CSdpAttrValueList
	{
public:
	IMPORT_C static CSdpAttrValueDES* NewDESL(MSdpElementBuilder* aBuilder);
	virtual TSdpElementType Type() const;
private:
	CSdpAttrValueDES(MSdpElementBuilder *aBuilder);
	};

/**
   Represents a data element alternative value of an attribute.

   A DEA can contain other values within it, including other
   sequences.  This enables arbitrary tree structures to be created.
**/
class CSdpAttrValueDEA : public CSdpAttrValueList
	{
public:
	IMPORT_C static CSdpAttrValueDEA* NewDEAL(MSdpElementBuilder* aBuilder);
	virtual TSdpElementType Type() const;
private:
	CSdpAttrValueDEA(MSdpElementBuilder *aBuilder);
	};

/**
   Represents a URL value of an attribute.

**/
class CSdpAttrValueURL : public CSdpAttrValue
	{
public:
	IMPORT_C static CSdpAttrValueURL* NewURLL(const TDesC8& aString);
	virtual ~CSdpAttrValueURL();
	virtual TSdpElementType Type() const;
	virtual const TPtrC8 Des() const;
	virtual TUint DataSize() const;
private:
	CSdpAttrValueURL();
	void ConstructL(const TDesC8& aString);
private:
	HBufC8 *iBuffer;
	};

class MSdpAttributeValueVisitor
	{
public:
    virtual void VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType)=0;
    virtual void StartListL(CSdpAttrValueList &aList)=0;
    virtual void EndListL()=0;
	};

/**
Utility class to convert EPOC integers into TDesC8's, in the correct
format for passing to the BuildIntL and BuildUintL members in SDP.
This is effectively a Big-endian variant of TPackgBuf.
**/
template<class T>
class TSdpIntBuf : public TBufC8<sizeof(T)>
	{
public:
	inline TSdpIntBuf(const T& aRef);
	};

TEMPLATE_SPECIALIZATION class TSdpIntBuf<TUint32> : public TSdpIntBuf<TUint>
	{
public:
	inline TSdpIntBuf(const TUint32& aRef);
	};

TEMPLATE_SPECIALIZATION class TSdpIntBuf<TUint16> : public TSdpIntBuf<TUint>
	{
public:
	inline TSdpIntBuf(const TUint16& aRef);
	};

class SdpUtil
	{
public:
	IMPORT_C static TUint GetUint(const TDesC8& aData);
	IMPORT_C static void PutUint(TUint8* aPtr, TInt64 aInt, TInt aNumberOfBytes);
	};

//*************************************************************
//
//  SDP Agent client API
//
//*************************************************************


/**
	Mixin class to be implemented by the user. The functions herein will be 
	called for the user of CSdpAgent when responses are received from the 
	remote SDP Server.
**/
class MSdpAgentNotifier
	{
public:
	virtual void NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount) = 0;
	virtual void AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue) = 0;
	virtual void AttributeRequestComplete(TSdpServRecordHandle, TInt aError) = 0;
	};

class CSdpAgentEng;
class CSdpSearchPattern;
class CSdpAttrIdMatchList;

/**
	Class for making SDP requests from a remote SDP server.
	To make SDP requests, construct a CSdpAgent using the Bluettoth address of 
	the remote device to query. Call SetRecordFilterL to specify the UUIDs to 
	be searched for. Then make calls to NextRecordRequestL() and one of the 
	overloads of AttributeRequestL to retrieve information from the remote 
	device.
**/
class CSdpAgent : public CBase
	{
public:
	IMPORT_C static CSdpAgent* NewL(MSdpAgentNotifier& aNotifier, const TBTDevAddr& aDevAddr);
	IMPORT_C static CSdpAgent* NewLC(MSdpAgentNotifier& aNotifier, const TBTDevAddr& aDevAddr);
	IMPORT_C ~CSdpAgent();
	IMPORT_C void SetRecordFilterL(const CSdpSearchPattern& aUUIDFilter);
	IMPORT_C void SetAttributePredictorListL(const CSdpAttrIdMatchList& aMatchList);
	IMPORT_C void NextRecordRequestL();

	IMPORT_C void AttributeRequestL(TSdpServRecordHandle aHandle, 
								   TSdpAttributeID aAttrID);
	IMPORT_C void AttributeRequestL(TSdpServRecordHandle aHandle, 
								   const CSdpAttrIdMatchList& aMatchList);
	IMPORT_C void AttributeRequestL(MSdpElementBuilder* aBuilder,
		                           TSdpServRecordHandle aHandle, 
								   TSdpAttributeID aAttrID);
	IMPORT_C void AttributeRequestL(MSdpElementBuilder* aBuilder,
		                           TSdpServRecordHandle aHandle, 
								   const CSdpAttrIdMatchList& aMatchList);
	IMPORT_C void Cancel();

private:
	CSdpAgent();
	void ConstructL(MSdpAgentNotifier& aNotifier, TBTDevAddr aDevAddr);


private:
	CSdpAgentEng* iAgentEngine;
	};


/**
	SDP UUID search pattern.
	Holds a list of UUIDs which will be matched in SDP Service Search Requests.
**/
class CSdpSearchPattern : public CBase, public MSdpElementBuilder
	{
public:
	IMPORT_C static CSdpSearchPattern* NewL();
	IMPORT_C void ConstructL();
	IMPORT_C ~CSdpSearchPattern();
	IMPORT_C TInt AddL(const TUUID& aUUID);
	IMPORT_C TInt Remove(const TUUID& aUUID);
	IMPORT_C TInt Find(const TUUID& aUUID, TInt &aPos) const;
	IMPORT_C TInt Count() const;
	IMPORT_C const TUUID At(TInt anIndex) const;
	IMPORT_C void Reset();
	inline TBool IsEmpty();

	
private:
	CSdpSearchPattern();

private:
	// Implementation of Mbuilder interface
	MSdpElementBuilder* BuildUUIDL(const TUUID& aUUID);
	MSdpElementBuilder* BuildDESL();
	MSdpElementBuilder* StartListL();
	MSdpElementBuilder* EndListL();
private:
	CArrayFixFlat<TUUID>* iUUIDArray;
	};

struct TAttrRange
	{
public:
	TAttrRange() {};
	TAttrRange(TSdpAttributeID aAttrId) : iStart(aAttrId), iEnd(aAttrId) {}
	TAttrRange(TSdpAttributeID aStart, TSdpAttributeID aEnd) : iStart(aStart), iEnd(aEnd) {ASSERT(iStart <= iEnd);}
	inline TBool IsInRange(TSdpAttributeID aAttrId) const;
	inline TBool IsContiguousWith(TSdpAttributeID aAttrId) const;
	inline TBool IsContiguousWith(TAttrRange aRange) const;

public:
	TSdpAttributeID iStart;
	TSdpAttributeID iEnd;
	};

#define KAttrRangeAll TAttrRange(0, KMaxTUint16)

class CSdpServRecord;
class CElementEncoder;
class MAttributeMatchHandler;

/**
	SDP Attribute ID Match list.
	Holds a list of Attribute IDs to be retrieved from a remote device in an 
	Service Attribute Request.
	Set in terms of ranges of attributes, each one specified through a 
	TAttrRange.
**/
class CSdpAttrIdMatchList : public CBase, public MSdpElementBuilder
	{
public:
	IMPORT_C static CSdpAttrIdMatchList* NewL();
	IMPORT_C static CSdpAttrIdMatchList* NewL(const CSdpAttrIdMatchList& aAttrMatchList);
	IMPORT_C ~CSdpAttrIdMatchList();
	IMPORT_C void AddL(TAttrRange aRange);
	IMPORT_C void RemoveL(TAttrRange aRange);
	TInt Find(TSdpAttributeID aAttrId, TInt &aPos) const;
	IMPORT_C TBool InMatchList(TSdpAttributeID aAttrId, TInt &aPos) const;
	inline TBool InMatchList(TSdpAttributeID aAttrId) const;
	inline TInt Count() const;

	void FindAttributesL(CSdpServRecord &aRec, MAttributeMatchHandler &aHandler) const;
    IMPORT_C TUint EncodeL(CElementEncoder& aEncoder) const;

private:
	// Implementation of MSdpElementBuilder interface
	MSdpElementBuilder* BuildUintL(const TDesC8& aUint);
	MSdpElementBuilder* BuildDESL();
	MSdpElementBuilder* StartListL();
	MSdpElementBuilder* EndListL();

private:
	CSdpAttrIdMatchList();
	CArrayFix<TAttrRange>* iList;

	__DECLARE_TEST;
	};


// interface via NetDB
// includes parser and builder to use it


const static TInt KErrSdpAlreadyBound = -6400;
const static TInt KErrSdpPeerError = -6401;
const static TInt KErrSdpClientNotConnected = -6402;
const static TInt KErrSdpUnsupportedVersion = -6403;
const static TInt KErrSdpBadRecordHandle = -6404;
const static TInt KErrSdpBadContinuationState = -6405;
const static TInt KErrSdpServerRejectedRequest = -6406;
const static TInt KErrSdpBadRequestBufferLength = -6407;
const static TInt KErrSdpBadResultBufferLength = -6408;
const static TInt KErrSdpBadUUIDLength = -6409;
const static TInt KErrSdpBadResultData = -6410;
const static TInt KErrSdpDatabaseFull = -6411;


template <class T> class CSdpStackFix;

/**
   Parser for element encoded SDP values.

   Parses an input buffer, calling a MSdpElementBuilder each time an element is
   decoded.  This parser can parse incrementally, and be fed new data
   as it arrives.
**/
class CElementParser : public CBase
	{
public:
class CLinearBuf : public CBase
	{
public:
	static CLinearBuf* NewLC(TInt aExpandSize);
	~CLinearBuf();
	void AppendL(const TDesC8& aData);
	const TPtrC8 Ptr() const;
	void Consume(TInt aLength);
	TInt Size() const;

private:
	CLinearBuf();
	void ConstructL(TInt aExpandSize);

	CBufFlat* iBuf;
	TInt iStartOffset;
	};

public:
	IMPORT_C static CElementParser* NewL(MSdpElementBuilder* aBuilder);
	IMPORT_C ~CElementParser();
	IMPORT_C TInt ParseElementsL(const TDesC8& aData);
	IMPORT_C TBool BufferedParseL(const TDesC8& aData);
	IMPORT_C void Reset();
	IMPORT_C void Reset(MSdpElementBuilder* aBuilder);
	void SetBuilder(MSdpElementBuilder* aBuilder);
	MSdpElementBuilder* Builder();

private:
	void ConstructL();
	CElementParser(MSdpElementBuilder* aBuilder);
	static TBool IsComposite(TUint8 aType);
	void CloseListsL();

	MSdpElementBuilder* iBuilder;
	CSdpStackFix<TInt>* iListStack;
	CLinearBuf* iRemainderBuf;
	};


#include <btsdp.inl>

#endif
