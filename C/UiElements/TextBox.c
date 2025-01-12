#include "TextBox.h"

#include <string.h>
#include <stdlib.h>

#include "../Globals.h"

void createTextBox(TextBox* textBox,Widget* parent, char* label, char* str, int sizePolicyX, int sizePolicyY, int alignmentX, int alignmentY, int x, int y, int w){
    textBox->widget = malloc(sizeof(Widget));

    createWidget(textBox->widget, parent, sizePolicyX, ABSOLUTE, alignmentX, alignmentY, x, y, w, 4, COLOR_BLACK);
    
    textBox->str = str;
    textBox->label = label;
    textBox->index = strlen(str);

    if((parent) && (parent->bgColor)){
        textBox->bgColor = parent->bgColor;
    }else{
        textBox->bgColor = COLOR_BLACK;
    }

    textBox->spaceAccepted = 0;
    textBox->focused = 0;

    textBox->uiBase = malloc(sizeof(UiBase));
    textBox->uiBase->render = &renderTextBox;
    textBox->uiBase->keyPress = &TBKeyPressCb;
    textBox->uiBase->mouseClick = &TBMouseClickCb;
    textBox->uiBase->mouseMove = &TBMouseMoveCb;
    textBox->uiBase->update = &updateTextBox;
    textBox->uiBase->delete = &deleteTextBox;


    textBox->uiBase->object = textBox;
    textBox->uiBase->widget = textBox->widget;
    textBox->uiBase->type = UI_TYPE_TEXTBOX;
}


void renderTextBox(TextBox* textBox){
    if(isWidgetVisible(textBox->widget->parent)){
        attron(COLOR_PAIR(textBox->bgColor));
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
        move(textBox->widget->topLeftY+2, textBox->widget->topLeftX + 1);
        printw("%s", textBox->str);
        textBox->index = strlen(textBox->str);
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
        attroff(COLOR_PAIR(textBox->bgColor));
        if(textBox->hovered && !textBox->focused)attroff(A_DIM);
    }
}
int TBMouseMoveCb(TextBox* textBox){
    if(isWidgetVisible(textBox->widget->parent)){
        if(((mEvent.x >= textBox->widget->topLeftX) && (mEvent.x < textBox->widget->topLeftX + textBox->widget->wCopy)) && 
        ((mEvent.y > textBox->widget->topLeftY) && (mEvent.y < textBox->widget->topLeftY + textBox->widget->hCopy))){
            textBox->hovered = 1;
            return 1;
        }else{
            textBox->hovered = 0;
            return 0;
        }
    }
}
int TBMouseClickCb(TextBox* textBox){
    if(isWidgetVisible(textBox->widget->parent)){
        if(mEvent.bstate & BUTTON1_PRESSED){
            if(textBox->hovered){
                textBox->focused = 1;
                return 1;
            }else{
                textBox->focused = 0;
                return 0;
            }
        }
    }else{
        textBox->focused = 0;
        return 0;
    }
}
int TBKeyPressCb(TextBox* textBox, int key){
    if(isWidgetVisible(textBox->widget->parent)){
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
                        if(key == ' '){
                            if(textBox->spaceAccepted){
                                textBox->str[(textBox->index)++] = key;
                            }
                        }else{
                            textBox->str[(textBox->index)++] = key;
                        }
                    }
                }
            }
            return 1;
        }
    }else{
        textBox->focused = 0;
        return 0;
    }
}

void resetTextbox(TextBox* textBox){
    while(textBox->index + 1){
        textBox->str[textBox->index--] = '\0';
    }
    textBox->index++;
}

void updateTextBox(TextBox* t){
    updateWidgetTopLeft(t->widget);
}
void deleteTextBox(TextBox* t){
    if(t->widget->parent){
        removeItemFromLinkedList(t->widget->parent->children, t->uiBase);
    }
    free(t->widget);
    free(t->uiBase);
    free(t);
}