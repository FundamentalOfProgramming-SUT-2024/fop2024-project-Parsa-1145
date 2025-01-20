#include "TextWidget.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../Globals.h"

void createTextWidget(TextWidget* t, Widget* parent, int alignmentX, int alignmentY, int x, int y, char* format, ...){
    t->widget = malloc(sizeof(Widget));

    t->format = copyString(format);

    int n = 0;
    t->strLength = strlen(format);
    FOR(i, t->strLength){
        switch (format[i]){
            case '\\':
                if(format[i+1] == '%'){
                    i++;
                }
                break;
            case '%':
                n++;
                break;
            default:
                break;
        }
    }

    va_list args;
    va_start(args, n);

    t->args = calloc(n ,sizeof(Arg));

    int k = 0;
    FOR(i, t->strLength){
        switch (format[i]){
            case '\\':
                if(format[i+1] == '%'){
                    i++;
                }
                break;
            case '%':
                i++;
                t->args[k].argType = format[i];
                switch (t->args[k].argType){
                    case 's':
                        t->args[k].arg.s = va_arg(args, char*);
                        break;
                    case 'd':
                        t->args[k].arg.d = va_arg(args, int*);
                        break;
                    case 'u':
                        t->args[k].arg.w = va_arg(args, wchar_t*);
                        break;
                    case 'f':
                        t->args[k].arg.f = va_arg(args, float*);
                        break;
                }
                k++;
                break;
            default:
                break;
        }
    }

    va_end(args);

    createWidget(t->widget, parent, ABSOLUTE, ABSOLUTE, alignmentX, alignmentY, x, y, 1, 1, NULL);

    if((parent) && (parent->bgColor)){
        t->bgColor = parent->bgColor;
    }else{
        t->bgColor = COLOR_BLACK;
    }

    t->uiBase = malloc(sizeof(UiBase));
    t->uiBase->render = &renderTextWidget;
    t->uiBase->keyPress = &defaultKeyPressCb;
    t->uiBase->mouseClick = &defaultMouseClickCb;
    t->uiBase->mouseMove = &defaultMouseMoveCb;
    t->uiBase->update = &updateTextWidget;
    t->uiBase->delete = &deleteTextWidget;

    t->uiBase->object = t;
    t->uiBase->widget = t->widget;
    t->uiBase->type = UI_TYPE_TEXTWIDGET;

}
void changeTextWidget(TextWidget* t, char* format, ...){
    free(t->format);
    free(t->args);

    t->format = copyString(format);

    int n = 0;
    t->strLength = strlen(format);
    FOR(i, t->strLength){
        switch (format[i]){
            case '\\':
                if(format[i+1] == '%'){
                    i++;
                }
                break;
            case '%':
                n++;
                break;
            default:
                break;
        }
    }

    va_list args;
    va_start(args, n);

    t->args = calloc(n ,sizeof(Arg));

    int k = 0;
    FOR(i, t->strLength){
        switch (format[i]){
            case '\\':
                if(format[i+1] == '%'){
                    i++;
                }
                break;
            case '%':
                i++;
                t->args[k].argType = format[i];
                switch (t->args[k].argType){
                    case 's':
                        t->args[k].arg.s = va_arg(args, char*);
                        break;
                    case 'd':
                        t->args[k].arg.d = va_arg(args, int*);
                        break;
                    case 'u':
                        t->args[k].arg.w = va_arg(args, wchar_t*);
                        break;
                    case 'f':
                        t->args[k].arg.f = va_arg(args, float*);
                        break;
                }
                k++;
                break;
            default:
                break;
        }
    }

    va_end(args);
}
void renderTextWidget(TextWidget* t){
    if(isWidgetVisible(t->widget)){
        attron(COLOR_PAIR(t->bgColor));
        move(t->widget->topLeftY, t->widget->topLeftX);

        int argIter = 0;
        FOR(i, t->strLength){
            switch (t->format[i]){
            case '\\':
                if(t->format[i+1] == '%'){
                    addch('%');
                    i++;
                }
                break;
            case '%':
                switch (t->format[i+1]){
                    case 's':
                        printw("%s", t->args[argIter].arg.s);
                        break;
                    case 'd':
                        printw("%d", t->args[argIter].arg.d[0]);
                        break;
                    case 'u':
                        printw("%lc", t->args[argIter].arg.w[0]);
                        break;
                    case 'f':
                        printw("%f", t->args[argIter].arg.f[0]);
                        break;
                }
                argIter++;
                i++;
                break;
            default:
                addch(t->format[i]);
                break;
            }
        }
        attroff(COLOR_PAIR(t->bgColor));
    }
}
void updateTextWidget(TextWidget* t){
    updateWidgetTopLeft(t->widget);
}
void deleteTextWidget(TextWidget* t){
    free(t->uiBase);
    free(t->widget);
    free(t->format);
    free(t->args);
    free(t);
}