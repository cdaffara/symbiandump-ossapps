// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "calcalendarinfo.h"
#include "calcalendarinfoimpl.h"
#include "agmcalendarinfo.h"

/** Constructor for the calendar info

@return An unintialised calendar info object.

@publishedAll
@released
*/
EXPORT_C CCalCalendarInfo* CCalCalendarInfo::NewL()
    {
    CCalCalendarInfo* self = new(ELeave) CCalCalendarInfo();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CCalCalendarInfo* CCalCalendarInfo::NewL(CCalCalendarInfoImpl* aCalendarInfoImpl)
    {
    CCalCalendarInfo* self = new(ELeave) CCalCalendarInfo();
    CleanupStack::PushL(self);
    self->ConstructL(aCalendarInfoImpl);
    CleanupStack::Pop(self);
    return self;
    }

CCalCalendarInfo::CCalCalendarInfo()
    {
    }

void CCalCalendarInfo::ConstructL()
    {
    iImpl = CCalCalendarInfoImpl::NewL();
    }

void CCalCalendarInfo::ConstructL(CCalCalendarInfoImpl* aCalendarInfoImpl)
    {
    iImpl = aCalendarInfoImpl;
    }

EXPORT_C CCalCalendarInfo::~CCalCalendarInfo()
    {
    delete iImpl;
    }

/** Checks if this calendar meta data has been initialised.

This is for the case where you are fetching the calendar info from
calendar iterator and you want to know if calendar info has been set
on the file. 

@return ETrue if the metadata has been created and set on the file. EFalse
if no metadata has been set on the file. Only FileNameL will be valid in this case.

@publishedAll
@released
*/
EXPORT_C TBool CCalCalendarInfo::IsValid() const
	{
	return iImpl->IsValid();
	}

/** The calendar file name that this meta data belongs to 

@return The calendar file name that this metadata belongs to. It will
have the format c:filename.ext.

@publishedAll
@released
*/
EXPORT_C const TDesC& CCalCalendarInfo::FileNameL() const
    {
    return iImpl->FileNameL();
	}

/** Set the user displayed filename of the calendar (e.g. Sports)

@param aName The new user displayed calendar name.

@publishedAll
@released
*/
EXPORT_C void CCalCalendarInfo::SetNameL(const TDesC& aName)
	{
	iImpl->SetNameL(aName);
	}

/** Get the user displayed filename of the calendar (e.g. Sports)

@return The user displayed calendar name.

@publishedAll
@released
*/
EXPORT_C const TDesC& CCalCalendarInfo::NameL() const
	{
	return iImpl->NameL();
	}

/** Set the user displayed description of the calendar.

@param aDescription The new user displayed calendar description.

@publishedAll
@released
*/
EXPORT_C void CCalCalendarInfo::SetDescriptionL(const TDesC& aDescription)
    {
    iImpl->SetDescriptionL(aDescription);
    }

/** Get the user displayed description of the calendar.

@return The user displayed calendar description.

@publishedAll
@released
*/
EXPORT_C const TDesC& CCalCalendarInfo::DescriptionL() const
    {
    return iImpl->DescriptionL();
    }

/** Set the color of the calendar.

@param aColor The new color of the calendar.

@publishedAll
@released
*/
EXPORT_C void CCalCalendarInfo::SetColor(TRgb aColor)
    {
    iImpl->SetColor(aColor);
    }

/** Get the color of the calendar.

@return The color of the calendar

@publishedAll
@released
*/
EXPORT_C TRgb CCalCalendarInfo::Color() const
    {
    return iImpl->Color();
    }

/** Set whether the calendar is enabled or disabled

If the calendar is disabled, alarms will also be disabled for this calendar.

@param aEnabled whether the calendar is enabled.

@publishedAll
@released
*/ 
EXPORT_C void CCalCalendarInfo::SetEnabled(TBool aEnabled)
	{
	iImpl->SetEnabled(aEnabled);
	}

/** Get whether the calendar is enabled or disabled

@return whether the calendar is enabled.

@publishedAll
@released
*/ 
EXPORT_C TBool CCalCalendarInfo::Enabled() const
	{
	return iImpl->Enabled();
	}

/** Get an array of all the property keys.

@return An array of all the property keys that are set on this calendar info.

@publishedAll
@released
*/ 
EXPORT_C CDesC8Array* CCalCalendarInfo::PropertyKeysL() const
    {
    return iImpl->PropertyKeysL();
    }

/** Fetch the value of a key.

@param aKey The key of the value to return. Note that the key is case insensitive.
@return A reference to the value.

@publishedAll
@released
*/ 
EXPORT_C const TDesC8& CCalCalendarInfo::PropertyValueL(const TDesC8& aKey) const
    {
    return iImpl->PropertyValueL(aKey);
    }

/** Set a property.

To set and get a property which is a T class you could do something like this:

@code
// Set a property that is a TTime
TTime now;
now.HomeTime();
TPckgC<TTime> timePckg(now);
calendarInfo->SetPropertyL(_L8("time"), timePckg);

calSession->SetCalendarInfoL(*calendarInfo);

// get a property that is a TTime
TTime time;
TPckgC<TTime> timeBuf(time);
timeBuf.Set(calendarInfo->PropertyValueL(_L8("time")));
time = timeBuf();    
@endcode

For more complicated, variable length object, you could use an RDesWriteStream to write
to the value descriptor and then read it back with an RDesReadStream. 

@param aKey The key of the property. Note that the key is case insensitive.
@param aValue The value of the property.

@leave KErrArgument If either the value or key is KNullDesC8

@publishedAll
@released
*/ 
EXPORT_C void CCalCalendarInfo::SetPropertyL(const TDesC8& aKey, const TDesC8& aValue)
    {
    iImpl->SetPropertyL(aKey, aValue);
    }

/** Remove a property from the 

@param aKey The key of the property to remove from the property set. Note that the key is case insensitive.

@leave KErrNotFound If the key does not exist in the properties.

@publishedAll
@released
*/ 
EXPORT_C void CCalCalendarInfo::RemovePropertyL(const TDesC8& aKey)
    {
    iImpl->RemovePropertyL(aKey);
    }

const CCalCalendarInfoImpl& CCalCalendarInfo::Impl() const
    {
    return *iImpl;
    }
	
// End of File
