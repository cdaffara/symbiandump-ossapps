/*!
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
#include <xqpluginloader.h>
#include <QtDebug>
#include <QList>
#include <QString>
#include "menuextension_stub.h"

int m_phoneButtonFlags;
int m_pluginUid;
bool m_useExtensionStub;

XQPluginLoader::XQPluginLoader(QObject* parent) :
    QObject(parent)
{
}

XQPluginLoader::XQPluginLoader(int uid, QObject* parent) :
    QObject(parent)
{
    m_pluginUid = uid;	
}

XQPluginLoader::~XQPluginLoader ()
{
}

int XQPluginLoader::uid()const
{
    return m_pluginUid;
}

QObject* XQPluginLoader::instance()
{
    if (m_useExtensionStub) {
        return new SvtMenu();
    }
    return 0;
}

void XQPluginLoader::setUid( int uid )
{
    m_pluginUid = uid;
}
