#ifndef Magnum_Video_GStVideoImporter_h
#define Magnum_Video_GStVideoImporter_h
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

/** @file
 * @brief Class @ref Magnum::Video::GStVideoImporter
 */

#include <Corrade/Containers/Array.h>

#include <Magnum/Video/AbstractImporter.h>

#include "MagnumPlugins/GStVideoImporter/configure.h"

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifndef MAGNUM_GSTVIDEOIMPORTER_BUILD_STATIC
#ifdef GStVideoImporter_EXPORTS
#define MAGNUM_GSTVIDEOIMPORTER_EXPORT CORRADE_VISIBILITY_EXPORT
#else
#define MAGNUM_GSTVIDEOIMPORTER_EXPORT CORRADE_VISIBILITY_IMPORT
#endif
#else
#define MAGNUM_GSTVIDEOIMPORTER_EXPORT CORRADE_VISIBILITY_STATIC
#endif
#define MAGNUM_GSTVIDEOIMPORTER_LOCAL CORRADE_VISIBILITY_LOCAL
#else
#define MAGNUM_GSTVIDEOIMPORTER_EXPORT
#define MAGNUM_GSTVIDEOIMPORTER_LOCAL
#endif

namespace Magnum {
namespace Video {

/**
@brief GStreamer video importer
*/
class MAGNUM_GSTVIDEOIMPORTER_EXPORT GStVideoImporter : public AbstractImporter
{
public:
    /**
         * @brief Default constructor
         *
         * In case you want to open images, use
         * @ref GStVideoImporter(PluginManager::Manager<AbstractImporter>&)
         * instead.
         */
    explicit GStVideoImporter();

    /**
         * @brief Constructor
         *
         * The plugin needs access to plugin manager for importing images.
         */
    explicit GStVideoImporter(PluginManager::Manager<AbstractImporter>& manager);

    /** @brief Plugin manager constructor */
    explicit GStVideoImporter(PluginManager::AbstractManager& manager, const std::string& plugin);

    ~GStVideoImporter();

private:
    struct Play;

    MAGNUM_GSTVIDEOIMPORTER_LOCAL ImporterFeatures doFeatures() const override;
    MAGNUM_GSTVIDEOIMPORTER_LOCAL bool doIsOpened() const override;
    MAGNUM_GSTVIDEOIMPORTER_LOCAL void doOpenFile(const std::string& filename) override;

    MAGNUM_GSTVIDEOIMPORTER_LOCAL void doPlay() override;
    MAGNUM_GSTVIDEOIMPORTER_LOCAL void doStop() override;
    MAGNUM_GSTVIDEOIMPORTER_LOCAL void doPause() override;

    MAGNUM_GSTVIDEOIMPORTER_LOCAL Int doGetFormat() const override;
    MAGNUM_GSTVIDEOIMPORTER_LOCAL Float doGetDuration() const override;
    MAGNUM_GSTVIDEOIMPORTER_LOCAL Float doGetPosition() const override;

    MAGNUM_GSTVIDEOIMPORTER_LOCAL bool init();
    MAGNUM_GSTVIDEOIMPORTER_LOCAL void deInit();

    MAGNUM_GSTVIDEOIMPORTER_LOCAL Containers::Array<char> doData() override;

    Containers::Pointer<Play> _p{ nullptr };
};

} // namespace Video
} // namespace Magnum

#endif
