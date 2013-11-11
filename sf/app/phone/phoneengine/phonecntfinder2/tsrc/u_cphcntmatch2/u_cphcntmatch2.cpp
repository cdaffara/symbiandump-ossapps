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

#include <QtGui>
#include <QtTest/QtTest>
#include <QDebug>
#include <QMap>
#include "qcontactdetailfielddefinition.h"
#include <e32cmn.h> //KNullDesC
#include "qcontact.h"
#include "qcontactdetail.h"
#include "qcontactmanager.h"
#include "qcontactringtone.h"
#include "qcontactavatar.h"
#include "qcontactname.h"
#include "qcontactphonenumber.h"
#include "cphcntmatch2.h"
#include "cphcntcontactidimpl2.h"


//---- test class

class U_CPhCntMatch2: public QObject
{
Q_OBJECT

public:
    U_CPhCntMatch2();
    ~U_CPhCntMatch2();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testFirstNameEmpty();
    void testFirstNameNotEmpty();
    void testLastNameEmpty();
    void testLastNameNotEmpty();
    void testGroupNameEmpty();
    void testGroupNameNotEmpty();
    void testNumberEmpty();
    void testNumberNotEmpty();
    //void testSeveralFirstNames();
    //void testEmptyList();
    void testPhoneNumberOverride();
    void testPhoneNumberNotSet();
    void testRelease();
    void testCli();
    void testContactId();
    void testNumberType();
    void testCompanyName();
    void testPersonalRingingTone();
    void testBelongsToGroups();
    void testTextToSpeechText();
    void testCallText();
    void testCallImage();
    void testHasThumbnailImage();
    void testAllDtmfNumbers();
    
private:
    
    CPhCntMatch2* mMatch; // SUT 
    MPhCntMatch* mMatchIf;
    QContactManager* cm;
    QContactDetail mDetail;
  
};

//--- testing

U_CPhCntMatch2::U_CPhCntMatch2()
    {
    }

U_CPhCntMatch2::~U_CPhCntMatch2()
    {

    }

void U_CPhCntMatch2::initTestCase()
    {
    
    cm = new QContactManager();
    QContact contact;
    QList<QContactLocalId> contactIds = cm->contactIds();
    if (!contactIds.isEmpty()) {
        // removes all contact 
       /*     for (int i = 0; i < contactIds.count(); i++) 
                {
                QContact a = cm->contact(contactIds.at(i));
                cm->removeContact(a.localId());
                }*/
        
        contact = cm->contact(contactIds.first());
        qDebug() << "This manager contains" << contact.displayLabel();
    } else {
        qDebug() << "This manager contains no contacts";
    }

    QList<QContactDetail> allDetails = contact.details();
    for (int i = 0; i < allDetails.size(); i++) {
        QContactDetail detail = allDetails.at(i);
        QContactDetailDefinition currentDefinition = cm->detailDefinition(detail.definitionName());
        QMap<QString, QContactDetailFieldDefinition> fields = currentDefinition.fields();

        qDebug("\tDetail #%d (%s):", i, detail.definitionName().toAscii().constData());
        foreach (const QString& fieldKey, fields.keys()) {
            qDebug() << "\t\t" << fieldKey << "(" << fields.value(fieldKey).dataType() << ") =" << detail.value(fieldKey);
        }
        qDebug();
    }
       
    }

void U_CPhCntMatch2::cleanupTestCase()
    {
    delete mMatch;
    delete cm;
    }

void U_CPhCntMatch2::testFirstNameEmpty()
    {
    QContact contact;
    QContactName name;
    name.setFirstName("");
    bool err = contact.saveDetail(&name); 
    err = cm->saveContact(&contact);
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(contact) );
    mMatchIf = mMatch;
    Q_ASSERT(mMatchIf->FirstName().Length() == 0);

    err = contact.removeDetail(&name);
    err = cm->removeContact(contact.localId());

    qDebug() << "U_CPhCntMatch2 : The first phone number of" << contact.displayLabel()
                     << "is" << contact.detail(QContactPhoneNumber::DefinitionName).value(QContactPhoneNumber::FieldNumber);
    
    qDebug()<<"U_CPhCntMatch2 : contact.details().count() " << contact.details().count();
    
    }

void U_CPhCntMatch2::testFirstNameNotEmpty()
    {
    QContact contact;
    _LIT(KFirstName, "firstname");
    QContactName name;
    name.setFirstName("firstname");
    bool err = contact.saveDetail(&name); 
    err = cm->saveContact(&contact);    

    qDebug()<<"U_CPhCntMatch2 : contact.details().count() " << contact.details().count();
    
    delete mMatch;
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(contact) );
    mMatchIf = mMatch;
    qDebug()<<"U_CPhCntMatch2 : contact.details().count() " << contact.details().count();
    qDebug()<<"U_CPhCntMatch2 : contact.details().at(0).value() " << contact.details().at(0).value(QContactName::FieldFirstName);
    HBufC* buf = NULL;
    buf = mMatchIf->FirstName().AllocL();
    QString qBuf((QChar*)buf->Ptr(),buf->Length());
    qDebug()<<"mMatchIf->FirstName() returned...: " << qBuf;
    Q_ASSERT( *buf == KFirstName()); 
    delete buf;
    
    err = contact.removeDetail(&name);
    err = cm->removeContact(contact.localId());
    }

void U_CPhCntMatch2::testLastNameEmpty()
    {
    QContact contact;
    QContactName lastname;
    lastname.setLastName("");
    bool err = contact.saveDetail(&lastname); 
    err = cm->saveContact(&contact);
    delete mMatch;
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(contact) );
    mMatchIf = mMatch;
    Q_ASSERT(mMatchIf->LastName().Length() == 0);

    err = contact.removeDetail(&lastname);
    err = cm->removeContact(contact.localId());
    }


void U_CPhCntMatch2::testLastNameNotEmpty()
    {
    QContact contact;
    _LIT(KLastName, "lastname");
    QContactName lastname;
    lastname.setLastName("lastname");
    bool err = contact.saveDetail(&lastname); 
    err = cm->saveContact(&contact);
    delete mMatch;
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(contact) );
    mMatchIf = mMatch;
    HBufC* buf = NULL;
    buf = mMatchIf->LastName().AllocL();
    QString qBuf((QChar*)buf->Ptr(),buf->Length());
    qDebug()<<"mMatchIf->LastName() returned...: " << qBuf;
    Q_ASSERT(*buf == KLastName());
    delete buf;

    err = contact.removeDetail(&lastname);
    err = cm->removeContact(contact.localId());
    }

void U_CPhCntMatch2::testGroupNameEmpty()
    {
    QContact contact;
    QContactName lastname;
    lastname.setLastName("");
    bool err = contact.saveDetail(&lastname); 
    err = cm->saveContact(&contact);
    delete mMatch;
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(contact) );
    mMatchIf = mMatch;
    Q_ASSERT(mMatchIf->GroupName().Length() == 0);

    err = contact.removeDetail(&lastname);
    err = cm->removeContact(contact.localId());
    }


void U_CPhCntMatch2::testGroupNameNotEmpty()
    {
    QContact contact;
    contact.setType(QContactType::TypeGroup);
    _LIT(KGroupName, "groupname");
    QContactName groupname;
    groupname.setCustomLabel("groupname");
    bool err = contact.saveDetail(&groupname); 
    err = cm->saveContact(&contact);
    delete mMatch;
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(contact) );
    mMatchIf = mMatch;
    HBufC* buf = NULL;
    buf = mMatchIf->GroupName().AllocL();
    QString qBuf((QChar*)buf->Ptr(),buf->Length());
    Q_ASSERT(*buf == KGroupName());
    delete buf;

    err = contact.removeDetail(&groupname);
    err = cm->removeContact(contact.localId());
    }

void U_CPhCntMatch2::testNumberEmpty()
    {
    QContact contact;
    QContactPhoneNumber number;
    number.setNumber("");
    bool err = contact.saveDetail(&number); 
    err = cm->saveContact(&contact);
    delete mMatch;
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(contact) );
    mMatchIf = mMatch;
    Q_ASSERT(mMatchIf->Number().Length() == 0);

    err = contact.removeDetail(&number);
    err = cm->removeContact(contact.localId());
    }
    
void U_CPhCntMatch2::testNumberNotEmpty()
    {
    QContact contact;
    _LIT(KNumber, "0501234567");
    QContactPhoneNumber number;
    number.setNumber("0501234567");
    bool err = contact.saveDetail(&number); 
    err = cm->saveContact(&contact);
   
    delete mMatch;
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(contact) );
    mMatchIf = mMatch;
    HBufC* buf = NULL;
    buf = mMatchIf->Number().AllocL();
    QString qBuf((QChar*)buf->Ptr(),buf->Length());
    Q_ASSERT(*buf == KNumber());
    delete buf;

    err = contact.removeDetail(&number);
    err = cm->removeContact(contact.localId());
    }
/*
void U_CPhCntMatch2::testSeveralFirstNames() //in case pb returns bad variants
    {
    mDetail.setValue("Abba");
    mList.clear();
    mList << mDetail;
    mList << mDetail;
    mList << mDetail;
    mContact.setDetails(mList);
    delete mMatch;
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(mContact) );
    mMatchIf = mMatch;
    Q_ASSERT(mMatchIf->FirstName().Length() == 0);
    }
    
void U_CPhCntMatch2::testEmptyList()
    {
    QList<QContactDetail> list;
    mContact.setDetails(list);
    delete mMatch;
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(mContact) );
    mMatchIf = mMatch;
    Q_ASSERT(mMatchIf->FirstName().Length() == 0);
    }
*/
void U_CPhCntMatch2::testPhoneNumberOverride()
    {
    QContact contact;
    _LIT(KNumber, "0501234567");
    _LIT(KNumberOverride, "888888888");
    
    QContactPhoneNumber number;
    number.setNumber("0501234567");
    bool err = contact.saveDetail(&number); 
    err = cm->saveContact(&contact);

    delete mMatch;
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(contact) );
    mMatch->SetNumberL(KNumberOverride());
    mMatchIf = mMatch;
    HBufC* buf = NULL;
    buf = mMatchIf->Number().AllocL();
    QString qBuf((QChar*)buf->Ptr(),buf->Length());
    Q_ASSERT(*buf == KNumberOverride());
    delete buf;

    err = contact.removeDetail(&number);
    err = cm->removeContact(contact.localId());
    }


void U_CPhCntMatch2::testPhoneNumberNotSet()
    {
    QContact contact;
    _LIT(KNumber, "0501234567");
    
    QContactPhoneNumber number;
    number.setNumber("0501234567");
    bool err = contact.saveDetail(&number); 
    err = cm->saveContact(&contact);

    delete mMatch;
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(contact) );
    mMatchIf = mMatch;
    HBufC* buf = NULL;
    buf = mMatchIf->Number().AllocL();
    QString qBuf((QChar*)buf->Ptr(),buf->Length());
    Q_ASSERT(*buf == KNumber());
    delete buf;

    err = contact.removeDetail(&number);
    err = cm->removeContact(contact.localId());
    }

void U_CPhCntMatch2::testRelease()
    {
    QContact contact;
    _LIT(KNumber, "1234");
    
    QContactPhoneNumber number;
    number.setNumber("1234");
    bool err = contact.saveDetail(&number); 
    err = cm->saveContact(&contact);
    
    delete mMatch;
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(contact) );
    mMatchIf = mMatch;
    mMatchIf->Release();
    Q_ASSERT(mMatchIf->Number() == KNumber());

    
    err = contact.removeDetail(&number);
    err = cm->removeContact(contact.localId());
    }

void U_CPhCntMatch2::testCli()
    {
    HBufC* buf = NULL;
    Q_ASSERT(mMatchIf->Cli(buf) == MPhCntMatch::ECliNumber );
    }

void U_CPhCntMatch2::testContactId()
    {
    QContact contact;
    QContactPhoneNumber number;
    number.setNumber("1234");
    bool err = contact.saveDetail(&number); 
    err = cm->saveContact(&contact);   
    
    delete mMatch;
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(contact) );
    mMatchIf = mMatch;
    
    CPhCntContactId* id = NULL;
    id = mMatchIf->ContactId();
    Q_ASSERT(id->ContactId()== contact.localId());
    
    err = contact.removeDetail(&number);
    err = cm->removeContact(contact.localId());
    
    }

void U_CPhCntMatch2::testNumberType()
    {
    Q_ASSERT(mMatchIf->NumberType() == MPhCntMatch::ENone);
    }

void U_CPhCntMatch2::testCompanyName()
    {
    Q_ASSERT(mMatchIf->CompanyName().Length() == 0);
    }

void U_CPhCntMatch2::testPersonalRingingTone()
    {
    _LIT(KRingtone, "Z:\\Data\\Sounds\\Digital\\Incoming.aac");
    QContact contact;
    QUrl name("Z:\\Data\\Sounds\\Digital\\Incoming.aac");
    QContactRingtone ringtone;
    ringtone.setAudioRingtoneUrl(name);
    bool err = contact.saveDetail( &ringtone );
    err = cm->saveContact(&contact);  
    
    delete mMatch;
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(contact) );
    mMatchIf = mMatch;
    
    HBufC* buf = NULL;
    buf = mMatchIf->PersonalRingingTone().AllocL();
    QString qBuf((QChar*)buf->Ptr(),buf->Length());
    
   
    Q_ASSERT(mMatchIf->PersonalRingingTone() == KRingtone());
    
    err = contact.removeDetail(&ringtone);
    err = cm->removeContact(contact.localId());
      
    }

void U_CPhCntMatch2::testBelongsToGroups()
    {
    CArrayFix<TContactItemId>* groups = NULL;
    Q_ASSERT(mMatchIf->BelongsToGroups(*groups) == false);
    }

void U_CPhCntMatch2::testTextToSpeechText()
    {
    Q_ASSERT(mMatchIf->TextToSpeechTextL() == NULL);
    }

void U_CPhCntMatch2::testCallText()
    {
    Q_ASSERT(mMatchIf->CallText().Length() == 0);
    }

void U_CPhCntMatch2::testCallImage()
    {
    _LIT(KCallImage, "Z:\\Data\\Images\\Avatar.jpg");
    QContact contact;
    QUrl image("Z:\\Data\\Images\\Avatar.jpg");
    QContactAvatar avatar;
    avatar.setImageUrl(image);
    bool err = contact.saveDetail( &avatar );
    err = cm->saveContact(&contact);  
    
    delete mMatch;
    TRAP_IGNORE( mMatch = CPhCntMatch2::NewL(contact) );
    mMatchIf = mMatch;
    
    HBufC* buf = NULL;
    buf = mMatchIf->PersonalRingingTone().AllocL();
    QString qBuf((QChar*)buf->Ptr(),buf->Length());
    
    Q_ASSERT(mMatchIf->CallImage() == KCallImage());
    
    err = contact.removeDetail(&avatar);
    err = cm->removeContact(contact.localId());
    }

void U_CPhCntMatch2::testHasThumbnailImage()
    {
    Q_ASSERT(mMatchIf->HasThumbnailImage()== false);
    }

void U_CPhCntMatch2::testAllDtmfNumbers()
    {
    //dummy array for now
    Q_ASSERT(mMatchIf->AllDtmfNumbers().Count() == 0);
    }


QTEST_MAIN(U_CPhCntMatch2)
#include "u_cphcntmatch2.moc"
