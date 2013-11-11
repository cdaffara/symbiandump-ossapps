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




#include "creator_browserelement.h"
#include "creator_traces.h"
#include "creator_browser.h"

using namespace creatorbrowser;

/*
 * 
 */
CCreatorBrowserElement* CCreatorBrowserElement::NewL(CCreatorEngine* aEngine, const TDesC& aName, const TDesC& aContext )
    {
    CCreatorBrowserElement* self = new (ELeave) CCreatorBrowserElement(aEngine);
    CleanupStack::PushL(self);
    self->ConstructL(aName, aContext);
    CleanupStack::Pop(self);
    return self;
    }
/*
 * 
 */
CCreatorBrowserElement::CCreatorBrowserElement(CCreatorEngine* aEngine) 
: 
CCreatorScriptElement(aEngine)
    {
    iIsCommandElement = ETrue;
    }


/*
 * 
 */
void CCreatorBrowserElement::ExecuteCommandL()
    {
    const CCreatorScriptAttribute* amountAttr = FindAttributeByName(KAmount);
    TInt entryAmount = 1;
    TBool addAll = EFalse;
    if( amountAttr )
        {
        entryAmount = ConvertStrToIntL(amountAttr->Value());
        }
    // Get 'fields' element 
    CCreatorScriptElement* fieldsElement = FindSubElement(KFields);
    if( fieldsElement )
        {
        // Get sub-elements
        const RPointerArray<CCreatorScriptElement>& fields = fieldsElement->SubElements();
        // Create browser entries, the amount of entries is defined by entryAmount:
        for( TInt cI = 0; cI < entryAmount; ++cI )
            {
            CBrowserParameters* param = 0;
            
            if( fields.Count() > 0 )
            	param = new (ELeave) CBrowserParameters;
            else
            	{
            	// No fields defined --> Add all fields with random values:
            	addAll = ETrue;
            	break;
            	}
            
            CleanupStack::PushL(param);
            
            for( TInt i = 0; i < fields.Count(); ++i )
                {
                CCreatorScriptElement* field = fields[i];
                TPtrC elemName = field->Name();                
                TPtrC elemContent = field->Content();
                const CCreatorScriptAttribute* randomAttr = fields[i]->FindAttributeByName(KRandomLength);
                if( elemName == KName )
                    {
                    if( randomAttr || elemContent.Length() == 0)
                        {
                        elemContent.Set(iEngine->RandomString(CCreatorEngine::ECompany));
                        }
                    if( iName->Des() == KBookmark )
                        {
                        SetContentToTextParamL(param->iBookmarkName, elemContent);                        
                        }
                    else if( iName->Des() == KBookmarkFolder )
                        {
                        SetContentToTextParamL(param->iBookmarkFolderName, elemContent);
                        }
                    else if( iName->Des() == KSavedPage )
                        {
                        SetContentToTextParamL(param->iSavedDeckLinkName, elemContent);
                        }
                    else if( iName->Des() == KSavedPageFolder )
                        {
                        SetContentToTextParamL(param->iSavedDeckFolderName, elemContent);
                        }
                    }
                
                else if( elemName == KPath )
                    {                    
                    if( randomAttr || elemContent.Length() == 0)
                        {
                        SetContentToTextParamL(param->iSavedDeckLocalAddress, iEngine->TestDataPathL(CCreatorEngine::ESavedDeck_1kB));
                        }
                    else
                        {
                        SetContentToTextParamL(param->iSavedDeckLocalAddress, elemContent);
                        }
                    }
                else if( elemName == KUsername )
                    {
                    if( randomAttr || elemContent.Length() == 0)
                        {
                        // Set username and password to same string
                        if( param->iBookmarkPassword && param->iBookmarkPassword->Length() > 0 )
                            {
                            SetContentToTextParamL(param->iBookmarkUsername, param->iBookmarkPassword->Des());
                            }
                        else
                            {
                            SetContentToTextParamL(param->iBookmarkUsername, iEngine->RandomString(CCreatorEngine::EFirstName));
                            }
                        }
                    else
                        {
                        SetContentToTextParamL(param->iBookmarkUsername, elemContent);
                        }
                    }
                else if( elemName == KPassword )
                    {
                    if( randomAttr || elemContent.Length() == 0)
                        {
                        // Set username and password to same string
                        if( param->iBookmarkUsername && param->iBookmarkUsername->Length() > 0 )
                            {
                            SetContentToTextParamL(param->iBookmarkPassword, param->iBookmarkUsername->Des());
                            }
                        else
                            {
                            SetContentToTextParamL(param->iBookmarkPassword, iEngine->RandomString(CCreatorEngine::EFirstName));                                                    
                            }
                        }
                    else
                        {
                        SetContentToTextParamL(param->iBookmarkPassword, elemContent);
                        }
                    }
                else if( elemName == KUrl )
                    {
                    if( randomAttr || elemContent.Length() == 0)
                        {
                        TDesC* tmpUrl = iEngine->CreateHTTPUrlLC();
                        if( tmpUrl )
                            {
                            SetContentToTextParamL(param->iBookmarkAddress, *tmpUrl );
                            }
                        CleanupStack::PopAndDestroy(); // tmpUrl                        
                        }
                    else
                        {
                        SetContentToTextParamL(param->iBookmarkAddress, elemContent);
                        }
                    }
                }
            iEngine->AppendToCommandArrayL(GetBrowserCommandL(), param);
            CleanupStack::Pop(); // param
            }
        }
    else
    	{
    	addAll = ETrue;
    	}
    
    if( addAll )
    	{
    	iEngine->AppendToCommandArrayL(GetBrowserCommandL(), 0, entryAmount);
    	}
    }

TInt CCreatorBrowserElement::GetBrowserCommandL() const
    {
    if( iName->Des() == KBookmark )
        {
        return ECmdCreateBrowserBookmarkEntries;
        }
    else if( iName->Des() == KBookmarkFolder )
        {
        return ECmdCreateBrowserBookmarkFolderEntries;
        }
    else if( iName->Des() == KSavedPage )
        {
        return ECmdCreateBrowserSavedPageEntries;
        }
    else if( iName->Des() == KSavedPageFolder )
        {
        return ECmdCreateBrowserSavedPageFolderEntries;
        }
    
    LOGSTRING("CCreatorFileElement::GetBrowserCommandL: Unknown browser entry");
    User::Leave(KErrGeneral);
    return 0; // Not reached, but disables compiler warning...
    }
