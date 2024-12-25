#include "TextBox.h"
#include "stdlib.h"
#include "Globals.h"
#include "string.h"

void createTextBox(TextBox* textBox,Widget* parent, char* label, char* str, int sizePolicyX, int sizePolicyY, int alignmentX, int alignmentY, int x, int y, int w){
    textBox->widget = malloc(sizeof(Widget));

    createWidget(textBox->widget, parent, sizePolicyX, ABSOLUTE, alignmentX, alignmentY, x, y, w, 4, COLOR_BLACK, COLOR_BLACK);
    
    textBox->str = str;
    textBox->label = label;
    textBox->index = strlen(str);

    if(parent != NULL){
        textBox->bgColor = parent->bgColor;
    }else{
        textBox->bgColor = COLOR_BLACK;
    }

    init_pair(colorPairNum, COLOR_WHITE, textBox->bgColor);
    textBox->colorPair = colorPairNum;
    colorPairNum++;
}


void renderTextBox(TextBox* textBox){
    updateWidgetTopLeft(textBox->widget);
    attron(COLOR_PAIR(textBox->colorPair));
    move(textBox->widget->topLeftY, textBox->widget->topLeftX + 1);
    printw("%s:", textBox->label);

    if(textBox->hovered && !textBox->focused)attron(A_DIM);
    move(textBox->widget->topLeftY+1, textBox->widget->topLeftX);
    addch(ACS_ULCORNER);
    FOR(i, textBox->widget->wCopy-2){
        addch(ACS_HLINE);
    }
    addch(ACS_URCORNER);

    move(textBox->widget->topLeftY+2, textBox->widget->topLeftX);
    addch(ACS_VLINE);
    FOR(i, textBox->widget->wCopy-2){
        addch(' ');
    }
    move(textBox->widget->topLeftY+2, textBox->widget->topLeftX + 2);
    printw("%s", textBox->str);
    if(textBox->focused){
        attron(A_BLINK);
        addch(ACS_VLINE);
        attroff(A_BLINK);
    }else{
        addch(' ');
    }
    move(textBox->widget->topLeftY+2, textBox->widget->topLeftX + textBox->widget->wCopy-1);
    addch(ACS_VLINE);
    
    move(textBox->widget->topLeftY+3, textBox->widget->topLeftX);
    addch(ACS_LLCORNER);
    FOR(i, textBox->widget->wCopy-2){
        addch(ACS_HLINE);
    }
    addch(ACS_LRCORNER);
    attroff(COLOR_PAIR(textBox->colorPair));
    if(textBox->hovered && !textBox->focused)attroff(A_DIM);
}
void textBoxMouseMoveCallback(TextBox* textBox){
    if(((mEvent.x >= textBox->widget->topLeftX) && (mEvent.x < textBox->widget->topLeftX + textBox->widget->wCopy)) && 
    ((mEvent.y > textBox->widget->topLeftY) && (mEvent.y < textBox->widget->topLeftY + textBox->widget->hCopy))){
        textBox->hovered = 1;
    }else{
        textBox->hovered = 0;
    }
}
void textBoxMouseClickCallback(TextBox* textBox){
    if(mEvent.bstate & BUTTON1_PRESSED){
        if(textBox->hovered){
            textBox->focused = 1;
        }else{
            textBox->focused = 0;
        }
    }
}
void textBoxKeyPressCallback(TextBox* textBox, int key){
    if(textBox->focused){
        switch(key){
        case KEY_BACKSPACE:
            if(textBox->index){
                textBox->str[--(textBox->index)] = '\0';
            }
            break;
        default:
            if(key < 129){
                if((key == '\n') || (key == '\t')){
                    textBox->focused = 0;
                }else{
                    textBox->str[(textBox->index)++] = key;
                }
            }
        }
    }
}

void resetTextbox(TextBox* textBox){
    while(textBox->index + 1){
        textBox->str[textBox->index--] = '\0';
    }
    textBox->index++;
}