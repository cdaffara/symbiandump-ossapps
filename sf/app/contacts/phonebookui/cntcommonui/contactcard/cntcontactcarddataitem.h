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

#ifndef CNTCONTACTCARDDATAITEM_H
#define CNTCONTACTCARDDATAITEM_H

#include <QObject>
#include <qmobilityglobal.h>
#include <qtcontacts.h>
#include <hbicon.h>

class CntUiContactCardExtension;

QTM_USE_NAMESPACE

class CntContactCardDataItem: public QObject
{
    Q_OBJECT    

public:
    
enum CntContactCardPosition
{ 

// Communication methods >>>>>
        EInternalServices = 0,
        ECallMobile = 100,
        ECallMobileSendMessage,
        ECallMobileHome,
        ECallMobileHomeSendMessage,
        ECallMobileWork,
        ECallMobileWorkSendMessage,
        ECallPhone,
        ECallPhoneHome,
        ECallPhoneWork,
        ECallFax,
        ECallFaxHome,
        ECallFaxWork,
        ECallPager,
        ECallAssistant,
        ECallCar,
        EEmail,
        EEmailHome,
        EEmailWork,
        EUrl,
        EUrlHome,
        EUrlWork,
        ECallDynamic,
        ECallDynamicHome,
        ECallDynamicWork,
        EEmailDynamic,
        EEmailDynamicHome,
        EEmailDynamicWork,
        EUrlDynamic,
        EUrlDynamicHome,
        EUrlDynamicWork,
        EInternetDynamic,
        EInternetDynamicHome,
        EInternetDynamicWork,
        EAddressDynamic,
        EAddressDynamicHome,
        EAddressDynamicWork,
        EDynamic,
        EGenericDynamic,
        ELastAction = 200,
        ELastActionHome,
        ELastActionWork,
        EExternalServices = 250,
        ESeparator = 300,
// <<<<< Communication methods end here.
        
        EInternetTelephone,
        EInternetTelephoneHome,
        EInternetTelephoneWork,
        ESip,
        EAddress,
        EAddressHome,
        EAddressWork,
        ECompanyDetails,
        EAssistantName,
        EBirthday,
        EAnniversary,
        ERingtone,
        ENote,
        ESpouse,
        EChildren,
        EOther = 400,
        ENotSupported = 1000
};    
    
public:
    CntContactCardDataItem(const QString& aTitle, int aPosition, bool aIsFocusable);
    virtual ~CntContactCardDataItem();

public:
    void setTitleText(const QString& aTitle);
    void setValueText(const QString& aValueText, Qt::TextElideMode aValueTextElideMode = Qt::ElideNone);
    void setIcon(HbIcon aIcon);
    void setSecondaryIcon(HbIcon aSecondaryIcon);
    void setAction(const QString& aAction);
    void setContactDetail(QContactDetail aDetail);
    void setActionDescriptor(const QContactActionDescriptor& aActionDescriptor);
    void setLongPressText(const QString& aLongPressText);
    void setContactCardExtensionAndActionIndex( CntUiContactCardExtension& aExtension, int aIndex );

public:
    QString titleText() const { return mTitle; }
    QString valueText() const { return mValueText; }
    HbIcon  icon() const { return mIcon; }
    HbIcon  secondaryIcon() const { return mSecondaryIcon; }
    QString action() const { return mAction; }
    QContactDetail detail() const { return mDetail; }
    Qt::TextElideMode elideMode() const { return mValueTextElideMode; }
    bool isFocusable() const { return mIsFocusable; }
    QContactActionDescriptor actionDescriptor() const { return mActionDescriptor; }
    QString longPressText() const { return mLongPressText; }

    /// Can return NULL which means this item is not from an extension.
    CntUiContactCardExtension* contactCardExtension() const {return mExtension;}
    int contactCardExtensionActionIndex() const {return mExtensionActionIndex;}

    int position() const { return mPosition; }
 
private:
    HbIcon                      mIcon;
    HbIcon                      mSecondaryIcon;
    QString                     mTitle;
    QString                     mValueText;
    QString                     mAction;
    QContactActionDescriptor    mActionDescriptor;
    QContactDetail              mDetail;
    Qt::TextElideMode           mValueTextElideMode;
    bool                        mIsFocusable;
    QString                     mLongPressText;
    CntUiContactCardExtension*  mExtension;
    int                         mExtensionActionIndex;
    
public:
    int                         mPosition;
};

#endif /* CNTCONTACTCARDDATAITEM_H */
