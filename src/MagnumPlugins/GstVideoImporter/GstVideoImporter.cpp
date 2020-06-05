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

#include "GstVideoImporter.h"

#include <unordered_map>

#include <gst/gst.h>
#include <gst/player/gstplayer.h>

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
#include <Magnum/Trade/ArrayAllocator.h>
#include <Magnum/Trade/CameraData.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/LightData.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Trade/MeshObjectData3D.h>
#include <Magnum/Trade/PhongMaterialData.h>
#include <Magnum/Trade/SceneData.h>
#include <Magnum/Trade/TextureData.h>
#include <MagnumPlugins/AnyImageImporter/AnyImageImporter.h>

namespace {
static void
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

} // namespace

namespace Magnum {
namespace Trade {

GstVideoImporter::GstVideoImporter() { init(); }

GstVideoImporter::GstVideoImporter(PluginManager::Manager<AbstractImporter>& manager) : AbstractImporter(manager) { init(); }

GstVideoImporter::GstVideoImporter(PluginManager::AbstractManager& manager, const std::string& plugin) : AbstractImporter(manager, plugin) { init(); }

GstVideoImporter::~GstVideoImporter()
{
    deInit();
}

ImporterFeatures
GstVideoImporter::doFeatures() const
{
    return ImporterFeature::OpenData | ImporterFeature::OpenState | ImporterFeature::FileCallback;
}

bool
GstVideoImporter::doIsOpened() const
{
    return _isOpened;
}

void
GstVideoImporter::doOpenFile(const std::string& filename)
{
    _isOpened = true;
    Debug{} << "GstVideoImporter doOpenFile: " << filename;
}

void
GstVideoImporter::doClose()
{
    _isOpened = false;
    Debug{} << "GstVideoImporter doOpenFile: ";
}

bool
GstVideoImporter::init()
{
    Debug{} << "GstVideoImporter initialized";
    gst_init(NULL, NULL);
    print_version();
    return true;
}

void
GstVideoImporter::deInit()
{
    Debug{} << "GstVideoImporter deInit";

    gst_deinit();
}

void
GstVideoImporter::doPlay()
{
}

void
GstVideoImporter::doStop()
{
}

void
GstVideoImporter::doPause()
{
}

} // namespace Trade
} // namespace Magnum

CORRADE_PLUGIN_REGISTER(GstVideoImporter, Magnum::Trade::GstVideoImporter,
                        "cz.mosra.magnum.Trade.AbstractImporter/0.3.1")
