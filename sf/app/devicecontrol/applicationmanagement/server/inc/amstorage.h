/*
 * Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: Implementation of applicationmanagement components
 *
 */

#ifndef __AMDSTORAGE_H__
#define __AMDSTORAGE_H__

#include <e32base.h>
#include <ssl.h>
#include <s32mem.h>
#include "ApplicationManagementCommon.h"

class CRepository;

namespace NApplicationManagement
    {

    class CDeploymentComponent;
    class CAMAppHideUtil;

    class RComponentIdArray : public RArray<TUint32>
        {
public:
        void SetListL(const TDesC8& aChilds);
        void GetListLC(HBufC8 *& aChilds, TInt &aLength);
        void RemoveByValue(TUint32 aValue);
        };

    typedef TPckgBuf<TCertInfo> TCertInfoPckg;

    class RCertArray : public RPointerArray<TCertInfoPckg>
        {
public:
        void ExternalizeL(RWriteStream& aStream) const;
        void InternalizeL(RReadStream& aStream);
        TInt FindByValue(const TCertInfoPckg &aPckg);
        };
    /**
     *  Factory class to Data component creation
     */
    class CDeliveryComponentStorage : public CBase
        {
        /**
         * Constructor
         */
        CDeliveryComponentStorage();

        /**
         * 2nd phase constructor. Loads compontns
         */
        void ConstructL();

        /**
         * Saves the state to disk
         */
        void PersistStateL();

        /**
         * Loads components from disk
         */
        void LoadComponentsL();

        /**
         * Creates and returns next free deployment component id
         */
        TInt NextKey();

        /**
         * Loads certificate array
         */
        void LoadCertsL();

        /**
         * Saves and closes certificate array
         */
        void CloseCertificatesL();

public:

        /**
         * Creates new storage object and constructs it
         */
        static CDeliveryComponentStorage* NewL();

        /**
         * Creates new storage object and constructs it, leaving it
         * on the cleanup stack
         */
        static CDeliveryComponentStorage* NewLC();

        /**
         * Destructor
         */
        virtual ~CDeliveryComponentStorage();

        /**
         * GetComponentIds Returns the const id array of component ids.
         */
        const RComponentIdArray &GetComponentIds() const;

        /**
         * Places all those component's ids in array who have possibly changed
         * state and no StateChangedL for that id called afterwards.
         * @param aArray array to hold the ids
         */
        void GetStateChangeComponentIdsL(RComponentIdArray &aArray);

        /**
         * StateChangedL Confirms that state change has been acknowledged by user
         * @param aInternalId The id whose state change is confirmed.
         */
        void StateChangedL(TUint32 aInternalId);

        /**
         * Creates new deployment component object having given state and user id
         * and constructs it. Optionally component may have owning certificate.
         * @param aState the state of to-be-created component
         * @param aUserId the user id of to-be-created component
         * @param aCertInfo the certificate of the owner of to-be-created component
         */
        CDeploymentComponent *NewComponentL(
                const TDeploymentComponentState &aState,
                const TDCUserId &aUserId, const TCertInfo *aCertInfo = NULL);

        /**
         * DeliverL makes the given component ot delivered state
         * @param aComponent the component that'll be delivered
         */
        void DeliverL(CDeploymentComponent &aComponent);

        /**
         * UpdateL updates the given component permanently
         * @param aComponent the component that'll be persisted
         */
        void UpdateL(const CDeploymentComponent &aComponent);

        /**
         * RemoveL Removes the deployment component having given id. The related object
         * will be invalid after call.
         * @param aInternalId the to-be-removed component's id
         */
        void RemoveL(TUint32 aInternalId);

        /**
         * InstalledL updates the given component state 
         * @param aComponent the component that'll be persisted
         */
        void InstalledL(CDeploymentComponent &aComponent);

    //Update Deplyoment Components State variable
	void SetDeploymentComponentState(CDeploymentComponent &aComponent,TDeploymentComponentState aState);
        CDeploymentComponent &ComponentL(TUint32 aInternalId);
        void ActivateL(CDeploymentComponent &aComponent);
        void DeactivateL(CDeploymentComponent &aComponent);

        void CheckForDuplicateNodesInDeployedL(
                CDeploymentComponent &aComponent);
        HBufC8* IntToDes8LC(const TInt aLuid);

        const RCertArray &Certificates() const
            {
            return iCertificates;
            }
protected:

        CDeploymentComponent *LoadComponentL(TInt aInternalId,
                const TDesC8 &aBuffer) const;
private:
        CRepository *iRepository;
        RPointerArray<CDeploymentComponent> iComponents;
        RComponentIdArray iComponentIds;
        TInt iNextId;
        RCertArray iCertificates;
        CAMAppHideUtil *iHidder;
        };

    }

#endif __AMDELIVERYCOMPSTORAGE_H__

//  End of File
