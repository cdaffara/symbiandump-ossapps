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
*    Spinner dialog shown when setting new background image
*
*/

#include <HbIconItem>
#include <HbDialog>
#include <HbInstance>
#include <HbDeviceProfile>

#include "hsspinnerdialog.h"

/*!
    \internal
    \class HsSpinnerDialog
    \ingroup group_hsutils
    \brief Spinner dialog, used when background image is changed. By default dialog is created with Qt::WA_DeleteOnClose attribute so dialog
    is automatically deleted when closed.    
    
*/

/*!
    \internal
    Constructor.
    
    \a parent Owner.
*/
HsSpinnerDialog::HsSpinnerDialog(QGraphicsItem *parent)
: HbDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(true);
    setBackgroundFaded(true);
    setTimeout(HbPopup::NoTimeout);
    setBackgroundItem(0);
    setDismissPolicy(HbPopup::NoDismiss);
    HbIconItem* spinnerAnimation = new HbIconItem("qtg_anim_loading");
    HbDeviceProfile profile;
    qreal factor = profile.unitValue();
    spinnerAnimation->setPreferredSize(factor*10,factor*10);
    spinnerAnimation->setParentItem(this);
    setContentWidget(spinnerAnimation);
}

/*!
    \internal
    Destructor.    
*/
HsSpinnerDialog::~HsSpinnerDialog()
{
}

/*!
    \internal
    Starts spinner animation.
*/
void HsSpinnerDialog::start()
{
    HbInstance::instance()->allMainWindows().first()->setInteractive(false);
    show();
}

/*!
    \internal
    Stops spinner animation, dialog is deleted after it is closed.
*/
void HsSpinnerDialog::stop()
{
    HbInstance::instance()->allMainWindows().first()->setInteractive(true);
    close();
}

