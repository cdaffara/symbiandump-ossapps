/*
 * Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:  mailbox branding resource reader
 *
 */

#ifndef MAILBOXINFO_P_H_
#define MAILBOXINFO_P_H_

#include <QObject>
#include <QMap>
class XQSettingsManager;
class XQCentralRepositorySettingsKey;

class EmailMailboxInfoPrivate : public QObject
{
Q_OBJECT

private:
    
    static EmailMailboxInfoPrivate *getInstance();
    static void releaseInstance(EmailMailboxInfoPrivate *&instance);
    
    EmailMailboxInfoPrivate();
    ~EmailMailboxInfoPrivate();
    
    QString name(const QString &identifier);
    QString icon(const QString &identifier);
    
    void processCenRepRecords(const QString &brandingId);
    int getCurrentCountryL() const;
    bool verifyTimeZone() const;
    void verifyMailAccountName(QString &brandingName) const;

    static EmailMailboxInfoPrivate *mSelf;
    static int mReferenceCount;

    XQSettingsManager *mSettingsManager;
    
    /*!
         holds domain name for WLB
     */
    QString mWlbDomainName;
    
    /*!
         temporary store for icon and name branding variables to avoid additional cenrep scan
     */
    QString mTempIcon;
    QString mTempName;
    
    /*!
         Map of already imported branding keys
     */
    QMap<unsigned long int, QString> mBrandingDataMap;

    friend class EmailMailboxInfo;
};

#endif /* MAILBOXINFO_P_H_ */
