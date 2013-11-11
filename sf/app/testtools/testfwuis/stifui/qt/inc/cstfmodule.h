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
* Description: QT C++ based Class.
*              CSTFModule used to describe a test module.
*
*/
#ifndef CSTFMODULE_H
#define CSTFMODULE_H
#include "QString"

class CSTFModule
{
public:
    CSTFModule(){isActive = true;}
    CSTFModule(QString name, QString inifile, QString cfgfile)
    {moduleName = name; iniFile = inifile; configFile = cfgfile; isActive = true;}

public:
    QString& Name(){return moduleName;}
    QString& IniFile(){return iniFile;}
    QString& ConfigFile(){return configFile;}
    bool IsActive(){return isActive;}
    

public:
    void SetName(const QString& name){moduleName = name; }
    void SetIniFile(const QString& inifile){iniFile = inifile;}
    void SetConfigFile(const QString& cfgfile){configFile = cfgfile;}
    void SetActive(const bool active) {isActive = active;}

private:
    QString moduleName;
    QString iniFile;
    QString configFile;
    bool isActive;


};
#endif // CSTFMODULE_H

// End of File
