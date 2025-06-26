#include "plugins/guacvc/guacvc.h"
#include "plugins/ptr-string.h"
#include "rdp.h"

#include <guacamole/client.h>
#include <guacamole/mem.h>
#include <winpr/stream.h>

#include <stdlib.h>

static UINT guac_rdp_vc_data(IWTSVirtualChannelCallback* cb, wStream* stream) {
    /* Webcam channel currently receives no data from server */
    return CHANNEL_RC_OK;
}

static UINT guac_rdp_vc_close(IWTSVirtualChannelCallback* cb) {
    guac_rdp_vc_channel_callback* vc_cb = (guac_rdp_vc_channel_callback*) cb;
    guac_client* client = vc_cb->client;
    guac_rdp_client* rdp_client = (guac_rdp_client*) client->data;

    guac_client_log(client, GUAC_LOG_DEBUG, "WEBCAM channel closed");
    rdp_client->webcam_channel = NULL;
    guac_mem_free(vc_cb);
    return CHANNEL_RC_OK;
}

static UINT guac_rdp_vc_new_connection(IWTSListenerCallback* lcb,
        IWTSVirtualChannel* channel, BYTE* data, int* accept,
        IWTSVirtualChannelCallback** ccb) {

    guac_rdp_vc_listener_callback* listener = (guac_rdp_vc_listener_callback*) lcb;
    guac_client* client = listener->client;
    guac_rdp_client* rdp_client = (guac_rdp_client*) client->data;

    guac_client_log(client, GUAC_LOG_DEBUG, "New WEBCAM channel connection");

    guac_rdp_vc_channel_callback* channel_cb =
        guac_mem_zalloc(sizeof(guac_rdp_vc_channel_callback));
    channel_cb->parent.OnDataReceived = guac_rdp_vc_data;
    channel_cb->parent.OnClose = guac_rdp_vc_close;
    channel_cb->channel = channel;
    channel_cb->client = client;

    rdp_client->webcam_channel = channel;

    *ccb = (IWTSVirtualChannelCallback*) channel_cb;
    return CHANNEL_RC_OK;
}

static UINT guac_rdp_vc_initialize(IWTSPlugin* plugin,
        IWTSVirtualChannelManager* manager) {

    guac_rdp_vc_plugin* vc_plugin = (guac_rdp_vc_plugin*) plugin;
    guac_rdp_vc_listener_callback* listener =
        guac_mem_zalloc(sizeof(guac_rdp_vc_listener_callback));

    vc_plugin->listener_callback = listener;
    listener->client = vc_plugin->client;
    listener->parent.OnNewChannelConnection = guac_rdp_vc_new_connection;

    manager->CreateListener(manager, "WEBCAM", 0,
            (IWTSListenerCallback*) listener, NULL);

    return CHANNEL_RC_OK;
}

static UINT guac_rdp_vc_terminated(IWTSPlugin* plugin) {
    guac_rdp_vc_plugin* vc_plugin = (guac_rdp_vc_plugin*) plugin;
    guac_mem_free(vc_plugin->listener_callback);
    guac_mem_free(vc_plugin);
    return CHANNEL_RC_OK;
}

UINT DVCPluginEntry(IDRDYNVC_ENTRY_POINTS* pEntryPoints) {
#ifdef PLUGIN_DATA_CONST
    const ADDIN_ARGV* args = pEntryPoints->GetPluginData(pEntryPoints);
#else
    ADDIN_ARGV* args = pEntryPoints->GetPluginData(pEntryPoints);
#endif

    guac_client* client = (guac_client*) guac_rdp_string_to_ptr(args->argv[1]);

    guac_rdp_vc_plugin* vc_plugin = (guac_rdp_vc_plugin*)
        pEntryPoints->GetPlugin(pEntryPoints, "guacvc");

    if (vc_plugin == NULL) {
        vc_plugin = guac_mem_zalloc(sizeof(guac_rdp_vc_plugin));
        vc_plugin->parent.Initialize = guac_rdp_vc_initialize;
        vc_plugin->parent.Terminated = guac_rdp_vc_terminated;
        vc_plugin->client = client;

        pEntryPoints->RegisterPlugin(pEntryPoints, "guacvc",
                (IWTSPlugin*) vc_plugin);

        guac_client_log(client, GUAC_LOG_DEBUG, "WEBCAM plugin loaded");
    }

    return CHANNEL_RC_OK;
}
