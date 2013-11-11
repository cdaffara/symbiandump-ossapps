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
 * Description: Message Indicator Plugin Class 
 *
 */

#ifndef MSGINDICATORPLUGIN_H
#define MSGINDICATORPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QVariant>

#include <hbindicatorplugininterface.h>
#include <hbindicatorinterface.h>

/**
 * Message indicator plugin class. 
 * Main class of message indicator plugin. 
 */
class MsgIndicatorPlugin: public QObject, public HbIndicatorPluginInterface
{
Q_OBJECT
Q_INTERFACES(HbIndicatorPluginInterface)

public:
    /**
     * Constructor
     */
     MsgIndicatorPlugin();
     
     /**
      * Destructor
      */
    ~MsgIndicatorPlugin();
    
    /**
     * @see HbIndicatorPluginInterface
     */
    QStringList indicatorTypes() const;
    
    /**
     * @see HbIndicatorPluginInterface
     */
    bool accessAllowed(const QString &indicatorType,
        const QVariantMap& securityInfo) const;   
    /**
     * @see HbIndicatorPluginInterface
     */
    HbIndicatorInterface* createIndicator(const QString &indicatorType);

    /**
     * @see HbIndicatorPluginInterface
     */
    int error() const;

    /**
     * Returns the indices of plugin types.
     * @param indicatorType QString the type of indicator
     * @return int the indicator type as an index.
     */
    int typeIndex(const QString &indicatorType) const;

private:
    Q_DISABLE_COPY(MsgIndicatorPlugin)
    
    /**
     * Error value
     */
    int mError;
    
    /**
     * Indicator types 
     * supported indicator types. 
     */
    QStringList mIndicatorTypes;
};

#endif // MSGINDICATORPLUGIN_H
