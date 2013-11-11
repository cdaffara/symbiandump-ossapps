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




#ifndef CREATORCONTACTSETCACHE_H_
#define CREATORCONTACTSETCACHE_H_

#include <e32base.h>

// Forward declarations
class CContactLinkCacheImp;

class CCreatorContactSet : public CBase
{
public:
    static CCreatorContactSet* NewL(TInt aLinkId, TInt aNumOfExistingContacts);
    virtual ~CCreatorContactSet();
    void AppendL(TUint32);
    TInt NumberOfExistingContacts() const;
    RArray<TUint32> ContactLinks();
    const RArray<TUint32> ContactLinks() const;

    TInt LinkId() const;

private:
    CCreatorContactSet(TInt aLinkId, TInt aNumOfExistingContacts);    
    //void ConstructL();
    TInt iLinkId;
    TInt iNumOfExistingContacts;
    RArray<TUint32> iContactLinks;
};

class MContactLinkCache
{
public:
    virtual void AppendL(CCreatorContactSet* aContactSet) = 0;
    virtual RArray<TUint32> ContactLinks(TInt aLinkId) = 0;
    virtual const RArray<TUint32> ContactLinks(TInt aLinkId) const = 0;    
    virtual RPointerArray<CCreatorContactSet>& ContactSets() = 0;
    virtual const RPointerArray<CCreatorContactSet>& ContactSets() const = 0;
    virtual const CCreatorContactSet& ContactSet(TInt aLinkId) const = 0;
    virtual CCreatorContactSet& ContactSet(TInt aLinkId) = 0;
};

class ContactLinkCache
{
public:
    static void InitializeL();
    static void DestroyL();
    
    static MContactLinkCache* Instance();
    
private:
    ContactLinkCache(){};
    static CContactLinkCacheImp* iImp;
};

#endif /*CREATORCONTACTSETCACHE_H_*/
