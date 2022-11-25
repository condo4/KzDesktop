#include <QDebug>
#include <QGuiApplication>
#include <wayland-server-protocol.h>
#include <linux/input-event-codes.h>
#include "vnc-keyboard-unstable-v1.h"


void toQtKey(uint32_t key, uint32_t *pkeycode, uint32_t *punicode)
{
    static const QMap<int, int> keyMap {
        { 0xff08, Qt::Key_Backspace },
        { 0xff09, Qt::Key_Tab       },
        { 0xff0d, Qt::Key_Return    },
        { 0xff1b, Qt::Key_Escape    },
        { 0xff63, Qt::Key_Insert    },
        { 0xffff, Qt::Key_Delete    },
        { 0xff50, Qt::Key_Home      },
        { 0xff57, Qt::Key_End       },
        { 0xff55, Qt::Key_PageUp    },
        { 0xff56, Qt::Key_PageDown  },
        { 0xff51, Qt::Key_Left      },
        { 0xff52, Qt::Key_Up        },
        { 0xff53, Qt::Key_Right     },
        { 0xff54, Qt::Key_Down      },
        { 0xffbe, Qt::Key_F1        },
        { 0xffbf, Qt::Key_F2        },
        { 0xffc0, Qt::Key_F3        },
        { 0xffc1, Qt::Key_F4        },
        { 0xffc2, Qt::Key_F5        },
        { 0xffc3, Qt::Key_F6        },
        { 0xffc4, Qt::Key_F7        },
        { 0xffc5, Qt::Key_F8        },
        { 0xffc6, Qt::Key_F9        },
        { 0xffc7, Qt::Key_F10       },
        { 0xffc8, Qt::Key_F11       },
        { 0xffc9, Qt::Key_F12       },
        { 0xffe1, Qt::Key_Shift     },
        { 0xffe2, Qt::Key_Shift     },
        { 0xffe3, Qt::Key_Control   },
        { 0xffe4, Qt::Key_Control   },
        { 0xffe7, Qt::Key_Meta      },
        { 0xffe8, Qt::Key_Meta      },
        { 0xffe9, Qt::Key_Alt       },
        { 0xffea, Qt::Key_Alt       },

        { 0xffb0, Qt::Key_0         },
        { 0xffb1, Qt::Key_1         },
        { 0xffb2, Qt::Key_2         },
        { 0xffb3, Qt::Key_3         },
        { 0xffb4, Qt::Key_4         },
        { 0xffb5, Qt::Key_5         },
        { 0xffb6, Qt::Key_6         },
        { 0xffb7, Qt::Key_7         },
        { 0xffb8, Qt::Key_8         },
        { 0xffb9, Qt::Key_9         },

        { 0xff8d, Qt::Key_Return    },
        { 0xffaa, Qt::Key_Asterisk  },
        { 0xffab, Qt::Key_Plus      },
        { 0xffad, Qt::Key_Minus     },
        { 0xffae, Qt::Key_Period    },
        { 0xffaf, Qt::Key_Slash     },

        { 0xff95, Qt::Key_Home      },
        { 0xff96, Qt::Key_Left      },
        { 0xff97, Qt::Key_Up        },
        { 0xff98, Qt::Key_Right     },
        { 0xff99, Qt::Key_Down      },
        { 0xff9a, Qt::Key_PageUp    },
        { 0xff9b, Qt::Key_PageDown  },
        { 0xff9c, Qt::Key_End       },
        { 0xff9e, Qt::Key_Insert    },
        { 0xff9f, Qt::Key_Delete    },
    };
    uint32_t unicode = 0;
    uint32_t keycode = keyMap.value(key, 0);

    if (keycode >= ' ' && keycode <= '~')
        unicode = keycode;

    if (!keycode) {
        if (key <= 0xff) {
            unicode = key;
            if (key >= 'a' && key <= 'z')
                keycode = Qt::Key_A + key - 'a';
            else if (key >= ' ' && key <= '~')
                keycode = Qt::Key_Space + key - ' ';
        }
    }
    *pkeycode = keycode;
    *punicode = unicode;
}


VncKeyboardManager::VncKeyboardManager(QWaylandCompositor *compositor)
    :QWaylandCompositorExtensionTemplate(compositor)
{
}

void VncKeyboardManager::initialize()
{
    QWaylandCompositorExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    init(compositor->display(), 1);
}

void VncKeyboardManager::vnc_virtual_keyboard_manager_v1_create_virtual_keyboard(Resource *resource, uint32_t id)
{
    new VncKeyboard(m_window, resource->client(), id, 1);
}

VncKeyboard::VncKeyboard(QQuickWindow *window, struct ::wl_client *client, int id, int version):
    QtWaylandServer::vnc_keyboard_v1(client, id, version),
    m_window(window),
    m_keymod(Qt::NoModifier)
{
}

void VncKeyboard::vnc_keyboard_v1_key(Resource *resource, uint32_t time, uint32_t key, uint32_t state)
{
    bool down = !!state;
    uint32_t keycode, unicode;

    toQtKey(key, &keycode, &unicode);

    if (keycode == Qt::Key_Shift)
        m_keymod = down ? m_keymod | Qt::ShiftModifier :
                          m_keymod & ~Qt::ShiftModifier;
    else if (keycode == Qt::Key_Control)
        m_keymod = down ? m_keymod | Qt::ControlModifier :
                          m_keymod & ~Qt::ControlModifier;
    else if (keycode == Qt::Key_Alt)
        m_keymod = down ? m_keymod | Qt::AltModifier :
                          m_keymod & ~Qt::AltModifier;
    if (unicode || keycode) {
        QKeyEvent event(down ? QEvent::KeyPress : QEvent::KeyRelease, keycode, m_keymod, QString(QChar(unicode)));
        QGuiApplication::sendEvent(m_window, &event);
    } else {
        qDebug() << __func__ << " no unicode or keycode. keysym is " << key << ". state is " << state;
    }
}

void VncKeyboard::vnc_keyboard_v1_destroy(Resource *)
{
    delete this;
}
