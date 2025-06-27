#include "channels/webcam.h"
#include "plugins/channels.h"
#include "rdp.h"

#include <guacamole/client.h>
#include <guacamole/mem.h>
#include <winpr/stream.h>

#include <string.h>

/* Allocate a new webcam structure. */
guac_rdp_webcam* guac_rdp_webcam_alloc(guac_client* client) {
    guac_rdp_webcam* webcam = guac_mem_alloc(sizeof(guac_rdp_webcam));
    webcam->client = client;
    webcam->svc = NULL;
    return webcam;
}

/* Free a webcam structure. */
void guac_rdp_webcam_free(guac_rdp_webcam* webcam) {
    guac_mem_free(webcam);
}

/* Handler called when the SVC connects. */
static void guac_rdp_webcam_connected(guac_rdp_common_svc* svc) {
    guac_rdp_client* rdp_client = (guac_rdp_client*) svc->client->data;
    rdp_client->webcam->svc = svc;
    svc->data = rdp_client->webcam;
    guac_client_log(svc->client, GUAC_LOG_DEBUG,
            "Webcam channel connected.");
}

/* Handler called when data is received. This implementation simply ignores
 * incoming data from the server. */
static void guac_rdp_webcam_receive(guac_rdp_common_svc* svc,
        wStream* input_stream) {
    guac_client_log(svc->client, GUAC_LOG_DEBUG,
            "Ignoring %" PRIuz " bytes received on webcam channel",
            Stream_GetRemainingLength(input_stream));
}

/* Handler called when the channel is closed. */
static void guac_rdp_webcam_terminate(guac_rdp_common_svc* svc) {
    guac_rdp_client* rdp_client = (guac_rdp_client*) svc->client->data;
    if (rdp_client->webcam != NULL)
        rdp_client->webcam->svc = NULL;
    guac_client_log(svc->client, GUAC_LOG_DEBUG,
            "Webcam channel disconnected.");
}

void guac_rdp_webcam_load_plugin(rdpContext* context) {
    guac_client* client = ((rdp_freerdp_context*) context)->client;
    /* Attempt to load a static channel named \"GUACCAM\" */
    if (guac_rdp_common_svc_load_plugin(context, "GUACCAM", 0,
                guac_rdp_webcam_connected, guac_rdp_webcam_receive,
                guac_rdp_webcam_terminate)) {
        guac_client_log(client, GUAC_LOG_WARNING,
                "Support for the webcam channel could not be loaded.");
    }
}

int guac_rdp_webcam_send_frame(guac_rdp_webcam* webcam,
        const void* data, int length, int width, int height) {

    if (webcam == NULL || webcam->svc == NULL)
        return 1;

    int packet_size = sizeof(guac_rdp_webcam_frame_header) + length;
    wStream* output = Stream_New(NULL, packet_size);

    guac_rdp_webcam_frame_header header = {
        .width = width,
        .height = height,
        .format = GUAC_RDP_WEBCAM_FORMAT_RGB24,
        .length = length
    };

    Stream_Write(output, &header, sizeof(header));
    Stream_Write(output, data, length);
    guac_rdp_common_svc_write(webcam->svc, output);

    return 0;
}

