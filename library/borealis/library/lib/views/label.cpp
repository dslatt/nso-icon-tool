/*
    Copyright 2019-2021 natinusala
    Copyright 2019 p-sam

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <borealis/core/application.hpp>
#include <borealis/core/font.hpp>
#include <borealis/core/i18n.hpp>
#include <borealis/core/util.hpp>
#include <borealis/views/label.hpp>

namespace brls
{

#define ELLIPSIS "\u2026"

static size_t strLen(const std::string& str)
{
    size_t res = 0, inc = 0;
    while (inc < str.length())
    {
        if (str[inc] & 0x80)
            if (str[inc] & 0x20)
                if (str[inc] & 0x10)
                    inc += 4;
                else
                    inc += 3;
            else
                inc += 2;
        else
            inc += 1;
        res++;
    }
    return res;
}

static void computeLabelHeight(Label* label, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode, YGSize* size, float* originalBounds)
{
    label->setIsWrapping(false);

    float requiredHeight = originalBounds[3] - originalBounds[1];
    if (heightMode == YGMeasureModeUndefined || heightMode == YGMeasureModeAtMost)
    {
        // Grow the label vertically as much as possible
        if (heightMode == YGMeasureModeAtMost)
            size->height = std::min(requiredHeight, height);
        else
            size->height = requiredHeight;
    }
    else if (heightMode == YGMeasureModeExactly)
    {
        size->height = height;
    }
    else
    {
        fatal("Unsupported Label height measure mode: " + std::to_string(heightMode));
    }
}

static YGSize labelMeasureFunc(YGNodeRef node, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode)
{
    NVGcontext* vg       = Application::getNVGContext();
    auto* label          = (Label*)YGNodeGetContext(node);
    std::string fullText = label->getFullText();

    YGSize size = {
        .width  = width,
        .height = height,
    };

    if (fullText.empty()) {
        if (widthMode == YGMeasureModeExactly && heightMode == YGMeasureModeExactly)
        {
            return size;
        }
        else if (heightMode == YGMeasureModeExactly)
        {
            size.width = 0;
        }
        else if (widthMode == YGMeasureModeExactly)
        {
            size.height = 0;
        }
        return size;
    }

    // XXX: workaround for a Yoga bug
    if (widthMode == YGMeasureModeAtMost && (width == 0 || std::isnan(width)))
    {
        widthMode = YGMeasureModeUndefined;
        width     = NAN;
    }

    // Setup nvg state for the measurements
    nvgFontSize(vg, label->getFontSize());
    nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgFontFaceId(vg, label->getFont());
    nvgTextLineHeight(vg, label->getLineHeight());

    // Measure the needed width for the ellipsis
    float bounds[4];
    nvgTextBounds(vg, 0, 0, ELLIPSIS, nullptr, bounds);
    float ellipsisWidth = bounds[2] - bounds[0];
    label->setEllipsisWidth(ellipsisWidth);

    // Measure the needed width for the fullText
    nvgTextBounds(vg, 0, 0, fullText.c_str(), nullptr, bounds);
    float requiredWidth = bounds[2] - bounds[0] - 0.5f;
    label->setRequiredWidth(requiredWidth);

    // XXX: This is an approximation since the given width here may not match the actual final width of the view
    float availableWidth = std::isnan(width) ? std::numeric_limits<float>::max() : width;

    // Width
    if (widthMode == YGMeasureModeUndefined || widthMode == YGMeasureModeAtMost)
    {
        // Grow the label horizontally as much as possible
        if (widthMode == YGMeasureModeAtMost)
            size.width = std::min(requiredWidth, availableWidth);
        else
            size.width = requiredWidth;
    }
    else if (widthMode == YGMeasureModeExactly)
    {
        size.width = width;
    }
    else
    {
        fatal("Unsupported Label width measure mode: " + std::to_string(widthMode));
    }

    // Height
    // Measure the required height, with wrapping

    // Is wrapping necessary and allowed ?
    if ((availableWidth < requiredWidth || fullText.find("\n") != std::string::npos) && !label->isSingleLine())
    {
        float boxBounds[4];
        nvgTextBoxBounds(vg, 0, 0, availableWidth, fullText.c_str(), nullptr, boxBounds);

        float requiredHeight = boxBounds[3] - boxBounds[1];

        // Undefined height mode, always wrap
        if (heightMode == YGMeasureModeUndefined)
        {
            label->setIsWrapping(true);
            size.height = requiredHeight;
        }
        // At most height mode, see if we have enough space
        else if (heightMode == YGMeasureModeAtMost)
        {
            if (height >= requiredHeight)
            {
                label->setIsWrapping(true);
                size.height = requiredHeight;
            }
            else
            {
                computeLabelHeight(label, width, widthMode, height, heightMode, &size, bounds);
            }
        }
        // Exactly mode, see if we have enough space
        else if (heightMode == YGMeasureModeExactly)
        {
            if (height >= requiredHeight)
            {
                label->setIsWrapping(true);
                size.height = height;
            }
            else
            {
                computeLabelHeight(label, width, widthMode, height, heightMode, &size, bounds);
            }
        }
        else
        {
            fatal("Unsupported Label height measure mode: " + std::to_string(heightMode));
        }
    }
    // No wrapping necessary or allowed, return the normal height
    else
    {
        computeLabelHeight(label, width, widthMode, height, heightMode, &size, bounds);
    }

    return size;
}

void Label::setCursor(int cursor) {
    this->cursor = cursor;
    this->cursor_blink = brls::getCPUTimeUsec();
}

Label::Label()
{
    Style style = Application::getStyle();
    Theme theme = Application::getTheme();

    // Default attributes
    this->font        = Application::getDefaultFont();
    this->fontSize    = style["brls/label/default_font_size"];
    this->lineHeight  = style["brls/label/default_line_height"];
    this->textColor   = theme["brls/text"];
    this->fontQuality = 1.0f;

    this->setHighlightPadding(style["brls/label/highlight_padding"]);

    // Setup the custom measure function
    YGNodeSetMeasureFunc(this->ygNode, labelMeasureFunc);

    // Set the max width and height to 100% to avoid overflowing
    // The view will be shortened if the text is too long
    YGNodeStyleSetMaxWidthPercent(this->ygNode, 100);
    YGNodeStyleSetMaxHeightPercent(this->ygNode, 100);

    // Register XML attributes
    this->registerStringXMLAttribute("text", [this](std::string value)
        { this->setText(value); });

    this->registerFloatXMLAttribute("fontSize", [this](float value)
        { this->setFontSize(value); });

    this->registerFloatXMLAttribute("fontQuality", [this](float value)
        { this->setFontQuality(value); });

    this->registerColorXMLAttribute("textColor", [this](NVGcolor color)
        { this->setTextColor(color); });

    this->registerFloatXMLAttribute("lineHeight", [this](float value)
        { this->setLineHeight(value); });

    this->registerBoolXMLAttribute("animated", [this](bool value)
        { this->setAnimated(value); });

    this->registerBoolXMLAttribute("autoAnimate", [this](bool value)
        { this->setAutoAnimate(value); });

    this->registerBoolXMLAttribute("singleLine", [this](bool value)
        { this->setSingleLine(value); });

    this->registerFloatXMLAttribute("cursor", [this](float value)
        { this->setCursor(value); });

    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "horizontalAlign", HorizontalAlign, this->setHorizontalAlign,
        {
            { "left", HorizontalAlign::LEFT },
            { "center", HorizontalAlign::CENTER },
            { "right", HorizontalAlign::RIGHT },
        });

    BRLS_REGISTER_ENUM_XML_ATTRIBUTE(
        "verticalAlign", VerticalAlign, this->setVerticalAlign,
        {
            { "baseline", VerticalAlign::BASELINE },
            { "top", VerticalAlign::TOP },
            { "center", VerticalAlign::CENTER },
            { "bottom", VerticalAlign::BOTTOM },
        });
}

void Label::setAnimated(bool animated)
{
    if (animated == this->animated || this->isWrapping || this->horizontalAlign != HorizontalAlign::LEFT)
        return;

    this->animated = animated;

    this->resetScrollingAnimation();
}

void Label::setAutoAnimate(bool autoAnimate)
{
    this->autoAnimate = autoAnimate;
}

void Label::setHorizontalAlign(HorizontalAlign align)
{
    this->horizontalAlign = align;
}

void Label::setVerticalAlign(VerticalAlign align)
{
    this->verticalAlign = align;
}

void Label::onFocusGained()
{
    View::onFocusGained();

    if (this->autoAnimate)
        this->setAnimated(true);
}

void Label::onFocusLost()
{
    View::onFocusLost();

    if (this->autoAnimate)
        this->setAnimated(false);
}

void Label::onParentFocusGained(View* focusedView)
{
    View::onParentFocusGained(focusedView);

    if (this->autoAnimate)
        this->setAnimated(true);
}

void Label::onParentFocusLost(View* focusedView)
{
    View::onParentFocusLost(focusedView);

    if (this->autoAnimate)
        this->setAnimated(false);
}

void Label::setTextColor(NVGcolor color)
{
    this->textColor = color;
}

std::string Label::STConverter(const std::string& text)
{
#if defined(OPENCC) && not defined(USE_LIBROMFS)
    static bool skip = Application::getLocale() != LOCALE_ZH_HANT && Application::getLocale() != LOCALE_ZH_TW;
    if (skip || !OPENCC_ON)
        return text;
    static opencc::SimpleConverter converter = opencc::SimpleConverter(std::string(BRLS_RESOURCES) + "opencc/s2t.json");
    return converter.Convert(text);
#endif
    return text;
}

void Label::setText(const std::string& text)
{
#ifdef OPENCC
    static bool trans = Application::getLocale() == LOCALE_ZH_HANT || Application::getLocale() == LOCALE_ZH_TW;
    if (trans && OPENCC_ON)
    {
        this->truncatedText = Label::STConverter(text);
        this->fullText      = this->truncatedText;
        this->stringLength  = strLen(this->fullText);
    }
    else
    {
        this->truncatedText = text;
        this->fullText      = text;
        this->stringLength  = strLen(this->fullText);
    }
    this->invalidate();
#else
    this->truncatedText = text;
    this->fullText      = text;
    this->stringLength  = strLen(text);

    this->invalidate();
#endif
}

void Label::setSingleLine(bool singleLine)
{
    this->singleLine = singleLine;

    this->invalidate();
}

void Label::setFontSize(float value)
{
    this->fontSize = value;

    this->invalidate();
}

void Label::setFontQuality(float value)
{
    this->fontQuality = value;

    this->invalidate();
}

void Label::setLineHeight(float value)
{
    this->lineHeight = value;

    this->invalidate();
}

void Label::setIsWrapping(bool isWrapping)
{
    this->isWrapping = isWrapping;
}

bool Label::isSingleLine()
{
    return this->singleLine;
}

enum NVGalign Label::getNVGVerticalAlign()
{
    switch (this->verticalAlign)
    {
        default:
        case VerticalAlign::BASELINE:
            return NVG_ALIGN_BASELINE;
        case VerticalAlign::TOP:
            return NVG_ALIGN_TOP;
        case VerticalAlign::CENTER:
            return NVG_ALIGN_MIDDLE;
        case VerticalAlign::BOTTOM:
            return NVG_ALIGN_BOTTOM;
    }
}

enum NVGalign Label::getNVGHorizontalAlign()
{
    switch (this->horizontalAlign)
    {
        default:
        case HorizontalAlign::LEFT:
            return NVG_ALIGN_LEFT;
        case HorizontalAlign::CENTER:
            return NVG_ALIGN_CENTER;
        case HorizontalAlign::RIGHT:
            return NVG_ALIGN_RIGHT;
    }
}

void Label::draw(NVGcontext* vg, float x, float y, float width, float height, Style style, FrameContext* ctx)
{
    if (width == 0)
        return;

    enum NVGalign horizAlign = this->getNVGHorizontalAlign();
    enum NVGalign vertAlign  = this->getNVGVerticalAlign();

    nvgFontSize(vg, this->fontSize);
    nvgTextAlign(vg, horizAlign | vertAlign);
    nvgFontFaceId(vg, this->font);
    nvgFontQuality(vg, this->fontQuality);
    nvgTextLineHeight(vg, this->lineHeight);
    nvgFillColor(vg, a(this->textColor));

    // Animated text
    if (this->animating)
    {
        nvgSave(vg);
        float scissorHeight = fontSize * lineHeight;
        nvgIntersectScissor(vg, x, y, width, scissorHeight < height ? height : scissorHeight);

        float baseX   = x - this->scrollingAnimation;
        float spacing = style["brls/label/scrolling_animation_spacing"];

        nvgText(vg, baseX, y + height / 2.0f, this->fullText.c_str(), nullptr);

        if (this->scrollingAnimation > 0)
            nvgText(vg, baseX + this->requiredWidth + spacing, y + height / 2.0f, this->fullText.c_str(), nullptr);

        nvgRestore(vg);
    }
    // Wrapped text
    else if (this->isWrapping)
    {
        nvgTextAlign(vg, horizAlign | NVG_ALIGN_TOP);
        nvgTextBox(vg, x, y, width, this->fullText.c_str(), nullptr);
    }
    // Truncated text
    else
    {
        float textX = x;
        float textY = y;

        if (horizAlign == NVG_ALIGN_CENTER)
            textX += width / 2;
        else if (horizAlign == NVG_ALIGN_RIGHT)
            textX += width;

        if (vertAlign == NVG_ALIGN_MIDDLE || vertAlign == NVG_ALIGN_BASELINE)
            textY += height / 2.0f;
        else if (vertAlign == NVG_ALIGN_BOTTOM)
            textY += height;

        float nextX = nvgText(vg, textX, textY, this->truncatedText.c_str(), nullptr);

        // 绘制编辑游标
        if (this->cursor >= (int)CursorPosition::END) {
            // blink
            auto blink = ((brls::getCPUTimeUsec() - cursor_blink) >> 10) % 1000 ;
            if (blink > 500)
                return;

            nvgSave(vg);
            float lineh;
            nvgTextMetrics(vg, NULL, NULL, &lineh);
            float cursorX = x;
            int textSize = this->truncatedText.size();
            if (this->cursor == (int)CursorPosition::END) {
                cursorX = nextX;
            } else if (this->cursor > (int)CursorPosition::START) {
                if (textSize > this->cursor) {
                    std::vector<NVGglyphPosition> glyphs;
                    glyphs.resize(textSize);
                    int nglyphs = nvgTextGlyphPositions(vg, x, y, this->truncatedText.c_str(), NULL, glyphs.data(), textSize);
                    if (nglyphs <= this->cursor) {
                        cursorX = nextX;
                    } else {
                        cursorX = glyphs.at(this->cursor).x;
                    }
                } else if (textSize == this->cursor) {
                    cursorX = nextX;
                }
            }
            nvgBeginPath(vg);
            nvgRect(vg, cursorX, y, 1, lineh);
            nvgFill(vg);
            nvgRestore(vg);
        }
    }
}

void Label::stopScrollingAnimation()
{
    // Extra check to avoid stopping inexisting timers on labels that
    // are never animated
    if (!this->animating)
        return;

    this->scrollingTimer.stop();
    this->scrollingTimer.reset();

    this->scrollingAnimation = 0.0f;

    this->animating = false;
}

void Label::onScrollTimerFinished()
{
    Style style = Application::getStyle();

    // Step 2: actual scrolling animation
    float target   = this->requiredWidth + style["brls/label/scrolling_animation_spacing"];
    float duration = target / style["brls/animations/label_scrolling_speed"];

    this->scrollingAnimation.reset();

    this->scrollingAnimation.addStep(target, duration, EasingFunction::linear);

    this->scrollingAnimation.setEndCallback([this](bool finished)
        {
        // Start over if the scrolling animation ended naturally
        if (finished)
            this->startScrollTimer(); });

    this->scrollingAnimation.start();

    this->animating = true;
}

void Label::startScrollTimer()
{
    Style style = Application::getStyle();

    // Step 1: timer before starting to scroll
    this->scrollingAnimation = 0.0f;

    this->scrollingTimer.reset();

    this->scrollingTimer.setDuration(style["brls/animations/label_scrolling_timer"]);

    this->scrollingTimer.setEndCallback([this](bool finished)
        { this->onScrollTimerFinished(); });

    this->scrollingTimer.start();

    this->animating = true;
}

void Label::resetScrollingAnimation()
{
    // Stop it
    this->stopScrollingAnimation();

    // Restart it if it needs to be
    if (this->animated)
    {
        float width = this->getWidth();

        if (width < this->requiredWidth)
            this->startScrollTimer();
    }
}

void Label::onLayout()
{
    float width = this->getWidth();

    if (width == 0)
    {
        this->resetScrollingAnimation();
        return;
    }

    // Prebake clipping
    if (!this->fullText.empty() && width < this->requiredWidth && !this->isWrapping)
    {
        // Compute the position of the ellipsis (in chars), should the string be truncated
        // Cannot do it in the measure function because the margins are not applied yet there
        auto vg = Application::getNVGContext();
        nvgFontSize(vg, this->fontSize);
        nvgTextAlign(vg, this->getNVGHorizontalAlign() | this->getNVGVerticalAlign());
        nvgFontFaceId(vg, this->font);
        nvgTextLineHeight(vg, this->lineHeight);

        std::vector<NVGglyphPosition> positions;
        positions.resize(stringLength);
        nvgTextGlyphPositions(vg, 0, 0, fullText.c_str(), nullptr, positions.data(), stringLength);

        const char* start   = fullText.c_str();
        this->truncatedText = fullText;
        for (auto& i : positions)
        {
            if (i.str == start)
                continue;
            if (i.str >= start + fullText.size())
                break;
            if (i.maxx + this->ellipsisWidth > width)
            {
                this->truncatedText = fullText.substr(0, i.str - start) + ELLIPSIS;
                break;
            }
        }
    }
    else
    {
        this->truncatedText = this->fullText;
    }

    this->resetScrollingAnimation(); // either stops it or restarts it with the new text
}

int Label::getFont()
{
    return this->font;
}

float Label::getFontSize()
{
    return this->fontSize;
}

float Label::getFontQuality()
{
    return this->fontQuality;
}

float Label::getLineHeight()
{
    return this->lineHeight;
}

NVGcolor Label::getTextColor()
{
    return this->textColor;
}

std::string Label::getFullText()
{
    return this->fullText;
}

void Label::setRequiredWidth(float requiredWidth)
{
    this->requiredWidth = requiredWidth;
}

void Label::setEllipsisWidth(float ellipsisWidth)
{
    this->ellipsisWidth = ellipsisWidth;
}

Label::~Label()
{
    this->stopScrollingAnimation();
}

View* Label::create()
{
    return new Label();
}

} // namespace brls
