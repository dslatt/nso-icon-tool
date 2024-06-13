//
// Created by Nadrino on 07/05/2023.
//

#ifndef CPP_GENERIC_TOOLBOX_BOREALIS_H
#define CPP_GENERIC_TOOLBOX_BOREALIS_H

#include "borealis.hpp"

#include <functional>
#include <string>
#include <thread>
#include <chrono>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"



namespace GenericToolbox::Borealis{

  const NVGcolor redNvgColor{nvgRGB(0xff, 0x64, 0x64)};
  const NVGcolor greenNvgColor{nvgRGB(0x00, 0xff, 0xc8)};
  const NVGcolor blueNvgColor{nvgRGB(0x00, 0xc8, 0xff)};
  const NVGcolor grayNvgColor{nvgRGB(80, 80, 80)};
  const NVGcolor orangeNvgColor{nvgRGB(208, 168, 50)};// (unsigned char) (245*0.85), (unsigned char) (198*0.85), (unsigned char) (59*0.85)


  class ProgressBarMonitorView : public brls::View {

  public:
    inline ProgressBarMonitorView() = default;
    inline ~ProgressBarMonitorView() override;

    // setters
    inline void setHeaderTitle(const std::string &header_){ _header_ = header_; }
    inline void setProgressColor(const NVGcolor &progressColor_){ _progressColor_ = progressColor_; }
    inline void setExecOnDelete(const std::function<void()> &execOnDelete_){ _execOnDelete_ = execOnDelete_; }
    inline void setTitlePtr(const std::string *titlePtr_){ _titlePtr_ = titlePtr_; }
    inline void setSubTitlePtr(const std::string *subTitlePtr_){ _subTitlePtr_ = subTitlePtr_; }
    inline void setProgressFractionPtr(const double *progressFractionPtr_){ _progressFractionPtr_ = progressFractionPtr_; }
    inline void setSubProgressFractionPtr(const double *subProgressFractionPtr_){ _subProgressFractionPtr_ = subProgressFractionPtr_; }

    // misc
    inline void resetMonitorAddresses();

    // overrides
    inline void draw(
        NVGcontext* vg, int x, int y, unsigned width, unsigned height,
        brls::Style* style, brls::FrameContext* ctx) override;

  protected:
    inline void drawProgressBar(NVGcontext *vg, int x, unsigned int width, unsigned int yPosition_, double fraction_);

  private:
    // user parameters
    std::string _header_{};
    NVGcolor _progressColor_{GenericToolbox::Borealis::greenNvgColor};
    std::function<void()> _execOnDelete_{};

    // monitoring
    const double* _subProgressFractionPtr_{nullptr};
    const double* _progressFractionPtr_{nullptr};
    const std::string* _titlePtr_{nullptr};
    const std::string* _subTitlePtr_{nullptr};
  };

  class PopupLoadingBox {

  public:
    inline PopupLoadingBox() = default;

    [[nodiscard]] inline ProgressBarMonitorView *getMonitorView() const { return _monitorView_; }
    [[nodiscard]] inline brls::Dialog *getLoadingBox() const { return _loadingBox_; }

    inline void pushView();
    inline void popView() const;
    inline bool isOnTopView() const { return ( _loadingBox_ == brls::Application::getTopStackView() ); }


  private:
    // memory handled by brls
    brls::Dialog* _loadingBox_{nullptr};
    ProgressBarMonitorView* _monitorView_{nullptr};
  };


  ProgressBarMonitorView::~ProgressBarMonitorView(){
    if( _execOnDelete_ ){ _execOnDelete_(); }
    brls::View::~View();
  }
  void ProgressBarMonitorView::resetMonitorAddresses(){
    _titlePtr_ = nullptr;
    _subTitlePtr_ = nullptr;
    _progressFractionPtr_ = nullptr;
    _subProgressFractionPtr_ = nullptr;
  }

  void ProgressBarMonitorView::draw(
      NVGcontext *vg,
      int x, int y, unsigned int width, unsigned int height,
      brls::Style *style, brls::FrameContext *ctx
      ) {

    float y_offset = 0;
    if(not _header_.empty()){
      y_offset += 12;

      // Drawing header
      nvgBeginPath(vg);
      nvgFontFaceId(vg, ctx->fontStash->regular);
      nvgFontSize(vg, float( style->Header.fontSize ) );
      nvgFillColor(vg, a(ctx->theme->textColor));
      nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
      nvgText(vg,
              float(x) + float(style->Header.rectangleWidth) + float(style->Header.padding),
              float(y) - float(height/2.),
              (_header_).c_str(), nullptr
      );

      // Separator
      nvgBeginPath(vg);
      nvgFillColor(vg, a(ctx->theme->separatorColor));
      nvgRect(vg, float(x), float(y) - float(height/2.) + float(style->Header.fontSize), float(width), 1);
      nvgFill(vg);
    }

    // Draw Title
    if( _titlePtr_ != nullptr ){
      nvgFillColor(vg, a(ctx->theme->textColor));
      nvgFontSize(vg, float(style->Label.dialogFontSize));
      nvgFontFaceId(vg, ctx->fontStash->regular);
      nvgTextLineHeight(vg, 1.0f);
      nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
      nvgBeginPath(vg);
      nvgText(vg, float(x) + float(width/2.), y + y_offset + height/2. - 1.8*style->Label.dialogFontSize, (*_titlePtr_).c_str(), nullptr);
    }

    // Draw subTitle
    if(_subTitlePtr_ != nullptr and not _subTitlePtr_->empty() ){ // .empty() does not work ?
      nvgBeginPath(vg);
      nvgFontFaceId(vg, ctx->fontStash->regular);
      nvgFontSize(vg, style->Header.fontSize);
      nvgFillColor(vg, a(ctx->theme->textColor));
      nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
      nvgText(vg, x + style->Header.rectangleWidth + style->Header.padding, y + y_offset + height / 2., (*_subTitlePtr_).c_str(), nullptr);
    }

    unsigned pBarYoffset = y + y_offset + 0.8f * height;
    if(_progressFractionPtr_ != nullptr){
      ProgressBarMonitorView::drawProgressBar(vg, x, width, pBarYoffset, *_progressFractionPtr_);
    }
    if( _subProgressFractionPtr_ != nullptr ){
      ProgressBarMonitorView::drawProgressBar(vg, x, width, pBarYoffset + 2 * 16, *_subProgressFractionPtr_);
    }

  }
  void ProgressBarMonitorView::drawProgressBar(NVGcontext *vg, int x, unsigned int width, unsigned int yPosition_, double fraction_) {
    if     ( fraction_ > 1 ) fraction_ = 1;
    else if( fraction_ < 0 ) fraction_ = 0;

    unsigned x_margin = 15;
    unsigned totalBarLength = width - 2*x_margin;
    unsigned progress_x_offset = x + x_margin;

    if( fraction_ != 1 ){
      /* Progress bar background */
      /* In official software this is more transparent instead of brighter. */
      nvgFillColor(vg, a(nvgRGBAf(1.f, 1.f, 1.f, 0.5f)));
      nvgBeginPath(vg);
      nvgRoundedRect(vg, float(progress_x_offset), float(yPosition_), float(totalBarLength), 16, 8);
      nvgFill(vg);
    }

    if( fraction_ != 0 ){
      /* Progress bar */
      nvgFillColor(vg, a(_progressColor_));
      nvgBeginPath(vg);
      nvgRoundedRect(vg, float(progress_x_offset), float(yPosition_), totalBarLength * fraction_, 16, 8);
      nvgFill(vg);
    }

  }
  void PopupLoadingBox::pushView(){
    // memory will be handled by brls
    _monitorView_ = new ProgressBarMonitorView();

    // creating a box will make the loading popup resized
    _loadingBox_ = new brls::Dialog(_monitorView_ );

    // make sure the user don't cancel while unfinished
    _loadingBox_->setCancelable( true );

    while( brls::Application::hasViewDisappearing() ){
      // wait for one extra frame before push
      std::this_thread::sleep_for(std::chrono::milliseconds( 16 ));
    }

    // push the box to the view
    brls::Application::pushView( _loadingBox_ );
  }
  void PopupLoadingBox::popView() const{
    // is another view is disappearing? -> let it's time to go
    while( brls::Application::hasViewDisappearing() ){
      // wait for one extra frame before push
      std::this_thread::sleep_for(std::chrono::milliseconds( 16 ));
    }

    // call if it's still on top
    if( this->isOnTopView() ){
      brls::Application::popView( brls::ViewAnimation::FADE );
    }
  }

}

#endif // CPP_GENERIC_TOOLBOX_BOREALIS_H
