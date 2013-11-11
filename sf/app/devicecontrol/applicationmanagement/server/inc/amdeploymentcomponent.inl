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
 * Description: Implementation of applicationmanagement components
 *
 */
#include "amdeploymentcomponentdata.h"

inline const TDesC8 &CDeploymentComponent::DownloadURI() const
    {
    return iDownloadURI;
    }

inline const TUid & CDeploymentComponent::Uid() const
    {
    return iUid;
    }

inline const TDeploymentComponentId &CDeploymentComponent::ComponentId() const
    {
    return iId;
    }

inline const TDeploymentComponentName &CDeploymentComponent::ComponentName() const
    {
    return iName;
    }

inline const TDeploymentComponentVersion &CDeploymentComponent::ComponentVersion() const
    {
    return iVersion;
    }

inline TDeploymentComponentState CDeploymentComponent::State() const
    {
    return iState;
    }

inline TUint32 CDeploymentComponent::InternalId() const
    {
    return iInternalId;
    }

inline const TDesC8& CDeploymentComponent::UserId() const
    {
    return iUserId;
    }

inline const TDeploymentComponentState& CDeploymentComponent::OldState() const
    {
    return iOldState;
    }

inline void CDeploymentComponent::StateChangeComplete()
    {
    iOldState = EDCSNone;
    }
inline TInt CDeploymentComponent::Status() const
    {
    return iStatus;
    }
inline const TDeploymentComponentPkgId& CDeploymentComponent::PkgId() const
    {
    return iPkgID;
    }
inline const TDeploymentComponentDescription& CDeploymentComponent::Description() const
    {
    return iDescription;
    }
inline const TDeploymentComponentPkgIdRef& CDeploymentComponent::PkgIDRef() const
    {
    return iPkgIDRef;
    }
inline const TDeploymentComponentPkgType& CDeploymentComponent::PkgType() const
    {
    return iPkgType;
    }

inline const TMidletnameName& CDeploymentComponent::MidletName() const
    {
    return iMidletName;
    }
inline const TMidletVendorName& CDeploymentComponent::MidletVendor() const
    {
    return iMideltVendorName;
    }
inline const TMidletVersion& CDeploymentComponent::MidletVersion() const
    {
    return iMidletVersion;
    }
