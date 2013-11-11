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
 * Description: This static utility class handles all contacts & phbk services related
 * common functions for UI modules.
 *
 */

#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <xqapplicationmanager.h>
#include "msgcontactsutil.h"


//---------------------------------------------------------------
// MsgContactsUtil::copyVCardToTemp
// @see header file
//---------------------------------------------------------------
QString MsgContactsUtil::copyVCardToTemp(const QString& filepath)
{
    QDir tempDir;
    QString tempFilePath(QDir::toNativeSeparators(tempDir.tempPath()));
    tempFilePath.append(QDir::separator());
    QFileInfo fInfo(filepath);
    tempFilePath.append(fInfo.fileName());
    QFile::copy(filepath, tempFilePath);
    return tempFilePath;
}

//---------------------------------------------------------------
// MsgContactsUtil::deleteVCardFromTemp
// @see header file
//---------------------------------------------------------------
void MsgContactsUtil::deleteVCardFromTemp(const QString& filepath)
{
    QFile::remove(filepath);
}

//---------------------------------------------------------------
// MsgContactsUtil::launchVCardViewer
// @see header file
//---------------------------------------------------------------
bool MsgContactsUtil::launchVCardViewer(const QString& filepath)
{
    //TODO: remove copyVcsFile after capabilities fix from Contacts
    QString newfilepath = copyVCardToTemp(filepath);

    QString service("phonebookservices");
    QString interface("com.nokia.symbian.IContactsEdit");
    QString operation("editCreateNewFromVCard(QString)");
    XQApplicationManager appManager;
    XQAiwRequest* request = appManager.create(service, interface, operation, true); //embedded
    if(request)
    {
	    QList<QVariant> args;
        args << newfilepath;
        request->setArguments(args);
        request->setEmbedded(true);
        request->setSynchronous(true);
        QVariant retValue;
        bool res = request->send(retValue);
        delete request;
    }
    else
    {
        return false;
    }
    //TODO: remove deleteVcsFile after capabilities fix from Contacts
    deleteVCardFromTemp(newfilepath);
    return true;
}


// EOF

