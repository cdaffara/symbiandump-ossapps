/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ApplicationManagementOptions implementation
*
*/



#include <e32debug.h>
#include <s32mem.h>
#include "debug.h"
#include "aminstalloptions.h"
#include "aminstalloptionsint.h"
	

namespace AMXml
	{
	
	struct TLangCodeMapping 
		{
		//const TText8 *const iCode ;
		const TText8 * iCode ;
		TLanguage iLang ;
		};
		
	// Language options
	const TLangCodeMapping KLangOptions[] = 
		{
		{_S8("EN"), ELangEnglish},
		{_S8("FR"), ELangFrench},
		{_S8("GE"), ELangGerman},
		{_S8("SP"), ELangSpanish},
		{_S8("IT"), ELangItalian},
		{_S8("SW"), ELangSwedish},
		{_S8("DA"), ELangDanish},
		{_S8("NO"), ELangNorwegian},
		{_S8("FI"), ELangFinnish},
		{_S8("AM"), ELangAmerican},
		{_S8("SF"), ELangSwissFrench},
		{_S8("SG"), ELangSwissGerman},
		{_S8("PO"), ELangPortuguese},
		{_S8("TU"), ELangTurkish},
		{_S8("IC"), ELangIcelandic},
		{_S8("RU"), ELangRussian},
		{_S8("HU"), ELangHungarian},
		{_S8("DU"), ELangDutch},
		{_S8("BL"), ELangBelgianFlemish},
		{_S8("AU"), ELangAustralian},
		{_S8("BF"), ELangBelgianFrench},
		{_S8("AS"), ELangAustrian},
		{_S8("NZ"), ELangNewZealand},
		{_S8("IF"), ELangInternationalFrench},
		{_S8("CS"), ELangCzech},
		{_S8("SK"), ELangSlovak},
		{_S8("PL"), ELangPolish},
		{_S8("SL"), ELangSlovenian},
		{_S8("TC"), ELangTaiwanChinese},
		{_S8("HK"), ELangHongKongChinese},
		{_S8("ZH"), ELangPrcChinese},
		{_S8("JA"), ELangJapanese},
		{_S8("TH"), ELangThai},
		{_S8("AF"), ELangAfrikaans },
		{_S8("SQ"), ELangAlbanian },
		{_S8("AH"), ELangAmharic },
		{_S8("AR"), ELangArabic },
		{_S8("HY"), ELangArmenian },
		{_S8("TL"), ELangTagalog },
		{_S8("BE"), ELangBelarussian },
		{_S8("BN"), ELangBengali },
		{_S8("BG"), ELangBulgarian },
		{_S8("MY"), ELangBurmese },
		{_S8("CA"), ELangCatalan },
		{_S8("HR"), ELangCroatian },
		{_S8("CE"), ELangCanadianEnglish },
		{_S8("IE"), ELangInternationalEnglish },
		{_S8("SF"), ELangSouthAfricanEnglish },
		{_S8("ET"), ELangEstonian },
		{_S8("FA"), ELangFarsi },
		{_S8("CF"), ELangCanadianFrench },
		{_S8("GD"), ELangScotsGaelic },
		{_S8("KA"), ELangGeorgian },
		{_S8("EL"), ELangGreek },
		{_S8("CG"), ELangCyprusGreek },
		{_S8("GU"), ELangGujarati },
		{_S8("HE"), ELangHebrew },
		{_S8("HI"), ELangHindi },
		{_S8("IN"), ELangIndonesian },
		{_S8("GA"), ELangIrish },
		{_S8("SZ"), ELangSwissItalian },
		{_S8("KN"), ELangKannada },
		{_S8("KK"), ELangKazakh },
		{_S8("KM"), ELangKhmer },
		{_S8("KO"), ELangKorean },
		{_S8("LO"), ELangLao },
		{_S8("LV"), ELangLatvian },
		{_S8("LT"), ELangLithuanian },
		{_S8("MK"), ELangMacedonian },
		{_S8("MS"), ELangMalay },
		{_S8("ML"), ELangMalayalam },
		{_S8("MR"), ELangMarathi },
		{_S8("MO"), ELangMoldavian },
		{_S8("MN"), ELangMongolian },
		{_S8("NN"), ELangNorwegianNynorsk },
		{_S8("BP"), ELangBrazilianPortuguese },
		{_S8("PA"), ELangPunjabi },
		{_S8("RO"), ELangRomanian },
		{_S8("SR"), ELangSerbian },
		{_S8("SI"), ELangSinhalese },
		{_S8("SO"), ELangSomali },
		{_S8("OS"), ELangInternationalSpanish },
		{_S8("LS"), ELangLatinAmericanSpanish },
		{_S8("SH"), ELangSwahili },
		{_S8("FS"), ELangFinlandSwedish },
		//{_S8("??"), ELangReserved1 },
		{_S8("TA"), ELangTamil },
		{_S8("TE"), ELangTelugu },
		{_S8("BO"), ELangTibetan },
		{_S8("TI"), ELangTigrinya },
		{_S8("CT"), ELangCyprusTurkish },
		{_S8("TK"), ELangTurkmen },
		{_S8("UK"), ELangUkrainian },
		{_S8("UR"), ELangUrdu },
		//{_S8("??"), ELangReserved2 },
		{_S8("VI"), ELangVietnamese },
		{_S8("CY"), ELangWelsh },
		{_S8("ZU"), ELangZulu },
		//{_S8("??"), ELangOther },
		//{_S8("??"), ELangNone  }
		};
		
	const TInt KNumLangOptions = (sizeof(KLangOptions)/sizeof(TLangCodeMapping));
		
	// boolean "value" attribute values
	_LIT8( KIOValueTrue, "yes" );
	_LIT8( KIOValueFalse, "no" );
	// notimp _LIT8( KIOAsk, "askuser" );
	
	_LIT8( KIODefaultLanguage, "*");
	
	// notimp _LIT8( KIOMissingOptionPolicyValDefault, "usedefault" );
	_LIT8( KIOMissingOptionPolicyValFail, "fail" );
	// notimp _LIT8( KIOMissingOptionPolicyValAsk, "askuser" );
	
	// "Name" attribute values
	_LIT8( KIODrive, "drive" );
	_LIT8( KIOLanguage, "lang" );
	_LIT8( KIOUpgrade, "upgrade" );
	_LIT8( KIOKill, "kill" );
	
	// sym specific
	_LIT8( KIOCapabilities, "capabilities" );
	_LIT8( KIOUntrusted, "untrusted" );
	_LIT8( KIOPkginfo, "pkginfo" );
	_LIT8( KIOOptionalItems, "optionals" );
	
	_LIT8 ( KIOOCSP, "ocsp" );
	_LIT8 ( KIOOCSPWarning, "ignoreocspwarn" );
	_LIT8 ( KIOOverwrite, "fileoverwrite" );
	
	_LIT8 ( KIOUpgradeData, "upgradedata" ); // iUpgradeData
	
	_LIT8( KUIOBreakDependency, "allowdependencybreak" );
	
	_LIT8( KIOAttrNameDefaultPolicy, "missingoptionpolicy" );
	
	_LIT8( KIODownload, "download" );
	_LIT8( KIOHttpUsername, "httpusername" );
	_LIT8( KIOHttpPassword, "httppassword" );
	_LIT8( KIOIAP, "conref" );
	
	namespace Reader
		{
		// XML node names
		_LIT8( KIOStartElement, "InstOpts") ;
		_LIT8( KIOStdOpt, "StdOpt");
		_LIT8( KIOStdSymOpt, "StdSymOpt" );
		_LIT8( KIOCustSymOpts, "CustSymOpts" );
		// notimp _LIT8( KIOOpt, "Opt" );

		// Xml attribute names
		// notimp _LIT8( KIOOptAttrIndex, "index" );
		_LIT8( KIOAttrName, "name" );
		_LIT8( KIOAttrValue, "value" );
		
		}
		
	namespace Writer
		{
		_LIT8( KIOStartElement, "<InstOpts>") ;
		_LIT8( KIOStdOptFormat, "<StdOpt name=\"%S\" value=\"%S\"/>" );
		_LIT8( KIOStdSymOptFormat, "<StdSymOpt name=\"%S\" value=\"%S\"/>" );
		// notimp _LIT8( KIOCustSymOptsStartFormat, "<CustSymOpts cnt=\"%d\">" );
		// notimp _LIT8( KIOCustSymOptsEnd, "</CustSymOpts>" );
		// notimp _LIT8( KIOOptFormat, "<Opt index=\"%d\" value=\"%S\"/>" );
		_LIT8( KIOStartElementEnd, "</InstOpts>") ;		
		
		_LIT8( KUIOStartElement, "<UninstOpts>" );
		_LIT8( KUIOStartElementEnd, "</UninstOpts>" );
		}
	}
	
///////////////////////////////////////////////////
//
// CAMInstOptsParserBase
//
///////////////////////////////////////////////////

	
CAMInstOptsParserBase::~CAMInstOptsParserBase()
	{
	delete iParser;
	iParser = NULL;
	}

void CAMInstOptsParserBase::SetPolicyValue( SwiUI::TPolicy &aPolicy, const TDesC8& aValue ) const
	{
	using namespace AMXml;
	
	if ( aValue == KIOValueTrue )
		{
		aPolicy = SwiUI::EPolicyAllowed ;
		}
	else if ( aValue == KIOValueFalse )
		{
		aPolicy = SwiUI::EPolicyNotAllowed ;
		}
	else
		{
		aPolicy = iDefaultPolicy;
		}
	}
void CAMInstOptsParserBase::ConstructL()
	{
	iParser = Xml::CParser::NewL( _L8( "text/xml"), *this );	
	
	}
	
void CAMInstOptsParserBase::ParseL()
	{
	Xml::ParseL( *iParser, iBuffer );
	}	
	
void CAMInstOptsParserBase::OnStartElementL(const Xml::RTagInfo& /*aElement*/, 
		const Xml::RAttributeArray& /*aAttributes*/, TInt aErrorCode)
	{
	RDEBUG("CAMInstOptsParserBase::OnStartElementL");
	if( aErrorCode != KErrNone )
		{
		RDEBUG_2( "		->OnStartElementL (aErrorCode '%d')", aErrorCode );		
		}
	}
	
void CAMInstOptsParserBase::OnEndDocumentL(TInt aErrorCode)
	{
	RDEBUG("CAMInstOptsParserBase::OnEndDocumentL");
	if( aErrorCode != KErrNone )
		{
		RDEBUG_2( "		->OnEndDocumentL (aErrorCode '%d')", aErrorCode );		
		}
	}

void CAMInstOptsParserBase::OnStartDocumentL(const Xml::RDocumentParameters& aDocParam, TInt aErrorCode)
	{
	RDEBUG("CAMInstOptsParserBase::OnStartDocumentL");
	if( aErrorCode != KErrNone )
		{
		const TDesC8 &d = aDocParam.CharacterSetName().DesC();
		RDEBUG8_3( "	->OnStartDocumentL (aDocParam '%S', aErrorCode %d)" , 
			&d,aErrorCode );	
		}
	}
	
void CAMInstOptsParserBase::OnEndElementL(const Xml::RTagInfo& aElement, TInt aErrorCode)
	{
	RDEBUG("CAMInstOptsParserBase::OnEndElementL");
	if( aErrorCode != KErrNone )
		{
		const TDesC8 &d = aElement.LocalName().DesC();
		RDEBUG8_3( "		->OnEndElementL (LocalName '%S', aErrorCode %d)", 
			&d,aErrorCode );
		}
	}
	
void CAMInstOptsParserBase::OnContentL(
		#ifdef _DEBUG 
			const TDesC8& aBytes
		#else 
			const TDesC8& /*aBytes*/
		#endif 
	,TInt aErrorCode )
	{
	RDEBUG("CAMInstOptsParserBase::OnContentL");
	if( aErrorCode != KErrNone )
		{
		#ifdef _DEBUG
			RDEBUG8_3( "		->OnContentL (abytes '%S', aErrorCode %d)", 
				&aBytes, aErrorCode );
		#else
			RDEBUG8_2( "		->OnContentL (aErrorCode %d)", aErrorCode );
		#endif		
		}
	
	}
void CAMInstOptsParserBase::OnStartPrefixMappingL(const RString& /*aPrefix*/, 
	const RString& /*aUri*/, TInt /*aErrorCode*/)
	{
	
	}
void CAMInstOptsParserBase::OnEndPrefixMappingL(const RString& /*aPrefix*/, TInt /*aErrorCode*/)
	{
	
	}
void CAMInstOptsParserBase::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, TInt /*aErrorCode*/)
	{
	
	}
void CAMInstOptsParserBase::OnSkippedEntityL(const RString& /*aName*/, TInt /*aErrorCode*/)
	{
	
	}
void CAMInstOptsParserBase::OnProcessingInstructionL(const TDesC8& /*aTarget*/, 
	const TDesC8& /*aData*/, TInt /*aErrorCode*/)
	{
	
	}
void CAMInstOptsParserBase::OnError(TInt aErrorCode)
	{
	RDEBUG("CAMInstOptsParserBase::OnError");
	if( aErrorCode != KErrNone )
		{
		RDEBUG_2( "		->OnError ( aErrorCode %d)",aErrorCode );
	}
	}
	

	
TAny* CAMInstOptsParserBase::GetExtendedInterface(const TInt32 /*aUid*/)
	{
	
	return NULL;		
	}


///////////////////////////////////////////////////
//
// CAMInstOptsParser
//
///////////////////////////////////////////////////

CAMInstOptsParser * CAMInstOptsParser::NewLC( const TDesC8 &aBuffer, NApplicationManagement::TAMInstallOptions &aResult )
	{
	CAMInstOptsParser *self = new ( ELeave ) CAMInstOptsParser( aBuffer, aResult );
	CleanupStack::PushL( self );
	self->ConstructL() ;
	return self ;
	}
	
CAMInstOptsParser *CAMInstOptsParser::NewL( const TDesC8 &aBuffer, 
	NApplicationManagement::TAMInstallOptions &aResult )	
	{
	CAMInstOptsParser *self = NewLC ( aBuffer, aResult );
	CleanupStack::Pop( self );
	return self ;
	}
	
CAMUninstOptsParser * CAMUninstOptsParser::NewLC( const TDesC8 &aBuffer, SwiUI::TUninstallOptions &aResult )
	{
	CAMUninstOptsParser *self = new ( ELeave ) CAMUninstOptsParser( aBuffer, aResult );
	CleanupStack::PushL( self );
	self->ConstructL() ;
	return self ;
	}
	
CAMUninstOptsParser *CAMUninstOptsParser::NewL( const TDesC8 &aBuffer, 
	SwiUI::TUninstallOptions &aResult )	
	{
	CAMUninstOptsParser *self = NewLC ( aBuffer, aResult );
	CleanupStack::Pop( self );
	return self ;
	}
	
	
CAMInstOptsParser::~CAMInstOptsParser()
	{
	
	}
	
	
void CAMInstOptsParser::OnStartElementL(const Xml::RTagInfo& aElement, 
		const Xml::RAttributeArray& aAttributes, 
		#ifdef _DEBUG 
			TInt aErrorCode
		#else 
			TInt /*aErrorCode*/
		#endif 
		)
	{
	
#ifdef _DEBUG
	RDEBUG8_3( "CAMInstOptsParser::OnStartElementL (LocalName '%S', aErrorCode %d)", 
		&aElement.LocalName().DesC(),aErrorCode );
	TInt count2( aAttributes.Count() );
	for( TInt i( 0 ); i < count2; i++ )
		{
		RDEBUG8_5( "   Attribute %d, type=%d, '%S'='%S' ",i,aAttributes[i].Type(), 
		 &aAttributes[i].Value().DesC(), &aAttributes[i].Attribute().LocalName().DesC() );
		
		}
#endif

	using namespace AMXml;

	
	const TPtrC8 name( aElement.LocalName().DesC() );
	if ( name == Reader::KIOStartElement )
		{
		
		if ( aAttributes.Count() == 1 )
			{
			TPtrC8 attrname( aAttributes[0].Attribute().LocalName().DesC() );
			if ( attrname == KIOAttrNameDefaultPolicy)
				{
				
				using namespace SwiUI;
				TPtrC8 attrval( aAttributes[0].Value().DesC() );
				if ( attrval == KIOMissingOptionPolicyValFail)
					{
					iDefaultPolicy = EPolicyNotAllowed;
					}
				else
					{
					iDefaultPolicy = EPolicyUserConfirm;
					}
				}
			}
		}
	else if ( name != Reader::KIOCustSymOpts )
		{
		RDEBUG8_2( "   Processing element '%S'", &name );
		TInt c( aAttributes.Count() );
		if ( c == 2 )
			{
			TPtrC8 attrname( aAttributes[0].Attribute().LocalName().DesC() );
			TPtrC8 attrnameval( aAttributes[0].Value().DesC() );
			TPtrC8 attrval( aAttributes[1].Attribute().LocalName().DesC() );
			TPtrC8 attrvalval( aAttributes[1].Value().DesC() );
			RDEBUG8_5( "   Name '%S'='%S' Val '%S'='%S' ",&attrname, &attrnameval,
				&attrval, &attrvalval );
			if ( attrname == Reader::KIOAttrName )
				{
				if ( attrval == Reader::KIOAttrValue )
					{
					if ( name == Reader::KIOStdOpt )
						{
						if ( attrnameval == KIODrive )
							{
							TInt len = attrvalval.Length();
							if (len == 0)
								{
								TChar KDefaultDrive = 'c';
								iResult.iOptions.iDrive = KDefaultDrive;
								//RDEBUG8_2( " Set drive to %c ", iResult.iOptions.iDrive );
								}
							else
								{
								RDEBUG8_2( " Setting drive to %c ", attrvalval[0] );
								iResult.iOptions.iDrive = attrvalval[0];		
								}
							}
						else if ( attrnameval == KIOLanguage )
							{
							if ( attrvalval == KIODefaultLanguage )
								{
								iResult.iOptions.iUsePhoneLang = ETrue;
								}
							else
								{
								for( TInt i( 0 ); i < KNumLangOptions; i++ )
									{
									if ( attrvalval == TPtrC8( KLangOptions[i].iCode ) )
										{
									#ifdef _DEBUG
										TPtrC8 code( KLangOptions[i].iCode );
										RDEBUG8_2( " OnStartElementL : Found language %S", &code);
									#endif
										iResult.iOptions.iLang = KLangOptions[i].iLang; 
										break;
										}
									}	
								}
							
							}
						else if ( attrnameval == KIOUpgrade )
							{
							RDEBUG8_2( " Setting Upgrade to %S ", &attrvalval );
							SetPolicyValue( iResult.iOptions.iUpgrade, attrvalval );
							}
						else if ( attrnameval == KIOKill )
							{
							RDEBUG8_2( " Setting iKillApp to %S ", &attrvalval );
							SetPolicyValue( iResult.iOptions.iKillApp, attrvalval );
							}
						else if ( attrnameval == KIOHttpUsername )
							{
							RDEBUG8_2( " Setting iLogin to %S ", &attrvalval );
							iResult.iOptions.iLogin.Copy( attrvalval.Left( SwiUI::KSWInstMaxUserNameLength ) );
							}
						else if ( attrnameval == KIOHttpPassword )
							{
							RDEBUG8_2( " Setting iPassword to %S ", &attrvalval );
							iResult.iOptions.iPassword.Copy( attrvalval.Left( SwiUI::KSWInstMaxPasswordLength ) );
							}
						else if ( attrnameval == KIODownload )
							{
							RDEBUG8_2( " Setting iDownload to %S ", &attrvalval );
							SetPolicyValue( iResult.iOptions.iDownload, attrvalval );
							}
						else if ( attrnameval == KIOIAP )
							{
							RDEBUG8_2( " Setting iConRef to %S ", &attrvalval );
							iResult.iConRef = attrvalval;
							
							
							}
							/*
	_LIT8( KIOIAP, "iap" );
*/

						else 
							{
							RDEBUG8_3( " WARNING Unsupported name attribute value: '%S' for '%S' ", &attrnameval, &name );
							}
						}
					else if ( name == Reader::KIOStdSymOpt )
						{
						if ( attrnameval == KIOCapabilities )
							{
							RDEBUG8_2( " Setting iCapabilities to %S ", &attrvalval );
							SetPolicyValue( iResult.iOptions.iCapabilities, attrvalval );
							}
						else if ( attrnameval == KIOUntrusted )
							{
							RDEBUG8_2( " Setting iUntrusted to %S ", &attrvalval );
							SetPolicyValue( iResult.iOptions.iUntrusted, attrvalval );
							}
						else if ( attrnameval == KIOPkginfo )
							{
							RDEBUG8_2( " Setting iPackageInfo to %S ", &attrvalval );
							SetPolicyValue( iResult.iOptions.iPackageInfo, attrvalval );
							}
						else if ( attrnameval == KIOOCSP )
							{
							RDEBUG8_2( " Setting iOCSP to %S ", &attrvalval );
							SetPolicyValue( iResult.iOptions.iOCSP, attrvalval );
							}
						else if ( attrnameval == KIOOCSPWarning )
							{
							RDEBUG8_2( " Setting iIgnoreOCSPWarnings to %S ", &attrvalval );
							SetPolicyValue( iResult.iOptions.iIgnoreOCSPWarnings, attrvalval );
							}
						else if ( attrnameval == KIOOptionalItems )
							{
							RDEBUG8_2( " Setting iOptionalItems to %S ", &attrvalval );
							SetPolicyValue( iResult.iOptions.iOptionalItems, attrvalval );
							}
						else if ( attrnameval == KIOOverwrite )
							{
							RDEBUG8_2( " Setting iOverwrite to %S ", &attrvalval );
							SetPolicyValue( iResult.iOptions.iOverwrite, attrvalval );
							}	
						else if ( attrnameval == KIOUpgradeData )
							{
							RDEBUG8_2( " Setting iUpgradeData to %S ", &attrvalval );
							SetPolicyValue( iResult.iOptions.iUpgradeData, attrvalval );
							}	
							
						else
							{
							RDEBUG8_3( " WARNING Unsupported name sym attribute value: '%S' for '%S' ", &attrnameval, &name );
							}
						}
					else
						{
						RDEBUG("WARNING, unsupported name attribute!");
						}
					}
				else
					{
					RDEBUG8_3( " ERROR invalid second attribute!: '%S', expteced: '%S' ", &attrval, &Reader::KIOAttrValue() );
					}
				}
			else
				{
				RDEBUG8_3( " ERROR invalid first attribute!: '%S', expteced: '%S' ", &attrname, &Reader::KIOAttrName() );
				}
			}
		else
			{
			RDEBUG8_3( " ERROR unsupported count of attributes!!: '%d' for '%S' ", c, &name);
			}
		}
	else
		{
		RDEBUG( " ERROR custom options not implemented!!!!" );
	
		}
	}
	

///////////////////////////////////////////////////
//
// CAMUninstOptsParser
//
///////////////////////////////////////////////////

CAMUninstOptsParser::~CAMUninstOptsParser()
	{
	
	}
	
void CAMUninstOptsParser::OnStartElementL(const Xml::RTagInfo& aElement, 
		const Xml::RAttributeArray& aAttributes, 
		#ifdef _DEBUG 
			TInt aErrorCode
		#else 
			TInt /*aErrorCode*/
		#endif 
		)
	{
	
#ifdef _DEBUG
	RDEBUG8_3( "CAMUninstOptsParser::OnStartElementL (LocalName '%S', aErrorCode %d)", 
		&aElement.LocalName().DesC(),aErrorCode );
	TInt count2( aAttributes.Count() );
	for( TInt i( 0 ); i < count2; i++ )
		{
		RDEBUG8_5( "   Attribute %d, type=%d, '%S'='%S' ",i,aAttributes[i].Type(), 
		 &aAttributes[i].Value().DesC(), &aAttributes[i].Attribute().LocalName().DesC() );
		
		}
#endif

	using namespace AMXml;

	
	const TPtrC8 name( aElement.LocalName().DesC() );
	if ( name == Reader::KIOStartElement )
		{
		
		}
	else if ( name != Reader::KIOCustSymOpts )
		{
		RDEBUG8_2( "   Processing element '%S'", &name );
		TInt c( aAttributes.Count() );
		if ( c == 2 )
			{
			TPtrC8 attrname( aAttributes[0].Attribute().LocalName().DesC() );
			TPtrC8 attrnameval( aAttributes[0].Value().DesC() );
			TPtrC8 attrval( aAttributes[1].Attribute().LocalName().DesC() );
			TPtrC8 attrvalval( aAttributes[1].Value().DesC() );
			RDEBUG8_5( "   Name '%S'='%S' Val '%S'='%S' ",&attrname, &attrnameval,
				&attrval, &attrvalval );
			if ( attrname == Reader::KIOAttrName )
				{
				if ( attrval == Reader::KIOAttrValue )
					{
					if ( name == Reader::KIOStdOpt )
						{
						if ( attrnameval == KIOKill )
							{
							RDEBUG8_2( " Setting iKillApp to %S ", &attrvalval );
							SetPolicyValue( iResult.iKillApp, attrvalval );
							}
						else 
							{
							RDEBUG8_3( " WARNING Unsupported name attribute value: '%S' for '%S' ", &attrnameval, &name );
							}
						}
					else if ( name == Reader::KIOStdSymOpt )
						{
						if ( attrnameval == KUIOBreakDependency )
							{
							RDEBUG8_2( " Setting iDependecyBreak to %S ", &attrvalval );
							SetPolicyValue( iResult.iBreakDependency, attrvalval );
							}
						else
							{
							RDEBUG8_3( " WARNING Unsupported name attribute value: '%S' for '%S' ", &attrnameval, &name );
							}
						}
					else
						{
						RDEBUG("WARNING, unsupported name attribute value");
						}
					}
				else
					{
					RDEBUG8_3( " ERROR invalid second attribute!: '%S', expteced: '%S' ", &attrval, &Reader::KIOAttrValue() );
					}
				}
			else
				{
				RDEBUG8_3( " ERROR invalid first attribute!: '%S', expteced: '%S' ", &attrname, &Reader::KIOAttrName() );
				}
			}
		else
			{
			RDEBUG8_3( " ERROR unsupported count of attributes!!: '%d' for '%S' ", c, &name);
			}
		}
	else
		{
		RDEBUG( " ERROR custom options not implemented!!!!" );
		
		}
	}
	

///////////////////////////////////////////////////
//
// InstallOptionsParser
//
///////////////////////////////////////////////////

EXPORT_C void InstallOptionsParser::ParseOptionsL( const TDesC8 &aBuffer, SwiUI::TInstallOptions &aResult)
	{
	NApplicationManagement::TAMInstallOptions opts;
	opts.iOptions.iUsePhoneLang = EFalse;
	CAMInstOptsParser *parser = CAMInstOptsParser::NewLC( aBuffer, opts );
	parser->ParseL();
	aResult = opts.iOptions;
	CleanupStack::PopAndDestroy( parser );
	}
	
EXPORT_C void InstallOptionsParser::ParseOptionsL( const TDesC8 &aBuffer, NApplicationManagement::TAMInstallOptions &aResult)
	{
	aResult.iOptions.iUsePhoneLang = EFalse;
	CAMInstOptsParser *parser = CAMInstOptsParser::NewLC( aBuffer, aResult );
	parser->ParseL();
	CleanupStack::PopAndDestroy( parser );
	}
	
EXPORT_C void InstallOptionsParser::ParseOptionsL( const TDesC8 &aBuffer, SwiUI::TUninstallOptions &aResult)
	{
	CAMUninstOptsParser *parser = CAMUninstOptsParser::NewLC( aBuffer, aResult );
	parser->ParseL();
	CleanupStack::PopAndDestroy( parser );
	}
	
EXPORT_C void InstallOptionsParser::SerializeOptionsL( const SwiUI::TUninstallOptions  &aOptions, CBufBase &aResult )
	{
	using namespace AMXml;
	using namespace AMXml::Writer;
	using namespace SwiUI;

	aResult.InsertL(aResult.Size(), Writer::KUIOStartElement );
	
	HBufC8 *tmp = HBufC8::NewLC( 128 );
	TPtr8 ptmp( tmp->Des() );
	
	if ( aOptions.iKillApp != EPolicyUserConfirm )
		{
		ptmp.Format( KIOStdOptFormat, &KIOKill, ( aOptions.iKillApp == EPolicyAllowed ? &KIOValueTrue : &KIOValueFalse ) );
		aResult.InsertL( aResult.Size(), ptmp );
		}
	if ( aOptions.iBreakDependency != EPolicyUserConfirm )
		{
		ptmp.Format( KIOStdSymOptFormat, &KUIOBreakDependency, ( aOptions.iKillApp == EPolicyAllowed ? &KIOValueTrue : &KIOValueFalse ) );
		aResult.InsertL( aResult.Size(), ptmp );
		}
	aResult.InsertL(aResult.Size(), Writer::KUIOStartElementEnd );
	
	CleanupStack::PopAndDestroy( tmp );
	}

EXPORT_C void InstallOptionsParser::SerializeOptionsL( const SwiUI::TInstallOptions &aOptions, CBufBase &aResult )
	{
	NApplicationManagement::TAMInstallOptions options;
	options.iOptions = aOptions;
	SerializeOptionsL( options, aResult );
	}
	
EXPORT_C void InstallOptionsParser::SerializeOptionsL( const NApplicationManagement::TAMInstallOptions &aOptions, CBufBase &aResult )
	{
		
	using namespace AMXml;
	using namespace AMXml::Writer;
	using namespace SwiUI;
	
	const SwiUI::TInstallOptions &options = aOptions.iOptions;
	
	aResult.InsertL(aResult.Size(), Writer::KIOStartElement );
	
	HBufC8 *tmp = HBufC8::NewLC( 128 );
	TPtr8 ptmp( tmp->Des() );
	TBuf8<1> b ;
	b.Append( options.iDrive );
	ptmp.Format( KIOStdOptFormat, &KIODrive, &b );
	aResult.InsertL( aResult.Size(), ptmp );
	
	if( options.iUsePhoneLang )
		{
		ptmp.Format( KIOStdOptFormat, &KIOLanguage, &KIODefaultLanguage )	;
		}
	else
		{
		if( options.iLang != ELangNone )
			{
			TBool found( EFalse );
			for( TInt i( 0 ); !found && i < KNumLangOptions; i++ )
				{
				if ( KLangOptions[i].iLang == options.iLang )
					{
				#ifdef _DEBUG
		 		 	TPtrC8 code( KLangOptions[i].iCode );
				 	RDEBUG8_2( " InstallOptionsParser::SerializeOptionsL : Found language %S", &code );
				#endif
					TPtrC8 p( KLangOptions[i].iCode );
					ptmp.Format( KIOStdOptFormat, &KIOLanguage, &p )	;
					
					found = ETrue;
					}
				}
			if ( !found )
				{
				ptmp.Format( KNullDesC8() );
				}
			}
		else
			{
			ptmp.Format( KNullDesC8() );
			}
		}
	aResult.InsertL( aResult.Size(), ptmp );
	
	if ( options.iUpgrade != EPolicyUserConfirm )
		{
		ptmp.Format( KIOStdOptFormat, &KIOUpgrade, ( options.iUpgrade == EPolicyAllowed ? &KIOValueTrue : &KIOValueFalse ) );
		aResult.InsertL( aResult.Size(), ptmp );
		
		}
	if ( options.iKillApp != EPolicyUserConfirm )
		{
		ptmp.Format( KIOStdOptFormat, &KIOKill, ( options.iKillApp == EPolicyAllowed ? &KIOValueTrue : &KIOValueFalse ) );
		aResult.InsertL( aResult.Size(), ptmp );
		}
	if ( options.iPackageInfo != EPolicyUserConfirm )
		{
		ptmp.Format( KIOStdSymOptFormat, &KIOPkginfo, ( options.iPackageInfo == EPolicyAllowed ? &KIOValueTrue : &KIOValueFalse ) );
		aResult.InsertL( aResult.Size(), ptmp );
		}	
	if ( options.iCapabilities != EPolicyUserConfirm )
		{
		ptmp.Format( KIOStdSymOptFormat, &KIOCapabilities, ( options.iCapabilities == EPolicyAllowed ? &KIOValueTrue : &KIOValueFalse ) );
		aResult.InsertL( aResult.Size(), ptmp );
		}
	if ( options.iUntrusted != EPolicyUserConfirm )
		{
		ptmp.Format( KIOStdSymOptFormat, &KIOUntrusted, ( options.iUntrusted == EPolicyAllowed ? &KIOValueTrue : &KIOValueFalse ) );
		aResult.InsertL( aResult.Size(), ptmp );
		}	
	if ( options.iOCSP != EPolicyUserConfirm )
		{
		ptmp.Format( KIOStdSymOptFormat, &KIOOCSP, ( options.iOCSP == EPolicyAllowed ? &KIOValueTrue : &KIOValueFalse ) );
		aResult.InsertL( aResult.Size(), ptmp );
		}	
	if ( options.iIgnoreOCSPWarnings != EPolicyUserConfirm )
		{
		ptmp.Format( KIOStdSymOptFormat, &KIOOCSPWarning, ( options.iIgnoreOCSPWarnings == EPolicyAllowed ? &KIOValueTrue : &KIOValueFalse ) );
		aResult.InsertL( aResult.Size(), ptmp );
		}	
	if ( options.iOptionalItems != EPolicyUserConfirm )
		{
		ptmp.Format( KIOStdSymOptFormat, &KIOOptionalItems, ( options.iOptionalItems == EPolicyAllowed ? &KIOValueTrue : &KIOValueFalse ) );
		aResult.InsertL( aResult.Size(), ptmp );
		}	
	if ( options.iOverwrite != EPolicyUserConfirm )
		{
		ptmp.Format( KIOStdSymOptFormat, &KIOOverwrite, ( options.iOverwrite == EPolicyAllowed ? &KIOValueTrue : &KIOValueFalse ) );
		aResult.InsertL( aResult.Size(), ptmp );
		}	
	if ( options.iUpgradeData != EPolicyUserConfirm )
		{
		ptmp.Format( KIOStdSymOptFormat, &KIOUpgradeData, ( options.iUpgradeData == EPolicyAllowed ? &KIOValueTrue : &KIOValueFalse ) );
		aResult.InsertL( aResult.Size(), ptmp );
		}	
	if ( options.iDownload != EPolicyUserConfirm )
		{
		ptmp.Format( KIOStdOptFormat, &KIODownload, ( options.iDownload == EPolicyAllowed ? &KIOValueTrue : &KIOValueFalse ) );
		aResult.InsertL( aResult.Size(), ptmp );
		}
	if ( options.iPassword != KNullDesC() )
		{
        TBuf8<KSWInstMaxPasswordLength> password;        
		password.Copy( options.iPassword );
		ptmp.Format( KIOStdOptFormat, &KIOHttpPassword, &password );
		aResult.InsertL( aResult.Size(), ptmp );
		}
	if ( options.iLogin != KNullDesC() )
		{
		TBuf8<KSWInstMaxUserNameLength> login;
		login.Copy( options.iLogin );
		ptmp.Format( KIOStdOptFormat, &KIOHttpUsername, &login );
		aResult.InsertL( aResult.Size(), ptmp );
		}	
	if ( aOptions.iConRef != KNullDesC8() )
		{
		ptmp.Format( KIOStdOptFormat, &KIOIAP, &aOptions.iConRef);
		aResult.InsertL( aResult.Size(), ptmp );
		}
	aResult.InsertL( aResult.Size(), KIOStartElementEnd );
	
	CleanupStack::PopAndDestroy( tmp );
	}
	
// End of File
