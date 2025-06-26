#ifndef GUAC_RDP_PLUGINS_GUACVC_H
#define GUAC_RDP_PLUGINS_GUACVC_H

#include <freerdp/dvc.h>
#include <guacamole/client.h>

/** Listener callback with access to Guacamole client. */
typedef struct guac_rdp_vc_listener_callback {
    IWTSListenerCallback parent; /* must be first */
    guac_client* client;
} guac_rdp_vc_listener_callback;

/** Channel callback with access to Guacamole client. */
typedef struct guac_rdp_vc_channel_callback {
    IWTSVirtualChannelCallback parent; /* must be first */
    IWTSVirtualChannel* channel;
    guac_client* client;
} guac_rdp_vc_channel_callback;

/** Data associated with the webcam plugin. */
typedef struct guac_rdp_vc_plugin {
    IWTSPlugin parent; /* must be first */
    guac_rdp_vc_listener_callback* listener_callback;
    guac_client* client;
} guac_rdp_vc_plugin;

#endif
