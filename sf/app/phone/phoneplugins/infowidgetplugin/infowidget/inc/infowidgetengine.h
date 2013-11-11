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

#ifndef INFOWIDGETENGINE_H_
#define INFOWIDGETENGINE_H_

#include <QObject>
#include <QString>
#include "infowidgetpreferences.h"

// Forward declarations 
class InfoWidgetNetworkHandler;
class InfoWidgetSatHandler;
class InfoWidgetLineHandler;

// Class declaration
class InfoWidgetEngine : public QObject 
    {
    Q_OBJECT
    
public:     
    enum HandlerEntity {
        NotDefined = 0, 
        NetworkHandler, 
        SatHandler, 
        LineHandler
    };
    
    class ModelData {    
    public: 
        int mcnIndicatorType() const { return m_mcnIndicatorType; }; 
        void setMcnIndicatorType(const int type){ m_mcnIndicatorType = type; };
        
        int homeZoneIndicatorType() const { return m_homeZoneIndicatorType; }; 
        void setHomeZoneIndicatorType(const int type){ m_homeZoneIndicatorType = type; };

        int activeLine() const { return m_activeLine; }; 
        void setActiveLine(const int line){ m_activeLine = line; };

        const QString& mcnName() const { return m_mcnName; };
        void setMcnName(const QString& name){ m_mcnName = name; };

        const QString& serviceProviderName() const { return m_serviceProviderName; };
        void setServiceProviderName(const QString& name){ m_serviceProviderName = name; };
        
        bool serviceProviderNameDisplayRequired() const { 
            return m_serviceProviderNameDisplayRequired; };
        void setServiceProviderNameDisplayRequired(bool required){ 
            m_serviceProviderNameDisplayRequired = required; };
        
        const QString& homeZoneTextTag() const { return m_homeZoneTextTag; };
        void setHomeZoneTextTag(const QString& tag){ m_homeZoneTextTag = tag; };

        const QString& satDisplayText() const { return m_satDisplayText; };
        void setSatDisplayText(const QString& text){ m_satDisplayText = text; };
        
    private: 
        int m_mcnIndicatorType; 
        int m_homeZoneIndicatorType;
        int m_activeLine; 
        bool m_serviceProviderNameDisplayRequired; 
        
        QString m_mcnName;
        QString m_serviceProviderName;
        QString m_homeZoneTextTag;
        QString m_satDisplayText;
    }; 
    
public:
    InfoWidgetEngine(QObject *parent = NULL);
    ~InfoWidgetEngine();
    
    const InfoWidgetEngine::ModelData &modelData() const;  
    
signals:
    void modelError(int operation, int errorCode);
    void modelChanged(); 
    
public slots: 
    void updateNetworkDataToModel();
    void updateSatDataToModel();
    void updateLineDataToModel();

    void handleNetworkError(
            int operation, 
            int errorCode); 
    void handleSatError(int operation, int errorCode); 
    void handleLineError(int operation, int errorCode);
    
    void suspend(); 
    void resume();
    
    void handlePreferencesChanged(
            InfoWidgetPreferences::Options options);
    
private: 
    ModelData m_modelData;
    
    // Own
    QScopedPointer<InfoWidgetNetworkHandler> m_networkHandler;
    QScopedPointer<InfoWidgetSatHandler> m_satHandler;
    }; 

#endif /* INFOWIDGETENGINE_H_ */


