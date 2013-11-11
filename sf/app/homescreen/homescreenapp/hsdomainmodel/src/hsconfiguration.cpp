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

#include <QMetaObject>
#include <QMetaProperty>
#include <QTextStream>
#include <QFile>

#include <HbDeviceProfile>

#include "hsconfiguration.h"
#include "hsdatabase.h"

/*!
    \class HsConfiguration
    \ingroup group_hsdomainmodel
    \brief 
*/

/*!

*/
HsConfiguration::HsConfiguration(QObject *parent)
  : QObject(parent),
    mBounceEffect(20),
    mPageChangeAnimationDuration(300),
    mPageChangeZoneAnimationDuration(600),
    mPageChangeZoneReverseAnimationDuration(200),
    mPageRemovedAnimationDuration(1000),
    mNewPageAddedAnimationDuration(1000),
    mWidgetDragEffectDuration(250),
    mWidgetDropEffectDuration(200),
    mDefaultPageId(1),
    mMaximumPageCount(8),
    mIsShortcutLabelVisible(true),
    mBounceFeedbackEffectDistance(3),
    mPageChangeZoneWidth(30),
    mPageIndicatorSpacing(8),
    mMaximumWidgetHeight(39),
    mMaximumWidgetWidth(48),
    mMinimumWidgetHeight(8.75),
    mMinimumWidgetWidth(8.75),
    mPageChangePanDistance(120),
    mPageChangeFeedbackEffect(HbFeedback::Sensitive),
    mWidgetPickFeedbackEffect(HbFeedback::ItemPick),
    mWidgetDropFeedbackEffect(HbFeedback::ItemDrop),
    mWidgetRepositionFeedbackEffect(HbFeedback::BounceEffect),
    mWidgetOverTrashbinFeedbackEffect(HbFeedback::ItemMoveOver),
    mWidgetDropToTrashbinFeedbackEffect(HbFeedback::ItemDrop),
    mShortcutWidgetTapFeedbackEffect(HbFeedback::BasicItem),
    mWidgetMoveBlockedFeedbackEffect(HbFeedback::Basic),
    mClockWidgetTapFeedbackEffect(HbFeedback::BasicItem),
    mWidgetSnappingFeedbackEffect(HbFeedback::ItemMoveOver),
    mIsSnapEnabled(true),
    mSnapTimeout(300),
    mSnapForce(30),
    mSnapGap(6),
    mPageMargin(0),
    mSnapLineFadeInDuration(100),
    mSnapLineFadeOutDuration(100),
    mIsSnapEffectsEnabled(true),    
    mSceneType(PageWallpapers),
    mBounceAnimationEasingCurve(QEasingCurve::Linear),
    mPageChangeAnimationEasingCurve(QEasingCurve::Linear),
    mWidgetOrganizerSearchSequence(SearchRowByRow)
{    
}

/*!

*/
HsConfiguration::~HsConfiguration()
{
}

/*!

*/
QSizeF HsConfiguration::minimumWidgetSizeInUnits() const
{
    return QSizeF(mMinimumWidgetWidth, mMinimumWidgetHeight);
}

/*!

*/
QSizeF HsConfiguration::maximumWidgetSizeInUnits() const
{
    return QSizeF(mMaximumWidgetWidth, mMaximumWidgetHeight);
}

/*!

*/
QSizeF HsConfiguration::minimumWidgetSizeInPixels() const
{
    HbDeviceProfile profile;
    qreal factor = profile.unitValue();
    return factor * minimumWidgetSizeInUnits();
}

/*!

*/
QSizeF HsConfiguration::maximumWidgetSizeInPixels() const
{
    HbDeviceProfile profile;
    qreal factor = profile.unitValue();
    return factor * maximumWidgetSizeInUnits();
}

/*!

*/
bool HsConfiguration::load()
{
    QVariantHash configuration;
    
    HsDatabase *db = HsDatabase::instance();
    if (!db->configuration(configuration)) {
        return false;
    }

    const QMetaObject *mo = metaObject();
    QHashIterator<QString, QVariant> i(configuration);
    while (i.hasNext()) {
        i.next();
        QByteArray name = i.key().toLatin1();
        QVariant value = i.value();
        QMetaProperty mp = mo->property(mo->indexOfProperty(name));
        if (mp.isEnumType()) {
            bool ok;
            value.toString().toInt(&ok);
            if (ok) {
                setProperty(name, value.toInt());
            } else {
                setProperty(name, mp.enumerator().keyToValue(value.toString().toLatin1()));
            }
        } else {
            setProperty(name, value);
        }
    }

    return true;
}

/*!

*/
bool HsConfiguration::importFromFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QTextStream in(&file);    
    
    const QMetaObject *mo = metaObject();
    while (!in.atEnd()) {        
        QStringList list = in.readLine().split(QLatin1String(" = "));
        QByteArray name = list.at(0).toLatin1();
        QByteArray value = list.at(1).toLatin1();
        QMetaProperty mp = mo->property(mo->indexOfProperty(name));
        if (mp.isEnumType()) {
            setProperty(name, mp.enumerator().keyToValue(value));
        } else {
            setProperty(name, value);
        }
    }

    return true;
}

/*!

*/
bool HsConfiguration::exportToFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    QTextStream out(&file);

    const QMetaObject *mo = this->metaObject();
    for (int i = mo->propertyOffset(); i < mo->propertyCount(); ++i) {
        QMetaProperty mp = mo->property(i);
        out << mp.name() << " = ";        
        if (mp.isEnumType()) {            
            out << mp.enumerator().valueToKey(mp.read(this).toInt());            
        } else {
            out << mp.read(this).toString();
        }        
        out << endl;        
    }

    return true;
}

/*!

*/
HsConfiguration *HsConfiguration::instance()
{
    return mInstance;
}
 
/*!

*/
HsConfiguration *HsConfiguration::takeInstance()
{
    HsConfiguration *instance = mInstance;
    mInstance = 0;
    return instance;
}

/*!

*/
void HsConfiguration::setInstance(HsConfiguration *instance)
{
    if (mInstance != instance) {
        delete mInstance;
        mInstance = instance; 
    }    
}

HsConfiguration *HsConfiguration::mInstance(0);
