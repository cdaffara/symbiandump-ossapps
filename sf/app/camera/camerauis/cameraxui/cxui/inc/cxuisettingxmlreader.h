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
#ifndef CXUISETTINGXMLREADER_H
#define CXUISETTINGXMLREADER_H


#include <QMap>
#include <QString>
#include <QStringList>

#include "cxuisettingsinfo.h"

class QDomElement;
class QDomNode;
class CxuiSettingRadioButtonList;
class CxeEngine;


/*!
* Class that handles reading of settings data from xml parsing it
*/
class CxuiSettingXmlReader
{

public:

    CxuiSettingXmlReader();
    ~CxuiSettingXmlReader();

    CxUiSettings::RadioButtonListParams getListSettingInfo(const QString &setting, bool &found);
    CxUiSettings::SliderParams getSliderSettingInfo(const QString &setting, bool &found);

public slots:

    void setXmlSource(const QString& uri);

private:

    void clear();
    void parse();
    void parseSettings(const QDomElement &e);
    CxUiSettings::RadioButtonListParams *parseSettingList(const QDomElement &e);
    CxUiSettings::SliderParams *parseSettingSlider(const QDomElement &e);
    QDomElement findChildElement(const QDomNode &parent, const QString &name);

private:
    QString mXmlUri;
    bool mParsed;
    QMap<QString, CxUiSettings::RadioButtonListParams*> mListParams;
    QMap<QString, CxUiSettings::SliderParams*> mSliderParams;
};

#endif // CXUISETTINGXMLREADER_H
