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
* Description:
*
*/

#ifndef CXUFULLSCREENPOPUP_H
#define CXUFULLSCREENPOPUP_H


#include <HbWidget>
#include <HbDocumentLoader>

class QString;
class QGraphicsScene;

/*!
* Class for showing simple full screen popups.
*/
class CxuiFullScreenPopup : public HbWidget
{
    Q_OBJECT

public:
    static CxuiFullScreenPopup *create(QGraphicsScene *scene,
                                       const QString &message,
                                       const QString &action = QString());
    ~CxuiFullScreenPopup();

    void handleMousePress();
    void handleMouseRelease();

public slots:
    void show();
    void hide();

private:
    qreal topZValue() const;
    void setScene(QGraphicsScene *scene);

private:
    CxuiFullScreenPopup();

    /*!
    * Internal custom DocML loader.
    */
    class CxuiFullScreenPopupLoader : public HbDocumentLoader
    {
    public:
        CxuiFullScreenPopupLoader();
        QObject *createObject(const QString& type, const QString &name);
    };

private:
    QGraphicsScene *mScene;
    bool mPopupShown;

    Q_DISABLE_COPY(CxuiFullScreenPopup)
};

#endif // CXUFULLSCREENPOPUP_H
