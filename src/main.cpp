#include<iostream>
#include "calculate.h"
#include "functions.h"
#include "constants.h"
#include <ftxui/screen/screen.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/util/ref.hpp>
#include <ftxui/component/captured_mouse.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <unistd.h>
#include <set>

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
    {L'A',L'C',L'B',L'^'},
    {L'7',L'8',L'9',L'/'},
    {L'4',L'5',L'6',L'*'},
    {L'1',L'2',L'3',L'-'},
    {L'0',L'.',L'=',L'+'}
};
vector<const Function*> functionLayout[]{
    {Sin, Cos},
    {Tan, Sqrt},
    {Inverse, Ln}
};
vector<const Constant*> constantLayout[]{
    {Pi, E}
};

void gui() {
    using namespace ftxui;
    string exp = "";
    Elements displayExp{ text(wstring(1, L' ')) | bgcolor(Color::GrayDark) };
    Fraction previousResult;
    bool hasPreviousResult = false;
    Fraction calcResult;
    const int BUTTON_WIDTH = 5;
    const int BUTTON_HEIGHT = 3;
    const int SCREEN_WIDTH = 33;
    const int SCREEN_HEIGHT = 20;

    set<wchar_t>possibleInputs = { L'(', L')' };
    int cursorPosition = 0;
    int expLength = 0;
    Component input_exp = Input({
        .content = &exp,
        .multiline = false,
        .on_change = [&]() {
            processExp();
            calcResult = calcExp(CalcRequest(to_wstring(exp), previousResult, hasPreviousResult));
        },
        .on_enter = [&]() {
            previousResult = calcResult;
            exp = "";
            cursorPosition = expLength = 0;
            hasPreviousResult = true;
            processExp();
        }
        });
    input_exp |= CatchEvent([&](Event event) { // Check input
        if (event.is_mouse())return false;
        if (event == Event::Character('=')) {
            input_exp->OnEvent(Event::Return);
            return true;
        }
        if (event == Event::Backspace) {
            cursorPosition = max(0, min(cursorPosition - 1, expLength));
            expLength = max(0, expLength - 1);
        }
        if (event == Event::ArrowLeft) {
            cursorPosition = max(0, min(cursorPosition - 1, expLength));
            processExp();
        }
        if (event == Event::ArrowRight) {
            cursorPosition = max(0, min(cursorPosition + 1, expLength));
            processExp();
        }
        if (!event.is_character())return false;
        wstring eventWString = to_wstring(event.character());
        if (possibleInputs.find(eventWString[0]) == possibleInputs.end()) return true;
        cursorPosition++;
        expLength++;
        return false;
        });
    Component numButton[20];
    Components componentsList = { input_exp };
    map<wchar_t, Component> stringButton;
    map<wchar_t, wstring> functionToDisplay;

    processExp = [&]() {
        displayExp.clear();
        wstring displayExpStr = L"";
        wstring w_exp = to_wstring(exp);
        bool cursorFlag = false;
        int pos = 0;
        for (auto i : w_exp) {
            wstring str = functionToDisplay.find(i) != functionToDisplay.end() ? functionToDisplay[i] : wstring(1, i);
            if (pos == cursorPosition) {
                displayExp.push_back(text(displayExpStr));
                displayExp.push_back(text(str) | bgcolor(Color::GrayDark));
                displayExpStr = L"";
                pos++;
                cursorFlag = true;
                continue;
            }
            displayExpStr += str;
            pos++;
        }
        displayExp.push_back(text(displayExpStr));
        if (!cursorFlag) {
            displayExp.push_back(text(wstring(1, L' ')) | bgcolor(Color::GrayDark));
        }
        };

    auto addButton = [&](wchar_t c, bool canInput = true, function<void()> func = nullptr, wstring displayAs = L"") {
        wstring* s = new wstring(1, c);
        if (displayAs == L"")displayAs = *s;
        stringButton[c] = Button(displayAs, func == nullptr ? [&, s]() {input_exp += s;} : func, ButtonOption::Border());
        componentsList.push_back(stringButton[c]);
        if (canInput)possibleInputs.insert(c);
        };


    addButton(L'A', false, [&]() {
        exp = "";
        cursorPosition = expLength = 0;
        previousResult = calcResult = 0;
        processExp();
        hasPreviousResult = false;
        }, L"AC");
    addButton(L'C', false, [&]() {
        exp = "";
        cursorPosition = expLength = 0;
        calcResult = previousResult;
        processExp();
        });
    addButton(L'B', false, [&]() {
        input_exp->OnEvent(Event::Backspace);
        }, L"␡");
    addButton(L'=', false, [&]() {
        previousResult = calcResult;
        exp = "";
        cursorPosition = expLength = 0;
        processExp();
        hasPreviousResult = true;
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
                addButton(column->functionChar, true, [&, column] {
                    wstring* s = new wstring(1, column->functionChar);
                    if (cursorPosition != 0 || !hasPreviousResult) {
                        input_exp += s;
                    }
                    else {
                        input_exp += s;
                        wstring s2 = to_wstring((double)previousResult);
                        previousResult = 0;
                        hasPreviousResult = false;
                        input_exp += &s2;
                    }
                    }, column->displayString);
            }
        }
    }
    for (auto row : constantLayout) {
        for (auto column : row) {
            if (stringButton.find(column->constantChar) == stringButton.end()) {
                addButton(column->constantChar);
            }
        }
    }


    Elements* buttons = new Elements();
    Elements* functions = new Elements();
    Elements* constants = new Elements();

    auto screen = ScreenInteractive::FixedSize(SCREEN_WIDTH, SCREEN_HEIGHT);

    Component exitButton = Button(L"×", [&]() {screen.Exit();}, ButtonOption::Ascii());
    componentsList.push_back(exitButton);

    auto component = Container::Vertical(componentsList);

    auto renderer = Renderer(component, [&] {
        input_exp->TakeFocus();
        buttons->clear();
        functions->clear();
        constants->clear();
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

        for (auto row : constantLayout) {
            Elements* flexboxElements = new Elements();
            int widthMultiplier = 1;
            const Constant* lastFunc = nullptr;
            for (auto column : row) {
                if (lastFunc == nullptr) {
                    lastFunc = column;
                    continue;
                }
                if (column == lastFunc) {
                    widthMultiplier++;
                }
                else {
                    flexboxElements->push_back(stringButton[lastFunc->constantChar]->Render()
                        | size(WidthOrHeight::WIDTH, Constraint::EQUAL, BUTTON_WIDTH * widthMultiplier)
                        | size(WidthOrHeight::HEIGHT, Constraint::EQUAL, BUTTON_HEIGHT));
                    widthMultiplier = 1;
                }
                lastFunc = column;
            }
            flexboxElements->push_back(stringButton[lastFunc->constantChar]->Render()
                | size(WidthOrHeight::WIDTH, Constraint::EQUAL, BUTTON_WIDTH * widthMultiplier)
                | size(WidthOrHeight::HEIGHT, Constraint::EQUAL, BUTTON_HEIGHT));
            functions->push_back(flexbox(*flexboxElements));
        }

        return window(hbox(text("Calculator "), filler(), exitButton->Render()), vbox(
            hasPreviousResult ? hbox(text(to_string(previousResult)), hbox(displayExp)) : hbox(displayExp),
            flexbox({ text(to_string(calcResult)) | color(Color::GrayLight) }, FlexboxConfig().Set(FlexboxConfig::JustifyContent::FlexEnd)),
            separator(),
            hbox(vbox(*buttons), separator(), vbox(*functions, *constants))
        ));
        });


    screen.Loop(renderer);
}

int main() {
    init();
    gui();
    return 0;
}