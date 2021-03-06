/* DUKTO - A simple, fast and multi-platform file transfer tool for LAN users
 * Copyright (C) 2011 Emanuele Colombo
 * Copyright (C) 2015 Arthur Zamarin
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#if defined(NOTIFY_LIBNOTIFY)
#include <libnotify/notify.h>
#endif

#include "systemtray.h"

#include <QWidget>

SystemTray* SystemTray::tray = NULL;

SystemTray::SystemTray(DuktoWindow& window, QObject* parent) :
    QSystemTrayIcon(QIcon(":/dukto.png"), parent),
    window(window)
{
#if defined(NOTIFY_LIBNOTIFY)
    notify_init ("Dukto");
#endif
    connect(this, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(on_activated(QSystemTrayIcon::ActivationReason)));
    SystemTray::tray = this;
}

SystemTray::~SystemTray()
{
#if defined(NOTIFY_LIBNOTIFY)
    notify_uninit();
#endif
}

void SystemTray::on_activated(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason)
    {
        case QSystemTrayIcon::Trigger:
            window.setVisible(!window.isVisible());
            break;
        case QSystemTrayIcon::MiddleClick:
            window.close();
            break;
        default:
            break;
    }
}

void SystemTray::received_file(QStringList* files, qint64)
{
    if(files->size() == 1)
        notify("Recieved File", files->at(0));
    else
        notify("Recieved Files and Folders", "");
}

void SystemTray::received_text(QString* text, qint64)
{
    notify("Recieved Text Snippet", *text);
}

void SystemTray::notify(QString title, QString body)
{
#if defined(NOTIFY_NATIVE_TRAY)
    this->showMessage(title, body);
#elif defined(NOTIFY_LIBNOTIFY)
    NotifyNotification* msg = notify_notification_new(title.toUtf8().constData(), body.toUtf8().constData(), NULL);
    notify_notification_show (msg, NULL);
    g_object_unref(G_OBJECT(msg));
#else
    Q_UNUSED(title);
    Q_UNUSED(body);
#endif
}
