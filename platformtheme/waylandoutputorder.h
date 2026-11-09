#ifndef WAYLANDOUTPUTORDER_H
#define WAYLANDOUTPUTORDER_H

#include <QStringList>
#include <QtWaylandClient/QWaylandClientExtensionTemplate>

#include "qwayland-kde-output-order-v1.h"

/*
    Wayland: QtWayland doesn't know which output is the primary one (KWin's output
    priority, Settings › Display), so QGuiApplication::primaryScreen() would be the
    first wl_output forever. KWin sends the order through kde_output_order_v1; the
    first output of it becomes Qt's primary screen, so primaryScreen() and
    primaryScreenChanged behave like on X11 (xrandr primary) in every app.
*/
class WaylandOutputOrder : public QWaylandClientExtensionTemplate<WaylandOutputOrder, &QtWayland::kde_output_order_v1::destroy>,
                           public QtWayland::kde_output_order_v1
{
    Q_OBJECT

public:
    // Only on the wayland platform; nullptr elsewhere
    static WaylandOutputOrder *create(QObject *parent);

protected:
    void kde_output_order_v1_output(const QString &outputName) override;
    void kde_output_order_v1_done() override;

private:
    explicit WaylandOutputOrder(QObject *parent);
    void applyPrimary();

    QStringList m_order;
    QStringList m_pending;
};

#endif // WAYLANDOUTPUTORDER_H
