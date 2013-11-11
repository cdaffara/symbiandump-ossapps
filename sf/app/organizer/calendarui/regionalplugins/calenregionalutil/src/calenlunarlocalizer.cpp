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
* Description:   Calendar Lunar Plugin 
*
*/



#include <qdatetime.h>
#include <qstringlist.h>
#include <hbi18ndef.h>
#include <hbglobal.h>
#include <hbparameterlengthlimiter.h>

#include <badesca.h> 
#include <eikenv.h>
#include <bautils.h>
#include <data_caging_path_literals.hrh>

#include "calendarui_debug.h"

#include "CalenLunarLocalizer.h"
#include "CalenLunarInfo.h"
#include "CalenLunarLocalizedInfo.h"
#include "CalenExtraRowFormatter.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenlunarlocalizerTraces.h"
#endif

//CONSTANTS
const TInt KCharMonth = 0x6708;
const TInt KCharFirstMonth = 0x6b63;
const TInt KCharChu = 0x521d;
const TInt KCharTen = 0x5341;
const TInt KCharTwenty= 0x5eff;
_LIT(KChineseDigits, 
     "\x0030\x4e00\x4e8c\x4e09\x56db\x4e94\x516d\x4e03\x516b\x4e5d");



// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarLocalizer* CCalenLunarLocalizer::NewL()
    {
    OstTraceFunctionEntry0( CCALENLUNARLOCALIZER_NEWL_ENTRY );
    CCalenLunarLocalizer* self = new (ELeave) CCalenLunarLocalizer;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    OstTraceFunctionExit0( CCALENLUNARLOCALIZER_NEWL_EXIT );
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::~CCalenLunarLocalizer
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarLocalizer::~CCalenLunarLocalizer()
    {
    OstTraceFunctionEntry0( CCALENLUNARLOCALIZER_CCALENLUNARLOCALIZER_ENTRY );
    delete iRowFormatter;
    OstTraceFunctionExit0( CCALENLUNARLOCALIZER_CCALENLUNARLOCALIZER_EXIT );
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::CCalenLunarLocalizer
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarLocalizer::CCalenLunarLocalizer() : iEikEnv( CEikonEnv::Static() )
    {
    OstTraceFunctionEntry0( DUP1_CCALENLUNARLOCALIZER_CCALENLUNARLOCALIZER_ENTRY );
    OstTraceFunctionExit0( DUP1_CCALENLUNARLOCALIZER_CCALENLUNARLOCALIZER_EXIT );
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::ConstructL
// -----------------------------------------------------------------------------
//
EXPORT_C void CCalenLunarLocalizer::ConstructL()
{
	OstTraceFunctionEntry0( CCALENLUNARLOCALIZER_CONSTRUCTL_ENTRY );
	iRowFormatter = CCalenExtraRowFormatter::NewL();

	iFestivalNames.append(hbTrId("txt_calendar_preview_title_spring_fest"));
	iFestivalNames.append(hbTrId("txt_calendar_preview_title_lantern_fest"));
	iFestivalNames.append(hbTrId("txt_calendar_preview_title_dragon_fest"));
	iFestivalNames.append(hbTrId("txt_calendar_preview_title_seventh_night_fest"));
	iFestivalNames.append(hbTrId("txt_calendar_preview_title_ghost_fest"));
	iFestivalNames.append(hbTrId("txt_calendar_preview_title_mid_autumn_fest"));
	iFestivalNames.append(hbTrId("txt_calendar_preview_title_double_fest"));
	iFestivalNames.append(hbTrId("txt_calendar_preview_title_eigth_day_fest"));
	iFestivalNames.append(hbTrId("txt_calendar_preview_title_new_year_fest"));

	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_li_chun"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_yu_shui"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_jing_zhe"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_chun_fen"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_qing_ming"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_gu_yu"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_li_xia"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_xiao_man"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_mang_zhong"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_xia_zhi"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_xiao_shu"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_da_shu"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_li_qiu"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_chu_shu"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_bai_lu"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_qiu_fen"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_han_lu"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_shaung_jiang"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_li_dong"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_xiao_xue"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_da_xue"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_dong_zhi"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_xiao_han"));
	iSolarTermNames.append(hbTrId("txt_calendar_preview_title_da_han"));

	iHeavenlyStemNames.append(hbTrId("txt_calendar_preview_title_jia"));
	iHeavenlyStemNames.append(hbTrId("txt_calendar_preview_title_yi"));
	iHeavenlyStemNames.append(hbTrId("txt_calendar_preview_title_bing"));
	iHeavenlyStemNames.append(hbTrId("txt_calendar_preview_title_ding"));
	iHeavenlyStemNames.append(hbTrId("txt_calendar_preview_title_wu"));
	iHeavenlyStemNames.append(hbTrId("txt_calendar_preview_title_ji"));
	iHeavenlyStemNames.append(hbTrId("txt_calendar_preview_title_geng"));
	iHeavenlyStemNames.append(hbTrId("txt_calendar_preview_title_xing"));
	iHeavenlyStemNames.append(hbTrId("txt_calendar_preview_title_reng"));
	iHeavenlyStemNames.append(hbTrId("txt_calendar_preview_title_gui"));

	iTerrestialBranchNames.append(hbTrId("txt_calendar_preview_title_tbranch_zi"));
	iTerrestialBranchNames.append(hbTrId("txt_calendar_preview_title_tbranch_chou"));
	iTerrestialBranchNames.append(hbTrId("txt_calendar_preview_title_tbranch_yin"));
	iTerrestialBranchNames.append(hbTrId("txt_calendar_preview_title_tbranch_mao"));
	iTerrestialBranchNames.append(hbTrId("txt_calendar_preview_title_tbranch_chen"));
	iTerrestialBranchNames.append(hbTrId("txt_calendar_preview_title_tbranch_si"));
	iTerrestialBranchNames.append(hbTrId("txt_calendar_preview_title_tbranch_wu"));
	iTerrestialBranchNames.append(hbTrId("txt_calendar_preview_title_tbranch_wei"));
	iTerrestialBranchNames.append(hbTrId("txt_calendar_preview_title_tbranch_shen"));
	iTerrestialBranchNames.append(hbTrId("txt_calendar_preview_title_tbranch_you"));
	iTerrestialBranchNames.append(hbTrId("txt_calendar_preview_title_tbranch_xu"));
	iTerrestialBranchNames.append(hbTrId("txt_calendar_preview_title_tbranch_hai"));

	iAnimalYearNames.append(hbTrId("txt_calendar_preview_title_year_of_the_rat"));
	iAnimalYearNames.append(hbTrId("txt_calendar_preview_title_year_of_the_ox"));
	iAnimalYearNames.append(hbTrId("txt_calendar_preview_title_year_of_the_tiger"));
	iAnimalYearNames.append(hbTrId("txt_calendar_preview_title_year_of_the_rabbit"));
	iAnimalYearNames.append(hbTrId("txt_calendar_preview_title_year_of_the_dragon"));
	iAnimalYearNames.append(hbTrId("txt_calendar_preview_title_year_of_the_snake"));
	iAnimalYearNames.append(hbTrId("txt_calendar_preview_title_year_of_the_horse"));
	iAnimalYearNames.append(hbTrId("txt_calendar_preview_title_year_of_the_sheep"));
	iAnimalYearNames.append(hbTrId("txt_calendar_preview_title_year_of_the_monkey"));
	iAnimalYearNames.append(hbTrId("txt_calendar_preview_title_year_of_the_rooster"));
	iAnimalYearNames.append(hbTrId("txt_calendar_preview_title_year_of_the_dog"));
	iAnimalYearNames.append(hbTrId("txt_calendar_preview_title_year_of_the_pig"));
	
	OstTraceFunctionExit0( CCALENLUNARLOCALIZER_CONSTRUCTL_EXIT );
}

// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::LocalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenLunarLocalizedInfo* CCalenLunarLocalizer::LocalizeL( TCalenLunarInfo& aInfo )
{
	OstTraceFunctionEntry0( CCALENLUNARLOCALIZER_LOCALIZEL_ENTRY );
	CCalenLunarLocalizedInfo* localized = CCalenLunarLocalizedInfo::NewL();
	CleanupStack::PushL( localized );


	if ( aInfo.HasFestival() ) 
	{
		QString festival = iFestivalNames.at(aInfo.iFestival);
		localized->iFestival.Set( 
			static_cast<const TUint16*>(festival.utf16()),festival.length());
	}

	if ( aInfo.HasSolarTerm() ) 
	{
		QString solarTerm = iSolarTermNames.at(aInfo.iSolarTerm);
		localized->iSolarTerm.Set( 
			static_cast<const TUint16*>(solarTerm.utf16()),solarTerm.length());
	}

	// Animal year
	QString animalYear = iAnimalYearNames.at(aInfo.AnimalYear());
	localized->iAnimalYear.Set( 
		static_cast<const TUint16*>(animalYear.utf16()),animalYear.length());

	// Lunar year
	QString heavenlyStemNames = iHeavenlyStemNames.at(aInfo.iHeavenlyStem);
	QString terrestialBranchNames = 
						iTerrestialBranchNames.at(aInfo.iTerrestialBranch);
	QString lunarYear = HbParameterLengthLimiter(
						hbTrId("txt_calendar_info_lunar_year")).arg(
						heavenlyStemNames).arg(terrestialBranchNames);
	localized->iLunarYear = 
			static_cast<const TUint16*> (lunarYear.utf16()), lunarYear.length();

	LocalizeMonthAndDayL(localized, aInfo);

	QString monthAndDay = 
			QString::fromUtf16(localized->LunarMonthAndDay().Ptr(),
								localized->LunarMonthAndDay().Length());
	QString yearString = 
			QString::fromUtf16(localized->LunarYear().Ptr(),
								localized->LunarYear().Length());
	QString fullLunarDate = HbParameterLengthLimiter(
							hbTrId("txt_calendar_info_lunar_full_date")).arg(
							monthAndDay).arg(yearString);
	
	localized->iFullLunarDate = 
			static_cast<const TUint16*> (fullLunarDate.utf16()), 
													fullLunarDate.length();

	QString format(r_qtn_date_usual_with_zero);
	TBuf<100> gregFormatBuf;
	gregFormatBuf = 
			static_cast<const TUint16*> (format.utf16()), format.length();
	
	aInfo.iGregorianDate.FormatL( localized->iGregorianDate, gregFormatBuf );

	CleanupStack::Pop( localized );
	
	OstTraceFunctionExit0( CCALENLUNARLOCALIZER_LOCALIZEL_EXIT );
	return localized;
}


// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::GetExtraRowTextL
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CCalenLunarLocalizer::GetExtraRowTextL( 
											CCalenLunarLocalizedInfo& aLocInfo )
    {
    OstTraceFunctionEntry0( CCALENLUNARLOCALIZER_GETEXTRAROWTEXTL_ENTRY );
    RArray<CCalenLunarLocalizedInfo::TField> fields;
    CleanupClosePushL( fields );
    fields.AppendL( CCalenLunarLocalizedInfo::EFestival );
    fields.AppendL( CCalenLunarLocalizedInfo::ESolarTerm );
    fields.AppendL( CCalenLunarLocalizedInfo::ELunarMonthAndDay );
    fields.AppendL( CCalenLunarLocalizedInfo::EAnimalYear );
    fields.AppendL( CCalenLunarLocalizedInfo::ELunarYear );
    
    TPtrC text = iRowFormatter->FormatExtraRowInformationL( aLocInfo, fields );
    CleanupStack::PopAndDestroy( &fields );

    OstTraceFunctionExit0( CCALENLUNARLOCALIZER_GETEXTRAROWTEXTL_EXIT );
    return text;
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::LocalizeMonthAndDayL
// -----------------------------------------------------------------------------
//
void CCalenLunarLocalizer::LocalizeMonthAndDayL(CCalenLunarLocalizedInfo* aLocInfo,
                                                TCalenLunarInfo& aInfo)
{
	OstTraceFunctionEntry0( CCALENLUNARLOCALIZER_LOCALIZEMONTHANDDAYL_ENTRY );
	TBuf<10> month;
	TBuf<10> day;
	GetChineseMonth(aInfo.iLunarDate, month);
	GetChineseDay(aInfo.iLunarDate, day);

	QString formattedString;
	if(aInfo.iLunarDate.iLeapMonth) {
		formattedString = HbParameterLengthLimiter(hbTrId(
						"txt_calendar_info_lunar_leap_date")).arg(
						QString((QChar*)month.Ptr(),month.Length())).arg(
						QString((QChar*)day.Ptr(),day.Length()));
	}else {
		formattedString = HbParameterLengthLimiter(hbTrId(
						"txt_calendar_info_lunar_date")).arg(
						QString((QChar*)month.Ptr(),month.Length())).arg(
						QString((QChar*)day.Ptr(),day.Length()));
	}
	aLocInfo->iLunarMonthAndDay = static_cast<const TUint16*> (
							formattedString.utf16()), formattedString.length();
	OstTraceFunctionExit0( CCALENLUNARLOCALIZER_LOCALIZEMONTHANDDAYL_EXIT );
}

// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::GetChineseMonth
// -----------------------------------------------------------------------------
//
void CCalenLunarLocalizer::GetChineseMonth(const TChineseDate& aChineseDate,
                                           TDes& aDes)
    {
    OstTraceFunctionEntry0( CCALENLUNARLOCALIZER_GETCHINESEMONTH_ENTRY );
    if (aChineseDate.iMonth == 1)
        {
        aDes.Append(KCharFirstMonth); // Special character for first month
        }
    else
        {
        GetChineseDigits(aChineseDate.iMonth, aDes);
        }
    
    aDes.Append(KCharMonth);
    OstTraceFunctionExit0( CCALENLUNARLOCALIZER_GETCHINESEMONTH_EXIT );
    }
    
// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::GetChineseDay
// -----------------------------------------------------------------------------
//
void CCalenLunarLocalizer::GetChineseDay(const TChineseDate& aChineseDate,
                                         TDes& aDes)
    {
    OstTraceFunctionEntry0( CCALENLUNARLOCALIZER_GETCHINESEDAY_ENTRY );
    if (aChineseDate.iDay <= 10) // day
        {
        aDes.Append(KCharChu); 
        }
    
    GetChineseDigits(aChineseDate.iDay, aDes);
    OstTraceFunctionExit0( CCALENLUNARLOCALIZER_GETCHINESEDAY_EXIT );
    }

// -----------------------------------------------------------------------------
// CCalenLunarLocalizer::GetChineseDigits
// -----------------------------------------------------------------------------
//
void CCalenLunarLocalizer::GetChineseDigits(TInt aNum, TDes& aDes)
    {
    OstTraceFunctionEntry0( CCALENLUNARLOCALIZER_GETCHINESEDIGITS_ENTRY );
    ASSERT( 0 < aNum && aNum <= 30 );
    TInt tens = aNum / 10;
    TInt ones = aNum % 10;
    if (aNum < 10) 
        {
        aDes.Append((KChineseDigits())[ones]);
        }
    else if (10 == aNum)
        {
        aDes.Append(KCharTen);
        }
    else if (10 < aNum && aNum < 20)
        {
        aDes.Append(KCharTen);
        aDes.Append((KChineseDigits())[ones]);
        }
    else if (20 == aNum)
        {
        aDes.Append((KChineseDigits())[tens]);
        aDes.Append(KCharTen);
        }
    else if (20 < aNum && aNum < 30)
        {
        aDes.Append(KCharTwenty);
        aDes.Append((KChineseDigits())[ones]);
        }
    else if (30 == aNum)
        {
        aDes.Append((KChineseDigits())[tens]);
        aDes.Append(KCharTen);
        }
    else 
        {
        __ASSERT_DEBUG(EFalse, User::Invariant());
        }
    OstTraceFunctionExit0( CCALENLUNARLOCALIZER_GETCHINESEDIGITS_EXIT );
    }

//EOF


