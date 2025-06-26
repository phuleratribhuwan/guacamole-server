#include "channels/webcam/webcam.h"
#include "plugins/channels.h"
#include "plugins/ptr-string.h"
#include "rdp.h"

#include <freerdp/freerdp.h>
#include <guacamole/client.h>
#include <guacamole/protocol.h>
#include <guacamole/socket.h>
#include <guacamole/stream.h>
#include <guacamole/user.h>
#include <winpr/stream.h>

int guac_rdp_webcam_handler(guac_user* user, guac_stream* stream,
        char* mimetype, char* name) {

    guac_client* client = user->client;
    guac_rdp_client* rdp_client = (guac_rdp_client*) client->data;

    /* Accept only if webcam channel is available */
    if (rdp_client->webcam_channel == NULL) {
        guac_protocol_send_ack(user->socket, stream, "FAIL", GUAC_PROTOCOL_STATUS_SERVER_ERROR);
        guac_socket_flush(user->socket);
        return 0;
    }

    stream->data = rdp_client;
    stream->blob_handler = guac_rdp_webcam_blob_handler;
    stream->end_handler = guac_rdp_webcam_end_handler;

    guac_protocol_send_ack(user->socket, stream, "OK", GUAC_PROTOCOL_STATUS_SUCCESS);
    guac_socket_flush(user->socket);
    return 0;
}

int guac_rdp_webcam_blob_handler(guac_user* user, guac_stream* stream,
        void* data, int length) {

    guac_rdp_client* rdp_client = (guac_rdp_client*) stream->data;
    IWTSVirtualChannel* channel = rdp_client->webcam_channel;
    if (channel == NULL)
        return 0;

    wStream* out = Stream_New(NULL, length);
    Stream_Write(out, data, length);

    pthread_mutex_lock(&(rdp_client->message_lock));
    channel->Write(channel, (UINT32) Stream_GetPosition(out), Stream_Buffer(out), NULL);
    pthread_mutex_unlock(&(rdp_client->message_lock));

    Stream_Free(out, TRUE);
    return 0;
}

int guac_rdp_webcam_end_handler(guac_user* user, guac_stream* stream) {
    return 0;
}

void guac_rdp_webcam_load_plugin(rdpContext* context) {
    guac_client* client = ((rdp_freerdp_context*) context)->client;
    char client_ref[GUAC_RDP_PTR_STRING_LENGTH];

    guac_rdp_ptr_to_string(client, client_ref);
    guac_freerdp_dynamic_channel_collection_add(context->settings, "guacvc", client_ref, NULL);
}

