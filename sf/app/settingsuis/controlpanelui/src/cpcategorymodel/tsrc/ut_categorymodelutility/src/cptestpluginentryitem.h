#ifndef CPTESTPLUGINENTRYITEM_H
#define CPTESTPLUGINENTRYITEM_H
#include <cpsettingformentryitemdata.h>

class CpTestPluginEntryItemData : public CpSettingFormEntryItemData
{
    Q_OBJECT
public:
    explicit CpTestPluginEntryItemData(const HbDataFormModelItem *parent /* = 0*/); 
    explicit CpTestPluginEntryItemData(CpItemDataHelper &itemDataHelper,
        const QString &text = QString(),
        const QString &description = QString(),
        const HbIcon &icon = HbIcon(),
        const HbDataFormModelItem *parent = 0);  
    virtual ~CpTestPluginEntryItemData();
public slots:
    void testOnLaunchView();
private:
    virtual CpBaseSettingView *createSettingView() const;
};
#endif // CPTESTPLUGINENTRYITEM_H
