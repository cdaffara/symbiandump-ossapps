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
 * Description: Menu Mode Wrapper
 *
 */


#include "hsmenumodewrapper.h"
/*!
    \class HsMenuModeWrapper
    \ingroup group_hsmenustateplugin

    \brief Menu Mode Wrapper.

    Represents objects to keep Hs Menu Mode.

    \sa Hs Menu Mode
*/

/*!
   Constructor.
 */

HsMenuModeWrapper::HsMenuModeWrapper(): mMode(Hs::NormalHsMenuMode)
{
}
    
/*!
    \return homescreen mode (add mode on / add mode off) set on the object
 */
Hs::HsMenuMode HsMenuModeWrapper::getHsMenuMode() const
{
    return mMode;
}

/*!
    \return homescreen token
 */
QVariant HsMenuModeWrapper::getHsToken() const
{
    return mToken;
}

/*!
    \return sets in the object homescreen mode (add mode on / add mode off)
 */
void HsMenuModeWrapper::setHsMenuMode(Hs::HsMenuMode menuMode, QVariant token) 
{
    mMode = menuMode;
    mToken = token;
}

