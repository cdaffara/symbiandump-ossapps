

#ifndef AUTOMATICANSWERTEST_H
#define AUTOMATICANSWERTEST_H


#include <QGraphicsItem>
#include <hbview.h>

class CEtelIncomingCallMonitor;
class HbPushButton;

class automaticanswertest : public HbView
{
    Q_OBJECT

public:
	automaticanswertest(QGraphicsItem *parent = 0);
    ~automaticanswertest();
    
private: 
    void createContent();

protected slots:
    void answer();    
    void stopanswer();
    
private:
    HbPushButton *answering;
    HbPushButton *stopanswering;
    CEtelIncomingCallMonitor* m_incomingVoiceCallMonitor;
};

#endif // AUTOMATICANSWERTEST_H
