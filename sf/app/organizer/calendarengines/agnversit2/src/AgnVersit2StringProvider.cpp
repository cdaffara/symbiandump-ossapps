/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file implements string loader for the X-tented iCal
*                properties which has to be language sensitive.
*
*/



// Class include.
#include "AgnVersit2StringProvider.h"

//debug
#include "calendarengines_debug.h"

// System includes.
#include <barsc.h>              // RResourceFile
#include <bautils.h>            // BaflUtils
#include <data_caging_path_literals.hrh>

// Resource include.
#include <agnversit2strings.rsg>

// Constants
_LIT(KResourceFilename, "agnversit2strings.rsc");


/**
Creates a new CAgnVersit2StringProvider and returns it
@return pointer to the new CAgnVersit2StringProvider
@internalTechnology
*/
CAgnVersit2StringProvider* CAgnVersit2StringProvider::NewL()
	{
	TRACE_ENTRY_POINT;
	
	CAgnVersit2StringProvider* self = new (ELeave) CAgnVersit2StringProvider;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;
	}
	
/**
Destructor Delete all objects owned by the class. Note that iResourceBuffer is
NOT deleted here, as it will be deleted during the call to
iStringArray.ResetAndDestroy()
@internalTechnology
*/
CAgnVersit2StringProvider::~CAgnVersit2StringProvider()
	{
	TRACE_ENTRY_POINT;
	
	iStringArray.ResetAndDestroy();
	iResourceFile.Close();
	iFs.Close();
	
	TRACE_EXIT_POINT;
	}

/**
Returns a string based on the TStringId parameter. If the array item at index
aStringId is NULL, then that particular string has not been requested yet. If
this is the case, then the string is retrieved from the resource file and set
to aIndex in the string array.
@param aStringId string index to return
@return reference to a descriptor containing the string
@internalTechnology
*/
const TDesC& CAgnVersit2StringProvider::StringL(TStringId aStringId)
	{
	TRACE_ENTRY_POINT;
	
	ASSERT(iStringArray.Count() >= aStringId + 1);
	if ( !iStringArray[aStringId] )
		{
		ReadStringFromResourceL(aStringId);	
		}
	
	TRACE_EXIT_POINT;
	return *iStringArray[aStringId];
	}

/**
Default constructor
@internalTechnology
*/
CAgnVersit2StringProvider::CAgnVersit2StringProvider()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT; 
	}
	
/**
Second phase construction
@leave
@internalTechnology
*/
void CAgnVersit2StringProvider::ConstructL()
	{
	TRACE_ENTRY_POINT;
	
	//Fill iStringArray with NULL pointers...
	for (TInt x = 0; x < EICalNumberOfStrings; ++x)
		{
		User::LeaveIfError(iStringArray.Append(NULL));
		}
	User::LeaveIfError(iFs.Connect());
	
/* JH 16.9. quick fix for the resource loading problem */
   TFileName fileName;
	// Get the complate path of the DLL from where it is currently loaded
	Dll::FileName( fileName );
	
	TFileName resFile;
	
	// Append the Drive letters ex., Z: or C:
	resFile.Append(fileName.Mid(0,2));
	resFile.Append(KDC_RESOURCE_FILES_DIR);
    resFile.Append(KResourceFilename);
   
	// Find a localised resource file if available -- if not, it will be left as .rsc
	BaflUtils::NearestLanguageFile(iFs, resFile);
	// Now we've got the full resource file name, so open it
	iResourceFile.OpenL(iFs, resFile);
	iResourceFile.ConfirmSignatureL(0);
	
	// Read all strings immediately in debug mode.
	__DEBUG_ONLY(ReadAllStringsL());
	
	TRACE_EXIT_POINT;
	}

/**
Reads a string from the resource file and adds it to the array of strings.
@param aStringId string to add
@internalTechnology
*/
void CAgnVersit2StringProvider::ReadStringFromResourceL(TStringId aStringId)
	{
	TRACE_ENTRY_POINT;
	
	TInt offset = 0;
	
	switch (aStringId)
		{
		case EICalProdIdValue:
			offset = R_ICAL_PRODID;
			break;
		case EICalTzidUtc:
			offset = R_TZID_UTC;
			break;
		case EICalTzidPlus:
			offset = R_TZID_PLUS;
			break;
		case EICalTzidMinus:
			offset = R_TZID_MINUS;
			break;
		case EICalTzidSlash:
			offset = R_TZID_SLASH;
			break;
		case EICalTzidStandard:
			offset = R_TZID_STANDARD;
			break;
		case EICalTzidDaylight:
			offset = R_TZID_DAYLIGHT;
			break;
		case EICalErrRruleNumRules:
			offset = R_ERRIMPORT_NUMRULES;
			break;
		case EICalErrRruleNoFreq:
			offset = R_ERRIMPORT_NOFREQ;
			break;
		case EICalErrRruleNotYearly:
			offset = R_ERRIMPORT_NOTYEARLY;
			break;
		case EICalErrValarmNotAllowed:
			offset = R_ERRIMPORT_NOTALLOWED;
			break;
		case EICalErrValarmNoTrigger:
			offset = R_ERRIMPORT_NOTRIGGER;
			break;
		case EICalErrValarmNoStart:
			offset = R_ERRIMPORT_NOSTART;
			break;
		case EICalErrAlarmAfterEvent:
			offset = R_ERRIMPORT_AFTEREVENT;
			break;
		case EICalXParamType:
			offset = R_XPARAM_TYPE;
			break;
		case EICalXParamAppointment:
			offset = R_XPARAM_APPT;
			break;
		case EICalXParamTodo:
			offset = R_XPARAM_TODO;
			break;
		case EICalXParamEvent:
			offset = R_XPARAM_EVENT;
			break;
		case EICalXParamReminder:
			offset = R_XPARAM_REMINDER;
			break;
		case EICalXParamAnniversary:
			offset = R_XPARAM_ANNIVERSARY;
			break;
		case EICalAppointment:
			offset = R_CAT_APPOINTMENT;
			break;
		case EICalBusiness:
			offset = R_CAT_BUSINESS;
			break;
		case EICalEducation:
			offset = R_CAT_EDUCATION;
			break;
		case EICalHoliday:
			offset = R_CAT_HOLIDAY;
			break;
		case EICalMeeting:
			offset = R_CAT_MEETING;
			break;
		case EICalMisc:
			offset = R_CAT_MISC;
			break;
		case EICalPersonal:
			offset = R_CAT_PERSONAL;
			break;
		case EICalPhoneCall:
			offset = R_CAT_PHONECALL;
			break;
		case EICalSick:
			offset = R_CAT_SICK;
			break;
		case EICalSpecial:
			offset = R_CAT_SPECIAL;
			break;
		case EICalTravel:
			offset = R_CAT_TRAVEL;
		break;
		case EICalVacation:
			offset = R_CAT_VACATION;
			break;
		default:
			User::Leave(KErrCorrupt);
			break;
		}
	HBufC8* resourceData = iResourceFile.AllocReadLC(offset);
	iResourceReader.SetBuffer(resourceData);
	iResourceBuffer = iResourceReader.ReadTPtrC().AllocL();
	iStringArray[aStringId] = iResourceBuffer;	// iStringArray takes ownership here.
	iResourceBuffer = NULL;
	
	CleanupStack::PopAndDestroy(resourceData);
	
	TRACE_EXIT_POINT;
	}
	
/**
Reads every string into the string array to prevent lazy initialisation from
confusing any OOM tests. If we don't do this, then the strings stored in the
array between one import and the next look like a memory leak.
@internalTechnology
*/
void CAgnVersit2StringProvider::ReadAllStringsL()
	{
	TRACE_ENTRY_POINT;
	
	for (TInt id = 0; id < EICalNumberOfStrings; ++id)
		{
		ReadStringFromResourceL(static_cast<TStringId>(id));
		}
	
	TRACE_EXIT_POINT;
	}
	
// End of file.
