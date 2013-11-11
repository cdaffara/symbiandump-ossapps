/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Capability service controller
*
*/


#ifndef _CAPABILITY_H_
#define _CAPABILITY_H_

#include <e32std.h>

//
// string sizes
//
const TInt KBufSize(128);
const TInt KTagSize(64);
const TInt KMaxSize(512);

const TInt KNestingLimit(10);

//
// this directory contains xml-format files for
// Service-elements
//
_LIT(KCapabilityDirC, "c:\\private\\101F99F6\\capability\\");
_LIT(KCapabilityDirCImport, "c:\\private\\101F99F6\\import\\");
_LIT(KCapabilityDirZ, "z:\\private\\101F99F6\\capability\\");

//
// folder service constants
//
_LIT(KFolderServiceName, "Folder-Browsing");
_LIT(KFolderServiceUid, "F9EC7BC4-953c-11d2-984E-525400DC9E09");
_LIT(KFolderServiceVersion, "1.0");
_LIT(KFolderServiceType, "x-obex/folder-listing");
_LIT(KFolderMemoryTypeTag, "MemType");
_LIT(KFolderNameTag, "Folder");

_LIT(KFolderImages, "Images");
_LIT(KFolderGraphics, "Graphics");
_LIT(KFolderTones, "Tones");
_LIT(KFolderMusic, "Music");
_LIT(KFolderVideos, "Videos");
_LIT(KFolderInstalls, "Installs");

_LIT( KSyncSolutionsServiceName, "SyncSolutions" );
_LIT( KSyncSolutionsServiceVersion, "1.0" );
_LIT( KSyncCalendar, "Calendar" );
_LIT( KSyncContacts, "Contacts" );
_LIT( KSyncSms, "Sms" );
_LIT( KSyncBookmark, "Bookmark" );

_LIT( KDesktopSync, "DesktopSync" );

enum TFolderTypes
    {
    EFolderImages,
    EFolderGraphics,
    EFolderTones,
    EFolderMusic,
    EFolderVideos,
    EFolderInstalls
    };


//
// misc capabily document strings
//
_LIT(KServiceHeader,   "<!-- Capability Object Service -->");
_LIT(KXmlVersion,      "<?xml version=\"1.0\" ?>");
_LIT(KComment1,        "<!-- OBEX Capability Object -->");
_LIT(KDocType,         "<!DOCTYPE Capability SYSTEM \"obex-capability.dtd\">");
_LIT(KRootBegin,       "<Capability Version=\"1.0\">");
_LIT(KRootEnd,         "</Capability>");
_LIT(KCommentBegin,    "<!--");
_LIT(KCommentEnd,      "-->");
_LIT(KFormatText, "  ");
_LIT(KXmlExtension, ".xml");


//
// manufacturers
//
_LIT(KManufacturerNokia, "Nokia");
_LIT(KManufacturerEricsson, "Ericsson");
_LIT(KManufacturerMotorola, "Motorola");
_LIT(KManufacturerPanasonic, "Panasonic");
_LIT(KManufacturerPsion, "Psion");
_LIT(KManufacturerIntel, "Intel");
_LIT(KManufacturerCogent, "Cogent");
_LIT(KManufacturerCirrus, "Cirrus");
_LIT(KManufacturerLinkup, "Linkup");
_LIT(KManufacturerTexasInstruments, "TexasInstruments");


//
// media types (TMediaType in e32std.h)
//
_LIT(KMediaMMC, "MMC");     // memory card
_LIT(KMediaFlash, "DEV");   // device memory

//
// TElementType contains all element types used in 
// capabilty xml document
//
//
enum TElementType
    {
    ECapability,
    EGeneral,
    EManufacturer,
    EModel,
    ESN,
    EOEM,
    ESW,
    EFW,
    EHW,
    ELanguage,
    EMemory,
    EMemType,
    ELocation,
    EFree,
    EUsed,
    EShared,
    EFileSize,
    EFolderSize,
    EFileNLen,
    EFolderNLen,
    ECaseSenN,

    EExt,
    EXNam,
    EXVal,
    
    EObject,
    EType,
    EName_Ext,
    ESize,

    EAccess,
    EProtocol,
    EEndpoint,
    ETarget,

    EInbox,
    EService,   
    EName,
    EUUID,
    EVersion,

    EAttVersion,
    EAttDate,
    
    EAttCurrentNetwork,
    EAttHomeNetwork,
    EAttCountryCode
    };




//
// KXmlTokens contains all tokens (=tags) used in 
// capabilty xml document
//
//

typedef const TText*  TTag;
NONSHARABLE_STRUCT( TXmlToken )
    {
    TInt id;
    TTag tag;
    };

const TXmlToken KXmlTokens[] =
    {
    {ECapability,   _S("Capability")},
    {EGeneral,      _S("General")},
    {EManufacturer, _S("Manufacturer")},
    {EModel,        _S("Model")},
    {ESN,           _S("SN")}, 
    {EOEM,          _S("OEM")},
    {ESW,           _S("SW")},
    {EFW,           _S("FW")},
    {EHW,           _S("HW")},
    {ELanguage,     _S("Language")},
    {EMemory,       _S("Memory")},
    {EMemType,      _S("MemType")},
    {ELocation,     _S("Location")},
    {EFree,         _S("Free")},
    {EUsed,         _S("Used")},
    {EShared,       _S("Shared")},
    {EFileSize,     _S("FileSize")},
    {EFolderSize,   _S("FolderSize")},
    {EFileNLen,     _S("FileNLen")},
    {EFolderNLen,   _S("FolderNLen")},
    {ECaseSenN,     _S("CaseSenN")},
    {EExt,          _S("Ext")},
    {EXNam,         _S("XNam")},
    {EXVal,         _S("XVal")},
    {EObject,       _S("Object")},
    {EType,         _S("Type")},
    {EName_Ext,     _S("Name_Ext")},
    {ESize,         _S("Size")},
    {EAccess,       _S("Access")},
    {EProtocol,     _S("Protocol")},
    {EEndpoint,     _S("Endpoint")},
    {ETarget,       _S("Target")},
    {EInbox,        _S("Inbox")},
    {EService,      _S("Service")},
    {EName,         _S("Name")},
    {EUUID,         _S("UUID")},
    {EVersion,      _S("Version")},
    {EXNam,             _S("XNam")},
    {EXVal,             _S("XVal")},
    {EAttVersion,       _S("Version")},
    {EAttDate,          _S("Date")},
    {EAttCurrentNetwork,_S("CurrentNetwork")},
    {EAttHomeNetwork,   _S("HomeNetwork")},
    {EAttCountryCode,   _S("CountryCode")}  
    };

#define NUMXMLTOKENS (sizeof(KXmlTokens)/sizeof(TXmlToken))




//
// string array to convert symbian TLanguage enums to 
// ISO 639 language codes
// 
// See "http://www.sil.org/iso639-3/default.asp" 
//

typedef const TText*  TLangString;
NONSHARABLE_STRUCT( TLangStringStruct )
    {
    TInt id;
    TLangString lang;
    };

/**
 * Language mapping table.
 */
const TLangStringStruct KLangStrings[] =
    {
    {ELangEnglish,   _S("en")},
    {ELangFrench,    _S("fr")},
    {ELangGerman,    _S("de")},
    {ELangSpanish,   _S("es")},
    {ELangItalian,   _S("it")},
    {ELangSwedish,   _S("sv")},
    {ELangDanish,    _S("da")},
    {ELangNorwegian, _S("no")},
    {ELangFinnish, _S("fi")},

    {ELangAmerican, _S("en_US")},
    {ELangSwissFrench, _S("fr_CH")},
    {ELangSwissGerman, _S("de_CH")},
    {ELangPortuguese, _S("pt")},
    {ELangTurkish, _S("tr")},
    {ELangIcelandic, _S("is")},
    {ELangRussian, _S("ru")},
    {ELangHungarian, _S("hu")},
    {ELangDutch, _S("nl")},
    
    {ELangBelgianFlemish, _S("nl_BE")},
    {ELangAustralian, _S("aus")},
    {ELangBelgianFrench, _S("fr_BE")},
    {ELangAustrian, _S("de_AT")},
    {ELangNewZealand, _S("en_NZ")},
    {ELangInternationalFrench, _S("fr")},
    {ELangCzech, _S("cs")},
    {ELangSlovak, _S("sk")},
    {ELangPolish, _S("pl")},
    
    {ELangSlovenian, _S("sl")},
    {ELangTaiwanChinese, _S("zh_TW")},
    {ELangHongKongChinese, _S("zh_HK")},
    {ELangPrcChinese, _S("zh_CN")},
    {ELangJapanese, _S("ja")},
    {ELangThai, _S("th")},
    {ELangAfrikaans, _S("af")},
    {ELangAlbanian, _S("sq")},
    {ELangAmharic, _S("am")},
    
    {ELangArabic, _S("ar")},
    {ELangArmenian, _S("hy")},
    {ELangTagalog, _S("tl")},
    {ELangBelarussian, _S("be")},
    {ELangBengali, _S("bn")},
    {ELangBulgarian, _S("bg")},
    {ELangBurmese, _S("my")},
    {ELangCatalan, _S("ca")},
    {ELangCroatian, _S("hr")},
    
    {ELangCanadianEnglish, _S("en_CA")},
    {ELangInternationalEnglish, _S("en")},
    {ELangSouthAfricanEnglish, _S("en_ZA")},
    {ELangEstonian, _S("et")},
    {ELangFarsi, _S("fa")},
    {ELangCanadianFrench, _S("fr_CA")},
    {ELangScotsGaelic, _S("gd")},
    {ELangGeorgian, _S("ka")},
    {ELangGreek, _S("el")},
    
    {ELangCyprusGreek, _S("el_CY")},
    {ELangGujarati, _S("gu")},
    {ELangHebrew, _S("he")},
    {ELangHindi, _S("hi")},
    {ELangIndonesian, _S("id")},
    {ELangIrish, _S("ga")},
    {ELangSwissItalian, _S("it_CH")},
    {ELangKannada, _S("kn")},
    {ELangKazakh, _S("kk")},
    
    {ELangKhmer, _S("km")},
    {ELangKorean, _S("ko")},
    {ELangLao, _S("lo")},
    {ELangLatvian, _S("lv")},
    {ELangLithuanian, _S("lt")},
    {ELangMacedonian, _S("mk")},
    {ELangMalay, _S("ms")},
    {ELangMalayalam, _S("ml")},
    {ELangMarathi, _S("mr")},
    
    {ELangMoldavian, _S("mo")},
    {ELangMongolian, _S("mn")},
    {ELangNorwegianNynorsk, _S("nn")},
    {ELangBrazilianPortuguese, _S("pt_BR")},
    {ELangPunjabi, _S("pa")},
    {ELangRomanian, _S("ro")},
    {ELangSerbian, _S("sr")},
    {ELangSinhalese, _S("si")},
    {ELangSomali, _S("so")},
    
    {ELangInternationalSpanish, _S("es")},
    {ELangLatinAmericanSpanish, _S("es_US")},
    {ELangSwahili, _S("sw")},
    {ELangFinlandSwedish, _S("sv_FI")},
    {ELangReserved1, _S("en")},  
    {ELangTamil, _S("ta")},
    {ELangTelugu, _S("te")},
    {ELangTibetan, _S("bo")},
    {ELangTigrinya, _S("ti")},
    
    {ELangCyprusTurkish, _S("tr_CY")},
    {ELangTurkmen, _S("tk")},
    {ELangUkrainian, _S("uk")},
    {ELangUrdu, _S("ur")},
    {ELangReserved2, _S("en")},
    {ELangVietnamese, _S("vi")},
    {ELangWelsh, _S("cy")},
    {ELangZulu, _S("zu")},
    {ELangOther, _S("other")},
    
    {ELangManufacturerEnglish, _S("en")},
    {ELangSouthSotho, _S("st")},
    {ELangBasque, _S("eu")},
    {ELangGalician, _S("gl")},
    {ELangJavanese, _S("jv")},
    {ELangMaithili, _S("bh")},
    {ELangAzerbaijani_Latin, _S("az")},
    {ELangAzerbaijani_Cyrillic, _S("az")},
    {ELangOriya, _S("or")},
    {ELangBhojpuri, _S("bh")},
    {ELangSundanese, _S("su")},
    {ELangKurdish_Latin, _S("ku")},
    {ELangKurdish_Arabic, _S("ku")},
    {ELangPashto, _S("ps")},
    {ELangHausa, _S("ha")},
    {ELangOromo, _S("om")},
    {ELangUzbek_Latin, _S("uz")},
    {ELangUzbek_Cyrillic, _S("uz")},
    {ELangSindhi_Arabic, _S("sd")},
    {ELangSindhi_Devanagari, _S("sd")},
    {ELangYoruba, _S("yo")},
    {ELangCebuano, _S("ceb")},
    {ELangIgbo, _S("ig")},
    {ELangMalagasy, _S("mg")},
    {ELangNepali, _S("ne")},
	{ELangAssamese, _S("as")},
    {ELangShona, _S("sn")},
    {ELangZhuang, _S("za")},
    {ELangMadurese, _S("mad")},
    
    {ELangEnglish_Apac, _S("en")},
    {ELangEnglish_Taiwan, _S("en")},
    {ELangEnglish_HongKong, _S("en")},
    {ELangEnglish_Prc, _S("en")},
    {ELangEnglish_Japan, _S("en")},
    {ELangEnglish_Thailand, _S("en")},
    
	{ELangFulfulde, _S("ff")},
    {ELangTamazight, _S("ber")},
    {ELangBolivianQuechua, _S("qu")},
    {ELangPeruQuechua, _S("qu")},
    {ELangEcuadorQuechua, _S("qu")},
    {ELangTajik_Cyrillic, _S("tg")},
    {ELangTajik_PersoArabic, _S("tg")},
    {ELangNyanja, _S("ny")},
    {ELangHaitianCreole, _S("ht")},
    {ELangLombard, _S("lmo")},
    {ELangKoongo, _S("kg")},
    {ELangAkan, _S("ak")},
    {ELangHmong, _S("hmn")},
    {ELangYi, _S("ii")},
    {ELangTshiluba, _S("lu")},
    {ELangIlocano, _S("ilo")},
    {ELangUyghur, _S("ug")},
    {ELangNeapolitan, _S("nap")},
    {ELangRwanda, _S("rw")},
    {ELangXhosa, _S("xh")},
    {ELangBalochi, _S("bal")},
    {ElangHiligaynon, _S("hil")},
    {ELangMinangkabau, _S("min")},
    {ELangMakhuwa, _S("vmw")},
    {ELangSantali, _S("sat")},
    {ELangGikuyu, _S("ki")},
    {ELangMoore, _S("mos")},
    {ELangGuarani, _S("gnHI")},
    {ELangRundi, _S("ru")},
    {ELangRomani_Latin, _S("rom")},
    {ELangRomani_Cyrillic, _S("rom")},
    {ELangTswana, _S("tn")},
    {ELangKanuri, _S("kr")},
    {ELangKashmiri_Devanagari, _S("ks")},
    {ELangKashmiri_PersoArabic, _S("ks")},
    {ELangUmbundu, _S("umb")},
    {ELangKonkani, _S("kok")},
    {ELangBalinese, _S("ban")},
    {ELangNorthernSotho, _S("nso")},
    {ELangWolof, _S("wo")},
    {ELangBemba, _S("bmy")}, 	 	 	
    {ELangTsonga, _S("ts")},
    {ELangYiddish, _S("yi")},
    {ELangKirghiz, _S("ky")},
    {ELangGanda, _S("lg")},
    {ELangSoga, _S("xog")},
    {ELangMbundu, _S("kmb")},
    {ELangBambara, _S("bm")},
    {ELangCentralAymara, _S("ay")},
    {ELangZarma, _S("dje")},
    {ELangLingala, _S("ln")},
    {ELangBashkir, _S("ba")},
    {ELangChuvash, _S("cv")},
    {ELangSwati, _S("ss")},
    {ELangTatar, _S("tt")},
    {ELangSouthernNdebele, _S("nr")},
    {ELangSardinian, _S("sc")},
    {ELangScots, _S("sco")},
    {ELangMeitei, _S("mni")},
    {ELangWalloon, _S("wa")},
    {ELangKabardian, _S("kbd")},
    {ELangMazanderani, _S("mzn")},
    {ELangGilaki, _S("glk")},
    {ELangShan, _S("shn")},
    {ELangLuyia, _S("luy")},
    {ELanguageLuo, _S("luo")},
    {ELangSukuma, _S(" 	suk")},
    {ELangAceh, _S("ace")},
    {ELangMalay_Apac, _S("ms")}
    };

#define NUMLANGSTRINGS (sizeof(KLangStrings)/sizeof(TLangStringStruct))




#endif // capability.h

