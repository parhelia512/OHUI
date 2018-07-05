
#ifndef MOZEMBEDSIGNAL_H_6768785
#define MOZEMBEDSIGNAL_H_6768785

#include "stdafx.h"

enum {
  DOM_LOADCOMPLETED,
  EMBED_LAST_SIGNAL
};

extern guint moz_embed_signals[EMBED_LAST_SIGNAL];

struct GtkMozEmbed
{
  GtkBin    bin;
  void     *data;
};

struct GtkMozEmbedSignal
{
  GtkBinClass parent_class;
  void (* dom_loadcompleted)     (GtkMozEmbed *embed);
};

void
gtk_moz_embed_signal_init(GtkMozEmbedSignal *klass);


#endif


