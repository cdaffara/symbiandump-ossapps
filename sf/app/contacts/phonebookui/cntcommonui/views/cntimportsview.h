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

#ifndef CNTIMPORTSVIEW_H_
#define CNTIMPORTSVIEW_H_

#include <QObject>
#include <hbpushbutton.h>
#include <hblistview.h>

#include "cntactionmenubuilder.h"
#include <hbdocumentloader.h>
#include "cntsimutility.h"
#include "cntimportviewcallback.h"

#include <cntabstractview.h>

class HbView;
class HbAction;
class QStandardItemModel;
class HbDialog;

class CntSimEngine;
QTM_BEGIN_NAMESPACE
class QContact;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CntImportsView : public QObject, public CntAbstractView, public CntImportViewCallback
{

Q_OBJECT

public slots:

    void onListViewActivated(const QModelIndex &index);
    void userCancelsImport();
     
public:
    CntImportsView();
    ~CntImportsView();

public: // From CntAbstractView
    void activate( const CntViewParameters aArgs );
    void deactivate();
    bool isDefault() const { return false; }
    HbView* view() const { return mView; }
    int viewId() const { return importsView; }
    inline void setEngine( CntAbstractEngine& aEngine ){ mEngine = &aEngine; }
    QContactManager *contactSymbianManager();
    
private:
    void showWaitNote();
    void setPreferredDetails( QContact *aContact );
    
private: // from CntImportViewCallback
    void setListBoxItemText(QString& aPrimary, QString& aSecondary);
    void setListBoxItemEnabled(bool aEnabled);
        
#ifdef PBK_UNIT_TEST
public slots:
#else
private slots:
#endif
    void showPreviousView();
    void closeImportPopup();
        
#ifdef PBK_UNIT_TEST
public :
#else
private :
#endif  
   // QContact* mContact;
    CntAbstractViewManager* mViewManager;
    CntSimEngine        *mSimEngine;
    HbListView              *mListView;
    HbDocumentLoader mDocumentLoader;
    HbView* mView; // own
    HbAction* mSoftkey;
	QStandardItemModel* mModel;
    HbDialog *mImportSimPopup;

    CntAbstractEngine* mEngine;
};

#endif /* CNTIMPORTSVIEW_H_ */
