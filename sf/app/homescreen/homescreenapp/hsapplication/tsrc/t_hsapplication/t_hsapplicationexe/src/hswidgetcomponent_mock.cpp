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

#include "hswidgetcomponent.h"

/*!

*/
HsWidgetComponent::~HsWidgetComponent()
{
}

/*!

*/
QString HsWidgetComponent::rootPath() const
{
    return mRootPath;
}

/*!

*/
QString HsWidgetComponent::uri() const
{
    return mUri;
}


bool HsWidgetComponent::isAvailable() const
{
    return  true;
}
/*!

*/
HsWidgetComponent::HsWidgetComponent(const QString &uri, QObject *parent)
  : QObject(parent),
    mUri(uri),
    mState(Available)
{
}


/*!

*/
void HsWidgetComponent::resolveRootPathAndTranslationFilename()
{
}

/*!

*/
void HsWidgetComponent::installTranslator()
{
}

/*!

*/
void HsWidgetComponent::uninstallTranslator()
{
}

void HsWidgetComponent::emitAboutToUninstall()
{
    if (mState == Available) {
        mState = Uninstalling;
        emit aboutToUninstall();
    }
}
void HsWidgetComponent::emitUninstalled()
{
    if (mState == Uninstalling) {
        mState = Available;
        emit uninstalled();
    }
}
void HsWidgetComponent::emitUpdated()
{
    if (mState == Uninstalling) {
        mState = Available;
        emit updated();
    }
}
void HsWidgetComponent::emitUnavailable()
{
    if (mState == Available) {
        mState = Unavailable;
        emit unavailable();
    }
}
void HsWidgetComponent::emitAvailable()
{
    if (mState == Unavailable) {
        mState = Available;
        emit available();
    }
}
