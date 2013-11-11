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



#include "creator_noteelement.h"
#include "creator_traces.h"
#include "creator_note.h"

using namespace creatornote;

/*
 * 
 */
CCreatorNoteElement* CCreatorNoteElement::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext )
    {
    CCreatorNoteElement* self = new (ELeave) CCreatorNoteElement(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop(self);
    return self;
    }
/*
 * 
 */
CCreatorNoteElement::CCreatorNoteElement(CCreatorEngine* aEngine) 
: 
CCreatorScriptElement(aEngine)
    {
    iIsCommandElement = ETrue;
    }

void CCreatorNoteElement::ExecuteCommandL()
    {
    const CCreatorScriptAttribute* amountAttr = FindAttributeByName(KAmount);
    TInt noteAmount = 1;    
    if( amountAttr )
        {
        noteAmount = ConvertStrToIntL(amountAttr->Value());
        }
    // Get 'fields' element 
    CCreatorScriptElement* fieldsElement = FindSubElement(KFields);
    if( fieldsElement && fieldsElement->SubElements().Count() > 0)
        {
        // Get sub-elements
        const RPointerArray<CCreatorScriptElement>& fields = fieldsElement->SubElements();        
        // Create note entries, the amount of entries is defined by noteAmount:
        for( TInt cI = 0; cI < noteAmount; ++cI )
            {            
            CNotepadParameters* param = new (ELeave) CNotepadParameters;
            CleanupStack::PushL(param);
            
            for( TInt i = 0; i < fields.Count(); ++i )
                {
                CCreatorScriptElement* field = fields[i];
                TPtrC elemName = field->Name();
                TPtrC elemContent = field->Content();
                const CCreatorScriptAttribute* randomAttr = fields[i]->FindAttributeByName(KRandomLength);
                TBool useMax = EFalse;
                if( randomAttr && randomAttr->Value() == KMax )
                    {
                    useMax = ETrue;
                    }
                
                if( elemName == KText )
                    {
                    if( randomAttr || elemContent.Length() == 0 )
                        {
                        if( useMax )
                            {
                            TDesC* temp = iEngine->CreateRandomStringLC(KNotepadFieldLength);                            
                            SetContentToTextParamL(param->iNoteText, *temp);
                            CleanupStack::PopAndDestroy(); // temp
                            }
                        else
                            {
                            SetContentToTextParamL(param->iNoteText, iEngine->RandomString(CCreatorEngine::EMessageText));
                            }
                        }
                    else
                        {
                        SetContentToTextParamL(param->iNoteText, elemContent);
                        }
                    }
                }
            iEngine->AppendToCommandArrayL(ECmdCreateMiscEntryNotes, param);
            CleanupStack::Pop(); // param
            }
        }
    else
    	{
    	iEngine->AppendToCommandArrayL(ECmdCreateMiscEntryNotes, 0, noteAmount);
    	}
    }
