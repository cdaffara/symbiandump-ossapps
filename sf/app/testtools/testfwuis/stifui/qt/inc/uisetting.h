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
*/

#ifndef UISETTING_H_
#define UISETTING_H_
#include <QHash>

const QString KShowOutput = "showoutput";
const QString KStyleSheet = "StyleSheet";
const QString KFilter = "filter";
const QString KFilterCaseSens = "filtercasesens";

class UiSetting
    {
public:
    UiSetting();
    ~UiSetting();
    
public:
    QString ReadSetting(const QString& item);
    void SetSetting(const QString& item,const QString& value);
    
private:
    bool load();
    bool save();
    void loadDefault();
    QString getDefaultValue(const QString& item);
    
private:
    QHash<QString, QString> settingList;
    };

#endif /* UISETTING_H_ */

// End of File
