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
#include <QEasingCurve>
#include "hspropertyanimationwrapper.h"

/*!
    \class HsMessageBoxWrapper
    \ingroup group_hsutils
    \brief 
*/

struct HsPropertyAnimationWrapperImpl
{
public:
    QPropertyAnimation *mPropertyAnimation;
};

/*!

*/
HsPropertyAnimationWrapper::HsPropertyAnimationWrapper(QObject *parent)
  : QObject(parent),mImpl(new HsPropertyAnimationWrapperImpl)
{
    mImpl->mPropertyAnimation = new QPropertyAnimation(parent);
    connect(mImpl->mPropertyAnimation,SIGNAL(finished()),SIGNAL(finished()));
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
   mImpl->mPropertyAnimation->setTargetObject(target);
}
/*!

*/
void HsPropertyAnimationWrapper::setPropertyName(const QByteArray &propertyName)
{
   mImpl->mPropertyAnimation->setPropertyName(propertyName);
}

bool HsPropertyAnimationWrapper::isRunning()
{
    return (mImpl->mPropertyAnimation->state() == QAbstractAnimation::Running);
}

void HsPropertyAnimationWrapper::setEndValue(const QVariant &value)
{
    mImpl->mPropertyAnimation->setEndValue(value);
}

void HsPropertyAnimationWrapper::setDuration(int msecs)
{
    mImpl->mPropertyAnimation->setDuration(msecs);
}

void HsPropertyAnimationWrapper::setForward()
{
    mImpl->mPropertyAnimation->setDirection(QAbstractAnimation::Forward);
}

void HsPropertyAnimationWrapper::setBackward()
{
    mImpl->mPropertyAnimation->setDirection(QAbstractAnimation::Backward);
}

bool HsPropertyAnimationWrapper::isForward() const
{
    return (mImpl->mPropertyAnimation->direction() == QAbstractAnimation::Forward);
}
/*!

*/
void HsPropertyAnimationWrapper::start()
{
    mImpl->mPropertyAnimation->start();
}
/*!

*/
void HsPropertyAnimationWrapper::stop()
{
    mImpl->mPropertyAnimation->stop();
}

/*!

*/
void HsPropertyAnimationWrapper::setEasingCurve(const QEasingCurve &curve)
{
    mImpl->mPropertyAnimation->setEasingCurve(curve);
}

