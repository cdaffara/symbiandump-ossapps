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
 *      Email message text content implementation.
 */

#include "nmapiheaders.h"

namespace EmailClientApi
{

/*!
    Class constructor.
*/
NmApiTextContent::NmApiTextContent()
{
    NM_FUNCTION;
    d = new NmApiTextContentPrivate();
}

/*!
    Class copy constructor.
*/
NmApiTextContent::NmApiTextContent(const NmApiTextContent &textcontent)
{
    NM_FUNCTION;
    d = textcontent.d;
}

/*!
    Class destructor.
*/
NmApiTextContent::~NmApiTextContent()
{
    NM_FUNCTION;
}

NmApiTextContent& NmApiTextContent::operator=(const NmApiTextContent &textcontent)
{
    if (this == &textcontent)
        return *this;
    
    d = textcontent.d;

    return *this;
}

/*!
   Getter for content
 */
QString NmApiTextContent::content() const
{
    NM_FUNCTION;
    return d->content();
}

/*!
   Setter for content
 */
void NmApiTextContent::setContent(const QString &content)
{
    NM_FUNCTION;
    d->setContent(content);
}

}
