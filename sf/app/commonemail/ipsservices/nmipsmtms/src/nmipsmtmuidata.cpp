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
* Description:  Definition of the class CFsUiDataMtm.
*
*/

#include "nmipsmtmuidata.h"

_LIT(KIpsMtmUiDataResourceFile, "ipsmtmdataui.rsc");

/*!
    \class CIpsUiDataMtm
    \brief Implements dummy mtm ui data class for symbian mtm loader. 
    All function leaves or returns false for capability functions
*/


/*!

 */
CIpsUiDataMtm::CIpsUiDataMtm(CRegisteredMtmDll& aRegisteredMtmDll) 
    : CBaseMtmUiData(aRegisteredMtmDll)
{
    
}

/*!

 */
CIpsUiDataMtm::~CIpsUiDataMtm()
{
    
}

/*!

 */
EXPORT_C CIpsUiDataMtm* CIpsUiDataMtm::NewL(
        CRegisteredMtmDll& aRegisteredMtmDll)
{
    CIpsUiDataMtm* self=new(ELeave) CIpsUiDataMtm(aRegisteredMtmDll);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

/*!

 */
void CIpsUiDataMtm::ConstructL()
{
    CBaseMtmUiData::ConstructL();
}

/*!

 */
void CIpsUiDataMtm::PopulateArraysL()
{
    if (iIconArrays->Count() == 0)
        {
        iIconArrays->AppendL(new (ELeave) CArrayPtrFlat<CFbsBitmap>(0));
        }
}

/*!

 */
void CIpsUiDataMtm::GetResourceFileName(TFileName& aFileName) const
{
    aFileName = KIpsMtmUiDataResourceFile;
}

/*!

 */
const CBaseMtmUiData::CBitmapArray& CIpsUiDataMtm::ContextIcon(
        const TMsvEntry& /*aContext*/, TInt /*aStateFlags*/) const
{
    return *iIconArrays->At(0);
}

/*!

 */
TBool CIpsUiDataMtm::CanCreateEntryL(
        const TMsvEntry& /*aParent*/, 
        TMsvEntry& /*aNewEntry*/, 
        TInt& /*aReasonResourceId*/) const
{
    return EFalse;
}

/*!

 */
TBool CIpsUiDataMtm::CanDeleteFromEntryL(
        const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
{
    return EFalse;
}

/*!

 */
TBool CIpsUiDataMtm::CanDeleteServiceL(
        const TMsvEntry& /*aService*/, TInt& /*aReasonResourceId*/) const
{
    return EFalse;
}

/*!

 */
TBool CIpsUiDataMtm::CanReplyToEntryL(
        const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
{
    return EFalse;
}

/*!

 */
TBool CIpsUiDataMtm::CanForwardEntryL(
        const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
{
    return EFalse;
}

/*!

 */
TBool CIpsUiDataMtm::CanEditEntryL(
        const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
{
    return EFalse;
}

/*!

 */
TBool CIpsUiDataMtm::CanViewEntryL(
        const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
{
    return EFalse;
}

/*!

 */
TBool CIpsUiDataMtm::CanOpenEntryL(
        const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
{
    return EFalse;
}

/*!

 */
TBool CIpsUiDataMtm::CanCloseEntryL(
        const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
{
    return EFalse;
}

/*!

 */
TBool CIpsUiDataMtm::CanCopyMoveToEntryL(
        const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
{
    return EFalse;
}

/*!

 */
TBool CIpsUiDataMtm::CanCopyMoveFromEntryL(
        const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
{
    return EFalse;
}

/*!

 */
TBool CIpsUiDataMtm::CanCancelL(
        const TMsvEntry& /*aContext*/, TInt& /*aReasonResourceId*/) const
{
    return EFalse;
}

/*!

 */
TInt CIpsUiDataMtm::OperationSupportedL(
        TInt /*aOperationId*/, const TMsvEntry& /*aContext*/) const
{
    return KErrNotSupported;
}

/*!

 */
TInt CIpsUiDataMtm::QueryCapability(
        TUid /*aFunctionId*/, TInt& /*aResponse*/) const
{
    return KErrNotSupported;
}

/*!

 */
HBufC* CIpsUiDataMtm::StatusTextL(
        const TMsvEntry& /*aContext*/) const
{
    return HBufC::NewL(0);
}


    

