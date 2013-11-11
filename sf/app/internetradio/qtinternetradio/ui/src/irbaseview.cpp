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
#include "irapplication.h"
#include "irbaseview.h"
#include "irviewmanager.h"

/*
 * Description  : constructor
 * Parameters   : aApplication : the pointer to the internet radio application object
 *                aViewId      : the id of the view
 * Return       : None
 */
IRBaseView::IRBaseView(IRApplication* aApplication, TIRViewId aViewId) : iApplication(aApplication),
                                                                         iViewId(aViewId),
                                                                         iUseNetworkReason(EIR_UseNetwork_NoReason)
{
    iNetworkController = iApplication->getNetworkController();        
    iPlayController = iApplication->getPlayController();
    iIsdsClient = iApplication->getIsdsClient();
    iFavorites = iApplication->getFavoritesDB();
    iSettings = iApplication->getSettings();
}

TIRViewId IRBaseView::id() const
{
    return iViewId;
}

/*
 * Description : handle system events reported by system event collector. If a sub-class wants
 *               to handle such system events, it must override this function.
 * Parameters  : aEvent : TIRSystemEventType
 * Return      : EIR_DoDefault : caller does default handling
 *               EIR_NoDefault : caller doesn't do default handling
 */
TIRHandleResult IRBaseView::handleSystemEvent(int aEvent)
{
    Q_UNUSED(aEvent);
    
    return EIR_DoDefault;
}

/*
 * Description : handle view commands : EIR_ViewCommand_ACTIVATE, EIR_ViewCommand_DEACTIVATE,
 *               EIR_ViewCommand_BACK
 * Parameters  : aCommand : TIRViewCommand
 *               aReason  : TIRViewCommandReason
 * Return      : EIR_DoDefault : caller does default handling
 *               EIR_NoDefault : caller doesn't do default handling
 */
TIRHandleResult IRBaseView::handleCommand(TIRViewCommand aCommand, TIRViewCommandReason aReason)
{
    Q_UNUSED(aCommand);
    Q_UNUSED(aReason);
    
    return EIR_DoDefault;
}

/*
 * Description : the action to be done when the view is starting view. Default implementation 
 *               is doing nothing.
 *               Note : 1.this function should only be called in void IRApplication::setLaunchView().  
 *                      2.this funciton is called after view is showed up,
 *                       so can NOT put any UI-related code here.   
 */
void IRBaseView::launchAction()
{
}

void IRBaseView::updateView()
{
}

void IRBaseView::setFlag(TViewFlags aFlag)
{
    iFlags |= aFlag;
}

void IRBaseView::clearFlag(TViewFlag aFlag)
{
    if(iFlags.testFlag(aFlag))
    {
        iFlags ^= aFlag;
    }
}
bool IRBaseView::testFlag(TViewFlag aFlag) const
{
    return iFlags.testFlag(aFlag);
}

void IRBaseView::setUseNetworkReason(TIRUseNetworkReason aReason)
{
    iUseNetworkReason = aReason;
}
    
TIRUseNetworkReason IRBaseView::getUseNetworkReason() const
{
    return iUseNetworkReason;
}
 

/*
 * Description : return the pointer to the view manager object owned by application
 * Parameters  : None
 * Return      : the pointer to the view manager object owned by application
 */
IRViewManager* IRBaseView::getViewManager() const
{
    return iApplication->getViewManager();
}

void IRBaseView::popupNote(const QString &aNote, const HbMessageBox::MessageBoxType &aNoteType) const
{
    switch (aNoteType)
    {
    case HbMessageBox::MessageTypeInformation:
        HbMessageBox::information(aNote, (QObject*)NULL, NULL, HbMessageBox::Ok);
        break;
        
    case HbMessageBox::MessageTypeWarning:
        HbMessageBox::warning(aNote, (QObject*)NULL, NULL, HbMessageBox::Ok);
        break;
        
    default:
        break;
    }
}

