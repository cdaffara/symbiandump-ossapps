#ifndef GLXMAINWINDOWEVENTFILTER_H
#define GLXMAINWINDOWEVENTFILTER_H
#include <QObject>
#include <QEvent>
class GlxMainWindowEventFilter : public QObject {
    Q_OBJECT
public:

    GlxMainWindowEventFilter() {}
    virtual bool eventFilter(QObject *o, QEvent *e) {
        Q_UNUSED(o);
        switch(e->type()) {
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
            e->accept();
            return true;
        default:
            break;
        }
        return false;
    }

};

#endif
