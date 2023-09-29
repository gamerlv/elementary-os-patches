/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

/*
 * Copyright (C) 2014 Red Hat
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Written by:
 *     Jasper St. Pierre <jstpierre@mecheye.net>
 */

#ifndef META_WAYLAND_OUTPUTS_H
#define META_WAYLAND_OUTPUTS_H

#include "backends/meta-monitor-manager-private.h"
#include "wayland/meta-wayland-private.h"

#define META_TYPE_WAYLAND_OUTPUT (meta_wayland_output_get_type ())
G_DECLARE_FINAL_TYPE (MetaWaylandOutput, meta_wayland_output,
                      META, WAYLAND_OUTPUT, GObject)

const GList * meta_wayland_output_get_resources (MetaWaylandOutput *wayland_output);

MetaLogicalMonitor * meta_wayland_output_get_logical_monitor (MetaWaylandOutput *wayland_output);

void meta_wayland_outputs_init (MetaWaylandCompositor *compositor);

#endif /* META_WAYLAND_OUTPUTS_H */
