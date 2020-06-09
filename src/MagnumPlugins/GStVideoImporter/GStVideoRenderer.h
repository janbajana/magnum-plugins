#ifndef Magnum_GStVideoRenderer_h
#define Magnum_GStVideoRenderer_h

#include <gst/gst.h>
#include <gst/player/gstplayer.h>

typedef struct _GstPlayerMagnumVideoRenderer
    GstPlayerMagnumVideoRenderer;
typedef struct _GstPlayerMagnumVideoRendererClass
    GstPlayerMagnumVideoRendererClass;

static void
gst_player_magnum_video_renderer_interface_init(GstPlayerVideoRendererInterface* iface);

#define GST_TYPE_PLAYER_MAGNUM_VIDEO_RENDERER (gst_player_magnum_video_renderer_get_type())
#define GST_IS_PLAYER_MAGNUM_VIDEO_RENDERER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_PLAYER_MAGNUM_VIDEO_RENDERER))
#define GST_IS_PLAYER_MAGNUM_VIDEO_RENDERER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_PLAYER_MAGNUM_VIDEO_RENDERER))
#define GST_PLAYER_MAGNUM_VIDEO_RENDERER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), GST_TYPE_PLAYER_MAGNUM_VIDEO_RENDERER, GstPlayerGtkVideoRendererClass))
#define GST_PLAYER_MAGNUM_VIDEO_RENDERER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_PLAYER_MAGNUM_VIDEO_RENDERER, GstPlayerGtkVideoRenderer))
#define GST_PLAYER_MAGNUM_VIDEO_RENDERER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_PLAYER_MAGNUM_VIDEO_RENDERER, GstPlayerGtkVideoRendererClass))
#define GST_PLAYER_MAGNUM_VIDEO_RENDERER_CAST(obj) ((GstPlayerGtkVideoRenderer*)(obj))

GType
gst_player_magnum_video_renderer_get_type(void);

GstPlayerVideoRenderer*
gst_player_magnum_video_renderer_new(void);

#endif