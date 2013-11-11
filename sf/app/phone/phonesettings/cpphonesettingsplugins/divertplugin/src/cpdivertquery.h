/*
 * cpdivertquery.h
 *
 *  Created on: Aug 10, 2010
 *      Author: jahelaak
 */

#ifndef CPDIVERTQUERY_H_
#define CPDIVERTQUERY_H_

#include <qobject.h>
#include <psetcalldivertingwrapper.h>

class HbAction;
class QString;
class HbListWidget;
class HbDialog;

class CpDivertQuery : public QObject
{
    Q_OBJECT
    
    // Activate divert phases
    enum ActivateDivertPhases
    {
        NonePhase,
        PopUpVoiceNumberListQueryPhase,
        PopUpTimerQueryPhase
    };
    
public:
    CpDivertQuery(PSetCallDivertingWrapper &callDivertingWrapper);
    virtual ~CpDivertQuery();
    
    
    void show(
            const QString &heading,
            PSCallDivertingCommand command,
            bool needTimeOutInfo);
    
signals:
    void result(PSCallDivertingCommand &command);
    void cancelled(PSCallDivertingCommand &command);
    void handleDivertingError(int aReason);
    
private slots:
    void voiceNumberListQueryClosed(HbAction* action);
    void popUpNumberEditorClosed(HbAction* action);
    void popUpTimerQueryClosed(HbAction* action);

private:
    /**
     Show to user divert number query list.
     */
    void popUpVoiceNumberListQuery(
            const QString& heading, PsServiceGroup serviceGroup);
    
    /**
     Show to user number query.
     */
    void popUpNumberEditor(const QString& heading);
    
    /**
     Show to user divert time out query list.
     */
    void popUpTimerQuery();
    
    
    void nextPhaseForActivateDivert(bool ok);
    
private:
    
    /**
     Creates dialog with default parameters
     */
    HbDialog* createDialog( const QString& heading ) const;

    /**
     Adds item to given list widget
     */
    void addItemToListWidget(HbListWidget* w,
        const QString& item, const QString& data) const;

    /**
     Adds item to given list widget
     */
    void addItemToListWidget(HbListWidget* w,
        const QString& item, const int& data) const;
    
private:
    PSetCallDivertingWrapper &m_callDivertingWrapper;
    ActivateDivertPhases m_activateDivertPhase;
    PSCallDivertingCommand m_divertCommand;
    bool m_needTimeOutInfo;
    HbDialog *m_dialog;
};

#endif /* CPDIVERTQUERY_H_ */
