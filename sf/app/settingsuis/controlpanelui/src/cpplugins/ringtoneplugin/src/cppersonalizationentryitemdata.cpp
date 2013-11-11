/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0""
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
#include "cppersonalizationentryitemdata.h"
#include <cpitemdatahelper.h>
#include <cpprofilemodel.h>
#include <cplogger.h>
#include <qdir.h>
#include <xqsettingsmanager.h>
#include <xqcentralrepositoryutils.h>
#include <xqcentralrepositorysearchcriteria.h>
#include <ProfileEngineInternalCRKeys.h>

#include "cpringtoneview.h"


const QString g_strNoTone("Z:\\resource\\No_Sound.wav");
CpPersonalizationEntryItemData::CpPersonalizationEntryItemData(CpItemDataHelper &itemDataHelper,
                const QString &text,
                const QString &description,
                const QString &icon,
                Profile_Tone_Types toneType,
                int profileId,
                const HbDataFormModelItem *parent)
                :CpSettingFormEntryItemData(CpSettingFormEntryItemData::ListEntryItem, itemDataHelper,text,description,icon,parent),
                mProfileModel(0),
                m_profileID(profileId),
                mToneType(toneType),
                mSettingManager(0)
{
    Q_UNUSED(itemDataHelper);   //reserve for future
    CPFW_LOG("CpPersonalizationEntryItemData::CpPersonalizationEntryItemData(), START");
    mProfileModel = new CpProfileModel();
    if( mProfileModel )
    {
        QString strRing = loadStringValue();
        if( QFileInfo(strRing) == QFileInfo(g_strNoTone) )
        {
            setDescription( hbTrId("txt_cp_list_no_tone" ) );   //sepcial handling about NoTone
        }
        else
        {
		    setDescription( QFileInfo(strRing).baseName() );
        }
    }
    else
    {
        CPFW_LOG("CpPersonalizationEntryItemData::mProfileModel:NULL!");
    }
    mSettingManager = new XQSettingsManager();
    XQCentralRepositorySettingsKey key(KCRUidProfileEngine.iUid,KProEngSilenceMode);
    
    //Monitoring the active ring tone
    XQCentralRepositorySettingsKey keyForActiveRingTone(KCRUidProfileEngine.iUid,KProEngActiveRingTone);
    
    QVariant silenceMode = mSettingManager->readItemValue( key,XQSettingsManager::TypeInt );
    setEnabled( !silenceMode.toInt() );
    mSettingManager->startMonitoring( key,XQSettingsManager::TypeInt );
    mSettingManager->startMonitoring(keyForActiveRingTone, XQSettingsManager::TypeString);
    connect(mSettingManager, SIGNAL(valueChanged (XQSettingsKey, QVariant)),
            this, SLOT(settingValueChanged(XQSettingsKey, QVariant)));

}
void CpPersonalizationEntryItemData::settingValueChanged( const XQSettingsKey& key, const QVariant& value )
{
    switch (key.key()) {
        case KProEngActiveRingTone:
        {
            QString strRing = loadStringValue();
            if( QFileInfo(strRing) == QFileInfo(g_strNoTone) )
            {
                setDescription( hbTrId("txt_cp_list_no_tone" ) );   //sepcial handling about NoTone
            }
            else
            {
                setDescription( QFileInfo(strRing).baseName() );
            }
            break;
        }
        case KProEngSilenceMode:
        {
            setEnabled( !value.toInt() );
            break;
            
        }
        default:
            break;
    }    
}

CpPersonalizationEntryItemData::~CpPersonalizationEntryItemData()
{
    if( mProfileModel )
    {
        delete mProfileModel;
        mProfileModel = 0;
    }
    if( mSettingManager )
    {
        delete mSettingManager;
    }
}

QString CpPersonalizationEntryItemData::loadStringValue() const
{
    QString strRing;
    switch( mToneType )
    {
    case TONE_Message:
        if( m_profileID>=0 )
        {
            strRing = mProfileModel->messageTone( m_profileID );
        }
        break;
    case TONE_Email:
        if( m_profileID >=0 )
        {
            strRing = mProfileModel->emailTone( m_profileID );
        }
        break;
    case TONE_Reminder:
        if( m_profileID >=0 )
        {
            strRing = mProfileModel->reminderTone( m_profileID );
        }
        break;
    case TONE_Ring:
    default:
        if( m_profileID <0 )
        {
            
            strRing = mProfileModel->ringTone();
        }
        else
        {
            strRing = mProfileModel->ringTone( m_profileID );
        }
        break;
    }

    return strRing;
}
void CpPersonalizationEntryItemData::storeStringValue( const QString &strValue ) const
{
    QString strInput = strValue;
    if( strInput.length() == 0 )
    {
        strInput = g_strNoTone;
    }
    switch( mToneType )
    {
    case TONE_Message:
        if( m_profileID >=0 )
        {
            mProfileModel->setMessageTone( m_profileID, strInput );
        }
        break;
    case TONE_Email:
        if( m_profileID >=0 )
        {
            mProfileModel->setEmailTone( m_profileID, strInput );
        }
        break;
    case TONE_Reminder:
        if( m_profileID >=0 )
        {
            mProfileModel->setReminderTone( m_profileID, strInput );
        }
        break;
    case TONE_Ring:
    default:
        if( m_profileID <0 )
        {
            mProfileModel->setRingTone( strInput );
        }
        else
        {
            mProfileModel->setRingTone( m_profileID, strInput );
        }
        break;
    }
}

void CpPersonalizationEntryItemData::handleOk(const QString &strFname)
{
    if(strFname.length())
    {   
        //lower level services(tone fetcher or music fetcher)
        //will guarantee strFname is a valid absolute file path.
        setDescription(QFileInfo(strFname).baseName());
    }
    else
    {
        setDescription( hbTrId("txt_cp_list_no_tone" ) );
    }
    storeStringValue(strFname);
}


void CpPersonalizationEntryItemData::handleError(int errorCode, const QString& errorMessage)
{
    Q_UNUSED(errorCode);
    Q_UNUSED(errorMessage);
}

CpBaseSettingView *CpPersonalizationEntryItemData::createSettingView() const
{
    CpRingToneView *pView = new CpRingToneView( );
    connect( pView, SIGNAL( selOK( const QString&)),SLOT(handleOk(const QString &)) );
    connect( pView, SIGNAL( selError( int, const QString& ) ), SLOT( handleOk(const QString &)) );
    return pView;
}
