#include "GStVideoRenderer.h"

#include <gst/gl/gl.h>
#include <gst/app/gstappsink.h>

// static GstFlowReturn
// new_sample(GstAppSink *appsink, gpointer user_data)
// {
//     printf("on client draw\n");
//     GstSample *sample = gst_app_sink_pull_sample (appsink);
//     GstBuffer *buf = gst_sample_get_buffer(sample);
//     GstCaps *caps = gst_sample_get_caps(sample);
//     GstVideoFrame v_frame;
//     GstVideoInfo v_info;
//     GstVideoGLTextureUploadMeta *upload_meta;

//     // /* FIXME don't do that every frame */
//     gst_video_info_from_caps(&v_info, caps);
//     printf("size %i %i %s\n", v_info.width, v_info.height, gst_video_format_to_string(v_info.finfo->format));

//     // upload_meta = gst_buffer_get_video_gl_texture_upload_meta(buf);
//     // printf("%p\n", upload_meta);

//     if (!gst_video_frame_map(&v_frame, &v_info, buf, GST_MAP_READ | GST_MAP_GL))
//     {
//         g_warning("Failed to map the video buffer");
//         return TRUE;
//     }

//     GLuint texture = *(guint *)v_frame.data[0];
//     printf("texture id %i\n", texture);
//     //   g_mutex_lock (&app_lock);

//     //   app_rendered = FALSE;
//     //   g_idle_add_full (G_PRIORITY_HIGH, executeCallback, &v_frame, NULL);

//     //   while (!app_rendered && !app_quit)
//     //     g_cond_wait (&app_cond, &app_lock);

//     //   g_mutex_unlock (&app_lock);

//     gst_video_frame_unmap(&v_frame);
//     gst_sample_unref(sample);

//     // return TRUE;
//     return GST_FLOW_OK;
// }

static GstElement*
create_video_sink() // GstPlayerVideoRenderer * self, GstPlayer * player)
{
    // printf("create video sink\n");
    // GstElement *glimagesink = gst_element_factory_make ("appsink", "thisapp");
    // g_assert_nonnull (glimagesink);

    // g_object_set(glimagesink, "emit-signals", 1, NULL);

    // g_signal_connect (G_OBJECT (glimagesink), "new-sample",
    //     G_CALLBACK (new_sample), NULL);

    // GstElement *glupload = gst_element_factory_make ("glupload", NULL);
    // g_assert_nonnull (glupload);

    // GstElement *bin = gst_bin_new ("uploadappsink");
    // gst_bin_add_many (GST_BIN (bin), glupload, glimagesink, NULL);
    // gst_element_link_many (glupload, glimagesink, NULL);

    // GstPad *pad = gst_element_get_static_pad (glupload, "sink");
    // GstPad *ghost_pad = gst_ghost_pad_new ("sink", pad);
    // gst_pad_set_active (ghost_pad, TRUE);
    // gst_element_add_pad (bin, ghost_pad);
    // gst_object_unref (pad);

    GstCaps* caps = gst_caps_from_string("video/x-raw(memory:GLMemory), format=RGBA");

    // videoconvert name=bla ! video/x-raw, width=320, height=240, format=RGBA ! glupload !
    GstElement* bin = gst_parse_launch("videoconvert name=first ! video/x-raw, format=RGBA ! glupload ! appsink name=glimagesink0 emit-signals=1", NULL);

    GstElement* conv = gst_bin_get_by_name(GST_BIN(bin), "first");
    GstPad* pad = gst_element_get_static_pad(conv, "sink");
    GstPad* ghost_pad = gst_ghost_pad_new("sink", pad);
    gst_pad_set_active(ghost_pad, TRUE);
    gst_element_add_pad(bin, ghost_pad);
    gst_object_unref(pad);
    gst_object_unref(conv);

    GstElement* glimagesink = gst_bin_get_by_name(GST_BIN(bin), "glimagesink0");
    // g_signal_connect (G_OBJECT (glimagesink), "new-sample",
    //     G_CALLBACK (new_sample), NULL);

    gst_app_sink_set_caps(GST_APP_SINK(glimagesink), caps);
    gst_caps_unref(caps);

    // gst_object_unref (glimagesink);

    return bin;
    // return glimagesink;
}

struct _GstPlayerMagnumVideoRenderer
{
    GObject parent;

    // GstElement *appsink;
};

struct _GstPlayerMagnumVideoRendererClass
{
    GObjectClass parent_class;
};

G_DEFINE_TYPE_WITH_CODE(GstPlayerMagnumVideoRenderer,
                        gst_player_magnum_video_renderer, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(GST_TYPE_PLAYER_VIDEO_RENDERER,
                                              gst_player_magnum_video_renderer_interface_init));

static void
gst_player_magnum_video_renderer_class_init(GstPlayerMagnumVideoRendererClass* klass)
{
    g_print("renderer class init\n");

    GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
    (void)gobject_class;

    //   gobject_class->get_property = gst_player_gtk_video_renderer_get_property;
    //   gobject_class->finalize = gst_player_gtk_video_renderer_finalize;

    //   gtk_video_renderer_param_specs
    //       [GTK_VIDEO_RENDERER_PROP_WIDGET] =
    //       g_param_spec_object ("widget", "Widget",
    //       "Widget to render the video into", GTK_TYPE_WIDGET,
    //       G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

    //   g_object_class_install_properties (gobject_class,
    //       GTK_VIDEO_RENDERER_PROP_LAST, gtk_video_renderer_param_specs);
}

static void
gst_player_magnum_video_renderer_init(GstPlayerMagnumVideoRenderer* /* self */)
{
    g_print("renderer init\n");

    //   GstElement *gtk_sink = gst_element_factory_make ("gtkglsink", NULL);

    //   if (gtk_sink) {
    //     GstElement *sink = gst_element_factory_make ("glsinkbin", NULL);
    //     g_object_set (sink, "sink", gtk_sink, NULL);

    //     self->sink = sink;
    //   } else {
    //     gtk_sink = gst_element_factory_make ("gtksink", NULL);
    //     self->sink = gst_object_ref_sink (gtk_sink);
    //   }

    //   g_assert (self->sink != NULL);

    //   g_object_get (gtk_sink, "widget", &self->widget, NULL);
}

static GstElement*
renderer_create_video_sink(GstPlayerVideoRenderer* /* self */, GstPlayer* /* player */)
{
    g_print("renderer create video sink\n");
    return create_video_sink();
}

static void
gst_player_magnum_video_renderer_interface_init(GstPlayerVideoRendererInterface* iface)
{
    g_print("renderer interface init\n");
    iface->create_video_sink = renderer_create_video_sink;
}

GstPlayerVideoRenderer*
gst_player_magnum_video_renderer_new(void)
{
    g_print("video renderer new\n");

    // GstElementFactory* factory;

    // factory = gst_element_factory_find("gtkglsink");
    // if (!factory)
    //     factory = gst_element_factory_find("gtksink");
    // if (!factory)
    //     return NULL;

    // gst_object_unref(factory);

    return static_cast<GstPlayerVideoRenderer*>(g_object_new(GST_TYPE_PLAYER_MAGNUM_VIDEO_RENDERER, NULL));
}