#ifndef GUAC_RDP_CHANNELS_WEBCAM_H
#define GUAC_RDP_CHANNELS_WEBCAM_H

#include <freerdp/freerdp.h>
#include <guacamole/user.h>

/* Handler for inbound webcam streams */
int guac_rdp_webcam_handler(guac_user* user, guac_stream* stream,
        char* mimetype, char* name);

/* Handler for data blobs within webcam streams */
int guac_rdp_webcam_blob_handler(guac_user* user, guac_stream* stream,
        void* data, int length);

/* Handler for end of webcam streams */
int guac_rdp_webcam_end_handler(guac_user* user, guac_stream* stream);

/* Adds Guacamole's webcam plugin to the dynamic channel list */
void guac_rdp_webcam_load_plugin(rdpContext* context);

#endif
