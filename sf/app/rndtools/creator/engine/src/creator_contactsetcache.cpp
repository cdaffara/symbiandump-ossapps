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



#include "creator_contactsetcache.h"


CContactLinkCacheImp* ContactLinkCache::iImp = 0;

CCreatorContactSet* CCreatorContactSet::NewL(TInt aLinkId, TInt aNumOfExistingContacts)
    {
    return new (ELeave) CCreatorContactSet(aLinkId, aNumOfExistingContacts);    
    }

TInt CCreatorContactSet::LinkId() const
    {
    return iLinkId;
    }

void CCreatorContactSet::AppendL(TUint32 aContactLink)
    {
    iContactLinks.AppendL(aContactLink);
    }

RArray<TUint32> CCreatorContactSet::ContactLinks()
    {
    return iContactLinks;
    }

const RArray<TUint32> CCreatorContactSet::ContactLinks() const
    {
    return iContactLinks;
    }


TInt CCreatorContactSet::NumberOfExistingContacts() const
    {
    return iNumOfExistingContacts;
    }

CCreatorContactSet::~CCreatorContactSet()
    {
    iContactLinks.Reset();
    iContactLinks.Close();
    }


CCreatorContactSet::CCreatorContactSet(TInt aLinkId, TInt aNumOfExistingContacts)
    {
    iLinkId = aLinkId;
    iNumOfExistingContacts = aNumOfExistingContacts;
    }

class CContactLinkCacheImp : public CBase, public MContactLinkCache
{
public:
    static CContactLinkCacheImp* NewL();
    virtual ~CContactLinkCacheImp();
    virtual void AppendL(CCreatorContactSet* aContactSet); 

    virtual RArray<TUint32> ContactLinks(TInt aLinkId);
    virtual const RArray<TUint32> ContactLinks(TInt aLinkId) const;

    virtual RPointerArray<CCreatorContactSet>& ContactSets();
    virtual const RPointerArray<CCreatorContactSet>& ContactSets() const;
    virtual const CCreatorContactSet& ContactSet(TInt aLinkId) const;
    virtual CCreatorContactSet& ContactSet(TInt aLinkId);
    
private:
    void ConstructL();
    CContactLinkCacheImp();
    
    RArray<TUint32> iEmptyLinks;

    RPointerArray<CCreatorContactSet> iContactSets;
    CCreatorContactSet* iDummyContactSet;
};

CContactLinkCacheImp* CContactLinkCacheImp::NewL()
    {
    CContactLinkCacheImp* self = new (ELeave) CContactLinkCacheImp();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

void CContactLinkCacheImp::ConstructL()
    {
    if( iDummyContactSet == 0 )
        iDummyContactSet = CCreatorContactSet::NewL(-1, 0);    
    }

CContactLinkCacheImp::CContactLinkCacheImp()
: iDummyContactSet(0)
    {}

CContactLinkCacheImp::~CContactLinkCacheImp()
    {
    iEmptyLinks.Reset();// just in case...
    iEmptyLinks.Close();

    iContactSets.ResetAndDestroy();
    iContactSets.Close();
    delete iDummyContactSet;
    }
void CContactLinkCacheImp::AppendL(CCreatorContactSet* aContactSet)
    {
    iContactSets.AppendL(aContactSet);
    }
RArray<TUint32> CContactLinkCacheImp::ContactLinks(TInt aLinkId)
    {
    for( TInt i = 0; i < iContactSets.Count(); ++i )
        {
        if( iContactSets[i]->LinkId() == aLinkId )
            {
            return iContactSets[i]->ContactLinks();
            }
        }
    return iEmptyLinks;
    }

const RArray<TUint32> CContactLinkCacheImp::ContactLinks(TInt aLinkId) const
    {
    for( TInt i = 0; i < iContactSets.Count(); ++i )
        {
        if( iContactSets[i]->LinkId() == aLinkId )
            {
            return iContactSets[i]->ContactLinks();
            }
        }
    return iEmptyLinks;
    }


const CCreatorContactSet& CContactLinkCacheImp::ContactSet(TInt aLinkId) const
    {
    for( TInt i = 0; i < iContactSets.Count(); ++i )
        {
        if( iContactSets[i]->LinkId() == aLinkId )
            {
            return *iContactSets[i];
            }
        }
        
    return *iDummyContactSet;
    }

CCreatorContactSet& CContactLinkCacheImp::ContactSet(TInt aLinkId)
    {
    for( TInt i = 0; i < iContactSets.Count(); ++i )
        {
        if( iContactSets[i]->LinkId() == aLinkId )
            {
            return *iContactSets[i];
            }
        }
        
    return *iDummyContactSet;
    }

RPointerArray<CCreatorContactSet>& CContactLinkCacheImp::ContactSets()
    {
    return iContactSets;
    }

const RPointerArray<CCreatorContactSet>& CContactLinkCacheImp::ContactSets() const
    {
    return iContactSets;
    }


void ContactLinkCache::InitializeL()
    {
    if( iImp == 0 )
        {
        iImp = CContactLinkCacheImp::NewL();
        }
    }

void ContactLinkCache::DestroyL()
    {
    delete iImp;
    iImp = 0;
    }

MContactLinkCache* ContactLinkCache::Instance()
    {
    return iImp;
    }


    /*{
public:
    static void ;
    static void DestructLD();
    
    static MContactLinkCache* Instance();
    
private:
    CContactLinkImp* iImp;
};*/

