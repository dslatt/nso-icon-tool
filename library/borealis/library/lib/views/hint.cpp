/*
    Copyright 2021 XITRIX

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
#include <borealis/core/i18n.hpp>
#include <borealis/core/logger.hpp>
#include <borealis/core/touch/tap_gesture.hpp>
#include <borealis/core/util.hpp>
#include <borealis/views/applet_frame.hpp>
#include <borealis/views/hint.hpp>

using namespace brls::literals;

namespace brls
{

const std::string hintXML = R"xml(
    <brls:Box
        width="auto"
        height="auto"
        axis="row"
        paddingTop="4"
        paddingBottom="4"
        paddingLeft="16"
        paddingRight="16"
        cornerRadius="6">
            <brls:Label
                id="icon"
                width="auto"
                height="auto"
                fontSize="25.5"/>

            <brls:Label
                id="hint"
                width="auto"
                height="auto"
                fontSize="21.5"
                marginLeft="8"/>

    </brls:Box>
)xml";

Hint::Hint(Action action, bool allowAButtonTouch)
    : Box(Axis::ROW)
    , action(action)
{
    this->inflateFromXMLString(hintXML);
    this->setFocusable(false);

    icon->setText(getKeyIcon(action.button));
    hint->setText(action.hintText);

    if ((action.button != BUTTON_A || allowAButtonTouch) && action.available && !Application::isInputBlocks())
    {
        this->addGestureRecognizer(new TapGestureRecognizer(this, [this, action]()
            { action.actionListener(this); }));
    }

    if (!action.available || Application::isInputBlocks())
    {
        Theme theme = Application::getTheme();
        icon->setTextColor(theme["brls/text_disabled"]);
        hint->setTextColor(theme["brls/text_disabled"]);
    }
}

std::string Hint::getKeyIcon(ControllerButton button, bool ignoreKeysSwap)
{
    if (!ignoreKeysSwap)
        button = InputManager::mapControllerState(button);

    switch (button)
    {
        case BUTTON_A:
            return "\uE0E0";
        case BUTTON_B:
            return "\uE0E1";
        case BUTTON_X:
            return "\uE0E2";
        case BUTTON_Y:
            return "\uE0E3";
        case BUTTON_LSB:
            return "\uE104";
        case BUTTON_RSB:
            return "\uE105";
        case BUTTON_LT:
            return "\uE0E6";
        case BUTTON_RT:
            return "\uE0E7";
        case BUTTON_LB:
            return "\uE0E4";
        case BUTTON_RB:
            return "\uE0E5";
        case BUTTON_START:
            return "\uE0EF";
        case BUTTON_BACK:
            return "\uE0F0";
        case BUTTON_LEFT:
            return "\uE0ED";
        case BUTTON_UP:
            return "\uE0EB";
        case BUTTON_RIGHT:
            return "\uE0EE";
        case BUTTON_DOWN:
            return "\uE0EC";
        default:
            return "\uE152";
    }
}

Hints::Hints()
{
    setAxis(Axis::ROW);
    setDirection(Direction::LEFT_TO_RIGHT);

    hintSubscription = Application::getGlobalHintsUpdateEvent()->subscribe([this]()
        {
            if (!AppletFrame::HIDE_BOTTOM_BAR || forceShown)
            {
                refillHints(Application::getCurrentFocus());
            } });

    this->registerBoolXMLAttribute("addBaseAction", [this](bool value)
        { this->setAddUnableAButtonAction(value); });

    this->registerBoolXMLAttribute("allowAButtonTouch", [this](bool value)
        { this->setAllowAButtonTouch(value); });

    this->registerBoolXMLAttribute("forceShown", [this](bool value)
        { this->forceShown = value; });
}

Hints::~Hints()
{
    Application::getGlobalHintsUpdateEvent()->unsubscribe(hintSubscription);
}

void Hints::refillHints(View* focusView)
{
    if (!focusView)
        return;

    // todo: 做一个缓存，可以节约 Hint 组件生成
    clearViews();

    std::set<ControllerButton> addedButtons; // we only ever want one action per key
    std::vector<Action> actions;

    while (focusView != nullptr)
    {
        for (auto& action : focusView->getActions())
        {
            if (action.hidden)
                continue;

            if (addedButtons.find(action.button) != addedButtons.end())
                continue;

            addedButtons.insert(action.button);
            actions.push_back(action);
        }

        focusView = focusView->getParent();
    }

    if (addUnableAButtonAction && std::find(actions.begin(), actions.end(), BUTTON_A) == actions.end())
    {
        actions.push_back(Action { BUTTON_A, 0, "hints/ok"_i18n, false, false, false, Sound::SOUND_NONE, NULL });
    }

    // Sort the actions
    std::stable_sort(actions.begin(), actions.end(), Hints::actionsSortFunc);

    for (Action action : actions)
    {
        Hint* hint = new Hint(action, allowAButtonTouch);
        addView(hint);
    }
}

bool Hints::actionsSortFunc(Action a, Action b)
{
    // From left to right:
    //  - first +
    //  - then all hints that are not B and A
    //  - finally B and A

    // + is before all others
    if (a.button == BUTTON_START)
        return true;

    // A is after all others
    if (b.button == BUTTON_A)
        return true;

    // B is after all others but A
    if (b.button == BUTTON_B && a.button != BUTTON_A)
        return true;

    // Keep original order for the rest
    return false;
}

View* Hints::create()
{
    return new Hints();
}

} // namespace brls
