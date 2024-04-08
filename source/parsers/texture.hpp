#pragma once

#include <vector>
#include <string_view>
#include <cstring>
#include <span>

namespace parsers {

    class Texture {

        class Reader {

            std::span<std::byte> data;
            std::ptrdiff_t offset = 0;

            public:

            Reader (std::span<std::byte> data) : data(data) {}

            constexpr const auto& get_span() const { return data; }
            constexpr const std::ptrdiff_t& get_offset() const { return offset; }

            template<typename T = std::nullopt_t>
            constexpr const auto get_range (std::ptrdiff_t range) const { 
                if constexpr (!std::is_same_v<T, std::nullopt_t>) 
                    return reinterpret_cast<T>(data.subspan(offset, range).data());
                else return data.subspan(offset, range);
            }

            template <typename T> T get() {
                T result;
                std::memcpy(&result, &data[offset], sizeof(T));
                offset += sizeof(T);
                return result;
            };

            template <> std::string_view get() {
                constexpr std::size_t string_length = 9;
                auto result = (char*)data.subspan(offset, string_length).data();
                offset += string_length;
                return result;
            };

            template <typename T = std::byte> 
            void skip (std::size_t amount = 1) { 
                offset += sizeof(T) * amount;
            }

        } reader;

        struct Header {

            Header (Reader& reader);

            std::string_view version;
            std::string_view index;

            enum class Format : uint32_t {
                UNKNOWN = 0xFFFFFFFF,
                ARGB8888 = 0,
                RGB888 = 1,
                RGB565 = 2,
                DXT5 = 4,
                DXT3 = 6,
                DXT1 = 7,
                RG88 = 8,
                R8 = 9,
                RG1616f = 10,
                R16f = 11,
                BC7 = 12,
                RGBa1010102 = 13,
                RGBA16161616f = 14,
                RGB161616f = 15
            } format;

            enum class Flags : uint32_t {
                NoFlags = 0,
                NoInterpolation = 1,
                ClampUVs = 2,
                IsGif = 4
            } flags;

            uint32_t texture_width;
            uint32_t texture_height;

            uint32_t width;
            uint32_t height;

            std::string_view container;
            uint32_t image_count;

            enum class Type { /// From Repkg github repo
                UNKNOWN = -1, /// Unknown format (returned value only, never use it as input value)
                BMP     = 0,  /// Windows or OS/2 Bitmap File (*.BMP)
                ICO     = 1,  /// Windows Icon (*.ICO)
                JPEG    = 2,  /// Independent JPEG Group (*.JPG, *.JIF, *.JPEG, *.JPE)
                JNG     = 3,  /// JPEG Network Graphics (*.JNG)
                KOALA   = 4,  /// Commodore 64 Koala format (*.KOA)
                LBM     = 5,  /// Amiga IFF (*.IFF, *.LBM)
                MNG     = 6,  /// Multiple Network Graphics (*.MNG)
                PBM     = 7,  /// Portable Bitmap (ASCII) (*.PBM)
                PBMRAW  = 8,  /// Portable Bitmap (BINARY) (*.PBM)
                PCD     = 9,  /// Kodak PhotoCD (*.PCD)
                PCX     = 10, /// Zsoft Paintbrush PCX bitmap format (*.PCX)
                PGM     = 11, /// Portable Graymap (ASCII) (*.PGM)
                PGMRAW  = 12, /// Portable Graymap (BINARY) (*.PGM)
                PNG     = 13, /// Portable Network Graphics (*.PNG)
                PPM     = 14, /// Portable Pixelmap (ASCII) (*.PPM)
                PPMRAW  = 15, /// Portable Pixelmap (BINARY) (*.PPM)
                RAS     = 16, /// Sun Rasterfile (*.RAS)
                TARGA   = 17, /// truevision Targa files (*.TGA, *.TARGA)
                TIFF    = 18, /// Tagged Image File Format (*.TIF, *.TIFF)
                WBMP    = 19, /// Wireless Bitmap (*.WBMP)
                PSD     = 20, /// Adobe Photoshop (*.PSD)
                CUT     = 21, /// Dr. Halo (*.CUT)
                XBM     = 22, /// X11 Bitmap Format (*.XBM)
                XPM     = 23, /// X11 Pixmap Format (*.XPM)
                DDS     = 24, /// DirectDraw Surface (*.DDS)
                GIF     = 25, /// Graphics Interchange Format (*.GIF)
                HDR     = 26, /// High Dynamic Range (*.HDR)
                FAXG3   = 27, /// Raw Fax format CCITT G3 (*.G3)
                SGI     = 28, /// Silicon Graphics SGI image format (*.SGI)
                EXR     = 29, /// OpenEXR format (*.EXR)
                J2K     = 30, /// JPEG-2000 format (*.J2K, *.J2C)
                JP2     = 31, /// JPEG-2000 format (*.JP2)
                PFM     = 32, /// portable floatmap (*.pfm)
                PICT    = 33, /// Macintosh PICT (*.PICT)
                RAW     = 34, /// RAW camera image (*.*)
            } type;

        } header;

        public:

        struct MipMap {

            MipMap (Reader& reader, const Header& header);

            uint32_t width;
            uint32_t height;

            bool compression = false;
            uint32_t size;

            std::vector<std::byte> pixels;

        };

        struct Frame {

            Frame (Reader& reader);

            uint32_t index;
            uint32_t duration;
            
            uint32_t x, y;

            uint32_t width;
            uint32_t height;

        };

        private:

        std::string_view animation_version;

        std::vector<std::vector<MipMap>> images;

        uint32_t gif_width, gif_height;
        std::vector<Frame> frames;

        public:

        Texture (std::span<std::byte> data);

        constexpr const auto& get_header() const { return header; }
        constexpr const auto& get_images() const { return images; }

    };

}