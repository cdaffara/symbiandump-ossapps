/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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



#include "creator_calendarelement.h"
#include "creator_traces.h"
#include "creator_calendar.h"

using namespace creatorcalendar;

CCreatorCalendarElement* CCreatorCalendarElement::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext )
    {
    CCreatorCalendarElement* self = new (ELeave) CCreatorCalendarElement(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop(self);
    return self;
    }

void CCreatorCalendarElement::FillEntryParamsL(TEntryType aType, const RPointerArray<CCreatorScriptElement>& fields, CCalenderInterimParameters* parameters)
    {
    if( parameters == 0 )
        return;
    
    TBool startResolved = EFalse;    
    TBool endResolved = EFalse;
    
    for( TInt i = 0; i < fields.Count(); ++i )
        {
        TPtrC elemName = fields[i]->Name();
        TPtrC elemContent = fields[i]->Content();
        const CCreatorScriptAttribute* randomAttr = fields[i]->FindAttributeByName(KRandomLength);
        
        if( elemName == KSummary || elemName == KDescription )
            {           
            if( randomAttr || elemContent.Length() == 0 )
                {      
                TPtrC reason;
                if( aType == EAppointment )
                    {
                    reason.Set(iEngine->RandomString (CCreatorEngine::EMeetingReason ));                    
                    }
                else if( aType == EEvent )
                    {
                    reason.Set(iEngine->RandomString(CCreatorEngine::EMemoText));                    
                    }
                else if( aType == EReminder )
                    {
                    reason.Set(iEngine->RandomString(CCreatorEngine::EReminderText));                    
                    }
                else if( aType == EAnniversary )
                    {                   
                    reason.Set(iEngine->RandomString(CCreatorEngine::EAnniversaryReason));                    
                    }
                else if( aType == ETodo )
                    {
                    reason.Set(iEngine->RandomString(CCreatorEngine::EToDoText));                    
                    }
                
                if( elemName == KDescription )
                    {
                    delete parameters->iDescription;
                    parameters->iDescription = 0;
                    parameters->iDescription = HBufC::NewL(reason.Length());
                    parameters->iDescription->Des().Copy(reason);
                    }
                else if(elemName == KSummary)
                    {
                    delete parameters->iSummary;
                    parameters->iSummary = 0;
                    parameters->iSummary = HBufC::NewL(reason.Length());
                    parameters->iSummary->Des().Copy(reason);
                    }
                }
            else if( elemName == KDescription )                
                {                
                delete parameters->iDescription;
                parameters->iDescription = 0;
                parameters->iDescription = HBufC::NewL(elemContent.Length());
                parameters->iDescription->Des().Copy(elemContent);
                }
            else if( elemName == KSummary)
                {
                delete parameters->iSummary;
                parameters->iSummary = 0;
                parameters->iSummary = HBufC::NewL(elemContent.Length());
                parameters->iSummary->Des().Copy(elemContent);
                }
            }
        else if( elemName == KLocation )
            {
            delete parameters->iLocation;
            parameters->iLocation = 0;
            
            if( randomAttr || elemContent.Length() == 0 )
                {
                TPtrC reason = iEngine->RandomString (CCreatorEngine::EMeetingPlace );
                parameters->iLocation = HBufC::NewL(reason.Length());
                parameters->iLocation->Des().Copy(reason);
                }
            else
                {
                parameters->iLocation = HBufC::NewL(elemContent.Length());
                parameters->iLocation->Des().Copy(elemContent);
                }
            }
        else if( elemName == KStarttime )
            {
            if( randomAttr || elemContent.Length() == 0 )
                {                
                // Random start time used. If end time is already resolved, let's use that one for the basis. 
                if( endResolved )
                    {                    
                    parameters->iStartTime = iEngine->RandomTime(parameters->iEndTime, CCreatorEngine::EDatePast);                    
                    }
                else
                    {
                    // Past or future:
                    CCreatorEngine::TRandomDateType pastOrfuture = 
                    (CCreatorEngine::TRandomDateType) iEngine->RandomNumber((TInt) CCreatorEngine::EDatePast,
                                                                            (TInt) CCreatorEngine::EDateFuture);
                    
                    parameters->iStartTime = 
                        iEngine->RandomTime(iEngine->RandomDate(pastOrfuture), pastOrfuture);  
                    }
                }
            else
                {
                parameters->iStartTime = ConvertToDateTimeL(elemContent);                
                }            
            startResolved = ETrue;
            }
        else if( elemName == KEndtime )
            {
            if( randomAttr || elemContent.Length() == 0 )
                {
                // Random end time used. If start time is already resolved, let's use that one.                
                if( startResolved )
                    {                    
                    parameters->iEndTime = iEngine->RandomTime(parameters->iStartTime, CCreatorEngine::EDateFuture);                    
                    }
                else
                    {
                    // Past or future:
                    CCreatorEngine::TRandomDateType pastOrfuture = 
                    (CCreatorEngine::TRandomDateType) iEngine->RandomNumber((TInt) CCreatorEngine::EDatePast,
                            (TInt) CCreatorEngine::EDateFuture);
                    
                    parameters->iEndTime = 
                    iEngine->RandomTime(iEngine->RandomDate(pastOrfuture), pastOrfuture);  
                    }                          
                }
            else
                {
                parameters->iEndTime = ConvertToDateTimeL(elemContent);
                }
            if( aType == ETodo )
                {
                parameters->iTodoDueTime = parameters->iEndTime;
                }
            endResolved = ETrue;
            }
        else if( elemName == KRecurrentFreq )
            {
            TInt randomFreq = 0;
            if( randomAttr || elemContent.Length() == 0 )
                {
                randomFreq = iEngine->RandomNumber(1, 4);
                }
            if(randomFreq == 1 || CompareIgnoreCase(elemContent, KDaily) == 0 )
                parameters->iRecurrentFrequency = TCalRRule::EDaily;
            else if( randomFreq == 2 || CompareIgnoreCase(elemContent, KWeekly) == 0 )
                parameters->iRecurrentFrequency = TCalRRule::EWeekly;
            else if( randomFreq == 3 || CompareIgnoreCase(elemContent, KMonthly) == 0 )
                parameters->iRecurrentFrequency = TCalRRule::EMonthly;
            else if( randomFreq == 4 || CompareIgnoreCase(elemContent, KYearly) == 0 )
                parameters->iRecurrentFrequency = TCalRRule::EYearly;
            else
                parameters->iRecurrentFrequency = TCalRRule::EInvalid;            
            }
        else if( elemName == KRecurrentInterval )
            {
            if( randomAttr || elemContent.Length() == 0 )
                {
                parameters->iRecurrentInterval = iEngine->RandomNumber(1,7);
                }
            else
                {
                parameters->iRecurrentInterval = ConvertStrToIntL(elemContent);
                }
            }
        else if( elemName == KRecurrentFrom )
            {
            if( randomAttr || elemContent.Length() == 0 )
                {
                if( startResolved )
                    {
                    // Let's use the start time if it's resolved.
                    parameters->iRecurrentStartTime = parameters->iStartTime;
                    }
                else
                    {
                    parameters->iRecurrentStartTime = iEngine->RandomDate(CCreatorEngine::EDateFuture);
                    }
                }
            else
                {
                parameters->iRecurrentStartTime = ConvertToDateTimeL(elemContent);
                }           
            }
        else if( elemName == KRecurrentTo )
            {
            if( randomAttr || elemContent.Length() == 0 )
                {
                parameters->iRepeatingCount = iEngine->RandomNumber(1, 6);
                //parameters->iRecurrentEndTime = iEngine->RandomDate(CCreatorEngine::EDateFuture);                
                }
            else
                {
                parameters->iRecurrentEndTime = ConvertToDateTimeL(elemContent);
                }           
            }
        else if( elemName == KRepeatingCount )
            {
            if( randomAttr || elemContent.Length() == 0 )
                {
                parameters->iRepeatingCount = iEngine->RandomNumber(1, 52);
                }
            else
                {
                parameters->iRepeatingCount = ConvertStrToIntL(elemContent);
                }
            }
        else if( elemName == KAlarmtime )
            {
            if( randomAttr || elemContent.Length() == 0 )
                {                
                parameters->iUseRandomAlarmTime = ETrue;
                }
            else
                {
                parameters->iAlarmTime = ConvertToDateTimeL(elemContent);
                }
            }
        else if( elemName == KOrganizername )
            {
            delete parameters->iOrganizerName;
            parameters->iOrganizerName = 0;
            if( randomAttr || elemContent.Length() == 0 )
                {
                TPtrC name = iEngine->RandomString (CCreatorEngine::EFirstName );
                parameters->iOrganizerName = HBufC::NewL(name.Length());
                parameters->iOrganizerName->Des().Copy(name);
                }
            else
                {            
                parameters->iOrganizerName = HBufC::NewL(elemContent.Length());
                parameters->iOrganizerName->Des().Copy(elemContent);
                }
            }
        else if( elemName == KOrganizeremail )
            {
            delete parameters->iOrganizerEmail;
            parameters->iOrganizerEmail = 0;
            if( randomAttr || elemContent.Length() == 0 )
                {
                TDesC* email = iEngine->CreateEmailAddressLC();
                parameters->iOrganizerEmail = (HBufC*) email;
                CleanupStack::Pop(); // email
                }
            else
                {
                parameters->iOrganizerEmail = HBufC::NewL(elemContent.Length());
                parameters->iOrganizerEmail->Des().Copy(elemContent);
                }
            }
        else if( elemName == KStatus )
            {
            if( randomAttr || elemContent.Length() == 0 )
                {
                if( aType == ETodo )
                	{
                	parameters->iStatus = (CCalEntry::TStatus) iEngine->RandomNumber(CCalEntry::ETodoNeedsAction, CCalEntry::ETodoInProcess);
                	}
                else
                	{
                	parameters->iStatus = (CCalEntry::TStatus) iEngine->RandomNumber(CCalEntry::ETentative, CCalEntry::ECancelled);
                	}
                }
            else if( CompareIgnoreCase(elemContent, KCalStatusTentative) == 0 )
                {
                parameters->iStatus = CCalEntry::ETentative;
                }
            else if( CompareIgnoreCase(elemContent, KCalStatusConfirmed) == 0 )
                {
                parameters->iStatus = CCalEntry::EConfirmed;
                }
            else if( CompareIgnoreCase(elemContent, KCalStatusCancelled) == 0 )
                {
                parameters->iStatus = CCalEntry::ECancelled;
                }
            else if( CompareIgnoreCase(elemContent, KCalStatusNeedsAction) == 0 )
                {
                parameters->iStatus = CCalEntry::ETodoNeedsAction;
                }
            else if( CompareIgnoreCase(elemContent, KCalStatusCompleted) == 0 )
                {
                parameters->iStatus = CCalEntry::ETodoCompleted;
                }
            else if( CompareIgnoreCase(elemContent, KCalStatusInProcess) == 0 )
                {
                parameters->iStatus = CCalEntry::ETodoInProcess;
                }
            else
                {
                parameters->iStatus = CCalEntry::ENullStatus;
                }
            }
        else if( elemName == KPriority )
            {
            if( randomAttr || elemContent.Length() == 0 )
                {
                parameters->iTodoPriority = iEngine->RandomNumber(1,3);
                }            
            else if( CompareIgnoreCase(elemContent, KPriorityHigh) == 0)
                {
                parameters->iTodoPriority = 1;
                }
            else if( CompareIgnoreCase(elemContent, KPriorityMedium) == 0)
                {
                parameters->iTodoPriority = 2;
                }
            else if( CompareIgnoreCase(elemContent, KPriorityLow) == 0 )
                {
                parameters->iTodoPriority = 3;
                }
            else
                {
                parameters->iTodoPriority = ConvertStrToIntL(elemContent);
                }
            }
        else if( elemName == KAttendees )
            {            
            const RPointerArray<CCreatorScriptElement>& attendeesSubElems = fields[i]->SubElements();
            for( TInt j = 0; j < attendeesSubElems.Count(); ++j )
                {
                TPtrC eName = attendeesSubElems[j]->Name();
                TPtrC eContent = attendeesSubElems[j]->Content();
                if( eName == KContactSetRef )
                    {
                    AppendContactSetReferenceL(*attendeesSubElems[j], parameters->iAttendeeLinkIds);
                    }
                else if( eName == KAttendee )
                    {
                    CCreatorCalenderAttendee* calAtt = CCreatorCalenderAttendee::NewL();
                    CleanupStack::PushL(calAtt);
                    const RPointerArray<CCreatorScriptElement>& attendeeFields = attendeesSubElems[j]->SubElements();
                    // Make sure that at least email is given:
                    if(attendeeFields.Count() == 0 )
                    	{
                    	CCreatorScriptElement* emailField =  CCreatorScriptElement::NewL(iEngine, KEmail );
                    	CleanupStack::PushL(emailField);
                    	fields[i]->SubElements().AppendL(emailField);
                    	CleanupStack::Pop(); // emailField	
                    	}
                    for( TInt k = 0; k < attendeeFields.Count(); ++k )
                        {
                        const CCreatorScriptAttribute* randomAttr = attendeeFields[k]->FindAttributeByName(KRandomLength);
                        TPtrC attElemName = attendeeFields[k]->Name();
                        TPtrC attElemCont = attendeeFields[k]->Content();
                        if( attElemName == KCommonname )
                            {
                            if( randomAttr || attElemCont.Length() == 0)
                                {
                                // Random data:
                                calAtt->SetCommonNameL(iEngine->RandomString(CCreatorEngine::EFirstName));
                                }
                            else
                                {
                                calAtt->SetCommonNameL(attElemCont);
                                }
                            }
                        else if( attElemName == KEmail )
                            {
                            if( randomAttr || attElemCont.Length() == 0)
                                {
                                // Random data:
                                TDesC* email = iEngine->CreateEmailAddressLC();
                                calAtt->SetEmailAddressL(*email);
                                CleanupStack::PopAndDestroy(); // email
                                }
                            else
                                {                                
                                calAtt->SetEmailAddressL(attElemCont);
                                }
                            }
                        else if( attElemName == KRole )
                            {
                            if( randomAttr || attElemCont.Length() == 0)
                                {
                                // Random role:
                                CCalAttendee::TCalRole randomRole = 
                                    (CCalAttendee::TCalRole) iEngine->RandomNumber(
                                            (TInt) CCalAttendee::EReqParticipant, 
                                            (TInt) CCalAttendee::EChair);
                                calAtt->SetRole(randomRole);
                                }                                
                            else if( CompareIgnoreCase(attElemCont, KRoleRequired) == 0 )
                                {
                                calAtt->SetRole(CCalAttendee::EReqParticipant);
                                }
                            else if( CompareIgnoreCase(attElemCont, KRoleOptional) == 0 )
                                {
                                calAtt->SetRole(CCalAttendee::EOptParticipant);
                                }
                            else if( CompareIgnoreCase(attElemCont, KRoleNonPart) == 0 )
                                {
                                calAtt->SetRole(CCalAttendee::ENonParticipant);
                                }
                            else if( CompareIgnoreCase(attElemCont, KRoleChair) == 0 )
                                {
                                calAtt->SetRole(CCalAttendee::EChair);
                                }
                            }
                        else if( attElemName == KStatus )
                            {
                            if( randomAttr || attElemCont.Length() == 0)
                                {
                                // Random role:
                                CCalAttendee::TCalStatus randomStatus = 
                                (CCalAttendee::TCalStatus) iEngine->RandomNumber(
                                        (TInt) CCalAttendee::ENeedsAction, 
                                        (TInt) CCalAttendee::EInProcess);
                                calAtt->SetStatus(randomStatus);
                                }                                
                            else if( CompareIgnoreCase(attElemCont, KStatusNeedsAction) == 0 )
                                {
                                calAtt->SetStatus(CCalAttendee::ENeedsAction);
                                }
                            else if( CompareIgnoreCase(attElemCont, KStatusAccepted) == 0 )
                                {
                                calAtt->SetStatus(CCalAttendee::EAccepted);
                                }
                            else if( CompareIgnoreCase(attElemCont, KStatusTentative) == 0 )
                                {
                                calAtt->SetStatus(CCalAttendee::ETentative);
                                }
                            else if( CompareIgnoreCase(attElemCont, KStatusConfirmed) == 0 )
                                {
                                calAtt->SetStatus(CCalAttendee::EConfirmed);
                                }
                            else if( CompareIgnoreCase(attElemCont, KStatusDeclined) == 0 )
                                {
                                calAtt->SetStatus(CCalAttendee::EDeclined);
                                }
                            else if( CompareIgnoreCase(attElemCont, KStatusCompleted) == 0 )
                                {
                                calAtt->SetStatus(CCalAttendee::ECompleted);
                                }
                            else if( CompareIgnoreCase(attElemCont, KStatusDelegated) == 0 )
                                {                                
                                calAtt->SetStatus(CCalAttendee::EDelegated);
                                }
                            else if( CompareIgnoreCase(attElemCont, KStatusInProcess) == 0 )
                                {                                
                                calAtt->SetStatus(CCalAttendee::EInProcess);
                                }
                            }
                        }
                    parameters->iAttendees.AppendL(calAtt);
                    CleanupStack::Pop(); // calAtt
                    }
                }
            }
        }    
    }

void CCreatorCalendarElement::ExecuteCommandL()
	{
	LOGSTRING("Creator: CCreatorCalendarElement::ExecuteCommandL");
	// Amount of calendar entries:
	const CCreatorScriptAttribute* calEntryAmountAttr = this->FindAttributeByName(KAmount);
	const CCreatorScriptAttribute* calEntryTypeAttr = this->FindAttributeByName(KType);    
	TInt entryAmount = 1;    
	if( calEntryAmountAttr )
		{
		entryAmount = ConvertStrToIntL(calEntryAmountAttr->Value());
		}
	if( calEntryTypeAttr == 0 )
		{
		LOGSTRING("ERROR in CCreatorCalendarElement::ExecuteCommandL: Type attribute is missing.");
		User::Leave(KErrGeneral); // type is required attribute
		}

	// Find fields element:
	CCreatorScriptElement* fieldsElement = FindSubElement(KFields);
	
	for( TInt cI = 0; cI < entryAmount; ++cI )
		{
		CCalenderInterimParameters* param = new (ELeave) CCalenderInterimParameters;
		CleanupStack::PushL(param);
		if( CompareIgnoreCase(calEntryTypeAttr->Value(), KCalTypeAppointment) == 0 )
			{
			if( fieldsElement && fieldsElement->SubElements().Count() > 0 )
				{
				FillEntryParamsL(EAppointment, fieldsElement->SubElements(), param);
				iEngine->AppendToCommandArrayL(ECmdCreateCalendarEntryAppointments, param);
				}
			else
				{
				iEngine->AppendToCommandArrayL(ECmdCreateCalendarEntryAppointments, 0, entryAmount);
				CleanupStack::PopAndDestroy(param);
				break; // We can step out from the for-loop, because the amount is given to engine.
				}
			}		
		else if( CompareIgnoreCase(calEntryTypeAttr->Value(), KCalTypeEvent ) == 0 )
			{
			if( fieldsElement && fieldsElement->SubElements().Count() > 0 )
				{
				FillEntryParamsL(EEvent, fieldsElement->SubElements(), param);
				iEngine->AppendToCommandArrayL(ECmdCreateCalendarEntryEvents, param);
				}
			else
				{
				iEngine->AppendToCommandArrayL(ECmdCreateCalendarEntryEvents, 0, entryAmount);
				CleanupStack::PopAndDestroy(param);
				break;  // We can step out from the for-loop, because the amount is given to engine.
				}
			}
		else if( CompareIgnoreCase(calEntryTypeAttr->Value(), KCalTypeReminder ) == 0 )
			{
			if( fieldsElement && fieldsElement->SubElements().Count() > 0 )
				{
				FillEntryParamsL(EReminder, fieldsElement->SubElements(), param);
				iEngine->AppendToCommandArrayL(ECmdCreateCalendarEntryReminders, param);
				}
			else
				{
				iEngine->AppendToCommandArrayL(ECmdCreateCalendarEntryReminders, 0, entryAmount);
				CleanupStack::PopAndDestroy(param);
				break;  // We can step out from the for-loop, because the amount is given to engine.
				}
			}
		else if( CompareIgnoreCase(calEntryTypeAttr->Value(), KCalTypeAnniversary ) == 0 )
			{
			if( fieldsElement && fieldsElement->SubElements().Count() > 0 )
				{
				FillEntryParamsL(EAnniversary, fieldsElement->SubElements(), param);
				iEngine->AppendToCommandArrayL(ECmdCreateCalendarEntryAnniversaries, param);
				}
			else
				{
				iEngine->AppendToCommandArrayL(ECmdCreateCalendarEntryAnniversaries, 0, entryAmount);
				CleanupStack::PopAndDestroy(param);
				break;  // We can step out from the for-loop, because the amount is given to engine.
				}
			}
		else if( CompareIgnoreCase(calEntryTypeAttr->Value(), KCalTypeTodo ) == 0 )
			{
			if( fieldsElement && fieldsElement->SubElements().Count() > 0 )
				{
				FillEntryParamsL(ETodo, fieldsElement->SubElements(), param);
				iEngine->AppendToCommandArrayL(ECmdCreateCalendarEntryToDos, param);
				}
			else
				{
				iEngine->AppendToCommandArrayL(ECmdCreateCalendarEntryToDos, 0, entryAmount);
				CleanupStack::PopAndDestroy(param);
				break;  // We can step out from the for-loop, because the amount is given to engine.
				}
			}
		CleanupStack::Pop(); // param
		}
	}

CCreatorCalendarElement::CCreatorCalendarElement(CCreatorEngine* aEngine) 
: 
CCreatorScriptElement(aEngine)
    {
    iIsCommandElement = ETrue;
    }
