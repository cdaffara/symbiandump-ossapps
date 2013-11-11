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



#include "creator_logelement.h"
#include "creator_traces.h"
#include "creator_log.h"

using namespace creatorlog;

/*
 * 
 */
CCreatorLogElement* CCreatorLogElement::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext )
    {
    CCreatorLogElement* self = new (ELeave) CCreatorLogElement(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop(self);
    return self;
    }
/*
 * 
 */
CCreatorLogElement::CCreatorLogElement(CCreatorEngine* aEngine) 
: 
CCreatorScriptElement(aEngine)
    {
    iIsCommandElement = ETrue;
    }
/*
 * 
 */
void CCreatorLogElement::ExecuteCommandL()
    {
    const CCreatorScriptAttribute* amountAttr = FindAttributeByName(KAmount);
    TInt logAmount = 1;    
    if( amountAttr )
        {
        logAmount = ConvertStrToIntL(amountAttr->Value());
        }
    // Get 'fields' element 
    CCreatorScriptElement* fieldsElement = FindSubElement(KFields);
    if( fieldsElement && fieldsElement->SubElements().Count() > 0 )
        {
        // Get sub-elements
        const RPointerArray<CCreatorScriptElement>& fields = fieldsElement->SubElements();        
        // Create log entries, the amount of entries is defined by logAmount:
        for( TInt cI = 0; cI < logAmount; ++cI )
            {
            TInt direction = -1;
            CLogsParameters* param = new (ELeave) CLogsParameters;
            CleanupStack::PushL(param);
            
            for( TInt i = 0; i < fields.Count(); ++i )
                {
                CCreatorScriptElement* field = fields[i];
                TPtrC elemName = field->Name();
                TPtrC elemContent = field->Content();
                const CCreatorScriptAttribute* randomAttr = field->FindAttributeByName(KRandomLength);
                const CCreatorScriptAttribute* increaseAttr = field->FindAttributeByName(KIncrease);
                TBool increase( EFalse );
                if ( increaseAttr )
                    {
                    increase = ConvertStrToBooleanL( increaseAttr->Value() );
                    }
                
                if( elemName == KDirection )
                    {
                    direction = GetLogCommandL(elemContent, randomAttr || elemContent.Length() == 0); 
                    }
                else if( elemName == KDuration )
                    {
                    if( randomAttr || elemContent.Length() == 0 )
                        {
                        param->iDuration = iEngine->RandomNumber(7200);
                        }
                    else
                        {
                        param->iDuration = ConvertStrToIntL(elemContent);
                        }
                    }
                else if( elemName == KPhonenumber )
                    {
                    if( randomAttr || elemContent.Length() == 0 )
                        {
                        SetContentToTextParamL(param->iPhoneNumber, iEngine->RandomString(CCreatorEngine::EPhoneNumber));
                        }
                    else
                        {
                        if ( increase )
                            {
                            delete param->iPhoneNumber;
                            param->iPhoneNumber = NULL;
                            param->iPhoneNumber = HBufC::NewL( elemContent.Length() + 3 );
                            IncreasePhoneNumL( elemContent, cI, param->iPhoneNumber );
                            }
                        else
                            {
                            SetContentToTextParamL(param->iPhoneNumber, elemContent);
                            }
                        }                    
                    }
                else if( elemName == KDatetime )
                    {
                    if( randomAttr || elemContent.Length() == 0 )
                        {
                        param->iEventTime = iEngine->RandomTime(iEngine->RandomDate(CCreatorEngine::EDatePast), CCreatorEngine::EDatePast); 
                        }
                    else
                        {
                        param->iEventTime = ConvertToDateTimeL(elemContent);
                        }
                    }
                }
            
            if( direction == -1 )
                {
                direction = GetLogCommandL(KEmpty, ETrue); 
                }
            iEngine->AppendToCommandArrayL(direction, param);
            CleanupStack::Pop(); // param
            }
        }
    else
    	{
    	for(TInt i = 0; i < logAmount; ++i )
    		{
    		iEngine->AppendToCommandArrayL(GetLogCommandL(KEmpty, ETrue), 0, 1);
    		}
    	}
    }

TInt CCreatorLogElement::GetLogCommandL( const TDesC& aLogCmdStr, TBool aRandom ) const
    {      
    if( aRandom )
        {
        TInt commandArray[] = {
                    ECmdCreateLogEntryMissedCalls,
                    ECmdCreateLogEntryReceivedCalls,
                    ECmdCreateLogEntryDialledNumbers
            };
        return commandArray[iEngine->RandomNumber(0, 2)];
        }
    
    if( CompareIgnoreCase(aLogCmdStr, KMissed) == 0 )
        return ECmdCreateLogEntryMissedCalls;
    else if( CompareIgnoreCase(aLogCmdStr, KIn) == 0 )
        return ECmdCreateLogEntryReceivedCalls;
    else if( CompareIgnoreCase(aLogCmdStr, KOut) == 0 )
        return ECmdCreateLogEntryDialledNumbers;
    
    LOGSTRING2("CCreatorLogElement::GetLogCommandL: Unknown log direction: %S", &aLogCmdStr);
    User::Leave(KErrNotFound);
    return -1; // Not reached, but disables compiler warning...
    }
