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
#ifndef GLXMPXCOMMANDHANDLER_H
#define GLXMPXCOMMANDHANDLER_H

#include <QObject>
#include<glxcommandhandler.h>
#include <glxfiltergeneraldefs.h>
#include <mpxcommand.h>
#include <mglxmedialistobserver.h>

#ifdef BUILD_COMMANDHANDLERBASE
#define GLX_COMMANDHANDLERBASE_EXPORT Q_DECL_EXPORT
#else
#define GLX_COMMANDHANDLERBASE_EXPORT Q_DECL_IMPORT
#endif
class MGlxMediaList;
class HbProgressDialog;
class HbAction;

class GLX_COMMANDHANDLERBASE_EXPORT GlxMpxCommandHandler : public GlxCommandHandler, public MGlxMediaListObserver
{
Q_OBJECT    
public:
    GlxMpxCommandHandler();
    virtual ~GlxMpxCommandHandler();
    virtual void executeCommand(int commandId,int collectionId,QList<QModelIndex> indexList = QList<QModelIndex>() );

public slots:
    void messageDialogClose(HbAction* action);
    void commandCancelled();
protected: // From MGlxMediaListObserver    
    /// See @ref MGlxMediaListObserver::HandleItemAddedL
    void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleMediaL
    void HandleMediaL(TInt aListIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleItemRemovedL
    void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleItemModifiedL
    void HandleItemModifiedL(const RArray<TInt>& aItemIndexes, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleAttributesAvailableL
    void HandleAttributesAvailableL(TInt aItemIndex,     
        const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);
            
    /// See @ref MGlxMediaListObserver::HandleFocusChangedL
    void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, TInt aNewIndex, TInt aOldIndex, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleItemSelectedL
    void HandleItemSelectedL(TInt aIndex, TBool aSelected, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleMessageL
    void HandleMessageL(const CMPXMessage& aMessage, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleCommandCompleteL
    void HandleCommandCompleteL(TAny* aSessionId, CMPXCommand* aCommandResult, TInt aError, MGlxMediaList* aList);
    
    /// See @ref MGlxMediaListObserver::HandleError
    void HandleError(TInt aError);
protected: // From derived class    
 
    virtual void DoHandleCommandCompleteL(TAny* aSessionId, 
            CMPXCommand* aCommandResult, TInt aError, MGlxMediaList* aList); 

    virtual CMPXCommand* CreateCommandL(TInt aCommandId, MGlxMediaList& aMediaList, TBool& aConsume) const = 0;
	virtual void DoExecuteCommandL(TInt aCommandId, MGlxMediaList& aMediaList, TBool& aConsume);
	virtual void HandleErrorL(TInt aErrorCode) const;
	virtual QString CompletionTextL() const;
	virtual QString ProgressTextL() const;        
    virtual QString ConfirmationTextL(bool multiSelection = false) const; 
private: 
    void ConfirmationNoteL(MGlxMediaList& aMediaList);
    void ConfirmationNoteSingleL(MGlxMediaList& aMediaList) ;
    void ConfirmationNoteMultipleL(MGlxMediaList& aMediaList) ;
	void ProgressNoteL(TInt aCommandId);
	void DismissProgressNoteL();
	void CompletionNoteL() const;
    void TryExitL(TInt aErrorCode);
    void CreateMediaListL(int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType) ;
    void CreateMediaListAlbumItemL(int aCollectionId, int aHierarchyId, TGlxFilterItemType aFilterType);
    void executeMpxCommand(bool execute);
protected:
    mutable bool iIsCmdActive;
private:
    // Instance of Media List
    MGlxMediaList* iMediaList;
	TBool iProgressComplete;
	HbProgressDialog* mProgressDialog;
	int mCommandId;
};
#endif //GLXMPXCOMMANDHANDLER_H
