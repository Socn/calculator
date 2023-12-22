#include<iostream>
#include "calculate.h"
#include "functions.h"
#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/util/ref.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include<unistd.h>

using namespace std;

function<void()> processExp = nullptr;

void operator+=(ftxui::Component s, wchar_t c) {
    s->OnEvent(ftxui::Event::Character(c));
}

void operator+=(ftxui::Component s, wstring* c) {
    for (auto i : *c) {
        s->OnEvent(ftxui::Event::Character(i));
    }
}

vector<wchar_t> buttonLayout[]{
    {L'A',L'C',L'B',L'/'},
    {L'7',L'8',L'9',L'*'},
    {L'4',L'5',L'6',L'-'},
    {L'1',L'2',L'3',L'+'},
    {L'0',L'0',L'.',L'='}
};
vector<const Function*> functionLayout[]{
    {Sin}
};

class InputComponent {
public:
    ftxui::Component input;
    InputComponent(ftxui::Component i) {
        input = i;
    }
    void OnEvent(ftxui::Event) {

    }
};

void gui() {
    using namespace ftxui;
    string exp = "";
    wstring displayExp;
    Fraction previousResult;
    Fraction calcResult;
    const int BUTTON_WIDTH = 5;
    const int BUTTON_HEIGHT = 3;
    const int SCREEN_WIDTH = 30;
    const int SCREEN_HEIGHT = 20;

    Ref<int> cursorPosition = 1000;
    // InputComponent input_exp = InputComponent(Input({
    //         .content = &exp,
    //         .multiline = false,
    //         .on_enter = [&]() {
    //         previousResult = calcResult;
    //         exp = "";
    //         }
    //     }));
    Component input_exp = Input({
        .content = &exp,
        .multiline = false,
        .on_enter = [&]() {
        previousResult = calcResult;
        exp = "";
        }
        });
    Component numButton[20];
    Components componentsList = { input_exp };
    map<wchar_t, Component> stringButton;
    map<wchar_t, wstring> functionToDisplay;



    processExp = [&]() {
        displayExp = L"";
        for (auto i : exp) {
            if (functionToDisplay.find(i) != functionToDisplay.end()) {
                displayExp += functionToDisplay[i];
            }
            else displayExp += i;
        }
        };

    auto addButton = [&](wchar_t c, function<void()> func = nullptr, wstring displayAs = L"") {
        wstring* s = new wstring(1, c);
        if (displayAs == L"")displayAs = *s;
        stringButton[c] = Button(displayAs, [&, s, func]() {
            if (func == nullptr) {
                input_exp += s;
            }
            else {
                func();
            }
            processExp();
            }, ButtonOption::Border());
        componentsList.push_back(stringButton[c]);
        };

    addButton(L'A', [&]() {
        exp = "";
        previousResult = 0;
        });
    addButton(L'C', [&]() {
        exp = "";
        });
    addButton(L'B', [&]() {
        input_exp->OnEvent(Event::Backspace);
        });
    addButton(L'=', [&]() {
        previousResult = calcResult;
        exp = "";
        });
    for (auto row : buttonLayout) {
        for (auto column : row) {
            if (stringButton.find(column) == stringButton.end()) {
                addButton(column);
            }
        }
    }
    for (auto row : functionLayout) {
        for (auto column : row) {
            functionToDisplay[column->functionChar] = column->displayString;
            if (stringButton.find(column->functionChar) == stringButton.end()) {
                addButton(column->functionChar, nullptr, column->displayString);
            }
        }
    }

    auto component = Container::Vertical(componentsList);

    auto renderer = Renderer(component, [&] {
        input_exp->TakeFocus();
        Elements* buttons = new Elements();
        Elements* functions = new Elements();
        for (auto row : buttonLayout) {
            Elements* flexboxElements = new Elements();
            int widthMultiplier = 1;
            wchar_t lastChar = L'\0';
            for (auto column : row) {
                if (lastChar == L'\0') {
                    lastChar = column;
                    continue;
                }
                if (column == lastChar) {
                    widthMultiplier++;
                }
                else {
                    flexboxElements->push_back(stringButton[lastChar]->Render()
                        | size(WidthOrHeight::WIDTH, Constraint::EQUAL, BUTTON_WIDTH * widthMultiplier)
                        | size(WidthOrHeight::HEIGHT, Constraint::EQUAL, BUTTON_HEIGHT));
                    widthMultiplier = 1;
                }
                lastChar = column;
            }
            flexboxElements->push_back(stringButton[lastChar]->Render()
                | size(WidthOrHeight::WIDTH, Constraint::EQUAL, BUTTON_WIDTH * widthMultiplier)
                | size(WidthOrHeight::HEIGHT, Constraint::EQUAL, BUTTON_HEIGHT));
            buttons->push_back(flexbox(*flexboxElements));
        }

        for (auto row : functionLayout) {
            Elements* flexboxElements = new Elements();
            int widthMultiplier = 1;
            const Function* lastFunc = nullptr;
            for (auto column : row) {
                if (lastFunc == nullptr) {
                    lastFunc = column;
                    continue;
                }
                if (column == lastFunc) {
                    widthMultiplier++;
                }
                else {
                    flexboxElements->push_back(stringButton[lastFunc->functionChar]->Render()
                        | size(WidthOrHeight::WIDTH, Constraint::EQUAL, BUTTON_WIDTH * widthMultiplier)
                        | size(WidthOrHeight::HEIGHT, Constraint::EQUAL, BUTTON_HEIGHT));
                    widthMultiplier = 1;
                }
                lastFunc = column;
            }
            flexboxElements->push_back(stringButton[lastFunc->functionChar]->Render()
                | size(WidthOrHeight::WIDTH, Constraint::EQUAL, BUTTON_WIDTH * widthMultiplier)
                | size(WidthOrHeight::HEIGHT, Constraint::EQUAL, BUTTON_HEIGHT));
            functions->push_back(flexbox(*flexboxElements));
        }

        return window(text("Calculator"), vbox(
            previousResult == 0 ? hbox(text(displayExp)) : hbox(text(to_string(previousResult)), text(displayExp)),
            // previousResult == 0 ? hbox(text(exp)) : hbox(text(to_string(previousResult)), text(exp)),
            flexbox({ text(to_string(calcExp(CalcRequest(exp,previousResult)))) | color(Color::GrayLight) }, FlexboxConfig().Set(FlexboxConfig::JustifyContent::FlexEnd)),
            separator(),
            hbox(vbox(*buttons), separator(), vbox(*functions))
        ));
        });

    auto screen = ScreenInteractive::FixedSize(SCREEN_WIDTH, SCREEN_HEIGHT);

    screen.Loop(renderer);
}

int main() {
    init();
    gui();
    return 0;
}