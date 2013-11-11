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

#include "hsmessageboxwrapper.h"

/*!
    \class HsMessageBoxWrapper
    \ingroup group_hsutils
    \brief 
*/
struct HsMessageBoxWrapperImpl{

};
/*!

*/
HsMessageBoxWrapper::HsMessageBoxWrapper(QObject *parent)
  : QObject(parent), mImpl(new HsMessageBoxWrapperImpl)
{
}

/*!

*/
HsMessageBoxWrapper::~HsMessageBoxWrapper()
{
  delete mImpl;
}

/*!

*/
void HsMessageBoxWrapper::setHeader(const QString &header)
{
 	Q_UNUSED(header);
}
/*!

*/
void HsMessageBoxWrapper::setQueryText(const QString &queryText)
{
   	Q_UNUSED(queryText);
}
/*!

*/
void HsMessageBoxWrapper::show()
{
    emit accepted();   
}
/*!

*/
void HsMessageBoxWrapper::close()
{
}

/*!

*/
void HsMessageBoxWrapper::onDialogClosed(int action)
{
    Q_UNUSED(action)
}