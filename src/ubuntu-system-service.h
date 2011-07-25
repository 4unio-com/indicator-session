/*
Copyright 2011 Canonical Ltd.

Authors:
    Conor Curran <conor.curran@canonical.com>

This program is free software: you can redistribute it and/or modify it 
under the terms of the GNU General Public License version 3, as published 
by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranties of 
MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR 
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _UBUNTU_SYSTEM_SERVICE_H_
#define _UBUNTU_SYSTEM_SERVICE_H_

#include <glib-object.h>
#include "session-dbus.h"

G_BEGIN_DECLS

#define UBUNTU_TYPE_SYSTEM_SERVICE             (ubuntu_system_service_get_type ())
#define UBUNTU_SYSTEM_SERVICE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), UBUNTU_TYPE_SYSTEM_SERVICE, UbuntuSystemService))
#define UBUNTU_SYSTEM_SERVICE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), UBUNTU_TYPE_SYSTEM_SERVICE, UbuntuSystemServiceClass))
#define UBUNTU_IS_SYSTEM_SERVICE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UBUNTU_TYPE_SYSTEM_SERVICE))
#define UBUNTU_IS_SYSTEM_SERVICE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), UBUNTU_TYPE_SYSTEM_SERVICE))
#define UBUNTU_SYSTEM_SERVICE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), UBUNTU_TYPE_SYSTEM_SERVICE, UbuntuSystemServiceClass))

typedef struct _UbuntuSystemServiceClass UbuntuSystemServiceClass;
typedef struct _UbuntuSystemService UbuntuSystemService;

struct _UbuntuSystemServiceClass
{
	GObjectClass parent_class;
};

GType ubuntu_system_service_get_type (void) G_GNUC_CONST;
UbuntuSystemService* ubuntu_system_service_new (SessionDbus* session_dbus);

G_END_DECLS

#endif /* _UBUNTU_SYSTEM_SERVICE_H_ */
