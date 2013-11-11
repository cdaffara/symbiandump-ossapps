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

#ifndef LOGSABSTRACTMODEL_H
#define LOGSABSTRACTMODEL_H

#include <QAbstractListModel>
#include <logsexport.h>
#include <hbextendedlocale.h>

class LogsEvent;
class LogsDbConnector;
class LogsModelItemContainer;
class HbIcon;
class LogsContact;
class LogsConfigurationParams;


/**
 * Abstract logs model.
 *
 */
class LogsAbstractModel : public QAbstractListModel 
{
    Q_OBJECT
    
public:
    
    /**
     * Additional data role types.
     */
    enum LogsModelRole {
           RoleFullEvent = Qt::UserRole + 1, // LogsEvent
           RoleCall,                         // LogsCall
           RoleMessage,                      // LogsMessage
           RoleContact,                      // LogsContact
           RoleDetailsModel,                 // LogsDetailsModel
           RoleBaseLast                      // Child classes may define roles above this val
    };

public:
    
    ~LogsAbstractModel();
          
    /**
     * Factory method for creating a new contact object. Transfers ownership.
     */
    LOGSENGINE_EXPORT LogsContact* createContact(const QString& number);
    
    /**
     * Returns cenrep key status of predictive search feature. 
     * @return 0 - feature is permanently off and can't be turned on,
     *         1 - feature is on
     *         2 - feature is temporarily off and can be turned on 
     *         negative value indicates some error in fetching the key
     */
    LOGSENGINE_EXPORT int predictiveSearchStatus();
    
    /**
     * Allows to modify cenrep key value of predictive search features. 
     * However, this function can't be used if feature is set permanently off 
     * (see predictiveSearchStatus())
     * @param enabled, specify whether cenrep key will be set to 1 or 2
     * @ return 0 if cenrep key value modified succesfully,
     *          -1 in case of some error
     */
    LOGSENGINE_EXPORT int setPredictiveSearch(bool enabled);
    
    /**
     * Configuration.
     * @param configuration parameters
     * @return 0 if configured succesfully
     */
    LOGSENGINE_EXPORT virtual int updateConfiguration(LogsConfigurationParams& params);

public:
    
    static QString directionIconName(const LogsEvent& event);
    static QString typeIconName(const LogsEvent& event);
        
    /**
     * Fills iconList based on the event.
     * @param event, 
     * @param iconList, on return contains list of icons 
     */
    void getDecorationData(const LogsEvent& event, QList<QVariant>& iconList) const;
    
    LogsDbConnector* dbConnector();
    
    /**
     * Construct a date and time string based on system locale
     */
    QString dateAndTimeString(const QDateTime& dateTime) const;
    /**
     * Construct a duration string using separator from system locale
     */
    QString durationString(const QTime& time) const;
    
    QString phoneNumString(const QString& number) const;


protected slots:

    virtual void contactSavingCompleted(bool modified);
    virtual void updateModel();

protected:

    QVariant doGetData(int role, const LogsModelItemContainer& item) const;
    
    virtual QVariant createCall(const LogsModelItemContainer& item) const;
    virtual QVariant createMessage(const LogsModelItemContainer& item) const;
    virtual QVariant createContact(const LogsModelItemContainer& item) const;
    virtual int doSetPredictiveSearch(bool enabled);
    
    explicit LogsAbstractModel();
    
protected: //data 
    
    LogsDbConnector* mDbConnector;
    QMap<QString, HbIcon*> mIcons;
    HbExtendedLocale* mExtendedLocale;
    
private:  // Testing related friend definitions
    
    friend class UT_LogsModel;
    friend class UT_LogsDetailsModel;
    friend class UT_LogsFilter;
    friend class UT_LogsCustomFilter;
    
};


/**
 * Specialized model may contain other model items than log events.
 */
class LogsModelItemContainer {
public:
    LogsModelItemContainer(LogsEvent* event = 0);
    virtual ~LogsModelItemContainer();
    LogsEvent* event() const;

protected:
    LogsEvent* mEvent; // not owned
};

#endif //LOGSABSTRACTMODEL_H
