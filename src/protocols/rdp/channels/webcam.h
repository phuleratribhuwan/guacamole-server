#ifndef GUAC_RDP_CHANNELS_WEBCAM_H
#define GUAC_RDP_CHANNELS_WEBCAM_H

#include "settings.h"
#include "channels/common-svc.h"

#include <freerdp/freerdp.h>
#include <guacamole/client.h>

/** Representation of a webcam channel. */
typedef struct guac_rdp_webcam {
    guac_client* client;          /**< Associated Guacamole client. */
    guac_rdp_common_svc* svc;     /**< The underlying static virtual channel. */
} guac_rdp_webcam;

/** Basic frame header for webcam frames sent through the channel. */
typedef struct guac_rdp_webcam_frame_header {
    uint32_t width;   /**< Width of the frame in pixels. */
    uint32_t height;  /**< Height of the frame in pixels. */
    uint32_t format;  /**< Pixel format identifier. */
    uint32_t length;  /**< Length of the frame data in bytes. */
} guac_rdp_webcam_frame_header;

/** Raw RGB24 pixel format identifier. */
#define GUAC_RDP_WEBCAM_FORMAT_RGB24 0

/* Allocate a new webcam structure. */
guac_rdp_webcam* guac_rdp_webcam_alloc(guac_client* client);

/* Free a webcam structure. */
void guac_rdp_webcam_free(guac_rdp_webcam* webcam);

/* Load the webcam plugin if enabled. */
void guac_rdp_webcam_load_plugin(rdpContext* context);

/* Send a frame over the webcam channel. */
int guac_rdp_webcam_send_frame(guac_rdp_webcam* webcam,
        const void* data, int length, int width, int height);

#endif /* GUAC_RDP_CHANNELS_WEBCAM_H */
