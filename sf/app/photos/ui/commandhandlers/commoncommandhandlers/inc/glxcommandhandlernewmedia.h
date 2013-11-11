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

#ifndef GLXCOMMANDHANDLERNEWMEDIA_H
#define GLXCOMMANDHANDLERNEWMEDIA_H

#include <glxmpxcommandhandler.h>
#include <glxmediaid.h>

#ifdef BUILD_COMMONCOMMANDHANDLERS
#define GLX_COMMONCOMMANDHANDLERS_EXPORT Q_DECL_EXPORT
#else
#define GLX_COMMONCOMMANDHANDLERS_EXPORT Q_DECL_IMPORT
#endif

class HbInputDialog;

class GLX_COMMONCOMMANDHANDLERS_EXPORT GlxCommandHandlerNewMedia : public GlxMpxCommandHandler
	{
	Q_OBJECT    
public:
    GlxCommandHandlerNewMedia();
    ~GlxCommandHandlerNewMedia();

    TInt ExecuteLD(TGlxMediaId& aNewMediaId,QString& aTitle);

private:
    QString CompletionTextL() const;
    QString ProgressTextL() const; 

protected: // From MGlxMediaListObserver    
    /// See @ref MGlxMediaListObserver::HandleItemAddedL
    void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
    void HandleError(TInt aError);
    void HandleErrorL(TInt aErrorCode);
protected: // from GlxMpxCommandHandler

    CMPXCommand* CreateCommandL(TInt aCommandId, MGlxMediaList& aMediaList, TBool& aConsume) const ;

	void DoHandleCommandCompleteL(TAny* aSessionId, CMPXCommand* aCommandResult, 
            TInt aError, MGlxMediaList* aList);

private:
    QString GenerateNewMediaItemTitleL(QString newMediaTilte,MGlxMediaList& aMediaList) const;
public:
    /**
     *  Id of new media
     */
    TGlxMediaId iNewMediaId;
    
    /**
     *  Active scheduler wait object. (Owned)
     */
    CActiveSchedulerWait* iSchedulerWait;
    
    /**
     * Error set by DoHandleCommandCompleteL()
     */
    mutable TInt iNewMediaCreationError;
    
    mutable HBufC* iNewMediaItemTitle;
private:
    bool mShowConfirmation;

	};

    
    
#endif // GLXCOMMANDHANDLERNEWMEDIA_H
