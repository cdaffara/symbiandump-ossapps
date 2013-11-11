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
 * 
 * Description:
 *     The header file of the query dialog utility of file manager
 */

#ifndef FMDLGUTILS_H
#define FMDLGUTILS_H

#include "fmcommon.h"

#include <QString>
#include <QStringList>
#include <QTime>

#include <hbglobal.h>
#include <hbmessagebox.h>

class FmDialog;
class HbAction;

/*!
    \class FmDlgUtils
    \brief The class FmDlgUtils is static utility class. Used to popup dialogs
 */
class FmDlgUtils : public QObject
{
    Q_OBJECT
public:
    static bool showSingleSettingQuery(
        const QString &title,
        const QStringList &textList,
        int &selectedIndex,
		const QString &associatedDrives = QString() );

	static bool showMultiSettingQuery(
        const QString &title,
        const QStringList &textList,
        quint32 &selection,
        int dominantIndex,
		const QString &associatedDrives = QString() );

    static bool showTimeSettingQuery(
        const QString &title, QTime &time,
        const QString &associatedDrives = QString() );

    static bool showTextQuery(     
        const QString &title, QString &text, QStringList validRegExpStringList = QStringList( Regex_ValidUnEmpty ),
	    int maxLength = -1, const QString &associatedDrives = QString(), bool isReturnFalseWhenNoTextChanged = true );

    static bool showSinglePasswordQuery(     
        const QString &title, QString &pwd, int maxLength = -1,
	    const QString &associatedDrives = QString() );

    static bool showMultiPasswordQuery(     
        const QString &firstLabel, const QString &secondLabel, QString &pwd, int maxLength = -1,
	    const QString &associatedDrives = QString() );
    
    static bool question( const QString &questionText,
        HbMessageBox::StandardButtons buttons = HbMessageBox::Yes | HbMessageBox::No );

    static void information( const QString &informationText,
        HbMessageBox::StandardButtons buttons = HbMessageBox::Ok,  bool isBlocking = false );
    
    static void warning( const QString &warningText,
        HbMessageBox::StandardButtons buttons = HbMessageBox::Ok, bool isBlocking = false );

    static QString showBackupDriveQuery( const QString& title );

    ~FmDlgUtils();
private:
    FmDlgUtils();
	static HbAction *executeDialog( FmDialog *dialog, const QString &associatedDrives );
};


#endif
