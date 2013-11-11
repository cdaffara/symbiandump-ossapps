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

#include "CpAboutUtils.h"
#include <sysversioninfo.h>
#include <sysutil.h>

#include <HbTextEdit>
#include <XQConversions>

/*!
    Create the readonly text edit. 
*/
HbTextEdit *CpAboutUtils::createTextEdit()
{
    HbTextEdit *edit = new HbTextEdit();
    edit->setReadOnly(true);
    edit->setCursorVisibility(Hb::TextCursorHidden);  
    edit->clearContextMenuFlag(Hb::ShowTextContextMenuOnSelectionClicked);
    edit->clearContextMenuFlag(Hb::ShowTextContextMenuOnLongPress);
    return edit;
}

/*! 
    Return the localized string by logic string connecting \a prefixString and
    \a index 
*/
QString CpAboutUtils::contentString(const QString &prefixString, int index)
{
    QString contentString;       
    QString localization(prefixString);
    localization.append(QString::number(index));
    contentString.append(QString(hbTrId(localization.toAscii().constData())));   
    contentString.replace(lineBreak, htmlLineBreak);    
    return contentString;
}

/*!
    Returns html link for \a textContent with content text.
*/
QString CpAboutUtils::linkHtmlContent(const QString &textContent)
{
    QString link(htmlLinkStart);
    link.append(textContent);
    link.append(htmlLinkEnd);    
    return link;
}

/*!
     Trim the html address \a string, also replace \n with <br>
 */
QString CpAboutUtils::preprocessText(const QString &string)
{
    QString str(string.trimmed());    
    str.replace(lineBreak, htmlLineBreak);    
    return str;
}

/*!
    Replace html link in \a stringwith anchor. 
 */
QString CpAboutUtils::findAndReplaceWithLink(const QString &string)
{
    //stands for the web address.
    QRegExp regExp("[a-zA-z]+://[^\\s]*");
    
    QString str(string);
    if (string.indexOf(regExp) != KErrNotFound) {
        foreach(const QString &match, regExp.capturedTexts()) {
            QString originalString(match.trimmed());
            if (originalString.endsWith('.')) {
                originalString = originalString.left(originalString.length()-1); 
            }
            QString replaceString("<a href = \"" + originalString + "\">"+ originalString + "</a>");
            str.replace(originalString, replaceString);                
        }
    }    
    return str;
}

/*!
    Returns the phone's model.
*/
QString CpAboutUtils::getPhoneModel()
{
    TBuf<100> phoneName;
    TInt error = SysVersionInfo::GetVersionInfo(SysVersionInfo::EModelVersion, phoneName);
    QString model;
    if (error == KErrNone) {
        if (phoneName.Length() > 0) {            
            model = XQConversions::s60DescToQString(phoneName);
            
        }
    }
    return model;
}

/*!
    Returns the produce release.
*/
QString CpAboutUtils::getProductRelease()
{

    TBuf<sysUtilVersionTextLength> productRelease;
    productRelease.Zero();
    QString release;
    if (SysUtil::GetPRInformation(productRelease) == KErrNone) {
        release = XQConversions::s60DescToQString(productRelease);
    }
    return release;
}

/*!
    Returns the software version.
*/
QString CpAboutUtils::getSoftwareVersion()
{
    TBuf<sysUtilVersionTextLength> swVersion;
    TBuf<sysUtilVersionTextLength> version;
    swVersion.Zero();
    version.Zero();
    QString versionText;
    if (SysUtil::GetSWVersion(version) == KErrNone) {
        TInt len = version.Length();
        TInt pos1 = version.Find(KEol);
        if (pos1 != KErrNotFound && len > pos1) {
            TBuf<sysUtilVersionTextLength> version1;
            version1.Zero();
            swVersion.Append(version.Left(pos1));
            versionText = XQConversions::s60DescToQString(swVersion);
        }
    } 
    return versionText;
}

/*!
     Returns the phone type.
*/
QString CpAboutUtils::getPhoneType()
{
    TBuf<sysUtilVersionTextLength> swVersion;
    TBuf<sysUtilVersionTextLength> swVersionDate;
    TBuf<sysUtilVersionTextLength> typeDesignator;
    TBuf<sysUtilVersionTextLength> version;
    swVersion.Zero();
    version.Zero();
    typeDesignator.Zero();
    swVersionDate.Zero();
    QString type;
    if (SysUtil::GetSWVersion(version) == KErrNone) {
        TInt len = version.Length();
        TInt pos1 = version.Find(KEol);
        if (pos1 != KErrNotFound && len > pos1) {
            TBuf<sysUtilVersionTextLength> version1;
            version1.Zero();
            swVersion.Append(version.Left(pos1));
            version1.Append(version.Right(len - pos1 - 1));
            len = version1.Length();
            pos1 = version1.Find(KEol);
            if (pos1 != KErrNotFound  && len > pos1) {
                swVersionDate.Append(version1.Left(pos1));
                version.Zero();
                version.Append(version1.Right(len - pos1 - 1));
                len= version.Length();
                pos1 = version.Find(KEol);
                if (pos1 != KErrNotFound  && len > pos1 ) {
                    typeDesignator.Append(version.Left(pos1)); 
                    if (typeDesignator.Length() > 0) { 
                        type = XQConversions::s60DescToQString(typeDesignator);
                    }
                }
            }
        }
    } 
    return type;
}
