/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include <qstringlist.h>
#include <hbglobal.h>

#include <badesca.h> 
#include <eikenv.h>
#include <hbparameterlengthlimiter.h>

#include "calendarui_debug.h"
#include "CalenExtraRowFormatter.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "calenextrarowformatterTraces.h"
#endif

// Constants
const TInt KZero( 0 );

// -----------------------------------------------------------------------------
// CollapseDuplicatesL
// -----------------------------------------------------------------------------
//
void CollapseDuplicatesL( TDes& aStr, TInt aPos, const TDesC& aSub )
    {
    OstTraceFunctionEntry0( _COLLAPSEDUPLICATESL_ENTRY );
    const TInt sublen = aSub.Length();
    if (aStr.Length() == KZero || sublen == KZero || aPos < KZero || (aPos >aStr.Length()))
        {
        OstTraceFunctionExit0( _COLLAPSEDUPLICATESL_EXIT );
        return;
        }

    TPtrC remaining = aStr.Mid( aPos );
    TInt fstInRemaining = remaining.Find( aSub );
    
    if ( fstInRemaining >= KZero )
        {
        TInt restPos = fstInRemaining + sublen;
        TPtrC rest = remaining.Mid( restPos );
        TInt sndInRest = rest.Find( aSub );

        // 1) two substrings found in sequence 
        if (sndInRest == KZero)
            { 
            // replace second substring with empty string
            TInt fst = aPos + fstInRemaining;
            TInt snd = aPos + restPos + sndInRest;
            aStr.Replace( snd, sublen, KNullDesC);
            // continue collapsing from first 
            CollapseDuplicatesL( aStr, fst, aSub );
            }
        // 2) substring found later in string 
        else if (sndInRest > KZero)
            {         
            // continue collapsing from this second substring
            TInt snd = aPos + restPos + sndInRest;
            CollapseDuplicatesL( aStr, snd, aSub );
            }
        // 3) No second substring found -> nothing to collapse
        else             
            {
            OstTraceFunctionExit0( DUP1__COLLAPSEDUPLICATESL_EXIT );
            return;
            }
        }
    // No substring found 
    else
        {
        OstTraceFunctionExit0( DUP2__COLLAPSEDUPLICATESL_EXIT );
        return;
        }
    OstTraceFunctionExit0( DUP3__COLLAPSEDUPLICATESL_EXIT );
    }

// -----------------------------------------------------------------------------
// RemoveLeadingAndTrailingL
// -----------------------------------------------------------------------------
//
void RemoveLeadingAndTrailingL( TDes& aStr, const TDesC& aSub )
    {
    OstTraceFunctionEntry0( _REMOVELEADINGANDTRAILINGL_ENTRY );
    // Trailing
    const TInt sublen = aSub.Length();
    if ( aStr.Right( sublen ).Find( aSub ) == KZero )
        {
        aStr.Replace( aStr.Length() - sublen, sublen, KNullDesC );
        }

    // Leading 
    if ( aStr.Left( sublen ).Find( aSub ) == KZero )
        {
        aStr.Replace( KZero, sublen, KNullDesC );
        }
        
    OstTraceFunctionExit0( _REMOVELEADINGANDTRAILINGL_EXIT );
    }

// -----------------------------------------------------------------------------
// CCalenExtraRowFormatter::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenExtraRowFormatter* CCalenExtraRowFormatter::NewL()
    {
    OstTraceFunctionEntry0( CCALENEXTRAROWFORMATTER_NEWL_ENTRY );
    CCalenExtraRowFormatter* self = new (ELeave) CCalenExtraRowFormatter;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    OstTraceFunctionExit0( CCALENEXTRAROWFORMATTER_NEWL_EXIT );
    return self;
    }

// -----------------------------------------------------------------------------
// CCalenExtraRowFormatter::~CCalenExtraRowFormatter
// -----------------------------------------------------------------------------
//
EXPORT_C CCalenExtraRowFormatter::~CCalenExtraRowFormatter()
    {
    OstTraceFunctionEntry0( CCALENEXTRAROWFORMATTER_CCALENEXTRAROWFORMATTER_ENTRY );
    OstTraceFunctionExit0( CCALENEXTRAROWFORMATTER_CCALENEXTRAROWFORMATTER_EXIT );
    }

// -----------------------------------------------------------------------------
// CCalenExtraRowFormatter::CCalenExtraRowFormatter
// -----------------------------------------------------------------------------
//
CCalenExtraRowFormatter::CCalenExtraRowFormatter()
    {
    OstTraceFunctionEntry0( DUP1_CCALENEXTRAROWFORMATTER_CCALENEXTRAROWFORMATTER_ENTRY );
    OstTraceFunctionExit0( DUP1_CCALENEXTRAROWFORMATTER_CCALENEXTRAROWFORMATTER_EXIT );
    }

// -----------------------------------------------------------------------------
// CCalenExtraRowFormatter::ConstructL
// -----------------------------------------------------------------------------
//
void CCalenExtraRowFormatter::ConstructL()
    {
    OstTraceFunctionEntry0( CCALENEXTRAROWFORMATTER_CONSTRUCTL_ENTRY );
    OstTraceFunctionExit0( CCALENEXTRAROWFORMATTER_CONSTRUCTL_EXIT );
    }


// -----------------------------------------------------------------------------
// CCalenExtraRowFormatter::FormatExtraRowInformationL
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CCalenExtraRowFormatter::FormatExtraRowInformationL( 
    CCalenLunarLocalizedInfo& aLocInfo, 
    RArray<CCalenLunarLocalizedInfo::TField>& aPrioritizedFields )
{
	OstTraceFunctionEntry0( CCALENEXTRAROWFORMATTER_FORMATEXTRAROWINFORMATIONL_ENTRY );
	if ( aPrioritizedFields.Count() == KZero)
	{
		iText = KNullDesC;

		OstTraceFunctionExit0( CCALENEXTRAROWFORMATTER_FORMATEXTRAROWINFORMATIONL_EXIT );
		return iText;
	}

	// Initialize substring labels
	RArray<CCalenLunarLocalizedInfo::TField> subLabels;
	CleanupClosePushL( subLabels );
	subLabels.AppendL( CCalenLunarLocalizedInfo::EAnimalYear );
	subLabels.AppendL( CCalenLunarLocalizedInfo::ELunarYear );
	subLabels.AppendL( CCalenLunarLocalizedInfo::ELunarMonthAndDay );
	subLabels.AppendL( CCalenLunarLocalizedInfo::EFestival );
	subLabels.AppendL( CCalenLunarLocalizedInfo::ESolarTerm );

	// ASSERT that all prioritized fields can be found from subLabels
	for ( TInt i=0; i < aPrioritizedFields.Count(); i++)
	{
		ASSERT( subLabels.Find( aPrioritizedFields[i] ) >= KZero );
	}

	// Initialize substring array 
	CPtrCArray* subs = new (ELeave) CPtrCArray(10);
	CleanupStack::PushL( subs );
	for ( TInt i = 0; i < subLabels.Count(); i++) 
	{
		subs->AppendL( TPtrC( KNullDesC ) );
	}

	// Set wanted fields to substring array
	for ( TInt i = 0; i < aPrioritizedFields.Count(); i++)
	{
		CCalenLunarLocalizedInfo::TField field = aPrioritizedFields[i];
		TInt subIx = subLabels.Find( field );
		// Replace 
		subs->Delete(subIx);
		RDebug::Print( _L("A sub count  %d"), subs->Count() );
		subs->InsertL(subIx, TPtrC( aLocInfo.GetField( field ) ) );
		RDebug::Print( _L("B sub count %d"), subs->Count() );
		RDebug::Print( _L("B field %S"), &(subs->At(subIx)) );

	}
	TBuf<100> textBuf;
	QStringList textDataStringList;
	for (TInt i=0; i < subLabels.Count(); i++) {
		textBuf = subs->At( i );
		textDataStringList.append(
							QString((QChar*)textBuf.Ptr(),textBuf.Length()));
	}
	// Get the locale specific separator
	QString separator = hbTrId("txt_calendar_preview_title_cale_separator");
	
	// Format all fields to single row 
	QString textDataString;
	textDataString = HbParameterLengthLimiter(
							hbTrId("txt_calendar_preview_title_123242526")).arg(
							textDataStringList.at(0)).arg(
							separator).arg(textDataStringList.at(1)).arg(
							separator).arg(textDataStringList.at(2)).arg(
							separator).arg(textDataStringList.at(3)).arg(
							separator).arg(textDataStringList.at(4));
	
	iText = static_cast<const TUint16*> (
							textDataString.utf16()), textDataString.length();

	// Now we have something like "Year of Dog%2GengYin%2%2"
	// where %2 is the separator txt_calendar_preview_title_cale_separator
	// First We need to remove multiple occurences of separator
	textBuf = static_cast<const TUint16*> (
										separator.utf16()), separator.length();
	CollapseDuplicatesL( iText, 0, textBuf);
	
	// Remove leading and trailing separators
	// Leading separator won't be there but trailing one is there for sure
	RemoveLeadingAndTrailingL( iText, textBuf );
	
	// CleanUp
	CleanupStack::PopAndDestroy( subs );
	CleanupStack::PopAndDestroy( &subLabels );
	
	OstTraceFunctionExit0( DUP1_CCALENEXTRAROWFORMATTER_FORMATEXTRAROWINFORMATIONL_EXIT );
	return iText;
}

//EOF

