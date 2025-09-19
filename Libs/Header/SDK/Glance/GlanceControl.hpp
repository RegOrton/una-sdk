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

#include "SDK/Glance/GlanceControl.h"

#include <cstdint>
#include <cstdbool>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <stdlib.h>

namespace SDK::Glance {

    class Control {
    public:
        Control(GlanceControl_t& control, GlanceType_t t)
            : mControl(control)
        {
            memset(&mControl, 0, sizeof(GlanceControl_t));

            mControl.type = t;
        }

        virtual ~Control() = default;

        void invalidate()
        {
            mControl.valid = false;
        }

    private:
        GlanceControl_t& mControl;
    };

    class ControlText : public Control {
    public:
        ControlText(GlanceControl_t& control)
            : Control(control, GLANCE_TYPE_TEXT)
            , mText(control.payload.text)
        {}

        virtual ~ControlText() = default;

        ControlText& init(GlancePoint_t pos, const char* s, GlanceFont_t fontID, uint8_t color)
        {
            const char* str = (s == nullptr) ? "" : s;

            if ((strlen(str) >= GLANCE_TEXT_SIZE)) {
                return *this;
            }
            
            mText.pos    = pos;
            mText.font   = fontID;
            mText.color  = color;
            strcpy(mText.str, str);

            invalidate();

            return *this;
        }

        ControlText& setText(const char* s = nullptr)
        {
            const char* str = (s == nullptr) ? "" : s;

            if ((strlen(str) >= GLANCE_TEXT_SIZE)) {
                return *this;
            }
            
            strcpy(mText.str, str);

            invalidate();

            return *this;
        }

        ControlText& at(GlancePoint_t pos)
        {
            mText.pos = pos;

            invalidate();

            return *this;
        }

        ControlText& font(GlanceFont_t id)
        {
            mText.font = id;

            invalidate();

            return *this;
        }

        ControlText& color(uint8_t color)
        {
            mText.color = color;

            invalidate();

            return *this;
        }

        ControlText& appendText(const char* s)
        {
            if (s == nullptr) {
                return *this;
            }

            const std::size_t cur = std::strlen(mText.str);

            if (cur + 1 >= GLANCE_TEXT_SIZE) {
                return *this;
            }

            const std::size_t cap = GLANCE_TEXT_SIZE - cur - 1;
            std::strncat(mText.str, s, cap);

            invalidate();

            return *this;
        }

        ControlText& print(const char* fmt, ...)
        {
          va_list args;
            va_start(args, fmt);
        #if defined(_MSC_VER)
            _vsnprintf_s(mText.str, GLANCE_TEXT_SIZE, _TRUNCATE, fmt, args);
        #else
            std::vsnprintf(mText.str, GLANCE_TEXT_SIZE, fmt, args);
            mText.str[GLANCE_TEXT_SIZE - 1] = '\0';
        #endif
            va_end(args);

            invalidate();

            return *this;
        }
    private:
        GlanceText_t& mText;
    };


    class ControlImage : public Control {
    public:
        ControlImage(GlanceControl_t& control)
            : Control(control, GLANCE_TYPE_IMAGE)
            , mImage(control.payload.image)
        {}

        virtual ~ControlImage() = default;

        ControlImage& init(GlancePoint_t pos, GlanceSize_t size, const uint8_t* buff)
        {
            mImage.pos  = pos;
            mImage.size = size;
            mImage.buff = buff;

            invalidate();

            return *this;
        }

        ControlImage& setImage(const uint8_t* buff)
        {
            mImage.buff = buff;

            invalidate();

            return *this;
        }

        ControlImage& at(GlancePoint_t pos)
        {
            mImage.pos = pos;

            invalidate();

            return *this;
        }

        /// @brief Resize for types that have GlanceSize_t.
        ControlImage& size(GlanceSize_t size)
        {
            mImage.size = size;

            invalidate();

            return *this;
        }

    private:
        GlanceImage_t& mImage;
    };

    class ControlLine : public Control {
    public:
        ControlLine(GlanceControl_t& control)
            : Control(control, GLANCE_TYPE_LINE)
            , mLine(control.payload.line)
        {}

        virtual ~ControlLine() = default;

        ControlLine& init(GlancePoint_t start, GlancePoint_t stop, uint8_t color)
        {
            mLine.start = start;
            mLine.stop  = stop;
            mLine.color = color;

            invalidate();

            return *this;
        }

        ControlLine& at(GlancePoint_t start)
        {
            mLine.start = start;

            invalidate();

            return *this;
        }

        ControlLine& to(GlancePoint_t stop)
        {
            mLine.stop = stop;

            invalidate();

            return *this;
        }
        
        ControlLine& color(uint8_t color)
        {
            mLine.color = color;

            invalidate();

            return *this;
        }

    private:
        GlanceLine_t& mLine;
    };

    class ControlRectangle : public Control {
    public:
        ControlRectangle(GlanceControl_t& control)
            : Control(control, GLANCE_TYPE_RECT)
            , mRect(control.payload.rect)
        {}

        virtual ~ControlRectangle() = default;

        ControlRectangle& init(GlancePoint_t pos,
                  GlanceSize_t  size,
                  uint8_t       lineColor,
                  uint8_t       bgColor,
                  bool          fill = true)
        {
            mRect.pos     = pos;
            mRect.size    = size;
            mRect.color   = lineColor;
            mRect.bgColor = bgColor;
            mRect.fill    = fill;

            invalidate();

            return *this;
        }

        ControlRectangle& at(GlancePoint_t pos)
        {
            mRect.pos = pos;

            invalidate();

            return *this;
        }

        ControlRectangle& size(GlanceSize_t sz)
        {
            mRect.size = sz;

            invalidate();

            return *this;
        }
        
        ControlRectangle& color(uint8_t color)
        {
            mRect.color = color;

            invalidate();

            return *this;
        }

        ControlRectangle& bgColor(uint8_t color)
        {
            mRect.bgColor = color;

            invalidate();

            return *this;
        }

        ControlRectangle& fill(bool f = true)
        {
            mRect.fill = f;

            invalidate();

            return *this;
        }

    private:
        GlanceRect_t& mRect;
    };

    template<std::size_t N>
    class Form {
    public:
        explicit Form() noexcept
            : mControls{}
            , mIndex()
        {}

        size_t size()     const noexcept { return mIndex; }
        size_t capacity() const noexcept { return N;      }

        GlanceControl_t&       data()       noexcept { return mControls; }
        const GlanceControl_t& data() const noexcept { return mControls; }

        ControlText      createText()  { return ControlText     { next(GLANCE_TYPE_TEXT)  }; }
        ControlImage     createImage() { return ControlImage    { next(GLANCE_TYPE_IMAGE) }; }
        ControlLine      createLine()  { return ControlLine     { next(GLANCE_TYPE_LINE)  }; }
        ControlRectangle createRect()  { return ControlRectangle{ next(GLANCE_TYPE_RECT)  }; }

    private:
        GlanceControl_t& next(GlanceType_t t)
        {
            assert(mIndex < N && "Form: out of capacity");

            auto& control = mControls[mIndex++];

            control.type  = t;

            return control;
        }

        GlanceControl_t mControls[N];
        size_t          mIndex;
    };
}

#endif
