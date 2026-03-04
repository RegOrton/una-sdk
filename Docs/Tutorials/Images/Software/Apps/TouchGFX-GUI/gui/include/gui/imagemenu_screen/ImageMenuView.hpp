#ifndef IMAGEMENUVIEW_HPP
#define IMAGEMENUVIEW_HPP

#include <gui_generated/imagemenu_screen/ImageMenuViewBase.hpp>
#include <gui/imagemenu_screen/ImageMenuPresenter.hpp>

class ImageMenuView : public ImageMenuViewBase
{
public:
    ImageMenuView();
    virtual ~ImageMenuView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    void handleKeyEvent(uint8_t key);

protected:
};

#endif // IMAGEMENUVIEW_HPP
