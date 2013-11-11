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
 *
 */

#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "uisetting.h"
const QString SETTINGFILE = "c:\\TestFramework\\StfQtUISetting.ini";

UiSetting::UiSetting()
    {
    if(!load())
        {
        loadDefault();
        }
    }

UiSetting::~UiSetting()
    {
    }

QString UiSetting::ReadSetting(const QString& item)
    {
    QString value = "";
    if(settingList.contains(item))
        {
        value = settingList.value(item);
        }
    else
        {
        value = getDefaultValue(item);
        settingList.insert(item, value);
        }
    return value;
    }


void UiSetting::SetSetting(const QString& item, const QString& value)
    {
    if(settingList.contains(item))
        {
        settingList.remove(item);
        }
    settingList.insert(item, value);
    save();
    }


void UiSetting::loadDefault()
    {
    settingList.clear();
    settingList.insert(KShowOutput, getDefaultValue(KShowOutput));
    settingList.insert(KStyleSheet, getDefaultValue(KStyleSheet));
    settingList.insert(KFilter, getDefaultValue(KFilter));
    settingList.insert(KFilterCaseSens, getDefaultValue(KFilterCaseSens));
    //add mor default setting here.
    }

QString UiSetting::getDefaultValue(const QString& item)
    {
    QString result = "";
    if(item == KShowOutput)
        {
        result = "true";
        }
    else if(item == KStyleSheet)
        {
        result = ":/qss/coffee.qss";
        }
    else if(item == KFilter)
        {
        result = "";
        }
    else if(item == KFilterCaseSens)
        {
        result = "false";
        }
    return result;
    }

bool UiSetting::load()
    {
    QFile file(SETTINGFILE);
    if(!file.open(QIODevice::ReadOnly))
        {
        return false;
        }
    QTextStream in(&file);
    QString line, item, value;
    int index;
    while(!in.atEnd())
        {
        line = in.readLine().trimmed();
        if(!line.startsWith("//"))
            {
            index = line.indexOf("=");
            if(index > 0 && index < line.length() - 1)
                {
                item = line.left(index).trimmed();
                value = line.right(line.length() - index  -1);
                if(item == KFilter) //For filter do not care about stored value
                    value = "";
                settingList.insert(item, value);
                }
            }
        //end while.
        }
    return true;
    }

bool UiSetting::save()
    {
    QFile file(SETTINGFILE);
    if(!file.open(QIODevice::WriteOnly))
        {
        return false;
        }
    QTextStream in(&file);
    in << "//STFQtUI Setting.\r\n";
    in << "//Created at: " + QDateTime::currentDateTime().toString("yyyy.mm.dd hh:mm::ss");
    in << "\r\n";
    for(int i=0;i< settingList.size();i++)
        {
        in << settingList.keys()[i];
        in << "=";
        in << settingList.value(settingList.keys()[i]);
        in << "\r\n";
        }
    return true;    
    }

// End of File
