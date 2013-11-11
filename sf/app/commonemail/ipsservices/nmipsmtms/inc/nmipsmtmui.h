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

#ifndef IPSIMAP4MTMUI_H
#define IPSIMAP4MTMUI_H


#include <e32base.h>
#include <mtmuibas.h>
#include <e32des8.h>

NONSHARABLE_CLASS ( CIpsMtmUi ) : public CBaseMtmUi
{
public:
    
    IMPORT_C static CIpsMtmUi* NewL(
        CBaseMtm& aBaseMtm,
        CRegisteredMtmDll& aRegisteredMtmDll);
    
    virtual ~CIpsMtmUi();
    
    CMsvOperation* OpenL(TRequestStatus& aStatus);
    CMsvOperation* CloseL(TRequestStatus& aStatus);
    CMsvOperation* EditL(TRequestStatus& aStatus);
    CMsvOperation* ViewL(TRequestStatus& aStatus); 
    CMsvOperation* OpenL(
            TRequestStatus& aStatus, 
            const CMsvEntrySelection& aSelection); 
    CMsvOperation* CloseL(
            TRequestStatus& aStatus, 
            const CMsvEntrySelection& aSelection);
    CMsvOperation* EditL(
            TRequestStatus& aStatus, 
            const CMsvEntrySelection& aSelection);
    CMsvOperation* ViewL(
            TRequestStatus& aStatus, 
            const CMsvEntrySelection& aSelection); 
    CMsvOperation* CancelL(
            TRequestStatus& aStatus, 
            const CMsvEntrySelection& aSelection);
    CMsvOperation* ReplyL(
            TMsvId aDestination, 
            TMsvPartList aPartlist, 
            TRequestStatus& aCompletionStatus);
    CMsvOperation* ForwardL(
            TMsvId aDestination, 
            TMsvPartList aPartList, 
            TRequestStatus& aCompletionStatus);
protected:
    
    CIpsMtmUi(CBaseMtm& aBaseMtm, 
            CRegisteredMtmDll& aRegisteredMtmDll);

    void ConstructL();
    
    void GetResourceFileName(TFileName& aFileName) const;
    
private:
    
    TBuf8<1>        iDummyBuf;
};


#endif //  IPSIMAP4MTMUI_H


