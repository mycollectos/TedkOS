#ifndef _INC_COMPOSITOR_H_
#define _INC_COMPOSITOR_H_

#include <stdint.h>
#include <stddef.h>
#include <inc/x86/real.h>
#include <inc/ui/vbe.h>
#include <inc/klibs/maybe.h>
#include <inc/drivers/ievent.h>
#include <inc/drivers/kkc.h>

using vbe::VBEMemHelp;

namespace ui {

class Drawable;
class Container;
class Window;

constexpr int32_t ScreenWidth = 1024;
constexpr int32_t ScreenHeight = 768;

template<int32_t width, int32_t height>
constexpr size_t RGBASize = width * height * 4;

template<int32_t width, int32_t height>
constexpr size_t RGBSize = width * height * 3;

constexpr size_t CalcRGBASize(int32_t width, int32_t height) { return width * height * 4; }
constexpr size_t CalcRGBSize(int32_t width, int32_t height) { return width * height * 3; }

enum DisplayMode {
    Video,
    Text
};

struct Rectangle {
    const int32_t x1;     // top left
    const int32_t y1;
    const int32_t x2;     // bottom right
    const int32_t y2;

    constexpr Rectangle(int32_t a, int32_t b, int32_t c, int32_t d)
        : x1(a), y1(b), x2(c), y2(d) { }

    static int32_t BoundX(int32_t input) { return input < 0 ? 0 : input > ScreenWidth ? ScreenWidth : input; }
    static int32_t BoundY(int32_t input) { return input < 0 ? 0 : input > ScreenHeight ? ScreenHeight : input; }

    const Rectangle bound() const {
        return Rectangle { .x1 = BoundX(x1), .y1 = BoundY(y1), .x2 = BoundX(x2), .y2 = BoundY(y2) };
    }

    bool hasPoint(int32_t x, int32_t y) const
    {
        return x >= x1 && x <= x2 && y >= y1 && y <= y2;
    }

    bool overlapsWith(const Rectangle& r2) const
    {
        if (x1 > r2.x2 || r2.x1 > x2) return false;
        if (y1 > r2.y2 || r2.y1 > y2) return false;
        return true;
    }
};

constexpr Rectangle EmptyRectangle(-1000, -1000, -2000, -2000);

typedef uint8_t (RGBAGroup)[4];
typedef uint8_t (RGBGroup)[3];
typedef RGBAGroup (PixelRow)[ScreenWidth];
typedef RGBGroup (RGBPixelRow)[ScreenWidth];

class Compositor : public KeyB::IEvent {
    private:
        real_context_t real_context;

        uint16_t originalVideoModeIndex;
        uint16_t videoModeIndex;

        // buildBuffer format: RGBA. + alignment.
        RGBPixelRow *buildBuffer;
        uint8_t* videoMemory;
        DisplayMode displayMode;

        Maybe<vbe::VideoModeInfo> infoMaybe;

        VBEMemHelp drawHelper;

        // Compositor singleton object
        static Compositor *comp;
        Compositor();

        void enterVideoMode();
        void enterTextMode();

        Container *theMouse;

    public:
        static Compositor* getInstance();

        void drawNikita();

        Container *rootContainer;

        // redraw entire rectangular area (costly!)
        void redraw(const Rectangle &rect);

        // draw single element
        void drawSingle(const Container *d, const Rectangle &rect);
        void drawSingle(const Container *d, const Rectangle &rect, const Rectangle &difference);

        Drawable* addText(int txtX, int txtY, char c);

        Container *getElementAtPosition(int absX, int absY);

        // Unit: text font width
        int32_t txtX = 0;

    public:
        virtual void key(uint32_t kkc, bool capslock);

        // Down and Up cuts changes to ONE single key at a time.
        virtual void keyDown(uint32_t kkc, bool capslock)
        {
        }

        virtual void keyUp(uint32_t kkc, bool capslock)
        {
        }

        virtual void show()
        {
            enterVideoMode();
        }

        virtual void hide()
        {
            enterTextMode();
        }
};

float alphaBlending(float p1, float p2, float alpha);

}

#endif
