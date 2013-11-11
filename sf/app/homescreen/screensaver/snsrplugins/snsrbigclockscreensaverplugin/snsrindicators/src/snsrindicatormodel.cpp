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
* Description:  Model for handling indicator data.
*
*/

#include <qglobal.h>
#ifdef Q_OS_SYMBIAN
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <coreapplicationuissdkcrkeys.h>
#endif //Q_OS_SYMBIAN

#include "snsrindicatormodel.h"
#include "snsrindicatorinfo.h"

/*!
    \class SnsrIndicatorModel
    \ingroup group_snsrbigclockscreensaverplugin
    \brief Model for handling indicator data.
 */

// TODO: what is the final type string of silent indicator?
// workaround solution: assume that it contains
// substring "silence" like in the demo app provided by the team.
const char *gSilentIndicatorTypeString = "silence";
const char *gOfflineIndicatorTypeString = "offline";


/*!
    Constructs a new SnsrIndicatorModel
    \param parent Parent object.
 */
SnsrIndicatorModel::SnsrIndicatorModel(QObject *parent): QObject(parent),
    mOfflineStateOn(false)
{   
    initializeOfflineModeIndication();
}

/*!
    Destructs the class.
 */
SnsrIndicatorModel::~SnsrIndicatorModel()
{
     mNotificationIndicators.clear();
     mSettingIndicators.clear();
     mIndicatorsToShow.clear();
#ifdef Q_OS_SYMBIAN
     if (mSettingsManager && mOfflineKey) {
         mSettingsManager->stopMonitoring(*mOfflineKey);
     }
     delete mOfflineKey;
#endif //Q_OS_SYMBIAN
}

/*!
    If there are already some active indicators when screensaver is
    launched, then this method is called.
    Indicators that are gonna be shown by the screensaver are
    added to the model. 
    \param activeIndicators Active universal indicators in 
    chronological order according to their arrival time.
 */
void SnsrIndicatorModel::handleActiveIndicators(
    const QList<HbIndicatorInterface*> &activeIndicators)
{
    mNotificationIndicators.clear();
    mSettingIndicators.clear();
    
    bool addedAny(false);
    
    for (int i = 0; i < activeIndicators.size(); ++i) {
        HbIndicatorInterface* activatedIndicator = activeIndicators.at(i);
        SnsrIndicatorInfo indicatorInfo;
        if (activatedIndicator 
            && showIndicatorInScreensaver(*activatedIndicator,indicatorInfo)) {
            addIndicator(indicatorInfo);
            connectToIndicatorsUpdateSignal(*activatedIndicator);
            addedAny = true;
        }
    }
    
    // Send signal only once if there were any such indicators that
    // screensaver is supposed to show (=added to the model).
    if (addedAny) {
        emitChangeSignal();
    }
}

/*!
    Called when some universal indicator gets activated.
    \param activatedIndicator Activated universal indicator
 */
void SnsrIndicatorModel::handleActivatedIndicator(
    HbIndicatorInterface *activatedIndicator)
{
    SnsrIndicatorInfo indicatorInfo;
    if (activatedIndicator
        && showIndicatorInScreensaver(*activatedIndicator,indicatorInfo)) {
        if (addIndicator(indicatorInfo)) {
            connectToIndicatorsUpdateSignal(*activatedIndicator);
            emitChangeSignal();
        }
    }
}

/*!
    Called when some universal indicator gets deactivated.
    \param deactivatedIndicator Deactivated universal indicator
 */
void SnsrIndicatorModel::handleDeactivatedIndicator(
    HbIndicatorInterface *deactivatedIndicator)
{
    if (deactivatedIndicator) {
        SnsrIndicatorInfo indicatorInfo(
            deactivatedIndicator->indicatorType(), 
            deactivatedIndicator->indicatorData(HbIndicatorInterface::MonoDecorationNameRole).toString(),
            deactivatedIndicator->category());
        
        if (findAndRemoveIndicator(indicatorInfo)) {
            emitChangeSignal();
        }
    }
}

/*!
    Called when some universal indicator updates its data by
    emitting dataChanged signal. 
    We listen to this signal because at least the silent indicator plugin demo 
    uses this method to inform the clients when it gets deactivated/activated 
    once it has been activated once by setting its icon path to empty/valid string.
    Don't know if this is going to be the final solution as it's unconventional (?) 
    but let's be prepared also to this kind of approach. 
 */
void SnsrIndicatorModel::handleUpdatedIndicator()
{
    HbIndicatorInterface* indicator =
        qobject_cast<HbIndicatorInterface*>(sender());
    if (!indicator) {
        return;
    }
    
    // If indicator's icon path was set to empty string, then treat it
    // like it were deactivated. And if not empty, then it's active again.
    if (indicator->indicatorData(
            HbIndicatorInterface::MonoDecorationNameRole).toString().isEmpty()) {
        handleDeactivatedIndicator(indicator);
    }
    else {
        handleActivatedIndicator(indicator);
    }
    
}

/*!
    Sends a signal with a list of all currently active indicators.
    No signal is sent if there are no active indicators currently.
 */
void SnsrIndicatorModel::initializeIndicatorWidget()
{
    getCurrentOfflineState(); // update status to be on the safe side
    
    if ( anyActiveIndicators() ) {
        emitChangeSignal();
    }
}

#ifdef Q_OS_SYMBIAN 
/*!
    listens airplane repository key.
 */
void SnsrIndicatorModel::offlineValueChanged( const XQSettingsKey &key, const QVariant &value )
{
    bool previousState(mOfflineStateOn);
    switch ( key.key() ) {
        case KCoreAppUIsNetworkConnectionAllowed:
             if(value.toInt() == ECoreAppUIsNetworkConnectionNotAllowed &&
                value.isValid())
                 {
                 mOfflineStateOn = true;                     
                 }
             else
                 {
                 mOfflineStateOn = false; 
                 }            
             if (previousState != mOfflineStateOn) {
                 emitChangeSignal();
             }
             break;
        default:
            break;

        }
}
#endif //Q_OS_SYMBIAN

/*!
    Add the indicator into this model. Handle here the order in which
    indicators are shown in screensaver: notification indicators should be
    shown in the same order as shown in statusbar, that is in reversed
    chronological order. Silent indicator should always be the right-most one.
    /retval true if indicator was added (not found already in the listings)
 */
bool SnsrIndicatorModel::addIndicator(const SnsrIndicatorInfo &indicatorInfo)
{
    // To be on the safe side, check that the indicator doesn't already
    // exists in the active indicator listings.
    bool added(false);
    
    // Use prepend to keep the list in reversed chronological order
    if (indicatorInfo.category == HbIndicatorInterface::NotificationCategory
        && !isIndicatorAlreadyAdded(indicatorInfo)) {
        mNotificationIndicators.prepend(indicatorInfo);
        added = true;
    }
    else if (indicatorInfo.category == HbIndicatorInterface::SettingCategory
             && !isIndicatorAlreadyAdded(indicatorInfo)) {
        mSettingIndicators.append(indicatorInfo);
        added = true;
    }
    
    return added;
}

/*!
    Check whether the indicator is already added in the active 
    indicator listing.
    /retval true if indicator is already added; false if not.
 */
bool SnsrIndicatorModel::isIndicatorAlreadyAdded(const SnsrIndicatorInfo &indicatorInfo) const
{
    bool alreadyExits(false);
    
    const QList<SnsrIndicatorInfo> &indicatorList = 
        indicatorInfo.category == HbIndicatorInterface::NotificationCategory 
        ? mNotificationIndicators : mSettingIndicators;
    
    for (int i = 0; i < indicatorList.size(); ++i) {
        if (indicatorList.at(i).type == indicatorInfo.type) {
            alreadyExits = true;
            break;
        }
    }
    return alreadyExits;
}

/*!
   Remove indicator if it can be found.
   /retval true if the indicator was removed
 */
bool SnsrIndicatorModel::findAndRemoveIndicator(const SnsrIndicatorInfo &indicatorInfo)
{
    bool removed(false);
    int index(-1);
    QList<SnsrIndicatorInfo> &indicatorList = 
        indicatorInfo.category == HbIndicatorInterface::NotificationCategory 
        ? mNotificationIndicators : mSettingIndicators;
    
    for (int i = 0; i < indicatorList.size(); ++i) {
        if (indicatorList.at(i).type == indicatorInfo.type) {
            index = i;
            break;
        }
    }
    if (index >= 0) {
        indicatorList.removeAt(index);
        removed = true;
    }
    return removed;
}

/*!
   Check whether to show the indicator in screensaver or not.
   Icon path is asked with HbIndicatorInterface::MonoDecorationNameRole 
   which refers to the indicator icons shown also in the statusbar:
   only 4 Notification indicators has premission to publish data with
   that role. As those 4 are the ones wanted also by screensaver, no
   extra checks are needed.
   From the Setting category one should show only Silent indicator.
   No indicators are shown from the Progress category.
   If indicator should be shown by screensaver, indicator info is
   packaged and returned within the parameter.
   
   /retval true if the indicator should be shown in screensaver
 */
bool SnsrIndicatorModel::showIndicatorInScreensaver(
        const HbIndicatorInterface &indicatorInterface, SnsrIndicatorInfo &indicatorInfo)
{
    bool show(false);
    // all indicators shown by screensaver must have valid status bar icon path
    QString iconPath = 
            indicatorInterface.indicatorData(HbIndicatorInterface::MonoDecorationNameRole).toString();
    QString typeString = indicatorInterface.indicatorType();
    HbIndicatorInterface::Category category = indicatorInterface.category();
    
    if (category == HbIndicatorInterface::NotificationCategory
        && !iconPath.isEmpty()) {
        // only 4 indicators of this category has permission to publish status bar
        // icon path->we should show all of them->no additional checks are neeeded.
        show = true;
    }
    else if (category == HbIndicatorInterface::SettingCategory
             && !iconPath.isEmpty()
             // TODO: what is the type string of silent indicator? 
             // workaround solution: assume that it contains
             // substring "silence" like in the demo app.
             // && type == gSilentIndicatorTypeString ) { 
             && typeString.contains(gSilentIndicatorTypeString, Qt::CaseInsensitive)) {  
        show = true;
    }
    else { // e.g. no Progress category indicators are shown
        show = false;
    }
 
    // package indicator data into indicatorInfo parameter
    indicatorInfo.type = typeString;
    indicatorInfo.iconPath = iconPath;
    indicatorInfo.category = category;
        
    return show;
}

/*!
   Start listening to indicator's dataChanged signals.
   /parameter indicator whose dataChanged signal we want to listen
 */
void SnsrIndicatorModel::connectToIndicatorsUpdateSignal(const HbIndicatorInterface &indicatorInterface)
{
    // Connect to silent indicator's dataChanged signal as it seems to 
    // use unconventional method (demo at least) to inform when
    // it's get deactivated/activated: dataChanged signal is emitted and
    // icon path is set to empty string/valid string.
    // It's assumed that we don't need to listen to other indicators
    // as we are not interested e.g. when primary/secondary texts change etc.
    
    // NOTE: do NOT disconnect the indicator signal anywhere explicitly here
    // or we won't get activation/deactivation messages.
    if (indicatorInterface.indicatorType().contains(gSilentIndicatorTypeString, Qt::CaseInsensitive)) {
        QObject::connect( &indicatorInterface, SIGNAL(dataChanged()),
                          this, SLOT(handleUpdatedIndicator()));
    }
}

/*!
   Check whether there is any active indicator currently.
   /retval true if there is at least one indicator to show
 */
bool SnsrIndicatorModel::anyActiveIndicators() const
{
  bool any(false);
  if ( mNotificationIndicators.size() != 0 
      || mSettingIndicators.size() != 0 
      || mOfflineStateOn ) {
      any = true;
  }
  return any;
}

/*!
    Send appropriate signal so that UI widget can update itself.
 */
void SnsrIndicatorModel::emitChangeSignal()
{   
    if (anyActiveIndicators()) {   
        updateIndicatorsToShowList();
        emit indicatorsUpdated(mIndicatorsToShow);
    }
    else {
        mIndicatorsToShow.clear();
        emit allIndicatorsDeactivated();
    }
}

/*!
   Updates the list that contains all the indicators that
   should be shown in UI. List should contain active indicators
   in this order: 
   offline + notification indicators + settings indicators.
   This is the list that will be sent to the widget.
 */
void SnsrIndicatorModel::updateIndicatorsToShowList()
{
    mIndicatorsToShow.clear();
    
    if (mOfflineStateOn) {
        mIndicatorsToShow.append(mOfflineIndicatorInfo);
    }
    mIndicatorsToShow.append(mNotificationIndicators);
    mIndicatorsToShow.append(mSettingIndicators);
}

/*!
 * Reads the airplanemode repository value and starts monitoring it.
 * if the value is 1, offline indicator is shown.
 */
void SnsrIndicatorModel::initializeOfflineModeIndication()
{
#ifdef Q_OS_SYMBIAN
    mOfflineIndicatorInfo = SnsrIndicatorInfo(
        gOfflineIndicatorTypeString,
        QLatin1String("qtg_mono_offline"),
        HbIndicatorInterface::NotificationCategory);
    
    mSettingsManager = new XQSettingsManager(this);
    //connect to offlineValueChanged slot so we get information if the value is changed while screensaver is on
    connect(mSettingsManager, SIGNAL(valueChanged(XQSettingsKey, QVariant)),
            this, SLOT(offlineValueChanged( const XQSettingsKey, const QVariant)));
    mOfflineKey = new XQSettingsKey(XQSettingsKey::TargetCentralRepository, 
                                    KCRUidCoreApplicationUIs.iUid, 
                                    KCoreAppUIsNetworkConnectionAllowed);
    if(mSettingsManager->readItemValue(*mOfflineKey).toInt() == ECoreAppUIsNetworkConnectionNotAllowed)
        {
        mOfflineStateOn = true;
        } 
    mSettingsManager->startMonitoring(*mOfflineKey);
#else
    mOfflineStateOn = false;
#endif //Q_OS_SYMBIAN
}

/*!
    Get current offline status.
 */
void SnsrIndicatorModel::getCurrentOfflineState()
{
#ifdef Q_OS_SYMBIAN
    if(mSettingsManager->readItemValue(*mOfflineKey).toInt() == ECoreAppUIsNetworkConnectionNotAllowed) {
        mOfflineStateOn = true;
    }
    else {
       mOfflineStateOn = false;
    }
#else
    mOfflineStateOn = false;
#endif //Q_OS_SYMBIAN
}

