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
* Description:
*
*/

#ifndef MEMSPYENGINEHELPERWINDOWSERVERIMPL_H
#define MEMSPYENGINEHELPERWINDOWSERVERIMPL_H

// System includes
#include <e32base.h>
#include <w32std.h>


// User includes
#include <memspy/engine/memspyenginehelperwindowserver.h>


// Classes referenced


NONSHARABLE_CLASS( CMemSpyEngineHelperWindowServerImpl ) : public CBase, public MMemSpyEngineHelperWindowServer
    {
public:
    IMPORT_C static CMemSpyEngineHelperWindowServerImpl* NewL();
    virtual ~CMemSpyEngineHelperWindowServerImpl();

private:
    CMemSpyEngineHelperWindowServerImpl();
    void ConstructL();

public: // API
    IMPORT_C MMemSpyEngineWindowGroupList* WindowGroupListL();
    IMPORT_C void GetWindowGroupListL( RArray<TMemSpyEngineWindowGroupBasicInfo>& aWindowGroups );
    IMPORT_C void GetWindowGroupDetailsL( TInt aWindowGroupId, TMemSpyEngineWindowGroupDetails& aWindowGroupDetails );
    IMPORT_C void SwitchToL( TInt aWindowGroupId );

private:
    RWsSession iWsSession;
    };




NONSHARABLE_CLASS( CMemSpyEngineWindowGroupListImpl ) : public CBase, public MMemSpyEngineWindowGroupList
    {
public:
    static CMemSpyEngineWindowGroupListImpl* NewLC();
    virtual ~CMemSpyEngineWindowGroupListImpl();
    
private:
    CMemSpyEngineWindowGroupListImpl();
    void ConstructL();

public: // API
    IMPORT_C TInt Count() const;
    IMPORT_C const TMemSpyEngineWindowGroupBasicInfo& At( TInt aIndex ) const;
    IMPORT_C TInt ItemIndex( const TMemSpyEngineWindowGroupBasicInfo& aEntry ) const;

public: // But not exported
    void AddItemL( TMemSpyEngineWindowGroupBasicInfo& aItem );

public: // From MDesCArray
    IMPORT_C TInt MdcaCount() const;
    IMPORT_C TPtrC MdcaPoint( TInt aIndex ) const;

private: // Data members
    RArray<TMemSpyEngineWindowGroupBasicInfo> iItems;
    };


#endif
