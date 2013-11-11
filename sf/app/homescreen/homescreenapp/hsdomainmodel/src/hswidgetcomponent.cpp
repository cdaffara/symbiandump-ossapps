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

#include <QCoreApplication>
#include <QDir>

#include "hswidgetcomponent.h"
#include "hsapp_defs.h"
#include "caservice.h"
#include "caquery.h"

/*!
    \class HsWidgetComponent
    \ingroup group_hsdomainmodel
    \brief Widget component represent a widget package.

    Widget component is accessed through a HsWidgetComponentRegistry. All widget instances from same type 
    are refering to one HsWidgetComponent and can listen component state changes
    
*/


/*!
    Destructor
*/
HsWidgetComponent::~HsWidgetComponent()
{
}

/*!
    Widget component installation path
*/
QString HsWidgetComponent::rootPath() const
{
    return mRootPath;
}

/*!
    Widget unique identifier
*/
QString HsWidgetComponent::uri() const
{
    return mUri;
}

/*!
    Returns true if widget is available thus it's root path exists
*/
bool HsWidgetComponent::isAvailable() const
{
    bool ret(true);
    if (mRootPath.isEmpty() || !QDir(mRootPath).exists()) {
        ret = false;
    }
    return  ret;
}
/*!
    \fn HsWidgetComponent::aboutToUninstall()

    This signal is emitted before component is uninstalled. 
    System may delete widget instances and release all resources 
    related to component. Component installation will fail, if installer 
    can't remove and install some file if someone has handle to it.

*/

/*!
    \fn HsWidgetComponent::uninstalled()

    This signal is emitted after component is uninstalled from device.
*/

/*!
    \fn HsWidgetComponent::updated()

    This signal is emitted when component is updated.
*/

/*!
    \fn HsWidgetComponent::unavailable()

    This signal is emitted when component comes unavailable i.e media is removed.
*/

/*!
    \fn HsWidgetComponent::available()

    This signal is emitted when component comes available i.e media is attached.
*/


/*!
    Constructor
*/
HsWidgetComponent::HsWidgetComponent(const QString &uri, QObject *parent)
  : QObject(parent),
    mUri(uri),
    mState(Available)
{
    resolveRootPathAndTranslationFilename();
	installTranslator();
	
}


/*!
    Resolves component's root path and translation file name if any
*/
void HsWidgetComponent::resolveRootPathAndTranslationFilename()
{
	CaQuery query;
    query.setEntryTypeNames(QStringList(Hs::widgetTypeName));
	query.setAttribute(Hs::widgetUriAttributeName, mUri);
	QList< QSharedPointer<CaEntry> > widgetEntries = CaService::instance()->getEntries(query);
	
    if (widgetEntries.isEmpty()) {
        return;
	}
    QSharedPointer<CaEntry> entry = widgetEntries.first();
    mRootPath = entry->attribute(Hs::widgetPathAttributeName);
    if (mRootPath.isEmpty() || !QDir(mRootPath).exists()) {
        mState = Unavailable;
    } else {
        mTranslationFilename = entry->attribute(Hs::translationFilename);
    }
}

/*!
    Search component's translation dile and installs translator to application
*/
void HsWidgetComponent::installTranslator()
{
    QStringList possiblePaths;
#ifdef Q_OS_WIN32
    possiblePaths << "resource/qt/translations";
#else
    
    QDir currentDir(mRootPath);
    if (!currentDir.exists() || mRootPath.isEmpty()) {
        return;
    }
    while (currentDir.cdUp()) {}
    QString drive = currentDir.path();
    drive.append("resource/qt/translations");
    possiblePaths << drive;
    // if it is not in installed,then check z drive
    possiblePaths << "z:/resource/qt/translations";
#endif

    for(int i=0; i<possiblePaths.count(); ++i) {
        QString candidate = QDir::toNativeSeparators(possiblePaths.at(i));   
        if (QDir(candidate).exists()) {
            QString fileName(mTranslationFilename);
            fileName.append("_");
            fileName.append(QLocale::system().name());
            if (mTranslator.load(fileName, candidate)) {
                QCoreApplication::installTranslator(&mTranslator);
                break;
            }
        }
    }        
}

/*!
    Unistall translator from application 
*/
void HsWidgetComponent::uninstallTranslator()
{
    QCoreApplication::removeTranslator(&mTranslator);
}

/*!
    Emit aboutToUninstall signal 
*/
void HsWidgetComponent::emitAboutToUninstall()
{
    if (mState == Available) {
        mState = Uninstalling;
        emit aboutToUninstall();
    }
}
/*!
    Emit uninstalled signal 
*/
void HsWidgetComponent::emitUninstalled()
{
    if (mState == Uninstalling) {
        mState = Available;
        emit uninstalled();
    }
}
/*!
    Emit updated signal 
*/
void HsWidgetComponent::emitUpdated()
{
    if (mState == Uninstalling) {
        mState = Available;
        emit updated();
    }
}
/*!
    Emit unavailable signal 
*/
void HsWidgetComponent::emitUnavailable()
{
    if (mState == Available) {
        mState = Unavailable;
        emit unavailable();
    }
}
/*!
    Emit available signal 
*/
void HsWidgetComponent::emitAvailable()
{
    if (mState == Unavailable) {
        mState = Available;
        emit available();
    }
}
