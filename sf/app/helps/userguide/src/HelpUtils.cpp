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
* Description: 
*
*/

#include <QFile>
#include <QMap>
#include <QLocale>

#include <hbstringutil.h>

#include "HelpUtils.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef Q_OS_SYMBIAN

#include <qsysteminfo.h>
#include <xqappmgr.h>

#include <e32const.h>
#include <eikenv.h>
#include <zipfile.h>

void LoadFileFromZipL(const TDesC& aZipName, const TDesC& aFileName, QString& desBuffer)
    {
    
    // open the zip file
    RFs& fs = CEikonEnv::Static()->FsSession();
    CZipFile* zipFile = CZipFile::NewL(fs, aZipName);
    CleanupStack::PushL(zipFile);
    TInt err = zipFile->OpenL();

    if(err == KErrNone)
        {
        CZipFileMember* member = zipFile->MemberL(aFileName);
        if(member) // member exist
            {
            CleanupStack::PushL(member);

            RZipFileMemberReaderStream* stream; 
            zipFile->GetInputStreamL(member, stream);
            CleanupStack::PushL(stream);
            
            HBufC8* buffer = HBufC8::NewLC(member->UncompressedSize());
            TPtr8 ptr(buffer->Des());
            User::LeaveIfError(stream->Read(ptr, member->UncompressedSize()));

            QByteArray byteArray((const char*)buffer->Ptr(), buffer->Length());
            desBuffer.append(byteArray);

            CleanupStack::PopAndDestroy(buffer);
            CleanupStack::PopAndDestroy(stream);
            CleanupStack::PopAndDestroy(member);
            }
        }
    
    zipFile->Close();
    CleanupStack::PopAndDestroy(zipFile);

    }

void FixPathSlash(TDes& aPath)
    {
    for(TInt i=0; i<aPath.Length(); i++)
        {
        if(aPath[i] == '/') aPath[i] = '\\';
        }
    }

void HelpUtils::loadHtmlFromZipFile(const QString& path, const QString& htmlFile, QString& htmlContent)
    {
    TFileName zipNameDes(TPtrC(path.utf16(), path.length()));
    TFileName subFileDes(TPtrC(htmlFile.utf16(), htmlFile.length()));
    FixPathSlash(zipNameDes);
    FixPathSlash(subFileDes);

    htmlContent.clear(); // clear all data

    QT_TRAP_THROWING(LoadFileFromZipL(zipNameDes, subFileDes, htmlContent));
    }
#else
void HelpUtils::loadHtmlFromZipFile(const QString& path, const QString& htmlFile, QString& htmlContent)
{
	Q_UNUSED(path);
	Q_UNUSED(htmlFile);
	Q_UNUSED(htmlContent);
	return;
}
#endif

QString HelpUtils::rootPath()
{
	return QString("z:/");
}

QString HelpUtils::UILocaleFromQtToSymbian()
{
#ifdef Q_OS_SYMBIAN

	static QMap<QString, int> iso_to_symbian_map;
	iso_to_symbian_map.insert("en_GB" ,ELangEnglish);             
	iso_to_symbian_map.insert("fr_FR" ,ELangFrench);              
	iso_to_symbian_map.insert("de_DE" ,ELangGerman);              
	iso_to_symbian_map.insert("es_ES" ,ELangSpanish);             
	iso_to_symbian_map.insert("it_IT" ,ELangItalian);             
	iso_to_symbian_map.insert("sv_SE" ,ELangSwedish);             
	iso_to_symbian_map.insert("da_DK" ,ELangDanish);              
	iso_to_symbian_map.insert("no_NO" ,ELangNorwegian);           
	iso_to_symbian_map.insert("fi_FI" ,ELangFinnish);             
	iso_to_symbian_map.insert("en_US" ,ELangAmerican);            
	iso_to_symbian_map.insert("fr_CH" ,ELangSwissFrench);         
	iso_to_symbian_map.insert("de_CH" ,ELangSwissGerman);         
	iso_to_symbian_map.insert("pt_PT" ,ELangPortuguese);          
	iso_to_symbian_map.insert("tr_TR" ,ELangTurkish);             
	iso_to_symbian_map.insert("is_IS" ,ELangIcelandic);           
	iso_to_symbian_map.insert("ru_RU" ,ELangRussian);             
	iso_to_symbian_map.insert("hu_HU" ,ELangHungarian);           
	iso_to_symbian_map.insert("nl_NL" ,ELangDutch);               
	iso_to_symbian_map.insert("nl_BE" ,ELangBelgianFlemish);      
	iso_to_symbian_map.insert("en_AU" ,ELangAustralian);          
	iso_to_symbian_map.insert("fr_BE" ,ELangBelgianFrench);       
	iso_to_symbian_map.insert("de_AT" ,ELangAustrian);            
	iso_to_symbian_map.insert("en_NZ" ,ELangNewZealand);          
	iso_to_symbian_map.insert("fr"    ,ELangInternationalFrench); 
	iso_to_symbian_map.insert("cs_CZ" ,ELangCzech);               
	iso_to_symbian_map.insert("sk_SK" ,ELangSlovak);              
	iso_to_symbian_map.insert("pl_PL" ,ELangPolish);              
	iso_to_symbian_map.insert("sl_SI" ,ELangSlovenian);           
	iso_to_symbian_map.insert("zh_TW" ,ELangTaiwanChinese);       
	iso_to_symbian_map.insert("zh_HK" ,ELangHongKongChinese);     
	iso_to_symbian_map.insert("zh_CN" ,ELangPrcChinese);          
	iso_to_symbian_map.insert("ja_JP" ,ELangJapanese);            
	iso_to_symbian_map.insert("th_TH" ,ELangThai);                
	iso_to_symbian_map.insert("af"    ,ELangAfrikaans);           
	iso_to_symbian_map.insert("sq_AL" ,ELangAlbanian);            
	iso_to_symbian_map.insert("am_ET" ,ELangAmharic);             
	iso_to_symbian_map.insert("ar_AE" ,ELangArabic);              
	iso_to_symbian_map.insert("hy_AM" ,ELangArmenian);            
	iso_to_symbian_map.insert("tl_PH" ,ELangTagalog);             
	iso_to_symbian_map.insert("be_BY" ,ELangBelarussian);         
	iso_to_symbian_map.insert("bn"    ,ELangBengali);             
	iso_to_symbian_map.insert("bg_BG" ,ELangBulgarian);           
	iso_to_symbian_map.insert("my_MM" ,ELangBurmese);             
	iso_to_symbian_map.insert("ca_ES" ,ELangCatalan);             
	iso_to_symbian_map.insert("hr_HR" ,ELangCroatian);            
	iso_to_symbian_map.insert("en_CA" ,ELangCanadianEnglish);     
	iso_to_symbian_map.insert( "en"   ,ELangInternationalEnglish);
	iso_to_symbian_map.insert("en_ZA" ,ELangSouthAfricanEnglish); 
	iso_to_symbian_map.insert("et_EE" ,ELangEstonian);            
	iso_to_symbian_map.insert("fa_IR" ,ELangFarsi);               
	iso_to_symbian_map.insert("fr_CA" ,ELangCanadianFrench);      
	iso_to_symbian_map.insert("gd_GB" ,ELangScotsGaelic);         
	iso_to_symbian_map.insert("ka_GE" ,ELangGeorgian);            
	iso_to_symbian_map.insert("el_GR" ,ELangGreek);               
	iso_to_symbian_map.insert("el_GR" ,ELangCyprusGreek);         
	iso_to_symbian_map.insert("gu_IN" ,ELangGujarati);            
	iso_to_symbian_map.insert("he_IL" ,ELangHebrew);              
	iso_to_symbian_map.insert("hi_IN" ,ELangHindi);               
	iso_to_symbian_map.insert("id_ID" ,ELangIndonesian);          
	iso_to_symbian_map.insert("ga_IE" ,ELangIrish);               
	iso_to_symbian_map.insert("it_CH" ,ELangSwissItalian);        
	iso_to_symbian_map.insert("kn_IN" ,ELangKannada);             
	iso_to_symbian_map.insert("kk_KZ" ,ELangKazakh);              
	iso_to_symbian_map.insert("km_KH" ,ELangKhmer);               
	iso_to_symbian_map.insert("ko_KR" ,ELangKorean);              
	iso_to_symbian_map.insert("lo_LA" ,ELangLao);                 
	iso_to_symbian_map.insert("lv_LV" ,ELangLatvian);             
	iso_to_symbian_map.insert("lt_LT" ,ELangLithuanian);          
	iso_to_symbian_map.insert("mk_MK" ,ELangMacedonian);          
	iso_to_symbian_map.insert("ms_MY" ,ELangMalay);               
	iso_to_symbian_map.insert("ml_IN" ,ELangMalayalam);           
	iso_to_symbian_map.insert("mr_IN" ,ELangMarathi);             
	iso_to_symbian_map.insert("mo_MD" ,ELangMoldavian);           
	iso_to_symbian_map.insert("mn_MN" ,ELangMongolian);           
	iso_to_symbian_map.insert("nn_NO" ,ELangNorwegianNynorsk);    
	iso_to_symbian_map.insert("pt_BR" ,ELangBrazilianPortuguese); 
	iso_to_symbian_map.insert("pa"    ,ELangPunjabi);             
	iso_to_symbian_map.insert("ro_RO" ,ELangRomanian);            
	iso_to_symbian_map.insert("sr_YU" ,ELangSerbian);             
	iso_to_symbian_map.insert("si_LK" ,ELangSinhalese);           
	iso_to_symbian_map.insert("so_SO" ,ELangSomali);              
	iso_to_symbian_map.insert( "es"   ,ELangInternationalSpanish);
	iso_to_symbian_map.insert( "es"   ,ELangLatinAmericanSpanish);
	iso_to_symbian_map.insert("sw"    ,ELangSwahili);             
	iso_to_symbian_map.insert("sv_FI" ,ELangFinlandSwedish);      
	iso_to_symbian_map.insert("ta_IN" ,ELangTamil);               
	iso_to_symbian_map.insert("te_IN" ,ELangTelugu);              
	iso_to_symbian_map.insert("bo_CN" ,ELangTibetan);             
	iso_to_symbian_map.insert("ti_ER" ,ELangTigrinya);            
	iso_to_symbian_map.insert("tr_GR" ,ELangCyprusTurkish);       
	iso_to_symbian_map.insert("tk_TM" ,ELangTurkmen);             
	iso_to_symbian_map.insert("uk_UA" ,ELangUkrainian);           
	iso_to_symbian_map.insert("ur_PK" ,ELangUrdu);                
	iso_to_symbian_map.insert("vi_VN" ,ELangVietnamese);          
	iso_to_symbian_map.insert("cy_GB" ,ELangWelsh);               
	iso_to_symbian_map.insert("zu_ZA" ,ELangZulu);                
	iso_to_symbian_map.insert("st_LS" ,ELangSouthSotho);          
	iso_to_symbian_map.insert("eu_ES" ,ELangBasque);       
	iso_to_symbian_map.insert("gl_ES" ,ELangGalician);                          
	iso_to_symbian_map.insert("en"    ,ELangEnglish_Apac);                
	iso_to_symbian_map.insert("en_TW" ,ELangEnglish_Taiwan);              
	iso_to_symbian_map.insert("en_HK" ,ELangEnglish_HongKong);                          
	iso_to_symbian_map.insert("en_CN" ,ELangEnglish_Prc);         
	iso_to_symbian_map.insert("en_JP" ,ELangEnglish_Japan);       
	iso_to_symbian_map.insert("en_TH" ,ELangEnglish_Thailand);     
	iso_to_symbian_map.insert("ms"    ,ELangMalay_Apac); 

	QString langName = QLocale::system().name();
	return QString("%1").arg(iso_to_symbian_map.value(langName),2,10,QChar('0'));
#else
	return QString("%1").arg(1,2,10,QChar('0'));
#endif
}

int HelpUtils::launchApplication(const QString& appUid)
{
#ifdef Q_OS_SYMBIAN

    QString uriStr("appto://");
    uriStr.append(appUid);
    QUrl uri(uriStr); 
    
    XQApplicationManager appmgr;
    XQAiwRequest* req5 = appmgr.create(uri, false);
    
    QList<QVariant> args;
    args << uri.encodedQuery();
    req5->setArguments(args);
    
    if (!req5->send())
    {
        return req5->lastError();
    }
#else
	Q_UNUSED(appUid);
#endif
    return 0;
}

bool HelpUtils::suppportFeatureID(int featureID)
{
#ifdef Q_OS_SYMBIAN
	if(featureID == -1)
	{
		return true;
	}
    QtMobility::QSystemInfo sysInfo;
	return sysInfo.hasFeatureSupported((QtMobility::QSystemInfo::Feature)featureID);
#else
	Q_UNUSED(featureID);
	return true;
#endif
}

Qt::Orientation HelpUtils::defaultOrientation()
{
	return Qt::Vertical;
}

Qt::SortOrder HelpUtils::sortOrder()
{
	return Qt::AscendingOrder;
}

int HelpUtils::findStr(const QString& strFrom, const QString& strToFind)
{
	return HbStringUtil::findC(strFrom, strToFind);
}

int HelpUtils::compareStr(const QString& str1, const QString& str2)
{
	return HbStringUtil::compareC(str1, str2);
}

// end of file
