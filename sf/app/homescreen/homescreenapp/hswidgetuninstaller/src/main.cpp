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
*
*/

#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <apacmdln.h>

#include "hswidgetcomponentparser.h"
#include "hswidgetinstallersender.h"
#include "hswidgetcomponentdescriptor.h"

const char UNINSTALL_WIDGET_FUNCTION_NAME[] = "widgetUninstalled(QVariantHash)";

int main(int argc, char *argv[])
{
	// Initialization
    CApaCommandLine* commandLine = 0;
    TInt err = CApaCommandLine::GetCommandLineFromProcessEnvironment(commandLine);
    QString commandParam((QChar*)commandLine->DocumentName().Ptr(),commandLine->DocumentName().Length());
    qDebug() << "HSWIDGETUNINSTALLER commandParam:" << commandParam;

    QCoreApplication app(argc, argv);
    QStringList args = QCoreApplication::arguments();

    if (commandParam.isEmpty()) {
        return -1;
    }

    HsWidgetComponentParser componentParser(commandParam);
    if ( !componentParser.error() ) {
        HsWidgetInstallerSender installerSender;
        HsWidgetComponentDescriptor componentDescriptor = componentParser.widgetComponentDescriptor();
        QDir componentDir(commandParam);
        componentDir.cdUp();
        QString componentPath = componentDir.path();
        if ( componentDir.exists(componentPath)) {
            componentDescriptor.setInstallationPath(componentPath);
        }
        installerSender.widgetChanged(UNINSTALL_WIDGET_FUNCTION_NAME, componentDescriptor);
    }

    qDebug() << "HSWIDGETUNINSTALLER finished";

    return 0; //app.exec();
}
