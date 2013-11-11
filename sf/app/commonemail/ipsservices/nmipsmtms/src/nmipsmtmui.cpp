/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Contains headers needed in plugin
*
*/

#include <msvapi.h>
#include <SendUiConsts.h>
#include <msvuids.h>

#include "nmipsmtmui.h"

_LIT(KIpsMtmUiResourceFile, "ipsmtmui.rsc");


/*!
    \class CIpsMtmUi
    \brief Implements dummy mtm ui class for symbian mtm loader. All function leaves or returns
           NULL exept NewL and GetResourceFileName
*/

/*!

 */
CIpsMtmUi::CIpsMtmUi(CBaseMtm& aBaseMtm, CRegisteredMtmDll& aRegisteredMtmDll) : 
    CBaseMtmUi(aBaseMtm, aRegisteredMtmDll)
{
    
}

/*!

 */
CIpsMtmUi::~CIpsMtmUi()
{
    
}

/*!

 */
EXPORT_C CIpsMtmUi* CIpsMtmUi::NewL(
    CBaseMtm& aBaseMtm,
    CRegisteredMtmDll& aRegisteredMtmDll)
{
    CIpsMtmUi* self=new(ELeave) CIpsMtmUi(aBaseMtm, aRegisteredMtmDll);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

/*!

 */
void CIpsMtmUi::ConstructL()
{
    CBaseMtmUi::ConstructL();
}


void CIpsMtmUi::GetResourceFileName(TFileName& aFileName) const
{
    aFileName = KIpsMtmUiResourceFile;
}

/*!
    Function always leaves
 */
CMsvOperation* CIpsMtmUi::OpenL(TRequestStatus& /*aStatus*/)
{
    User::Leave(KErrNotSupported); 
    return NULL;
}

/*!
    Function always leaves
 */
CMsvOperation* CIpsMtmUi::CloseL(TRequestStatus& /*aStatus*/)
{
    User::Leave(KErrNotSupported); 
    return NULL;
}

/*!
    Function always leaves
 */
CMsvOperation* CIpsMtmUi::EditL(TRequestStatus& /*aStatus*/)
{
    User::Leave(KErrNotSupported); 
    return NULL;
}

/*!
    Function always leaves
 */
CMsvOperation* CIpsMtmUi::ViewL(TRequestStatus& /*aStatus*/)
{
    User::Leave(KErrNotSupported); 
    return NULL;
}

/*!
    Function always leaves
 */
CMsvOperation* CIpsMtmUi::OpenL(
        TRequestStatus& /*aStatus*/, 
        const CMsvEntrySelection& /*aSelection*/)
{
    User::Leave(KErrNotSupported); 
    return NULL;
}

/*!
    Function always leaves
 */
CMsvOperation* CIpsMtmUi::CloseL(
        TRequestStatus& /*aStatus*/, 
        const CMsvEntrySelection& /*aSelection*/)
{
    User::Leave(KErrNotSupported); 
    return NULL;
}

/*!
    Function always leaves
 */
CMsvOperation* CIpsMtmUi::EditL(
        TRequestStatus& /*aStatus*/, 
        const CMsvEntrySelection& /*aSelection*/)
{
    User::Leave(KErrNotSupported); 
    return NULL;
}

/*!
    Function always leaves
 */
CMsvOperation* CIpsMtmUi::ViewL(
        TRequestStatus& /*aStatus*/, 
        const CMsvEntrySelection& /*aSelection*/)
{
    User::Leave(KErrNotSupported); 
    return NULL;
}

/*!
    Function always leaves
 */
CMsvOperation* CIpsMtmUi::CancelL(
        TRequestStatus& /*aStatus*/, 
        const CMsvEntrySelection& /*aSelection*/)
{
    User::Leave(KErrNotSupported); 
    return NULL;
}

/*!
    Function always leaves
 */
CMsvOperation* CIpsMtmUi::ReplyL(
        TMsvId /*aDestination*/, 
        TMsvPartList /*aPartlist*/, 
        TRequestStatus& /*aCompletionStatus*/)
{
    User::Leave(KErrNotSupported); 
    return NULL;
}

/*!
    Function always leaves
 */
CMsvOperation* CIpsMtmUi::ForwardL(
        TMsvId /*aDestination*/, 
        TMsvPartList /*aPartList*/, 
        TRequestStatus& /*aCompletionStatus*/)
{
    User::Leave(KErrNotSupported); 
    return NULL;
}
