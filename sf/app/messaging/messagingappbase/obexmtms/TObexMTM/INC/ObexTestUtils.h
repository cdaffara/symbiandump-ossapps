// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//


#ifndef __OBEXTESTUTILS_H__
#define __OBEXTESTUTILS_H__



// System includes
#include "msvtestutils.h"
// User includes

// forward declarations

class CObexTestUtils : public CMsvTestUtils
    {
    public:
        static CObexTestUtils* NewL(RTest& aTest, TUint aCreationFlags = ETuNone);
        static CObexTestUtils* NewLC(RTest& aTest, TUint aCreationFlags = ETuNone);
        ~CObexTestUtils();
    
    public:     //from CMsvTestUtils
        virtual void InstantiateClientMtmsL();
		virtual void InstantiateServerMtmsL();
		virtual void DeleteServicesL();
	    virtual void CreateServicesL();
	    virtual void FindExistingServicesL();
		virtual void CreateServerMtmRegsL();
	    virtual void InstallMtmGroupsL();
        virtual void Reset();
        virtual void Panic(TInt aPanic);

    public:        
        TMsvId CreateObexServiceL(TUid aMsgType);  
        void InstantiateBTClientMtmL();
		void InstantiateBTServerMtmL();
        void InstantiateIRClientMtmL();
		void InstantiateIRServerMtmL();

        const CBaseMtm& GetCurrentObexClientMtm() const;
        CBaseMtm& GetCurrentObexClientMtm();
        const CBaseServerMtm& GetCurrentObexServerMtm() const;
        CBaseServerMtm& GetCurrentObexServerMtm();
        TMsvId GetCurrentServiceId() const;

        TBool SelectObexMtmL(TUid aMsgType);
        TUid CurrentObexMtm() const;

		void RegisterIRMtmL();
		void RegisterBTMtmL();
		void UnRegisterIRMtmL();
		void UnRegisterBTMtmL();


    private:    //methods
        CObexTestUtils(RTest& aTest);
        void ConstructL(TUint aCreationFlags);
      

    private:    //attribs
        CBaseMtm* iBTClientMtm;
        CBaseServerMtm* iBTServerMtm;
        CBaseMtm* iIRClientMtm;
        CBaseServerMtm* iIRServerMtm;
        TMsvId iBTServiceId;
        TMsvId iIRServiceId;
        TUid iCurrentMtm;               //the currently selected MTM
        
    };

#endif // __OBEXTESTUTILS_H__
