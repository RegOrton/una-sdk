#include <gui/imagemenu_screen/ImageMenuView.hpp>
#include <gui/imagemenu_screen/ImageMenuPresenter.hpp>

ImageMenuPresenter::ImageMenuPresenter(ImageMenuView& v)
    : view(v)
{

}

void ImageMenuPresenter::activate()
{

}

void ImageMenuPresenter::deactivate()
{

}

void ImageMenuPresenter::gotoImageList()
{
    static_cast<FrontendApplication*>(Application::getInstance())->gotoImageListScreenNoTransition();
}

void ImageMenuPresenter::gotoImageViewer()
{
    static_cast<FrontendApplication*>(Application::getInstance())->gotoImageViewerScreenNoTransition();
}

void ImageMenuPresenter::gotoImageProp()
{
    static_cast<FrontendApplication*>(Application::getInstance())->gotoImagePropScreenNoTransition();
}

void ImageMenuPresenter::back()
{
    model->exitApp();
}
