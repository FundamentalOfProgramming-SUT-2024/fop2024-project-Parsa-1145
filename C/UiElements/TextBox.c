#include "TextBox.h"

#include <string.h>
#include <stdlib.h>

#include "../Globals.h"

void createTextBox(TextBox* textBox,Widget* parent, char* label, char* str, int sizePolicyX, int sizePolicyY, int alignmentX, int alignmentY, int x, int y, int w){
    textBox->widget = calloc(1, sizeof(Widget));

    createWidget(textBox->widget, parent, sizePolicyX, ABSOLUTE, alignmentX, alignmentY, x, y, w, 3+(label[0]!=NULL), COLOR_BLACK);
    
    textBox->str = str;
    textBox->label = label;
    textBox->index = strlen(str);

    if(parent){
        textBox->bgColor = getTopColor(parent);
        if(!textBox->bgColor) textBox->bgColor = COLOR_BLACK;
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
    textBox->uiBase->isHovered = &defaultIsHovered;
    textBox->uiBase->z = &(textBox->widget->z);


    textBox->uiBase->object = textBox;
    textBox->uiBase->widget = textBox->widget;
    textBox->uiBase->type = UI_TYPE_TEXTBOX;
}
void createNumberInput(TextBox* textBox, Widget* parent, char* label, int* num, int maxV, int minV, int sizePolicyX, int sizePolicyY, int alignmentX, int alignmentY, int x, int y, int w){
    textBox->widget = calloc(1, sizeof(Widget));

    createWidget(textBox->widget, parent, sizePolicyX, ABSOLUTE, alignmentX, alignmentY, x, y, w, 3+(label[0]!=NULL), COLOR_BLACK);
    

    textBox->max = maxV;
    textBox->min = minV;
    textBox->maxCharachters = (int)(max(snprintf(NULL, 0, "%d", maxV), snprintf(NULL, 0, "%d", minV)));
    textBox->str = calloc(1, textBox->maxCharachters + 2);
    sprintf(textBox->str, "%d", num[0]);
    textBox->integer = num;
    textBox->label = label;
    textBox->index = strlen(textBox->str);

    if(parent){
        textBox->bgColor = getTopColor(parent);
        if(!textBox->bgColor) textBox->bgColor = COLOR_BLACK;
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
    textBox->uiBase->isHovered = &defaultIsHovered;
    textBox->uiBase->z = &(textBox->widget->z);


    textBox->uiBase->object = textBox;
    textBox->uiBase->widget = textBox->widget;
    textBox->uiBase->type = UI_TYPE_TEXTBOX;
}

void renderTextBox(TextBox* textBox){
    if(isWidgetVisible(textBox->widget->parent)){
        color_set(textBox->bgColor, NULL);
        int top = textBox->widget->topLeftY;
        if(textBox->label[0]){
            moveInFrameBuffer(uiFrameBuffer ,top++, textBox->widget->topLeftX + 1);
            framBufferPrintW(uiFrameBuffer,  textBox->widget->z,"%s:", textBox->label);
        }

        if((hoveredElement == textBox->uiBase) && !textBox->focused)attron(A_DIM);
        moveInFrameBuffer(uiFrameBuffer ,top++, textBox->widget->topLeftX);
        addWchToFrameBuffer(uiFrameBuffer, WACS_ULCORNER->chars[0], textBox->widget->z, 0, 0);
        
        FOR(i, textBox->widget->wCopy-2){
            addWchToFrameBuffer(uiFrameBuffer, WACS_HLINE->chars[0], textBox->widget->z, 0, 0);
        }
        addWchToFrameBuffer(uiFrameBuffer, WACS_URCORNER->chars[0], textBox->widget->z, 0, 0);

        moveInFrameBuffer(uiFrameBuffer, top, textBox->widget->topLeftX);
        addWchToFrameBuffer(uiFrameBuffer, WACS_VLINE->chars[0], textBox->widget->z, 0, 0);
        FOR(i, textBox->widget->wCopy-2){
            addWchToFrameBuffer(uiFrameBuffer, ' ', textBox->widget->z, 0, 0);
        }
        moveInFrameBuffer(uiFrameBuffer, top, textBox->widget->topLeftX + 1);
        if(textBox->integer && textBox->integer[0]){
            sprintf(textBox->str, "%d", textBox->integer[0]);
            textBox->index = strlen(textBox->integer);
        }
        framBufferPrintW(uiFrameBuffer,  textBox->widget->z, "%s", textBox->str);
        textBox->index = strlen(textBox->str);
        if(textBox->focused){
            attron(A_BLINK);
            addWchToFrameBuffer(uiFrameBuffer, WACS_VLINE->chars[0], textBox->widget->z, 0, 0);
            attroff(A_BLINK);
        }else{
            addWchToFrameBuffer(uiFrameBuffer, ' ', textBox->widget->z, 0, 0);
        }
        moveInFrameBuffer(uiFrameBuffer, top++, textBox->widget->topLeftX + textBox->widget->wCopy-1);
        addWchToFrameBuffer(uiFrameBuffer, WACS_VLINE->chars[0], textBox->widget->z, 0, 0);
        
        moveInFrameBuffer(uiFrameBuffer, top, textBox->widget->topLeftX);
        addWchToFrameBuffer(uiFrameBuffer, WACS_LLCORNER->chars[0], textBox->widget->z, 0, 0);
        FOR(i, textBox->widget->wCopy-2){
            addWchToFrameBuffer(uiFrameBuffer, WACS_HLINE->chars[0], textBox->widget->z, 0, 0);
        }
        addWchToFrameBuffer(uiFrameBuffer, WACS_LRCORNER->chars[0], textBox->widget->z, 0, 0);
        color_set(0, NULL);
        if((hoveredElement == textBox->uiBase) && !textBox->focused)attroff(A_DIM);
    }
}
int TBMouseMoveCb(TextBox* textBox){
}
int TBMouseClickCb(TextBox* textBox){
    if(isWidgetVisible(textBox->widget->parent)){
        if(mEvent.bstate & BUTTON1_PRESSED){
            if((hoveredElement == textBox->uiBase)){
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
                if(textBox->integer){
                    if(textBox->index == 0){
                        textBox->integer[0] = 0;
                    }else{
                        sscanf(textBox->str, "%d", textBox->integer);
                    }
                }
                break;
            case KEY_UP:
                if(textBox->integer){
                    if(textBox->integer[0] + 1 <= textBox->max){
                        textBox->integer[0]++;
                        sprintf(textBox->str, "%d", textBox->integer[0]);
                        textBox->index = strlen(textBox->str);
                    }
                }
                break;
            case KEY_DOWN:
                if(textBox->integer){
                    if(textBox->integer[0] - 1 >= textBox->min){
                        textBox->integer[0]--;
                        sprintf(textBox->str, "%d", textBox->integer[0]);
                        textBox->index = strlen(textBox->str);
                    }
                }
                break;
            default:
                if((!textBox->maxCharachters) || (textBox->index < textBox->maxCharachters)){
                    if(key < 129){
                        if((key == '\n') || (key == '\t')){
                            if(textBox->finishedCallback){
                                textBox->finishedCallback();
                            }
                            
                            textBox->focused = 0;
                        }else{
                            if(textBox->integer){
                                if((key >= '0') && (key <= '9')){
                                    int tmp;
                                    textBox->str[(textBox->index)++] = key;
                                    sscanf(textBox->str, "%d", &tmp);
                                    if((textBox->min <= tmp) && (tmp <= textBox->max)){
                                        textBox->integer[0] = tmp;
                                    }else{
                                        textBox->str[--(textBox->index)] = 0;
                                        if(textBox->index){
                                            char tmp2 = textBox->str[textBox->index - 1];
                                            textBox->str[(textBox->index)-1] = key;
                                            sscanf(textBox->str, "%d", &tmp); 
                                            if((textBox->min <= tmp) && (tmp <= textBox->max)){
                                                textBox->integer[0] = tmp;
                                            }else{
                                                textBox->str[(textBox->index)-1] = tmp2;
                                            }
                                        }
                                    }
                                }else if(key == '-'){
                                    if((textBox->index == 0) && (textBox->min < 0)){
                                        textBox->str[(textBox->index)++] = key;
                                    }
                                }
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
                } 
                break;
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
    free(t->widget);
    deleteUiBase(t->uiBase);
    free(t);
}