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

#ifndef NMIPSMTMUIDATA_H_
#define NMIPSMTMUIDATA_H_

#include <e32base.h>
#include <mtudcbas.h>

NONSHARABLE_CLASS ( CIpsUiDataMtm ) : public CBaseMtmUiData
{
public:
    
    IMPORT_C static CIpsUiDataMtm* NewL(
        CRegisteredMtmDll& aRegisteredMtmDll);
    
    virtual ~CIpsUiDataMtm();
    
    const CBitmapArray& ContextIcon(
            const TMsvEntry& aContext, TInt aStateFlags) const;
    TBool CanCreateEntryL(
            const TMsvEntry& aParent, TMsvEntry& aNewEntry, TInt& aReasonResourceId) const;
    TBool CanDeleteFromEntryL(
            const TMsvEntry& aContext, TInt& aReasonResourceId) const;
    TBool CanDeleteServiceL(
            const TMsvEntry& aService, TInt& aReasonResourceId) const;
    TBool CanReplyToEntryL(
            const TMsvEntry& aContext, TInt& aReasonResourceId) const;
    TBool CanForwardEntryL(
            const TMsvEntry& aContext, TInt& aReasonResourceId) const;
    TBool CanEditEntryL(
            const TMsvEntry& aContext, TInt& aReasonResourceId) const;
    TBool CanViewEntryL(
            const TMsvEntry& aContext, TInt& aReasonResourceId) const;
    TBool CanOpenEntryL(
            const TMsvEntry& aContext, TInt& aReasonResourceId) const;
    TBool CanCloseEntryL(
            const TMsvEntry& aContext, TInt& aReasonResourceId) const;
    TBool CanCopyMoveToEntryL(
            const TMsvEntry& aContext, TInt& aReasonResourceId) const;
    TBool CanCopyMoveFromEntryL(
            const TMsvEntry& aContext, TInt& aReasonResourceId) const;
    TBool CanCancelL(
            const TMsvEntry& aContext, TInt& aReasonResourceId) const;
    TInt OperationSupportedL(
            TInt aOperationId, const TMsvEntry& aContext) const;
    TInt QueryCapability(
            TUid aFunctionId, TInt& aResponse) const;
    HBufC* StatusTextL(
            const TMsvEntry& aContext) const;

protected:

    CIpsUiDataMtm(CRegisteredMtmDll& aRegisteredMtmDll);
    void ConstructL();

    void PopulateArraysL();
    void GetResourceFileName(TFileName& aFileName) const;
};
#endif /* NMIPSMTMUIDATA_H_ */
