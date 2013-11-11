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

#include <QPropertyAnimation>
#include "hspropertyanimationwrapper.h"

/*!
    \class HsMessageBoxWrapper
    \ingroup group_hsutils
    \brief 
*/
struct HsPropertyAnimationWrapperImpl
{
public:
    bool running;
    bool forward;
};

/*!

*/
HsPropertyAnimationWrapper::HsPropertyAnimationWrapper(QObject *parent)
  : QObject(parent),mImpl(new HsPropertyAnimationWrapperImpl)
{
    mImpl->running = false;
    mImpl->forward = true;
}

/*!

*/
HsPropertyAnimationWrapper::~HsPropertyAnimationWrapper()
{
  
}

/*!

*/
void HsPropertyAnimationWrapper::setTargetObject(QObject *target)
{
   Q_UNUSED(target)
}
/*!

*/
void HsPropertyAnimationWrapper::setPropertyName(const QByteArray &propertyName)
{
   Q_UNUSED(propertyName)
}

bool HsPropertyAnimationWrapper::isRunning()
{
    return mImpl->running;
}
void HsPropertyAnimationWrapper::setEndValue(const QVariant &value)
{
     Q_UNUSED(value)
}
void HsPropertyAnimationWrapper::setDuration(int msecs)
{
     Q_UNUSED(msecs)
}
void HsPropertyAnimationWrapper::setForward()
{
    mImpl->forward = true;
}

void HsPropertyAnimationWrapper::setBackward()
{
     mImpl->forward = false;
}

bool HsPropertyAnimationWrapper::isForward() const
{
    return mImpl->forward;
}
void HsPropertyAnimationWrapper::start()
{
    mImpl->running = true;
    emit finished();
}
/*!

*/
void HsPropertyAnimationWrapper::stop()
{
    mImpl->running = false;
}

/*!

*/
void HsPropertyAnimationWrapper::setEasingCurve(const QEasingCurve &curve)
{
   Q_UNUSED(curve)
}
