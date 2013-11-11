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
 *      Email message text content private implementation.
 */

#include "nmapiheaders.h"

namespace EmailClientApi
{

/*!
    Class constructor.
*/
NmApiTextContentPrivate::NmApiTextContentPrivate()
{
    NM_FUNCTION;
}

/*!
    Class copy constructor.
*/
NmApiTextContentPrivate::NmApiTextContentPrivate(const NmApiTextContentPrivate &textcontentprivate)
 : QSharedData(textcontentprivate),
   mContent(textcontentprivate.mContent)
{
    NM_FUNCTION;
}

/*!
    Class destructor.
*/
NmApiTextContentPrivate::~NmApiTextContentPrivate()
{
    NM_FUNCTION;
}

/*!
   Getter for content
 */
QString NmApiTextContentPrivate::content() const
{
    NM_FUNCTION;
    return mContent;
}

/*!
   Setter for content
 */
void NmApiTextContentPrivate::setContent(const QString &content)
{
    NM_FUNCTION;
    mContent = content;
}

}
