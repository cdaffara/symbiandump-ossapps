/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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

#include "cpprofilemonitor_p.h"
#include "cpprofilemonitor.h"
#include "cpprofilemodel.h"
#include <ProEngFactory.h>
#include <CProfileChangeNotifyHandler.h>

/*
 * Construtor
 */
CpProfileMonitorPrivate::CpProfileMonitorPrivate():
                                mProfileNotifier(0)
{
    
}

/*
 * Desconstructor
 */
CpProfileMonitorPrivate::~CpProfileMonitorPrivate()
{
    delete mProfileModel;
    delete mProfileNotifier;
}

/*
 * Initializing for the monitoring profile event
 */
void CpProfileMonitorPrivate::initialize(CpProfileMonitor *parent)
{
    q_ptr = parent;
    mProfileModel = new CpProfileModel();
    TRAP_IGNORE(mProfileNotifier = ProEngFactory::NewNotifyHandlerL());
    TRAP_IGNORE(mProfileNotifier->RequestProfileActivationNotificationsL(*this));
    TRAP_IGNORE(mProfileNotifier->RequestProfileNameArrayNotificationsL(*this));
    TRAP_IGNORE(mProfileNotifier->RequestProfileNotificationsL(*this, EProfileWrapperGeneralId));
    TRAP_IGNORE(mProfileNotifier->RequestProfileNotificationsL(*this, EProfileWrapperMeetingId));
}

/*
 * From MProEngProfileObserver
 */
void CpProfileMonitorPrivate::HandleProfileModifiedL( TInt aProfileId )
{
    if (aProfileId == mProfileModel->activeProfileId()) {
        q_ptr->avtiveProfileModified();
    } else {
        q_ptr->profileModified(aProfileId);
    }
}

/*
 * From MProEngProfileActivationObserver
 */
void CpProfileMonitorPrivate::HandleProfileActivatedL( TInt aProfileId )
{
    q_ptr->profileActivated( aProfileId );
}

/*
 * From MProEngProfileNameArrayObserver
 */
void CpProfileMonitorPrivate::HandleProfileNameArrayModificationL()
{
    q_ptr->profileNameModified();
}

/*
 * Stop receiveing notification
 */
void CpProfileMonitorPrivate::stopMonitoring()
{
    mProfileNotifier->CancelAll();
}
