// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CALCATEGORY_H__
#define __CALCATEGORY_H__

#include <e32base.h>

class CCalCategoryImpl;

/** Calendar categories can be associated with a calendar entry.

There are 12 built in category types defined by TCalCategoryType. Additionally,
it is possible to define extended category types which are held as a descriptor
of any length.

Any number of categories can be specified for an entry. CCalEntry provides member 
functions for adding, deleting and fetching categories for an entry. 

CCalCategoryManager functions enable the client to add\\delete\\fetch categories 
from a Calendar session and to get a list of entries filtered by their category. 
@publishedAll
@released
*/
NONSHARABLE_CLASS(CCalCategory) : public CBase
    {
public:
	/** Calendar category type.
	@publishedAll
	@released
	*/
	enum TCalCategoryType 
		{
		/** Appointment */
		ECalAppointment,
		/** Business */
		ECalBusiness,
		/** Education */
		ECalEducation,	
		/** Holiday */
		ECalHoliday,
		/** Meeting */
		ECalMeeting,
		/** All other types */
		ECalMiscellaneous, 
		/** Personal */
		ECalPersonal,
		/** Phone call */
		ECalPhoneCall,
		/** Sick day */
		ECalSickDay,
		/** Special occasion */
		ECalSpecialOccasion,
		/** Travel */
		ECalTravel,
		/** Vacation */
		ECalVacation,
		/** Extended */
		ECalExtended
		};

public:
	IMPORT_C static CCalCategory* NewL(TCalCategoryType aCategory);
	IMPORT_C static CCalCategory* NewL(const TDesC& aExtendedName);
	IMPORT_C ~CCalCategory();

	IMPORT_C TCalCategoryType Category() const;
	IMPORT_C const TDesC& ExtendedCategoryName() const;
	
public:	
	CCalCategoryImpl* Impl() const;
	static CCalCategory* NewL(CCalCategoryImpl* aImpl);

private:
	CCalCategory();
	CCalCategory(CCalCategoryImpl* aImpl);
	void ConstructL(TCalCategoryType aCategory);
	void ConstructL(const TDesC& aExtendedName);

private: // member data
	CCalCategoryImpl* iCalCategoryImpl;
	};
	
#endif // __CALCATEGORY_H__
