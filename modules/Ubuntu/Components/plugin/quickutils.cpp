/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "quickutils.h"

#include <QGuiApplication>
#include <QtQuick/QQuickView>
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlContext>
#include <QtCore/QAbstractListModel>
#include <QtCore/QAbstractProxyModel>
#include <QtQml/QQmlPropertyMap>
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlEngine>

#include <private/qquicktextinput_p.h>
#include <private/qquicktextedit_p.h>

QuickUtils::QuickUtils(QObject *parent) :
    QObject(parent),
    m_rootView(0),
    m_touchScreenAvailable(false)
{
    QGuiApplication::instance()->installEventFilter(this);
    /*
     * Do we have touch screen?
     */
    QList<const QTouchDevice*> devices = QTouchDevice::devices();
    Q_FOREACH(const QTouchDevice* dev, devices) {
        m_touchScreenAvailable = (dev->type() == QTouchDevice::TouchScreen);
        if (m_touchScreenAvailable) {
            break;
        }
    }
}

void QuickUtils::log(const QString &log)
{
    qDebug() << log;
    instance().m_consoleLog += log + '\n';
    Q_EMIT instance().consoleLogChanged();
}

void QuickUtils::clearLog()
{
    instance().m_consoleLog.clear();
    Q_EMIT instance().consoleLogChanged();
}

QString QuickUtils::consoleLog() const
{
    return m_consoleLog;
}
/*!
 * \internal
 * Filter events to catch ChildAdded, when the the application gets the topmost
 * window assigned. Need to check the topmost windows each time as widgets added
 * to the application are not signaled in any other way.
 */
bool QuickUtils::eventFilter(QObject *obj, QEvent *event)
{
    if (!m_rootView && (event->type() == QEvent::ApplicationActivate)) {
        lookupQuickView();
        Q_EMIT activated();
    }
    if (event->type() == QEvent::ApplicationDeactivate) {
        Q_EMIT deactivated();
    }

    return QObject::eventFilter(obj, event);
}

/*!
 * \internal
 * \deprecated
 * Returns the current root object.
 */
QQuickItem *QuickUtils::rootObject()
{
    qmlInfo(this) << "WARNING: QuickUtils.rootObject property is deprecated: Use QuickUtils::rootItem() function instead.";
    if (!m_rootView)
        lookupQuickView();
    return (m_rootView) ? m_rootView->rootObject() : 0;
}

/*!
 * \internal
 * Returns the root item of a given item. In case there is a QQuickWindow (Window)
 * found in the hierarchy, the function will return the contentItem of the window.
 */
QQuickItem *QuickUtils::rootItem(QObject *object)
{
    QuickUtils &utils = instance();
    // make sure we have the m_rootView updated
    utils.lookupQuickView();
    if (!object) {
        return (utils.m_rootView) ? utils.m_rootView->rootObject() : 0;
    }

    QQuickItem *item = qobject_cast<QQuickItem*>(object);
    // the given object may be a non-visual element (QtObject or QQmlComponent)
    // therefore those objects' parent object should be considered
    QQuickItem *parentItem = item ? item->parentItem() : qobject_cast<QQuickItem*>(object->parent());
    while (parentItem && parentItem->parentItem()) {
        parentItem = parentItem->parentItem();
    }

    if (utils.m_rootView && (utils.m_rootView->contentItem() == parentItem)) {
        // when traversing visual parents of an element from the application,
        // we reach QQuickView's contentItem, whose size is invalid. Therefore
        // we need to return the QQuickView's rootObject() instead of the topmost
        // item found
        return utils.m_rootView->rootObject();
    }
    return parentItem;
}


QString QuickUtils::inputMethodProvider() const
{
    return QString(getenv("QT_IM_MODULE"));
}

/*!
 * \internal
 * Returns the class name (type) of a QtQuick item.
 */
QString QuickUtils::className(QObject *item)
{
    if (!item) {
        return QString("(null)");
    }
    QString result = item->metaObject()->className();
    return result.left(result.indexOf("_QML"));
}


/*!
 * \internal
 * Get QQuickView from the application's window list and connect its status change
 * signal as the root element is set after the root element completion.
 */
void QuickUtils::lookupQuickView()
{
    if (m_rootView)
        return;
    Q_FOREACH (QWindow *w, QGuiApplication::topLevelWindows()) {
        m_rootView = qobject_cast<QQuickView*>(w);
        if (m_rootView) {
            // connect in case we get the root object changed
            QObject::connect(m_rootView, SIGNAL(statusChanged(QQuickView::Status)),
                             this, SIGNAL(rootObjectChanged()));
            // emit changed signal so we update the eventual bindings
            if (m_rootView->rootObject())
                Q_EMIT rootObjectChanged();
            break;
        }
    }
}

QObject* QuickUtils::createQmlObject(const QUrl &url, QQmlEngine *engine)
{
    /* FIXME: if the directory pointed to by url contains a qmldir file that
       declares a JavaScript module then QQmlComponent::create() fails with
       the error "QQmlComponent: Component is not ready".
    */
    QQmlComponent *component = new QQmlComponent(engine, url, QQmlComponent::PreferSynchronous);
    QObject* result = component->create();
    delete component;
    return result;
}
