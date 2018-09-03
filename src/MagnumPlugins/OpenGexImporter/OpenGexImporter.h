#ifndef Magnum_Trade_OpenGexImporter_h
#define Magnum_Trade_OpenGexImporter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>

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

/** @file
 * @brief Class @ref Magnum::Trade::OpenGexImporter
 */

#include <Magnum/Trade/AbstractImporter.h>

#include "Magnum/OpenDdl/OpenDdl.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_OPENGEXIMPORTER_BUILD_STATIC
    #ifdef OpenGexImporter_EXPORTS
        #define MAGNUM_TRADE_OPENGEXIMPORTER_EXPORT CORRADE_VISIBILITY_EXPORT
    #else
        #define MAGNUM_TRADE_OPENGEXIMPORTER_EXPORT CORRADE_VISIBILITY_IMPORT
    #endif
#else
    #define MAGNUM_TRADE_OPENGEXIMPORTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL CORRADE_VISIBILITY_LOCAL
#else
#define MAGNUM_TRADE_OPENGEXIMPORTER_EXPORT
#define MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL
#endif

namespace Magnum { namespace Trade {

/**
@brief OpenGEX importer

Imports the [OpenDDL](http://openddl.org)-based [OpenGEX](http://opengex.org)
format.

Supports importing of scene, object, camera, mesh, texture and image data.

This plugin depends on the @ref Trade library and the @ref AnyImageImporter
plugin. It is built if `WITH_OPENGEXIMPORTER` is enabled when building Magnum
Plugins. To use as a dynamic plugin, you need to load
the @cpp "OpenGexImporter" @ce plugin from `MAGNUM_PLUGINS_IMPORTER_DIR`. To
use as a static plugin or as a dependency of another plugin with CMake, you
need to request the `OpenGexImporter` component of the `MagnumPlugins` package
and link to the `MagnumPlugins::OpenGexImporter` target. See
@ref building-plugins, @ref cmake-plugins and @ref plugins for more information.

@section Trade-OpenGexImporter-limitations Behavior and limitations

The plugin supports @ref Feature::OpenData and @ref Feature::FileCallback
features. The importer fully loads the top-level file during initial import,
meaning there is at most one @ref ImporterFileCallbackPolicy::LoadTemporary
callback followed by @ref ImporterFileCallbackPolicy::Close and the data can
freed right after the @ref openData() / @ref openFile() function exits. In case
of images, the files are loaded on-demand inside @ref image2D() calls with
@ref ImporterFileCallbackPolicy::LoadTemporary and
@ref ImporterFileCallbackPolicy::Close is emitted right after the file is fully
read.

-   Import of animation data is not supported at the moment.
-   `half` data type results in parsing error.
-   On OpenGL ES, usage of double type and on WebGL additionally also usage of
    64bit integer types results in parsing error.

@subsection Trade-OpenGexImporter-limitations-scenes Scene hierarchy import

-   Object-only transformations are not supported.
-   Additional material references after the first one for given geometry node
    are ignored.
-   Geometry node visibility, shadow and motion blur properties are ignored.

@subsection Trade-OpenGexImporter-limitations-camera Camera import

-   Camera type is always @ref CameraType::Perspective3D
-   Default FoV for cameras that don't have it specified is @cpp 35.0_degf @ce
-   Default near and far plane for cameras that don't have it specified is
    @cpp 0.01f @ce and @cpp 100.0f @ce
-   Aspect ratio is ignored and hardcoded to @cpp 1.0f @ce

@subsection Trade-OpenGexImporter-limitations-lights Light import

-   Light attenuation properties are not yet supported.
-   Light textures are not yet supported.

@subsection Trade-OpenGexImporter-limitations-meshes Mesh import

-   64bit indices are not supported.
-   Quads are not supported.
-   Additional mesh LoDs after the first one are ignored.
-   `w` coordinate for vertex positions and normals is ignored if present.

@subsection Trade-OpenGexImporter-limitations-materials Material import

-   Alpha mode is always @ref MaterialAlphaMode::Opaque and alpha mask always
    @cpp 0.5f @ce
-   Two-sided property is ignored, always set to @cpp false @ce
-   All materials are imported as @ref Trade::PhongMaterialData with ambient
    color always set to @cpp 0x000000ff_rgbaf @ce.
-   `emission`, `opacity` and `transparency` attributes are not supported.
-   `normal` textures are not supported.

@subsection Trade-OpenGexImporter-limitations-textures Texture import

-   Texture coordinate transformation is ignored.
-   Textures using other than the first coordinate set are not supported.
-   Texture type is always @ref Trade::TextureData::Type::Texture2D, wrapping
    is always @ref SamplerWrapping::ClampToEdge, minification and
    magnification is @ref SamplerFilter::Linear and mipmap selection is
    @ref SamplerMipmap::Linear.
-   If multiple textures have the same image filename string, given image is
    present in the image list only once. Note that only a simple string
    comparison is used without any path normalization.

@subsection Trade-OpenGexImporter-state Access to internal importer state

Generic importer for OpenDDL files is implemented in the @ref OpenDdl::Document
class available as part of this plugin and access to it is provided through
importer-specific data accessors:

-   Calling @ref importerState() returns pointer to parsed @ref OpenDdl::Document
-   Calling `*::importerState()` on data class instances returned from this
    importer return pointer to @ref OpenDdl::Structure of these types:
    -   @ref AbstractMaterialData::importerState() returns @ref OpenGex::Material
        structure
    -   @ref CameraData::importerState() returns @ref OpenGex::CameraObject
        structure
    -   @ref TextureData::importerState() returns @ref OpenGex::Texture
        structure
    -   @ref MeshData3D::importerState() returns @ref OpenGex::GeometryObject
        structure
    -   @ref ObjectData3D::importerState() returns @ref OpenGex::Node,
        @ref OpenGex::BoneNode, @ref OpenGex::GeometryNode,
        @ref OpenGex::CameraNode or @ref OpenGex::LightNode structure
*/
class MAGNUM_TRADE_OPENGEXIMPORTER_EXPORT OpenGexImporter: public AbstractImporter {
    public:
        /**
         * @brief Default constructor
         *
         * In case you want to open images, use
         * @ref OpenGexImporter(PluginManager::Manager<AbstractImporter>&)
         * instead.
         */
        explicit OpenGexImporter();

        /**
         * @brief Constructor
         *
         * The plugin needs access to plugin manager for importing images.
         */
        explicit OpenGexImporter(PluginManager::Manager<AbstractImporter>& manager);

        /** @brief Plugin manager constructor */
        explicit OpenGexImporter(PluginManager::AbstractManager& manager, const std::string& plugin);

        ~OpenGexImporter();

        /**
         * @brief Importer state
         *
         * Provides access to the parsed OpenDDL document. See class
         * documentation for more information.
         */
        const OpenDdl::Document* importerState() const {
            return static_cast<const OpenDdl::Document*>(AbstractImporter::importerState());
        }

    private:
        struct Document;

        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL Features doFeatures() const override;

        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL bool doIsOpened() const override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL void doOpenData(Containers::ArrayView<const char> data) override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL void doOpenFile(const std::string& filename) override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL void doClose() override;

        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL Int doDefaultScene() override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL UnsignedInt doSceneCount() const override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL Containers::Optional<SceneData> doScene(UnsignedInt id) override;

        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL UnsignedInt doCameraCount() const override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL Containers::Optional<CameraData> doCamera(UnsignedInt id) override;

        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL UnsignedInt doObject3DCount() const override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL Int doObject3DForName(const std::string& name) override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL std::string doObject3DName(UnsignedInt id) override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL std::unique_ptr<ObjectData3D> doObject3D(UnsignedInt id) override;

        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL UnsignedInt doLightCount() const override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL Containers::Optional<LightData> doLight(UnsignedInt id) override;

        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL UnsignedInt doMesh3DCount() const override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL Containers::Optional<MeshData3D> doMesh3D(UnsignedInt id) override;

        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL UnsignedInt doMaterialCount() const override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL Int doMaterialForName(const std::string& name) override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL std::string doMaterialName(UnsignedInt id) override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL std::unique_ptr<AbstractMaterialData> doMaterial(UnsignedInt id) override;

        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL UnsignedInt doTextureCount() const override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL Containers::Optional<TextureData> doTexture(UnsignedInt id) override;

        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL UnsignedInt doImage2DCount() const override;
        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL Containers::Optional<ImageData2D> doImage2D(UnsignedInt id) override;

        MAGNUM_TRADE_OPENGEXIMPORTER_LOCAL const void* doImporterState() const override;

        std::unique_ptr<Document> _d;
};

}}

#endif
