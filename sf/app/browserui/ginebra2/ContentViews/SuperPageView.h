/*
 * SuperPageView.h
 *
 *  Created on: Jun 11, 2010
 *      Author: lewontin
 */

#include "controllableviewimpl.h"

#ifndef SUPERPAGEVIEW_H_
#define SUPERPAGEVIEW_H_

namespace GVA {

class GWebContentView;

class SuperPageView : public ControllableViewBase
{
      Q_OBJECT
      
  public:
      SuperPageView(GWebContentView * contentView, QObject * parent = 0, const QString &objectName = QString::null);
      virtual ~SuperPageView();
      //Reimplement ControllableViewBase methods
      static QString Type() { return "superPage"; }
      virtual QString type() const { return Type(); }
      QGraphicsWidget* widget() const;
      virtual QList<QAction*> getContext();
      virtual void show();
      virtual void hide() {;}
  public slots:
      void addAction(const QString& action, const QString & script = QString());
  private slots:
      void invokeScriptAction();
  private:
      GWebContentView * m_contentView;
      QMap<QString, QAction*>  m_actions;
};
}
#endif /* SUPERPAGEVIEW_H_ */
