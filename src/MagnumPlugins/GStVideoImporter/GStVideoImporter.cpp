/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
              Vladimír Vondruš <mosra@centrum.cz>
    Copyright © 2017 Jonathan Hale <squareys@googlemail.com>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include "GStVideoImporter.h"
#include "GStVideoRenderer.h"

#include <unordered_map>

#include <gst/gst.h>
#include <gst/gl/gl.h>
#include <gst/player/gstplayer.h>
#include <gst/app/gstappsink.h>
#include <gst/gl/x11/gstgldisplay_x11.h>

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/Optional.h>

#include <Corrade/Utility/Algorithms.h>
#include <Corrade/Utility/ConfigurationGroup.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Directory.h>
#include <Corrade/Utility/String.h>

#include <Magnum/FileCallback.h>
#include <Magnum/Mesh.h>
#include <Magnum/Math/Vector.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/PixelStorage.h>

namespace {

void
print_video_info(GstPlayerVideoInfo* info)
{
    gint fps_n, fps_d;
    guint par_n, par_d;

    if (info == NULL)
        return;

    g_print("  width : %d\n", gst_player_video_info_get_width(info));
    g_print("  height : %d\n", gst_player_video_info_get_height(info));
    g_print("  max_bitrate : %d\n",
            gst_player_video_info_get_max_bitrate(info));
    g_print("  bitrate : %d\n", gst_player_video_info_get_bitrate(info));
    gst_player_video_info_get_framerate(info, &fps_n, &fps_d);
    g_print("  framerate : %.2f\n", static_cast<gdouble>(fps_n) / fps_d);
    gst_player_video_info_get_pixel_aspect_ratio(info, &par_n, &par_d);
    g_print("  pixel-aspect-ratio  %u:%u\n", par_n, par_d);
}

void
print_version()
{
    gchar* version_str;

    version_str = gst_version_string();
    g_print("%s\n", version_str);
    g_free(version_str);
}

static void
end_of_stream_cb(GstPlayer* /* gstplayer */, gpointer /* user_data */)
{
    g_print("Reached end of play list.\n");
}

static void
player_state_changed(GstPlayer* /* gstplayer */, GstPlayerState state, gpointer /* user_data */)
{
    g_print("state changed to %s\n", gst_player_state_get_name(state));
}

static void
player_error(GstPlayer* /* gstplayer */, GError* error, gpointer /* user_data */)
{
    g_print("####BEGINERROR\n%s\nENDERROR#####\n", error->message);
}

static gboolean
sync_bus_call(GstBus* /* bus */, GstMessage* msg, gpointer /* data */)
{
    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_NEED_CONTEXT: {
        const gchar* context_type;

        gst_message_parse_context_type(msg, &context_type);
        g_print("GStreamer player needs context %s\n", context_type);

        //   if (g_strcmp0 (context_type, GST_GL_DISPLAY_CONTEXT_TYPE) == 0) {
        //     GstContext *display_context =
        //         gst_context_new (GST_GL_DISPLAY_CONTEXT_TYPE, TRUE);
        //     gst_context_set_gl_display (display_context, sdl_gl_display);
        //     gst_element_set_context (GST_ELEMENT (msg->src), display_context);
        //     return TRUE;
        //   } else if (g_strcmp0 (context_type, "gst.gl.app_context") == 0) {
        //     GstContext *app_context = gst_context_new ("gst.gl.app_context", TRUE);
        //     GstStructure *s = gst_context_writable_structure (app_context);
        //     gst_structure_set (s, "context", GST_TYPE_GL_CONTEXT, sdl_context,
        //         NULL);
        //     gst_element_set_context (GST_ELEMENT (msg->src), app_context);
        //     return TRUE;
        //   }
        break;
    }
    default:
        break;
    }
    return FALSE;
}

GstPlayer*
create_player()
{
    GstPlayerVideoRenderer* renderer = gst_player_magnum_video_renderer_new();
    G_TYPE_CHECK_INSTANCE_TYPE(renderer, gst_player_magnum_video_renderer_get_type());
    GstPlayer* player = gst_player_new(renderer, NULL); // gst_player_g_main_context_signal_dispatcher_new(NULL));

    g_signal_connect(player, "state-changed", G_CALLBACK(player_state_changed), NULL);
    g_signal_connect(player, "error", G_CALLBACK(player_error), NULL);
    g_signal_connect(player, "end-of-stream", G_CALLBACK(end_of_stream_cb), NULL);

    GstPipeline* pipeline = GST_PIPELINE(gst_player_get_pipeline(player));

    GstBus* bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_signal_watch(bus);
    // g_signal_connect (bus, "message::error", G_CALLBACK (end_stream_cb), loop);
    // g_signal_connect (bus, "message::warning", G_CALLBACK (end_stream_cb), loop);
    // g_signal_connect (bus, "message::eos", G_CALLBACK (end_stream_cb), loop);
    gst_bus_enable_sync_message_emission(bus);
    g_signal_connect(bus, "sync-message", G_CALLBACK(sync_bus_call), NULL);
    gst_object_unref(bus);

    // g_thread_new(NULL, mainrunner, NULL);

    return player;
}
} // namespace

namespace Magnum {
namespace Video {

struct GStVideoImporter::Play
{
    GstPlayer* player{ nullptr };
    Containers::Array<char> data;
    bool isOpened = false;
};

GStVideoImporter::GStVideoImporter() { init(); }

GStVideoImporter::GStVideoImporter(PluginManager::Manager<AbstractImporter>& manager) : AbstractImporter(manager) { init(); }

GStVideoImporter::GStVideoImporter(PluginManager::AbstractManager& manager, const std::string& plugin) : AbstractImporter(manager, plugin) { init(); }

GStVideoImporter::~GStVideoImporter()
{
    deInit();
}

ImporterFeatures
GStVideoImporter::doFeatures() const
{
    return ImporterFeature::OpenData | ImporterFeature::OpenFile | ImporterFeature::OpenURL;
}

bool
GStVideoImporter::doIsOpened() const
{
    return _p && _p->isOpened;
}

void
GStVideoImporter::doOpenFile(const std::string& filename)
{
    Debug{} << "GStVideoImporter doOpenFile: " << filename;

    if (!_p)
    {
        _p.reset(new Play);
        _p->player = create_player();
    }

    gst_player_set_uri(_p->player, filename.c_str());
    _p->isOpened = true;
}

bool
GStVideoImporter::init()
{
    Debug{} << "GStVideoImporter init";
    gst_init(NULL, NULL);
    print_version();
    return true;
}

void
GStVideoImporter::deInit()
{
    Debug{} << "GStVideoImporter deInit";
    gst_deinit();
}

void
GStVideoImporter::doPlay()
{
    Debug{} << "GStVideoImporter doPlay";
    gst_player_play(_p->player);
}

void
GStVideoImporter::doStop()
{
    _p->isOpened = false;
    Debug{} << "GStVideoImporter doStop";
    gst_player_stop(_p->player);
}

void
GStVideoImporter::doPause()
{
    Debug{} << "GStVideoImporter doStop";
    gst_player_pause(_p->player);
}

Int
GStVideoImporter::doGetFormat() const
{
    Debug{} << "GStVideoImporter doGetFormat";
    return 0;
}

Float
GStVideoImporter::doGetDuration() const
{
    Debug{} << "GStVideoImporter doGetDuration";
    return gst_player_get_duration(_p->player) / 1000000000.0f;
}

Float
GStVideoImporter::doGetPosition() const
{
    Debug{} << "GStVideoImporter doGetPosition";
    return gst_player_get_position(_p->player) / 1000000000.0f;
}

Containers::Array<char>
GStVideoImporter::doData()
{
    Containers::Array<char> copy(_p->data.size());
    std::copy(_p->data.begin(), _p->data.end(), copy.begin());
 
    return copy;
}

} // namespace Video
} // namespace Magnum

CORRADE_PLUGIN_REGISTER(GStVideoImporter, Magnum::Video::GStVideoImporter,
                        "cz.mosra.magnum.Video.AbstractImporter/0.1")
