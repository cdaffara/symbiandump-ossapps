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

#include "mpxvideo_debug.h"
#include "videosettingsgroup.h"
#include "videosettingsaccesspointentry.h"
#include <cpsettingformitemdata.h>
#include <cpitemdatahelper.h>
#include <hbdataformmodel.h>
#include <hblineedit.h>
#include <hbinputeditorinterface.h>
#include <hbdataformviewitem.h>
#include <hbinputstandardfilters.h>
#include <hbvalidator.h>
#include <hbpushbutton.h>
#include <mpsettingsmodel.h>
#include <MPSettEngPluginImplementationUIDs.hrh>
#include <xqconversions.h>
#include <qapplication.h>
#include <hbapplication.h>

const int PROXY_LOWEST_ACCEPTED_VALUE = 1;
const int UDP_LOWEST_ACCEPTED_VALUE = 1024;
const int PORT_HIGHEST_ACCEPTED_VALUE = 65535;

void gValidatePortValues(int& value, int bottom, int top)
{
    if(value < bottom)
    {
        value = bottom;
    }

    if(value > top)
    {
        value = top;
    }
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoSettingsGroup::VideoSettingsGroup(CpItemDataHelper &itemDataHelper) :
    CpSettingFormItemData(HbDataFormModelItem::GroupItem, QString()),
    mItemDataHelper(itemDataHelper),
    mSettingsModel(0),
    mAccessPointItem(0),
    mLowestUDPPortItem(0),
    mHighestUDPPortItem(0),
    mUseProxyItem(0),
    mProxyServerItem(0),
    mProxyPortItem(0)
{
    MPX_ENTER_EXIT(_L("VideoSettingsGroup::VideoSettingsGroup()"));

    // Load the translation file.
    QString lang = QLocale::system().name();
    bool loaded(false);

    loaded = mTranslator.load( "videos_" + lang, QString("c:/resource/qt/translations") );

    if (!loaded)
    {
        mTranslator.load("videos_" + lang, QString("z:/resource/qt/translations") );
    }

    // Install the translator
    HbApplication* app = qobject_cast<HbApplication*>(qApp);
    if(app)
    {
        app->installTranslator(&mTranslator);
    }

    this->setLabel(hbTrId("txt_videos_subhead_video_streaming_settings"));

    TRAPD(err, InitSettingsModelL());
    if(err != KErrNone)
    {
        delete mSettingsModel;
        mSettingsModel = 0;
        return;
    }

    itemDataHelper.connectToForm(SIGNAL(itemShown(const QModelIndex)), this, SLOT(itemShown(const QModelIndex)));

    // Access point in use
    mAccessPointItem = new VideoSettingsAccessPointEntry(itemDataHelper,
        hbTrId("txt_videos_dblist_access_point"), this);

    // Lowest UDP port
    mLowestUDPPortItem = new CpSettingFormItemData(HbDataFormModelItem::TextItem,
        hbTrId("txt_videos_dblist_min_udp_port"), this);
    mLowestUDPPortItem->setContentWidgetData(QString("maxRows"), 1);
    mItemDataHelper.addConnection(mLowestUDPPortItem, SIGNAL(editingFinished()),
        this, SLOT(lowestUdpPortEditingFinished()));

    // Highest UDP port
    mHighestUDPPortItem = new CpSettingFormItemData(HbDataFormModelItem::TextItem,
        hbTrId("txt_videos_dblist_max_udp_port"), this);
    mHighestUDPPortItem->setContentWidgetData(QString("maxRows"), 1);
    mItemDataHelper.addConnection(mHighestUDPPortItem, SIGNAL(editingFinished()),
        this, SLOT(highestUdpPortEditingFinished()));

    // Use proxy
    mUseProxyItem = new CpSettingFormItemData(HbDataFormModelItem::CheckBoxItem, QString(), this);
    mUseProxyItem->setContentWidgetData(QString("text"), hbTrId("txt_videos_formlabel_proxy_in_use"));

    // Proxy server address
    mProxyServerItem = new CpSettingFormItemData(HbDataFormModelItem::TextItem,
        hbTrId("txt_videos_dblist_proxy_host_name"), this);
    mProxyServerItem->setContentWidgetData(QString("maxRows"), 1);
    mProxyServerItem->setContentWidgetData(QString("placeholderText"), hbTrId("txt_videos_dblist_none"));
    mItemDataHelper.addConnection(mProxyServerItem, SIGNAL(editingFinished()),
        this, SLOT(proxyServerEditingFinished()));

    // Proxy port number
    mProxyPortItem = new CpSettingFormItemData(HbDataFormModelItem::TextItem,
        hbTrId("txt_videos_dblist_proxy_port"), this);
    mProxyPortItem->setContentWidgetData(QString("maxRows"), 1);
    mItemDataHelper.addConnection(mProxyPortItem, SIGNAL(editingFinished()),
        this, SLOT(proxyPortEditingFinished()));

    loadSettings();
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoSettingsGroup::~VideoSettingsGroup()
{
    MPX_ENTER_EXIT(_L("VideoSettingsGroup::~VideoSettingsGroup()"));

    TRAP_IGNORE( mSettingsModel->StoreSettingsL() );

    delete mSettingsModel;
}

// ---------------------------------------------------------------------------
// getAccessPointId
// ---------------------------------------------------------------------------
//
uint VideoSettingsGroup::getAccessPointId()
{
    MPX_ENTER_EXIT(_L("VideoSettingsGroup::getAccessPointId()"));

    TUint32 defaultAp(0);
    mSettingsModel->GetDefaultAp(defaultAp);

    MPX_DEBUG(_L("Returned access point id = %i"), defaultAp);

    return defaultAp;
}

// ---------------------------------------------------------------------------
// setAccessPointId
// ---------------------------------------------------------------------------
//
void VideoSettingsGroup::setAccessPointId(const uint& defaultAp)
{
    MPX_ENTER_EXIT(_L("VideoSettingsGroup::setAccessPointId()"),
        _L("defaultAp = %i"), defaultAp);

    uint oldAp = getAccessPointId();

    if(oldAp != defaultAp)
    {
        mSettingsModel->SetDefaultAp(defaultAp);
        mUseProxyItem->setContentWidgetData(QString("checked"), false);
    }
}

// ---------------------------------------------------------------------------
// lowestUdpPortValueChanged
// ---------------------------------------------------------------------------
//
void VideoSettingsGroup::lowestUdpPortEditingFinished()
{
    MPX_ENTER_EXIT(_L("VideoSettingsGroup::lowestUdpPortValueChanged()"));

    int minPort = mLowestUDPPortItem->contentWidgetData(QString("text")).toString().toInt();
    int maxPort = mHighestUDPPortItem->contentWidgetData(QString("text")).toString().toInt();

    MPX_DEBUG(_L("minudp = %i, maxudp = %i"), minPort, maxPort);

    if(minPort > maxPort)
    {
        minPort = maxPort;
    }

    gValidatePortValues(minPort, UDP_LOWEST_ACCEPTED_VALUE, PORT_HIGHEST_ACCEPTED_VALUE);

    mLowestUDPPortItem->setContentWidgetData(QString("text"), QString::number(minPort));

    mSettingsModel->SetMinUDPPort(minPort);
}

// ---------------------------------------------------------------------------
// highestUdpPortValueChanged
// ---------------------------------------------------------------------------
//
void VideoSettingsGroup::highestUdpPortEditingFinished()
{
    MPX_ENTER_EXIT(_L("VideoSettingsGroup::highestUdpPortValueChanged()"));

    int maxPort = mHighestUDPPortItem->contentWidgetData(QString("text")).toString().toInt();
    int minPort = mLowestUDPPortItem->contentWidgetData(QString("text")).toString().toInt();

    MPX_DEBUG(_L("minudp = %i, maxudp = %i"), minPort, maxPort);

    if(maxPort < minPort)
    {
        maxPort = minPort;
    }

    gValidatePortValues(maxPort, UDP_LOWEST_ACCEPTED_VALUE, PORT_HIGHEST_ACCEPTED_VALUE);

    mHighestUDPPortItem->setContentWidgetData(QString("text"), QString::number(maxPort));

    mSettingsModel->SetMaxUDPPort(maxPort);
}

// ---------------------------------------------------------------------------
// useProxyToggled
// ---------------------------------------------------------------------------
//
void VideoSettingsGroup::useProxyToggled(int state)
{
    MPX_ENTER_EXIT(_L("VideoSettingsGroup::useProxyToggled()"),
        _L("state = %i"), state);

    mProxyServerItem->setEnabled(state == Qt::Checked);
    mProxyPortItem->setEnabled(state == Qt::Checked);

    mSettingsModel->SetProxyMode(state == Qt::Checked ? 1 : 0);
}

// ---------------------------------------------------------------------------
// proxyServerValueChanged
// ---------------------------------------------------------------------------
//
void VideoSettingsGroup::proxyServerEditingFinished()
{
    MPX_ENTER_EXIT(_L("VideoSettingsGroup::proxyServerValueChanged()"));

    QString text = mProxyServerItem->contentWidgetData(QString("text")).toString();

    MPX_DEBUG(_L("new proxy server = '%s'"), text.data());

    HBufC* proxyServer = XQConversions::qStringToS60Desc(text);
    TRAP_IGNORE( mSettingsModel->SetProxyHostNameL(*proxyServer) );
    delete proxyServer;
}

// ---------------------------------------------------------------------------
// proxyPortValueChanged
// ---------------------------------------------------------------------------
//
void VideoSettingsGroup::proxyPortEditingFinished()
{
    MPX_ENTER_EXIT(_L("VideoSettingsGroup::proxyPortValueChanged()"));

    int proxyPort = mProxyPortItem->contentWidgetData(QString("text")).toString().toInt();

    MPX_DEBUG(_L("new proxy port = %i"), proxyPort);

    gValidatePortValues(proxyPort, PROXY_LOWEST_ACCEPTED_VALUE, PORT_HIGHEST_ACCEPTED_VALUE);

    mProxyPortItem->setContentWidgetData(QString("text"), QString::number(proxyPort));

    mSettingsModel->SetProxyPort(proxyPort);
}

// ---------------------------------------------------------------------------
// itemShown
// ---------------------------------------------------------------------------
//
void VideoSettingsGroup::itemShown(const QModelIndex& index)
{
    MPX_ENTER_EXIT(_L("VideoSettingsGroup::itemShown()"),
        _L("index row = %i"), index.row());

    HbDataFormModelItem* modelItem = mItemDataHelper.modelItemFromModelIndex(index);

    if(modelItem == mLowestUDPPortItem ||
       modelItem == mHighestUDPPortItem ||
       modelItem == mProxyPortItem )
    {
        HbLineEdit *editor = static_cast<HbLineEdit *>(mItemDataHelper.widgetFromModelIndex(index));

        editor->setMaxLength(5);

        HbEditorInterface editorIf(editor);
        editorIf.setFilter(HbDigitsOnlyFilter::instance());
    }

    if(modelItem == mProxyServerItem)
    {
        mItemDataHelper.addConnection(mUseProxyItem, SIGNAL(stateChanged(int)), this, SLOT(useProxyToggled(int)));
        HbEditorInterface editor(mItemDataHelper.widgetFromModelIndex(index));
        editor.setFilter(HbUrlFilter::instance());
    }
}

// ---------------------------------------------------------------------------
// InitSettingsModelL
// ---------------------------------------------------------------------------
//
void VideoSettingsGroup::InitSettingsModelL()
{
    MPX_ENTER_EXIT(_L("VideoSettingsGroup::InitSettingsModelL()"));

    TUid settingsUid = {KMPSettEngImplUidROP};
    mSettingsModel = CMPSettingsModel::NewL(settingsUid);
    mSettingsModel->LoadSettingsL(EConfigUser);
}

// ---------------------------------------------------------------------------
// loadSettings
// ---------------------------------------------------------------------------
//
void VideoSettingsGroup::loadSettings()
{
    MPX_ENTER_EXIT(_L("VideoSettingsGroup::loadSettings()"));

    int lowestUdpPort(0);
    mSettingsModel->GetMinUDPPort(lowestUdpPort);
    mLowestUDPPortItem->setContentWidgetData(QString("text"), QString::number(lowestUdpPort));

    int highestUdpPort(0);
    mSettingsModel->GetMaxUDPPort(highestUdpPort);
    mHighestUDPPortItem->setContentWidgetData(QString("text"), QString::number(highestUdpPort));

    int proxyMode(0);
    mSettingsModel->GetProxyMode(proxyMode);
    mUseProxyItem->setContentWidgetData(QString("checked"), proxyMode == 1);
    useProxyToggled(proxyMode == 1 ? Qt::Checked : Qt::Unchecked);

    TBuf<496> proxyServerBuf;
    mSettingsModel->GetProxyHostName(proxyServerBuf);
    QString proxyServer = XQConversions::s60DescToQString(proxyServerBuf);
    mProxyServerItem->setContentWidgetData(QString("text"), proxyServer);

    int proxyPort;
    mSettingsModel->GetProxyPort(proxyPort);
    mProxyPortItem->setContentWidgetData(QString("text"), QString::number(proxyPort));

    mAccessPointItem->setIapId(getAccessPointId());
}
