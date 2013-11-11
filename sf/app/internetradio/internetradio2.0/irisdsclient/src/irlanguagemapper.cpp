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
* Description:  The implementation for presentation elements.
*
*/


#include <btsdp.h>

#include "irlanguagemapper.h"
#include "irdebug.h"

//============================= MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// IRLanguageMapper::MapLanguage
// ---------------------------------------------------------------------------

TBufC8<KIRLanguageCodeLength> IRLanguageMapper::MapLanguage(
                TLanguage aLanguage )
    {
    IRLOG_DEBUG( "IRLanguageMapper::MapLanguage - Entering" );
    TUint16 value = 0;

    switch ( aLanguage )
        {
        case ELangEnglish:
            value = KLanguageEnglish;
            break;
        case ELangFrench:
            value = KLanguageFrench;
            break;
        case ELangGerman:
            value = KLanguageGerman;
            break;
        case ELangSpanish:
            value = KLanguageSpanish;
            break;
        case ELangItalian:
            value = KLanguageItalian;
            break;
        case ELangSwedish:
            value = KLanguageSwedish;
            break;
        case ELangDanish:
            value = KLanguageDanish;
            break;
        case ELangNorwegian:
            value = KLanguageNorwegian;
            break;
        case ELangFinnish:
            value = KLanguageFinnish;
            break;
        case ELangAmerican:
            value = KLanguageEnglish;
            break;
        case ELangSwissFrench:
            value = KLanguageFrench;
            break;
        case ELangSwissGerman:
            value = KLanguageGerman;
            break;
        case ELangPortuguese:
            value = KLanguagePortuguese;
            break;
        case ELangTurkish:
            value = KLanguageTurkish;
            break;
        case ELangIcelandic:
            value = KLanguageIcelandic;
            break;
        case ELangRussian:
            value = KLanguageRussian;
            break;
        case ELangHungarian:
            value = KLanguageHungarian;
            break;
        case ELangDutch:
            value = KLanguageDutch;
            break;
        case ELangBelgianFlemish:
            value = KLanguageDutch;
            break;
        case ELangAustralian:
            value = KLanguageEnglish;
            break;
        case ELangBelgianFrench:
            value = KLanguageFrench;
            break;
        case ELangAustrian:
            value = KLanguageGerman;
            break;
        case ELangNewZealand:
            value = KLanguageEnglish;
            break;
        case ELangInternationalFrench:
            value = KLanguageFrench;
            break;
        case ELangCzech:
            value = KLanguageCzech;
            break;
        case ELangSlovak:
            value = KLanguageSlovak;
            break;
        case ELangPolish:
            value = KLanguagePolish;
            break;
        case ELangSlovenian:
            value = KLanguageSlovenian;
            break;
        case ELangTaiwanChinese:
            value = KLanguageChinese;
            break;
        case ELangHongKongChinese:
            value = KLanguageChinese;
            break;
        case ELangPrcChinese:
            value = KLanguageChinese;
            break;
        case ELangJapanese:
            value = KLanguageJapanese;
            break;
        case ELangThai:
            value = KLanguageThai;
            break;
        case ELangAfrikaans:
            value = KLanguageAfrikaans;
            break;
        case ELangAlbanian:
            value = KLanguageAlbanian;
            break;
        case ELangAmharic:
            value = KLanguageAmharic;
            break;
        case ELangArabic:
            value = KLanguageArabic;
            break;
        case ELangArmenian:
            value = KLanguageArmenian;
            break;
        case ELangTagalog:
            value = KLanguageTagalog;
            break;
        case ELangBelarussian:
            value = KLanguageRussian;
            break;
        case ELangBengali:
            value = KLanguageBengali;
            break;
        case ELangBulgarian:
            value = KLanguageBulgarian;
            break;
        case ELangBurmese:
            value = KLanguageBurmese;
            break;
        case ELangCatalan:
            value = KLanguageCatalan;
            break;
        case ELangCroatian:
            value = KLanguageCroatian;
            break;
        case ELangCanadianEnglish:
            value = KLanguageEnglish;
            break;
        case ELangInternationalEnglish:
            value = KLanguageEnglish;
            break;
        case ELangSouthAfricanEnglish:
            value = KLanguageEnglish;
            break;
        case ELangEstonian:
            value = KLanguageEstonian;
            break;
        case ELangFarsi:
            value = KLanguagePersian;
            break;
        case ELangCanadianFrench:
            value = KLanguageFrench;
            break;
        case ELangScotsGaelic:
            value = KLanguageScotsGaelic;
            break;
        case ELangGeorgian:
            value = KLanguageGeorgian;
            break;
        case ELangGreek:
            value = KLanguageGreek;
            break;
        case ELangCyprusGreek:
            value = KLanguageGreek;
            break;
        case ELangGujarati:
            value = KLanguageGujarati;
            break;
        case ELangHebrew:
            value = KLanguageHebrew;
            break;
        case ELangHindi:
            value = KLanguageHindi;
            break;
        case ELangIndonesian:
            value = KLanguageIndonesian;
            break;
        case ELangIrish:
            value = KLanguageIrish;
            break;
        case ELangSwissItalian:
            value = KLanguageItalian;
            break;
        case ELangKannada:
            value = KLanguageKannada;
            break;
        case ELangKazakh:
            value = KLanguageKazakh;
            break;
        case ELangKhmer:
            value = KLanguageCambodian;
            break;
        case ELangKorean:
            value = KLanguageKorean;
            break;
        case ELangLao:
            value = KLanguageLaothian;
            break;
        case ELangLatvian:
            value = KLanguageLatvian;
            break;
        case ELangLithuanian:
            value = KLanguageLithuanian;
            break;
        case ELangMacedonian:
            value = KLanguageMacedonian;
            break;
        case ELangMalay:
            value = KLanguageMalay;
            break;
        case ELangMalayalam:
            value = KLanguageMalayalam;
            break;
        case ELangMarathi:
            value = KLanguageMarathi;
            break;
        case ELangMoldavian:
            value = KLanguageMoldavian;
            break;
        case ELangMongolian:
            value = KLanguageMongolian;
            break;
        case ELangNorwegianNynorsk:
            value = KLanguageNorwegian;
            break;
        case ELangBrazilianPortuguese:
            value = KLanguagePortuguese;
            break;
        case ELangPunjabi:
            value = KLanguagePunjabi;
            break;
        case ELangRomanian:
            value = KLanguageRomanian;
            break;
        case ELangSerbian:
            value = KLanguageSerbian;
            break;
        case ELangSinhalese:
            value = KLanguageSinhalese;
            break;
        case ELangSomali:
            value = KLanguageSomali;
            break;
        case ELangInternationalSpanish:
            value = KLanguageSpanish;
            break;
        case ELangLatinAmericanSpanish:
            value = KLanguageSpanish;
            break;
        case ELangSwahili:
            value = KLanguageSwahili;
            break;
        case ELangFinlandSwedish:
            value = KLanguageSwedish;
            break;
        case ELangTamil:
            value = KLanguageTamil;
            break;
        case ELangTelugu:
            value = KLanguageTelugu;
            break;
        case ELangTibetan:
            value = KLanguageTibetan;
            break;
        case ELangTigrinya:
            value = KLanguageTigrinya;
            break;
        case ELangCyprusTurkish:
            value = KLanguageTurkish;
            break;
        case ELangTurkmen:
            value = KLanguageTurkmen;
            break;
        case ELangUkrainian:
            value = KLanguageUkrainian;
            break;
        case ELangUrdu:
            value = KLanguageUrdu;
            break;
        case ELangVietnamese:
            value = KLanguageVietnamese;
            break;
        case ELangWelsh:
            value = KLanguageWelsh;
            break;
        case ELangZulu:
            value = KLanguageZulu;
            break;
        default:
            value = KLanguageEnglish;
            break;
        }

    /* translate the two bytes into actual 'string' */
    TBuf8<KIRLanguageCodeLength> ret;
    TUint char1 = ( value >> 8 ) & 0xFFu;
    TUint char2 = value & 0xFFu;
    ret.Append( static_cast<TUint8> ( char1 ) );
    ret.Append( static_cast<TUint8> ( char2 ) );
    IRLOG_DEBUG( "IRLanguageMapper::MapLanguage - Exiting" );
    return ret;
    }

//  End of File  
