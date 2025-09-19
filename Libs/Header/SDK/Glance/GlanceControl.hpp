/**
 ******************************************************************************
 * @file    GlanceControl.hpp
 * @date    18-September-2025
 * @author  Oleksandr Tymoshenko <oleksandr.tymoshenko@droid-technologies.com>
 * @brief   API for use in Glance applications
 * 
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef __GLANCE_CONTROL_HPP
#define __GLANCE_CONTROL_HPP

#include <cstdint>
#include <cstdbool>
#include <variant>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <type_traits>

namespace SDK::Glance {

    enum Color : uint8_t {
        WHITE        = 0x3F, // C0C0C0 -> 11 11 11
        GRAY         = 0x2A, // 808080 -> 10 10 10
        GRAY_DARK    = 0x15, // 404040 -> 01 01 01
        BLACK        = 0x00, // 000000 -> 00 00 00
        YELLOW_DARK  = 0x38, // C08000 -> 11 10 00
        TEAL         = 0x0A, // 008080 -> 00 10 10
        TEAL_DARK    = 0x05, // 004040 -> 00 01 01
        GREEN        = 0x0C, // 00C000 -> 00 11 00
        DARK_GREEN   = 0x04, // 004000 -> 00 01 00
        BROWN        = 0x39, // C08040 -> 11 10 01
        RED          = 0x30, // C00000 -> 11 00 00
        DARK_RED     = 0x10, // 400000 -> 01 00 00
        BLUE         = 0x03, // 0000C0 -> 00 00 11
        DARK_BLUE    = 0x01, // 000040 -> 00 00 01
        CYAN         = 0x1F, // 40C0C0 -> 01 11 11
        CYAN_LIGHT   = 0x2F, // 80C0C0 -> 10 11 11
    };

    enum class Font {
        POPPINS_REGULAR_18 = 0,
        POPPINS_MEDIUM_10,
        POPPINS_MEDIUM_18,
        POPPINS_MEDIUM_25,
        POPPINS_SEMIBOLD_18,
        POPPINS_SEMIBOLD_20,
        POPPINS_SEMIBOLD_25,
        POPPINS_SEMIBOLD_30,
        POPPINS_SEMIBOLD_35,
        POPPINS_ITALIC_18,
        POPPINS_ITALIC_20,
        POPPINS_LIGHTITALIC_18,
    };

    class Point {
    public:
        Point()
            : x()
            , y()
        {}

        Point(uint16_t x, uint16_t y)
            : x(x)
            , y(y)
        {}

        uint16_t x;
        uint16_t y;
    };

    class Size {
    public:
        Size()
            : w()
            , h()
        {}

        Size(uint16_t w, uint16_t h)
            : w(w)
            , h(h)
        {}

        uint16_t w;
        uint16_t h;
    };

    //// Payloads
    struct Text {
        static constexpr std::size_t kMax = 64;  ///< Buffer size incl. '\0'

        Point   pos{};                                  ///< Text position
        char    str[kMax]{};                            ///< Storage for text (NUL-terminated)
        Font    fontID = Font::POPPINS_REGULAR_18;      ///< Default font
        uint8_t color  = static_cast<uint8_t>(Color::WHITE);  ///< Default color

        /// @brief Replace text with a C-string, clamped to kMax-1 chars.
        void set(const char* s)
        {
            if (!s) {
                str[0] = '\0';
                return;
            }

            // snprintf ensures null-termination
            std::snprintf(str, kMax, "%s", s);
        }

        /// @brief Append C-string, clamped to buffer.
        void append(const char* s)
        {
            if (!s || !*s) {
                return;
            }

            const std::size_t cur = std::strlen(str);

            if (cur + 1 >= kMax) {
                return;
            }

            const std::size_t cap = kMax - cur - 1;
            std::strncat(str, s, cap);
            str[kMax - 1] = '\0';
        }

        /// @brief printf-like into the same buffer.
        void print(const char* fmt, ...)
        {
            if (!fmt) {
                str[0] = '\0';
                return;
            }

            va_list args;
            va_start(args, fmt);
        #if defined(_MSC_VER)
            _vsnprintf_s(str, kMax, _TRUNCATE, fmt, args);
        #else
            std::vsnprintf(str, kMax, fmt, args);
            str[kMax - 1] = '\0';
        #endif
            va_end(args);
        }
    };

    struct Image {
        Point          pos{};
        Size           size{};
        const uint8_t* buff = nullptr; // Points to persistent image data
    };

    struct Line {
        Point   pos{};
        Point   stop{};
        uint8_t color = Color::WHITE;
    };

    struct Rectangle {
        Point   pos{};
        Size    size{};
        uint8_t color   = Color::WHITE; // Color Line
        uint8_t bgColor = Color::BLACK;
        bool    fill    = false;
    };

    using Data = std::variant<Text, Image, Line, Rectangle>;

    class Control {
    public:
        Control()
            : mData(Text{})
            , mID(0)
            , mValid(false)
        {}

        const Data& data()
        {
            return mData;
        }

        void setID(uint8_t id)
        {
            mID = id;
        }

        uint8_t getID()
        {
            return mID;
        }

        void validate()
        {
            mValid = true;
        }

        bool isValid() const
        {
            return mValid;
        }

        ///////////////////////////////////////
        //// Full "emplace"
        ///////////////////////////////////////

        Control& createText(Point pos, const char* s, Font fontID, uint8_t color)
        {
            Text t;

            t.pos    = pos;
            t.fontID = fontID;
            t.color  = color;
            t.set(s);

            mData = t;

            invalidate();

            return *this;
        }

        Control& createImage(Point p, Size sz, const uint8_t* buff)
        {
            mData = Image{ p, sz, buff };

            invalidate();

            return *this;
        }

        Control& createLine(Point a, Point b, uint8_t color)
        {
            mData = Line{ a, b, color };

            invalidate();

            return *this;
        }

        Control& createRectangle(Point   pos,
                                 Size    size,
                                 uint8_t lineColor,
                                 uint8_t bgColor,
                                 bool    fill=true)
        {
            mData = Rectangle{ pos, size, lineColor, bgColor, fill };

            invalidate();

            return *this;
        }

        ///////////////////////////////////////
        //// Minimal "emplace"
        ///////////////////////////////////////


        // Minimal setText that only updates text on existing Text payload,
        // or creates a default Text once and preserves style/pos next time.
        Control& createText(const char* s = nullptr)
        {
            Text& t = ensureText();

            t.set(s);

            invalidate();

            return *this;
        }

        Control& createImage(const uint8_t* buff)
        {
            Image& image = ensureImage();

            image.buff = buff;

            invalidate();

            return *this;
        }

        Control& createLine()
        {
            mData = ensureLine();

            invalidate();

            return *this;
        }

        Control& createRectangle()
        {
            mData = ensureRectangle();

            invalidate();

            return *this;
        }

        ///////////////////////////////////////
        //// Fluent helpers
        ///////////////////////////////////////

        Control& appendText(const char* s)
        {
            Text& t = ensureText();

            t.append(s);

            invalidate();

            return *this;
        }

        Control& print(const char* fmt, ...)
        {
            Text& t = ensureText();

            va_list args;
            va_start(args, fmt);
        #if defined(_MSC_VER)
            _vsnprintf_s(t.str, Text::kMax, _TRUNCATE, fmt, args);
        #else
            std::vsnprintf(t.str, Text::kMax, fmt, args);
            t.str[Text::kMax - 1] = '\0';
        #endif
            va_end(args);

            invalidate();

            return *this;
        }

        Control& font(Font fontID)
        {
            std::visit([&](auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, Text>) {
                    v.fontID = fontID;
                }
              }, mData);

            invalidate();

            return *this;
        }

        /// @brief Set position depending on active type (ext/Image/Rectangle: pos).
        Control& at(Point pos)
        {
            std::visit([&](auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, Text> || std::is_same_v<T, Image> || std::is_same_v<T, Rectangle>) {
                    v.pos = pos;
                }
              }, mData);

            invalidate();

            return *this;
        }

        /// @brief Set position depending on active type (ext/Image/Rectangle: pos).
        Control& to(Point stop)
        {
            std::visit([&](auto& v) {
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, Line>) {
                    v.stop = stop;
                }
              }, mData);

            invalidate();

            return *this;
        }

        /// @brief Resize for types that have Size.
        Control& size(Size size)
        {
            std::visit([&](auto& v){
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, Image> || std::is_same_v<T, Rectangle>) {
                    v.size = size;
                }
            }, mData);

            invalidate();

            return *this;
        }

        /// @brief Update color
        Control& color(uint8_t color)
        {
            std::visit([&](auto& v){
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, Text> || std::is_same_v<T, Line> || std::is_same_v<T, Rectangle>) {
                    v.color = color;
                }
            }, mData);

            invalidate();

            return *this;
        }

        /// @brief Update color
        Control& bgColor(uint8_t color)
        {
            std::visit([&](auto& v){
                using T = std::decay_t<decltype(v)>;
                if constexpr (std::is_same_v<T, Rectangle>) {
                    v.bgColor = color;
                }
            }, mData);

            invalidate();

            return *this;
        }

        /// @brief Set rectangle fill flag.
        Control& fill(bool t = true)
        {
            if (auto* v = std::get_if<Rectangle>(&mData)) {
                v->fill = t;
            }

            invalidate();

            return *this;
        }

    private:
        /// Ensure variant holds Text; if not, switch once to default Text.
        Text& ensureText()
        {
            if (auto* t = std::get_if<Text>(&mData)) {
                return *t;
            }

            mData = Text{};

            return std::get<Text>(mData);
        }

        /// Ensure variant holds Image; if not, switch once to default Image.
        Image& ensureImage()
        {
            if (auto* t = std::get_if<Image>(&mData)) {
                return *t;
            }

            mData = Image{};

            return std::get<Image>(mData);
        }

        /// Ensure variant holds Image; if not, switch once to default Image.
        Line& ensureLine()
        {
            if (auto* t = std::get_if<Line>(&mData)) {
                return *t;
            }

            mData = Line{};

            return std::get<Line>(mData);
        }

        /// Ensure variant holds Image; if not, switch once to default Image.
        Rectangle& ensureRectangle()
        {
            if (auto* t = std::get_if<Rectangle>(&mData)) {
                return *t;
            }

            mData = Rectangle{};

            return std::get<Rectangle>(mData);
        }

        void invalidate()
        {
            mValid = false;
        }

        Data     mData;
        uint32_t mID;
        bool     mValid;
    };
}

#endif
