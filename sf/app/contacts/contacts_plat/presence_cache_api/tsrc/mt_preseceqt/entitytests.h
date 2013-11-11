/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for presence cache reader and writer.
*
*/

#ifndef __ENTITY_TESTS_H__
#define __ENTITY_TESTS_H__

#include <QObject>
#include <QMap>
#include <prcpresencebuddyinfo_qt.h>


class PrcPresenceReader;
class PrcPresenceWriter;
class EntityTests : public QObject
{
Q_OBJECT
public:
    explicit EntityTests(QObject *parent = 0);

    

private slots:  // Init & cleanup
        void initTestCase();
        void cleanupTestCase();
        void test1();
        void test2();
        void test3();
        void test4();
        void test5();
        void test6();
        void test7();
        void test8();

private:
        void saveBuddy(QMap<QString,QString>& map);
        void fetchAndVerifyBuddy( QMap<QString,QString>& map );
        void  subscribeBuddy( QString &buddyId);
        void  unSubscribeBuddy( QString &buddyId);
        PrcPresenceBuddyInfoQt* createBuddyForWriting(QMap<QString,QString>& map);
        void CheckServices(int nbrServices,int nbrBuddies);
        void CheckOneService(QString service,int buddiesInService);

        PrcPresenceBuddyInfoQt::AvailabilityValues getAvailbility(QString aval);
        void fetchAllBuddiesInService(QString service, int numberofbuddies  );

public slots:
void handlePresenceNotificationInClient(bool succcess, PrcPresenceBuddyInfoQt* aPresenceBuddyInfo);
void handlePresenceReadInClient(bool success, QList<PrcPresenceBuddyInfoQt*> buddyInfoList);
void handlePresencewriteInclient(bool success);

private: //data
        PrcPresenceWriter* iWriter;
        PrcPresenceReader* iReader;
private:

   static QString K_Buddy ;
   static QString K_Atext ;
   static QString K_Avalue ;
   static QString K_Newkey ;
   static QString K_Newvalue ;
   static QString K_Avatar ;
   static  QString K_Statustext ;
   QMap<QString,QString> iMapDataForAsyncTest;
   bool iNotificationReceived ;
   int iNumberOfBuddiesInService;
   QList <QString> iBuddiesInService;
};

#endif // __ENTITY_TESTS_H__
