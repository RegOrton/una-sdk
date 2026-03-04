#include <gui/imagemenu_screen/ImageMenuView.hpp>
#include <texts/TextKeysAndLanguages.hpp>

ImageMenuView::ImageMenuView()
{

}

void ImageMenuView::setupScreen()
{
    ImageMenuViewBase::setupScreen();

    // Set menu title
    menu1.setTitle(T_MENU);
    menu1.showTitle(true);

    // Set number of items
    menu1.setNumberOfItems(4);

    menu1.getNotSelectedItem(0)->config(T_LIST);
    menu1.getSelectedItem(0)->config(T_LIST);

    menu1.getNotSelectedItem(1)->config(T_VIEWER);
    menu1.getSelectedItem(1)->config(T_VIEWER);

    menu1.getNotSelectedItem(2)->config(T_PROPS);
    menu1.getSelectedItem(2)->config(T_PROPS);

    menu1.getNotSelectedItem(3)->config(T_BACK);
    menu1.getSelectedItem(3)->config(T_BACK);

    menu1.invalidate();
}

void ImageMenuView::tearDownScreen()
{
    ImageMenuViewBase::tearDownScreen();
}

void ImageMenuView::handleKeyEvent(uint8_t key)
{
    if (key == Gui::Config::Button::L1) {
        menu1.selectPrev();
        menu1.invalidate();
    }

    if (key == Gui::Config::Button::L2) {
        menu1.selectNext();
        menu1.invalidate();
    }

    if (key == Gui::Config::Button::R1) {
        int selected = menu1.getSelectedItem();
        switch (selected) {
        case 0:
            // Navigate to Image List
            presenter->gotoImageList();
            break;
        case 1:
            // Navigate to Image Viewer
            presenter->gotoImageViewer();
            break;
        case 2:
            // Navigate to Image Properties
            presenter->gotoImageProp();
            break;
        case 3:
            // Go back
            presenter->back();
            break;
        }
    }

    if (key == Gui::Config::Button::R2) {
        presenter->back();
    }
}
