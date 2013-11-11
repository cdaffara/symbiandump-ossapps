#ifndef SECONDPLUGINFORCPCATEGORYMODEL_H
#define SECONDPLUGINFORCPCATEGORYMODEL_H

#include <qobject.h>
#include <cpplugininterface.h>
#include <cplauncherinterface.h>

class SecondPluginForCpCategoryModel : public QObject, public CpPluginInterface, public CpLauncherInterface
{
    Q_OBJECT
    Q_INTERFACES(CpPluginInterface)
    Q_INTERFACES(CpLauncherInterface)
public:
    SecondPluginForCpCategoryModel();
    virtual ~SecondPluginForCpCategoryModel();
    virtual QList<CpSettingFormItemData*> createSettingFormItemData(CpItemDataHelper &itemDataHelper) const;
    virtual CpBaseSettingView *createSettingView(const QVariant &hint) const;
};

#endif
