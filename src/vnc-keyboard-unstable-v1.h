#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H

#include "wayland-util.h"
#include <QQuickWindow>
#include <QtWaylandCompositor/QWaylandCompositorExtensionTemplate>
#include <QtWaylandCompositor/QWaylandQuickExtension>
#include <QtWaylandCompositor/QWaylandCompositor>
#include "qwayland-server-vnc-keyboard-unstable-v1.h"

class VncKeyboardManager  : public QWaylandCompositorExtensionTemplate<VncKeyboardManager>
        , public QtWaylandServer::vnc_virtual_keyboard_manager_v1
{
    Q_OBJECT
    Q_PROPERTY(QQuickWindow *window MEMBER m_window);

public:
    VncKeyboardManager(QWaylandCompositor *compositor = nullptr);
    void initialize() override;

protected:
    void vnc_virtual_keyboard_manager_v1_create_virtual_keyboard(Resource *resource, uint32_t id) override;

private:
    QQuickWindow *m_window;
};

class VncKeyboard  : public QWaylandCompositorExtensionTemplate<VncKeyboard>
        , public QtWaylandServer::vnc_keyboard_v1
{
    Q_OBJECT
public:
    VncKeyboard(QQuickWindow *window, struct ::wl_client *client, int id, int version);

protected:
    void vnc_keyboard_v1_key(Resource *resource, uint32_t time, uint32_t key, uint32_t state) override;
    void vnc_keyboard_v1_destroy(Resource *resource) override;

private:
    QQuickWindow *m_window;
    Qt::KeyboardModifiers m_keymod;
};

Q_COMPOSITOR_DECLARE_QUICK_EXTENSION_CLASS(VncKeyboardManager)

#endif // VIRTUALKEYBOARD_H
