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
* Description:   Class implementing the definition of CICalBase. This class is base class from which ICal class has been derived.
*
*/



// Class include.
#include "ICalBase.h"	// CICalBase

//debug
#include "calendarengines_debug.h"

// User includes.
#include "ICalComponent.h"			// CICalComponent
#include "ICalContentLineReader.h"	// CICalContentLineReader
#include "ICalContentLineWriter.h"	// CICalContentLineWriter
#include "ICalErrors.h"				// Error codes
#include "ICalKeyWords.h"			// Literals
#include "ICalProperty.h"			// CICalProperty
#include "ICalUtil.h"				// NICalUtil
#include "ICalValue.h"				// CICalValue

using namespace NICalUtil;

/**
Destructor.
@internalTechnology
*/
CICalBase::~CICalBase()
	{
	TRACE_ENTRY_POINT;
	
	iComponents.ResetAndDestroy();
	iProperties.ResetAndDestroy();
	
	TRACE_EXIT_POINT;
	}
	
/**
Creates a new property with a value from the given parameters, adds it to 
this object and returns a modifiable reference to it.
@param aName Name of the property to add.
@param aValue Value of the property to add.
@return A reference to a new property owned by this object.
@leave Leaves with KErrUnsupportedProperty if the given property is not valid.
for this component.
@publishedPartner
*/
EXPORT_C CICalProperty& CICalBase::AddPropertyL(const TDesC& aName, const TDesC& aValue)
	{
	TRACE_ENTRY_POINT;
	
	CICalProperty* property = CreatePropertyL(aName);	// Not taking ownership.
	
	if (!property)
		{
		User::Leave(KErrUnsupportedProperty);	
		}
	
	property->AddValueL(aValue);
	
	TRACE_EXIT_POINT;
	return *property;
	}

/**
Creates a new property with a value and adds it to this object, returning a
reference to it. Ownership of aValue is transferred and it will be deleted if
this function leaves.
@leave Leaves with KErrPropertyHasNoValue if aValue is Null.
@leave Leaves with KErrUnsupportedProperty if the given property is not valid.
@return A new property
@publishedPartner
*/
EXPORT_C CICalProperty& CICalBase::AddPropertyL(const TDesC& aName, CICalValue* aValue)
	{
	TRACE_ENTRY_POINT;
	
	if (!aValue)
		{
		User::Leave(KErrPropertyHasNoValue);
		}
		
	CleanupStack::PushL(aValue);
	CICalProperty* property = CreatePropertyL(aName);	// Not taking ownership.
	
	if (!property)
		{
		User::Leave(KErrUnsupportedProperty);	
		}
	
	CleanupStack::Pop(aValue);
	property->AddValueL(aValue);
	
	TRACE_EXIT_POINT;
	return *property;
	}

/**
Creates a new component, adds it to this object, and returns a modifiable
reference to it.
@param aType The type of component to be created.
@return A new component
@leave Leaves with KErrUnsupportedComponent if the given component is not a
valid subcomponent for this object.
@publishedPartner
*/
EXPORT_C CICalComponent& CICalBase::AddComponentL(TICalComponentType aType)
	{
	TRACE_ENTRY_POINT;
	
	CICalComponent* component = CreateComponentL(aType);	// Not taking ownership.
	
	if (!component)
		{
		User::Leave(KErrUnsupportedComponent);
		}
	TRACE_EXIT_POINT;	
	return *component;
	}
	
/**
Access function for the component array.
@return The array of components as a constant reference.
@publishedPartner
*/
EXPORT_C const RPointerArray<CICalComponent>& CICalBase::Components() const
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return iComponents; 
	}

/**
Access function for the property array.
@return The array of properties as a constant reference.
@publishedPartner
*/
EXPORT_C const RPointerArray<CICalProperty>& CICalBase::Properties() const
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return iProperties;
	}

/**
Returns the descriptor form of this component type.
@return The descriptor form of this component type.
@publishedPartner
*/
EXPORT_C const TDesC& CICalBase::TypeStringL() const
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return TypeStringL(iComponentType);
	}
	
/**
Access method returning the concrete type as an enumeration.
@return The type of the concrete derived class.
@publishedPartner
*/
EXPORT_C CICalBase::TICalComponentType CICalBase::Type() const
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return iComponentType;
	}
	
/**
Checks for a component already existing in current object's sub-components
@param aType The type of the component to check.
@return ETrue if the property does exist, EFalse otherwise
@publishedPartner
*/
EXPORT_C TBool CICalBase::ComponentExists(TICalComponentType aType) const
	{
	const TInt count = iComponents.Count();
	
	for (TInt i = 0; i < count; i++)
		{
		if (iComponents[i]->Type() == aType)
			{
			TRACE_EXIT_POINT;
			return ETrue;
			}
		}
	
	TRACE_EXIT_POINT;	
	return EFalse;
	}

/**
Finds the first property with a particular name and returns a pointer to it.
Ownership is not passed out.
@param aName The name of the property to search for.
@return A pointer to the property, or NULL.
@publishedPartner
*/
EXPORT_C const CICalProperty* CICalBase::FindProperty(const TDesC& aName) const
	{
	TRACE_ENTRY_POINT;
	
	const TInt count = iProperties.Count();
	
	for (TInt p = 0; p < count; ++p)
		{
		if (iProperties[p]->Type().CompareF(aName) == 0)
			{
			TRACE_EXIT_POINT;
			return iProperties[p];
			}
		}
	
	TRACE_EXIT_POINT;
	return NULL;
	}

/**
Takes a line reader and reads lines from it until the end of the component is
located. Any other END:, or an end of file, are treated as errors.
@param aReader The line reader to read from.
@leave Leaves with KErrCorrupt if the component is corrupt.
@internalTechnology
*/
void CICalBase::InternalizeL(CICalContentLineReader& aReader)
	{
	TRACE_ENTRY_POINT;
	
	TPtrC line;
	TPtrC name;
	TPtrC parameters;
	TPtrC values;
	
	TInt error(aReader.GetNextContentLine(line));

	while (error == KErrNone)
		{
		if (ExtractSectionsL(line, name, parameters, values) == KErrNone)
			{	
			if (name.CompareF(KICalBegin) == 0)
				{
				// This is the start of a component:
				
				TICalComponentType type(TypeFromNameL(values));
				
				if (type == EICalInvalid)
					{
					aReader.SkipComponentL(values);
					}
				else
					{
					CICalComponent* component = CreateComponentL(type);	// Not taking ownership.
					
					if (component)
						{
						component->InternalizeL(aReader);	
						}
					else
						{
						// This component cannot be nested - ignore it
						aReader.SkipComponentL(values);
						}
					}
				}
			else if (name.CompareF(KICalEnd) == 0)
				{
				// This is the end of a component.
				if (values.CompareF(TypeStringL(iComponentType)) != 0)
					{
					User::Leave(KErrCorrupt);	// This is the end of a different component!
					}
				break;
				}
			else
				{
				// This is a property
				
				// Only allow properties with at least one value - if there is no value then
				// ignore the property and continue.
				if (values.Length() > 0)
					{	
					CICalProperty* property = CreatePropertyL(name); // Not taking ownership
					if (property)
						{
						if (name.CompareF(KICalRRule) == 0)
							{
							// This is a special case - commas have a different meaning.
							ExtractParametersL(parameters, *property);
							property->AddValueL(values);
							}
						else if (name.CompareF(KICalCategories) == 0)
							{
							// Outlook escapes the comma separators between categories.
							// This is not part of the iCal specification.
							ExtractParametersL(parameters, *property);
							ExtractPropertyValuesL(values, *property, EEscapeValueSeparators);
							}
						else if (name.CompareF(KICalTzid) == 0)
							{
							// Outlook doesn't escape commas in TZID fields.
							// This is not part of the iCal specification.
							ExtractParametersL(parameters, *property);
							property->AddValueL(values);
							}
						else
							{
							ExtractPropertyL(parameters, values, *property);
							}
						
						// Remove property if it doesn't have a value.
						if (property->Values().Count() == 0)
							{
							TInt index(iProperties.Find(property));
							
							if (index >= 0)
								{
								iProperties.Remove(index);
								}
								
							delete property;
							}
						else
							{
							// If the property is a method, remember it.
							if (name.CompareF(KICalMethod) == 0)
								{
								TPtrC val;
								val.Set(property->Values()[0]->TextL());
								if (val.CompareF(KICalPublish) == 0)
									{
									iMethod = CICalBase::EMethodPublish;
									}
								else if (val.CompareF(KICalRequest) == 0)
									{
									iMethod = CICalBase::EMethodRequest;
									}
								else if (val.CompareF(KICalReply) == 0)
									{
									iMethod = CICalBase::EMethodReply;
									}
								else if (val.CompareF(KICalAdd) == 0)
									{
									iMethod = CICalBase::EMethodAdd;
									}
								else if (val.CompareF(KICalCancel) == 0)
									{
									iMethod = CICalBase::EMethodCancel;
									}
								else if (val.CompareF(KICalRefresh) == 0)
									{
									iMethod = CICalBase::EMethodRefresh;
									}
								else if (val.CompareF(KICalCounter) == 0)
									{
									iMethod = CICalBase::EMethodCounter;
									}
								else if (val.CompareF(KICalDeclineCounter) == 0)
									{
									iMethod = CICalBase::EMethodDeclineCounter;
									}
								else
									{
									User::Leave(KErrCorrupt);
									}
								}
							}
						}
					}
				}
			}
			
		error = aReader.GetNextContentLine(line);
		}
		
	if ((error != KErrNone) && (error != KErrEof))
		{
		User::Leave(error);
		}
		
    TRACE_EXIT_POINT;
	}

/**
Takes a line writer and exports this component, including all owned properties
and sub components, to it.
@param aWriter the writer to export to.
@internalTechnology
*/
void CICalBase::ExternalizeL(CICalContentLineWriter& aWriter) const
	{
	TRACE_ENTRY_POINT;
	
	aWriter.AppendL(KICalBegin());
	aWriter.AppendL(KICalColon());
	aWriter.AppendL(TypeStringL(iComponentType));
	
	aWriter.WriteContentLineL();
	
	// Externalize any component properties:
	TInt count(iProperties.Count());
	
	for (TInt prop = 0; prop < count; ++prop)
		{
		// Only externalise properties with a value.
		if (iProperties[prop]->Values().Count() > 0)
			{
			iProperties[prop]->ExternalizeL(aWriter);
			}
		}

	// Externalize any child components:	
	count = iComponents.Count();
	for (TInt comp = 0; comp < count; ++comp)
		{
		iComponents[comp]->ExternalizeL(aWriter);
		}

	aWriter.AppendL(KICalEnd());
	aWriter.AppendL(KICalColon());
	aWriter.AppendL(TypeStringL(iComponentType));
	aWriter.WriteContentLineL();
	
	TRACE_EXIT_POINT;
	}

/**
Converts between a TICalComponentType and the type as a descriptor. 
@param aName The type as a descriptor.
@return The type as an enumeration.
@leave KErrCorrupt if this is not a valid type.
@internalTechnology
*/
CICalBase::TICalComponentType CICalBase::TypeFromNameL(const TDesC& aName)
	{
	TRACE_ENTRY_POINT;
	
	TICalComponentType type(EICalInvalid);

	if (aName.CompareF(KICalVCalendar) == 0)
		{
		type = EICalCalendar;
		}
	else if (aName.CompareF(KICalEvent) == 0)
		{
		type = EICalEvent;
		}
	else if (aName.CompareF(KICalTodo) == 0)
		{
		type = EICalTodo;
		}
	else if (aName.CompareF(KICalJournal) == 0)
		{
		type = EICalJournal;
		}
	else if (aName.CompareF(KICalAlarm) == 0)
		{
		type = EICalAlarm;
		}
	else if (aName.CompareF(KICalTimeZone) == 0)
		{
		type = EICalTimeZone;
		}
	else if (aName.CompareF(KICalFreeBusy) == 0)
		{
		type = EICalFreeBusy;
		}
	else if (aName.CompareF(KICalStandard) == 0)
		{
		type = EICalStandard;
		}
	else if (aName.CompareF(KICalDaylight) == 0)
		{
		type = EICalDaylight;
		}
	else
		{
		type = EICalInvalid;
		}
	
	TRACE_EXIT_POINT;
	return type;
	}
	
/**
Constructor
@internalTechnology
*/
CICalBase::CICalBase()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}
	
/**
Converts between a TICalComponentType and the type as a descriptor.
@param aType The type as an enumeration.
@return The type as a descriptor.
@leave KErrCorrupt if this is not a valid type.
@internalTechnology
*/
const TDesC& CICalBase::TypeStringL(TICalComponentType aType) const
	{
	TRACE_ENTRY_POINT;
	
	switch (aType)
		{
		case EICalCalendar:
		    TRACE_EXIT_POINT;
			return KICalVCalendar();
		case EICalEvent:
		    TRACE_EXIT_POINT;
			return KICalEvent();
		case EICalJournal:
		    TRACE_EXIT_POINT;
			return KICalJournal();
		case EICalAlarm:
		    TRACE_EXIT_POINT;
			return KICalAlarm();
		case EICalFreeBusy:
		    TRACE_EXIT_POINT;
			return KICalFreeBusy();
		case EICalTodo:
		    TRACE_EXIT_POINT;
			return KICalTodo();
		case EICalTimeZone:
		    TRACE_EXIT_POINT;
			return KICalTimeZone();
		case EICalStandard:
		    TRACE_EXIT_POINT;
			return KICalStandard();
		case EICalDaylight:
		    TRACE_EXIT_POINT;
			return KICalDaylight();
		case EICalInvalid:
			// Fall through...
		default:
			User::Leave(KErrCorrupt);
			break;	
		};

    TRACE_EXIT_POINT;		
	return KNullDesC;	// Never reached.
	}

/**
Private implementation of AddComponentL(), used directly during InternalizeL().
Checks that the given component can be nested within this component. The returned
pointer will be NULL if it is not supported - the calling function does NOT take
ownership of the returned pointer.
@param aType The type of component to be created.
@return pointer to newly added component. This will be NULL if the property is
not supported.
@leave Leaves if there is an error adding a new component.
@internalTechnology
*/
CICalComponent* CICalBase::CreateComponentL(TICalComponentType aType)
	{
	TRACE_ENTRY_POINT;
	
	CICalComponent* component = NULL;
	
	if (ValidateComponent(aType))
		{
		component = CICalComponent::CreateICalComponentLC(aType, iMethod);
		User::LeaveIfError(iComponents.Append(component));
		CleanupStack::Pop(component);
		}
	
	TRACE_EXIT_POINT;	
	return component;	
	}
	
/**
Private implementation of AddPropertyL(), used directly during InternalizeL().
Checks that the given property is supported by this component. The returned
pointer will be NULL if it is not supported - the calling function does NOT
take ownership of the returned pointer.
@param aName Name of the property to add.
@return pointer to newly added property. This will be NULL if the property is
not supported.
@leave Leaves if there is an error adding a new property.
@internalTechnology
*/
CICalProperty* CICalBase::CreatePropertyL(const TDesC& aName)
	{
	TRACE_ENTRY_POINT;
	
	CICalProperty* property = NULL;
	
	if (ValidateProperty(aName))
		{
		property = CICalProperty::NewLC(aName);
		User::LeaveIfError(iProperties.Append(property));
		CleanupStack::Pop(property);
		}
	
	TRACE_EXIT_POINT;	
	return property;
	}

TBool CICalBase::ValidateProperty(const TDesC& aName) const
	{
	TRACE_ENTRY_POINT;
	
	if (
	//Not checking properties in these components here...
	(iComponentType == EICalCalendar) || (iComponentType == EICalAlarm) || 
	(iComponentType == EICalTimeZone) || (iComponentType == EICalDaylight) || 
	(iComponentType == EICalStandard) || (iComponentType == EICalJournal) ||
	(iComponentType == EICalEvent) || (iComponentType == EICalTodo) ||
	
	//If no METHOD property exists, then RFC2446 does not apply...
	(iComponentMethodBitMask & EMaskEventNone)    ||
	(iComponentMethodBitMask & EMaskTodoNone)     ||
	(iComponentMethodBitMask & EMaskJournalNone)  ||
	(iComponentMethodBitMask & EMaskFreeBusyNone) ||
	
	//Validate the property against RFC2446	
	((iComponentMethodBitMask & EICalAttendeeFlags) && (aName.CompareF(KICalAttendee) == 0)) ||
	((iComponentMethodBitMask & EICalDtStampFlags) && (aName.CompareF(KICalDtstamp) == 0)) ||
	((iComponentMethodBitMask & EICalDtStartFlags) && (aName.CompareF(KICalDtstart) == 0)) ||
	((iComponentMethodBitMask & EICalOrganizerFlags) && (aName.CompareF(KICalOrganizer) == 0)) ||
	((iComponentMethodBitMask & EICalSummaryFlags) && (aName.CompareF(KICalSummary) == 0)) ||
	((iComponentMethodBitMask & EICalUIDFlags) && (aName.CompareF(KICalUid) == 0)) ||
	((iComponentMethodBitMask & EICalRecurrenceIdFlags) && (aName.CompareF(KICalRecurrenceId) == 0)) ||
	((iComponentMethodBitMask & EICalSequenceFlags) && (aName.CompareF(KICalSequence) == 0)) ||
	((iComponentMethodBitMask & EICalAttachFlags) && (aName.CompareF(KICalAttach) == 0)) ||
	((iComponentMethodBitMask & EICalCategoriesFlags) && (aName.CompareF(KICalCategories) == 0)) ||
	((iComponentMethodBitMask & EICalClassFlags) && (aName.CompareF(KICalClass) == 0)) ||
	((iComponentMethodBitMask & EICalCommentFlags) && (aName.CompareF(KICalComment) == 0)) ||
	((iComponentMethodBitMask & EICalContactFlags) && (aName.CompareF(KICalContact) == 0)) ||
	((iComponentMethodBitMask & EICalCreatedFlags) && (aName.CompareF(KICalCreated) == 0)) ||
	((iComponentMethodBitMask & EICalDescriptionFlags) && (aName.CompareF(KICalDescription) == 0)) ||
	((iComponentMethodBitMask & EICalDtEndFlags) && (aName.CompareF(KICalDtend) == 0)) ||
	((iComponentMethodBitMask & EICalDurationFlags) && (aName.CompareF(KICalDuration) == 0)) ||
	((iComponentMethodBitMask & EICalExDateFlags) && (aName.CompareF(KICalExdate) == 0)) ||
	((iComponentMethodBitMask & EICalExRuleFlags) && (aName.CompareF(KICalExrule) == 0)) ||
	((iComponentMethodBitMask & EICalGeoFlags) && (aName.CompareF(KICalGeo) == 0)) ||
	((iComponentMethodBitMask & EICalLastModifiedFlags) && (aName.CompareF(KICalLastmodified) == 0)) ||
	((iComponentMethodBitMask & EICalLocationFlags) && (aName.CompareF(KICalLocation) == 0)) ||
	((iComponentMethodBitMask & EICalPriorityFlags) && (aName.CompareF(KICalPriority) == 0)) ||
	((iComponentMethodBitMask & EICalRDateFlags) && (aName.CompareF(KICalRdate) == 0)) ||
	((iComponentMethodBitMask & EICalRelatedToFlags) && (aName.CompareF(KICalRelatedto) == 0)) ||
	((iComponentMethodBitMask & EICalRequestStatusFlags) && (aName.CompareF(KICalRequeststatus) == 0)) ||
	((iComponentMethodBitMask & EICalResourcesFlags) && (aName.CompareF(KICalResources) == 0)) ||
	((iComponentMethodBitMask & EICalRRuleFlags) && (aName.CompareF(KICalRRule) == 0)) ||
	((iComponentMethodBitMask & EICalStatusFlags) && (aName.CompareF(KICalStatus) == 0)) ||
	((iComponentMethodBitMask & EICalTranspFlags) && (aName.CompareF(KICalTransp) == 0)) ||
	((iComponentMethodBitMask & EICalUrlFlags) && (aName.CompareF(KICalUrl) == 0)) ||
	((iComponentMethodBitMask & EICalDueFlags) && (aName.CompareF(KICalDue) == 0)) ||
	((iComponentMethodBitMask & EICalPercentCompleteFlags) && (aName.CompareF(KICalPercentcomplete) == 0)) ||
	((iComponentMethodBitMask & EICalFreeBusyFlags) && (aName.CompareF(KICalFreebusy) == 0))
	)
		{
		TRACE_EXIT_POINT;
		return ValidatePropertyImpl(aName);
		}
	
	//Allow all X-Properties
	else if ((aName.Length() >= 2) && (aName.Left(2).CompareF(KICalXProperty) == 0))
		{
		TRACE_EXIT_POINT;
		return ETrue;
		}
		
	TRACE_EXIT_POINT;
	return EFalse;
	}

// End of File

