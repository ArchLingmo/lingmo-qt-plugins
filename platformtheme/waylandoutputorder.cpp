#include "waylandoutputorder.h"

#include <QGuiApplication>
#include <QLoggingCategory>
#include <QScreen>

// Qt Private
#include <qpa/qwindowsysteminterface.h>
#include <QtWaylandClient/private/qwaylanddisplay_p.h>
#include <QtWaylandClient/private/qwaylandintegration_p.h>

Q_LOGGING_CATEGORY(LOG_OUTPUT_ORDER, "lingmo.platformtheme.outputorder", QtInfoMsg)

WaylandOutputOrder *WaylandOutputOrder::create(QObject *parent)
{
    if (!QGuiApplication::platformName().startsWith(QLatin1String("wayland")))
        return nullptr;
    return new WaylandOutputOrder(parent);
}

WaylandOutputOrder::WaylandOutputOrder(QObject *parent)
    : QWaylandClientExtensionTemplate(1, parent)
{
    // The theme is made while the platform starts, before any window: one round
    // trip brings the current order in, so windows open on the right screen
    initialize();
    if (isActive() && integration() && integration()->display())
        integration()->display()->forceRoundTrip();
    else
        qCDebug(LOG_OUTPUT_ORDER) << "kde_output_order_v1 not offered (yet)";

    // Screens Qt learns about later (monitor plugged in, a slow output) may be
    // the first one of the order
    QMetaObject::invokeMethod(this, [this] {
        connect(qGuiApp, &QGuiApplication::screenAdded, this, &WaylandOutputOrder::applyPrimary, Qt::QueuedConnection);
        connect(qGuiApp, &QGuiApplication::screenRemoved, this, &WaylandOutputOrder::applyPrimary, Qt::QueuedConnection);
        applyPrimary();
    }, Qt::QueuedConnection);
}

void WaylandOutputOrder::kde_output_order_v1_output(const QString &outputName)
{
    m_pending.append(outputName);
}

void WaylandOutputOrder::kde_output_order_v1_done()
{
    m_order = m_pending;
    m_pending.clear();
    qCDebug(LOG_OUTPUT_ORDER) << "output order" << m_order;
    applyPrimary();
}

void WaylandOutputOrder::applyPrimary()
{
    const QList<QScreen *> screens = QGuiApplication::screens();
    for (const QString &name : std::as_const(m_order)) {
        for (QScreen *screen : screens) {
            if (screen->name() != name)
                continue;
            if (screen != QGuiApplication::primaryScreen() && screen->handle()) {
                qCDebug(LOG_OUTPUT_ORDER) << "primary screen" << name;
                QWindowSystemInterface::handlePrimaryScreenChanged(screen->handle());
            }
            return;
        }
    }
}
