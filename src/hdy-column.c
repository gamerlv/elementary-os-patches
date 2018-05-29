/*
 * Copyright (C) 2018 Purism SPC
 *
 * SPDX-License-Identifier: LGPL-2.1+
 */

#include "hdy-column.h"

#include <glib/gi18n.h>
#include <math.h>

/**
 * SECTION:hdy-column
 * @short_description: A container letting its child grow up to a given width.
 * @Title: HdyColumn
 *
 * The #HdyColumn widget limits the size of the widget it contains to a given
 * maximum width. The expansion of the child from its minimum to its maximum
 * size is eased out for a smooth transition.
 *
 * If the child requires more than the requested maximum width, it will be
 * allocated the minimum width it can fit in instead.
 */

#define HDY_EASE_OUT_TAN_CUBIC 3

enum {
  PROP_0,
  PROP_MAXIMUM_WIDTH,
  LAST_PROP,
};

struct _HdyColumn
{
  GtkBin parent_instance;

  gint maximum_width;
};

static GParamSpec *props[LAST_PROP];

G_DEFINE_TYPE (HdyColumn, hdy_column, GTK_TYPE_BIN)

static gdouble
ease_out_cubic (gdouble progress)
{
  gdouble tmp = progress - 1;

  return tmp * tmp * tmp + 1;
}

static void
hdy_column_get_property (GObject    *object,
                                  guint       prop_id,
                                  GValue     *value,
                                  GParamSpec *pspec)
{
  HdyColumn *self = HDY_COLUMN (object);

  switch (prop_id) {
  case PROP_MAXIMUM_WIDTH:
    g_value_set_int (value, hdy_column_get_maximum_width (self));
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
hdy_column_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  HdyColumn *self = HDY_COLUMN (object);

  switch (prop_id) {
  case PROP_MAXIMUM_WIDTH:
    hdy_column_set_maximum_width (self, g_value_get_int (value));
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
hdy_column_get_preferred_width (GtkWidget *widget,
                                gint      *minimum,
                                gint      *natural)
{
  GtkBin *bin = GTK_BIN (widget);
  GtkWidget *child;

  *minimum = 0;
  *natural = 0;

  child = gtk_bin_get_child (bin);
  if (child && gtk_widget_get_visible (child))
    gtk_widget_get_preferred_width (child, minimum, natural);

  *minimum = (gdouble) *minimum;
  *natural = (gdouble) *natural;
}

static void
hdy_column_get_preferred_height_and_baseline_for_width (GtkWidget *widget,
                                                        gint       width,
                                                        gint      *minimum,
                                                        gint      *natural,
                                                        gint      *minimum_baseline,
                                                        gint      *natural_baseline)
{
  GtkBin *bin = GTK_BIN (widget);
  GtkWidget *child;

  *minimum = 0;
  *natural = 0;

  if (minimum_baseline)
    *minimum_baseline = -1;

  if (natural_baseline)
    *natural_baseline = -1;

  child = gtk_bin_get_child (bin);
  if (child && gtk_widget_get_visible (child))
    gtk_widget_get_preferred_height_and_baseline_for_width (child,
                                                            width,
                                                            minimum,
                                                            natural,
                                                            minimum_baseline,
                                                            natural_baseline);
}

static void
hdy_column_get_preferred_height (GtkWidget *widget,
                                 gint      *minimum,
                                 gint      *natural)
{
  hdy_column_get_preferred_height_and_baseline_for_width (widget, -1,
                                                          minimum, natural,
                                                          NULL, NULL);
}

static void
hdy_column_size_allocate (GtkWidget     *widget,
                          GtkAllocation *allocation)
{
  HdyColumn *self = HDY_COLUMN (widget);
  GtkBin *bin = GTK_BIN (widget);
  GtkAllocation child_allocation;
  gint minimum_width = 0, maximum_width;
  gdouble amplitude, threshold, progress;
  gint baseline;
  GtkWidget *child;

  gtk_widget_set_allocation (widget, allocation);

  child = gtk_bin_get_child (bin);
  if (child == NULL)
    return;

  if (gtk_widget_get_visible (child))
    gtk_widget_get_preferred_width (child, &minimum_width, NULL);

  /* Sanitize the maximum width to use for computations. */
  maximum_width = MAX (minimum_width, self->maximum_width);
  amplitude = maximum_width - minimum_width;
  threshold = (HDY_EASE_OUT_TAN_CUBIC * amplitude + (gdouble) minimum_width);
  if (allocation->width <= minimum_width)
    child_allocation.width = (gdouble) allocation->width;
  else if (allocation->width >= threshold)
    child_allocation.width = maximum_width;
  else {
    progress = (allocation->width - minimum_width) / (threshold - minimum_width);
    child_allocation.width = (ease_out_cubic (progress) * amplitude + minimum_width);
  }
  child_allocation.height = allocation->height;

  if (!gtk_widget_get_has_window (widget)) {
    /* This allways center the child vertically. */
    child_allocation.x = allocation->x + (allocation->width - child_allocation.width) / 2;
    child_allocation.y = allocation->y;
  }
  else {
    child_allocation.x = 0;
    child_allocation.y = 0;
  }

  baseline = gtk_widget_get_allocated_baseline (widget);
  gtk_widget_size_allocate_with_baseline (child, &child_allocation, baseline);
}

static void
hdy_column_class_init (HdyColumnClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  object_class->get_property = hdy_column_get_property;
  object_class->set_property = hdy_column_set_property;

  widget_class->get_preferred_width = hdy_column_get_preferred_width;
  widget_class->get_preferred_height = hdy_column_get_preferred_height;
  widget_class->get_preferred_height_and_baseline_for_width = hdy_column_get_preferred_height_and_baseline_for_width;
  widget_class->size_allocate = hdy_column_size_allocate;

  gtk_container_class_handle_border_width (container_class);

  /**
   * HdyColumn:maximum_width:
   *
   * The maximum width to allocate to the child.
   */
  props[PROP_MAXIMUM_WIDTH] =
      g_param_spec_int ("maximum-width",
                        _("Maximum width"),
                        _("The maximum width allocated to the child"),
                        0, G_MAXINT, 0,
                        G_PARAM_READWRITE | G_PARAM_EXPLICIT_NOTIFY);

  g_object_class_install_properties (object_class, LAST_PROP, props);
}

static void
hdy_column_init (HdyColumn *self)
{
}

/**
 * hdy_column_new:
 *
 * Creates a new #HdyColumn.
 *
 * Returns: a new #HdyColumn
 */
HdyColumn *
hdy_column_new (void)
{
  return g_object_new (HDY_TYPE_COLUMN, NULL);
}

/**
 * hdy_column_get_maximum_width:
 * @self: a #HdyColumn
 *
 * Gets the maximum width to allocate to the contained child.
 *
 * Returns: the maximum width to allocate to the contained child.
 */
gint
hdy_column_get_maximum_width (HdyColumn *self)
{
  g_return_val_if_fail (HDY_IS_COLUMN (self), 0);

  return self->maximum_width;
}

/**
 * hdy_column_set_maximum_width:
 * @self: a #HdyColumn
 * @maximum_width: the maximum width
 *
 * Sets the maximum width to allocate to the contained child.
 */
void
hdy_column_set_maximum_width (HdyColumn *self,
                              gint       maximum_width)
{
  g_return_if_fail (HDY_IS_COLUMN (self));

  self->maximum_width = maximum_width;
}
