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
#ifndef _CNTSETTINGSVIEW_H__
#define _CNTSETTINGSVIEW_H__

#include <cntabstractview.h>
#include <QObject>

class HbAction;
class HbView;
class HbDataForm;
class HbDocumentLoader;
class CntSettingsModel;

/**
* View class for displaying and changing settings in phonebook
*/
class CntSettingsView : public QObject, public CntAbstractView
{
    Q_OBJECT
    
public:
    CntSettingsView();
    ~CntSettingsView();
    
protected: // From CntAbstractView
    void activate( const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const;
    HbView* view() const;
    int viewId() const;
    inline void setEngine( CntAbstractEngine& aEngine ){ mEngine = &aEngine; }
    
private slots:
    void back();
    
private:
    HbDocumentLoader* document();
    
private:
    HbView* mView;
    HbDocumentLoader* mDoc;
    HbDataForm* mForm;
    HbAction* mBack;
    
    CntAbstractViewManager* mViewMgr;
    CntViewParameters mArgs;
    
    CntSettingsModel* mModel;
    CntAbstractEngine* mEngine;
    friend class TestCntSettings;
};
#endif

