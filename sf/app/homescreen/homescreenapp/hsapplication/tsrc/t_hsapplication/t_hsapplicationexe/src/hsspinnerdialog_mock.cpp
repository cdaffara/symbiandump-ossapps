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
}

/*!
    \internal
*/
HsSpinnerDialog::~HsSpinnerDialog()
{
}

/*!
    \internal
*/
void HsSpinnerDialog::start()
{
}

/*!
    \internal
*/
void HsSpinnerDialog::stop()
{
}

