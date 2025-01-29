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
                if(format[i+1] == 'o'){
                    n+= 3;
                }else if(format[i+1] != 'O'){
                    n++;
                }
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
                switch (format[i]){
                    case 's':
                        t->args[k].s = va_arg(args, char*);
                        k++;
                        break;
                    case 'd':
                        t->args[k].d = va_arg(args, int*);
                        k++;
                        break;
                    case 'u':
                        t->args[k].w = va_arg(args, wchar_t*);
                        k++;
                        break;
                    case 'f':
                        t->args[k].f = va_arg(args, float*);
                        k++;
                        break;
                    case 'o':
                        t->args[k].color[0] = va_arg(args, int);
                        t->args[k+1].color[1] = va_arg(args, int);
                        t->args[k+2].color[2] = va_arg(args, int);
                        k+= 3;
                        break;
                }
                
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
                if(format[i+1] == 'o'){
                    n+= 3;
                }else  if(format[i+1] != 'O'){
                    n++;
                }
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
                switch (format[i]){
                    case 's':
                        t->args[k].s = va_arg(args, char*);
                        k++;
                        break;
                    case 'd':
                        t->args[k].d = va_arg(args, int*);
                        k++;
                        break;
                    case 'u':
                        t->args[k].w = va_arg(args, wchar_t*);
                        k++;
                        break;
                    case 'f':
                        t->args[k].f = va_arg(args, float*);
                        k++;
                        break;
                    case 'o':
                        t->args[k].color[0] = va_arg(args, int*);
                        t->args[k+1].color[1] = va_arg(args, int*);
                        t->args[k+2].color[1] = va_arg(args, int*);
                        k+=3;
                        break;
                }
                
                break;
            default:
                break;
        }
    }

    va_end(args);
}
void renderTextWidget(TextWidget* t){
    int prevColor = 0;
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
                        printw("%s", t->args[argIter].s);
                        argIter++;
                        break;
                    case 'd':
                        printw("%d", t->args[argIter].d[0]);
                        argIter++;
                        break;
                    case 'u':
                        printw("%Lc", t->args[argIter].w[0]);
                        argIter++;
                        break;
                    case 'f':
                        printw("%f", t->args[argIter].f[0]);
                        argIter++;
                        break;
                    case 'o':
                        if (prevColor) attroff(COLOR_PAIR(prevColor));
                        prevColor = rgb[t->args[argIter].color[0]][t->args[argIter+1].color[1]][t->args[argIter+2].color[2]];
                        argIter +=3;
                        attron(COLOR_PAIR(prevColor));
                        break;
                    case 'O':
                        attroff(COLOR_PAIR(prevColor));
                        prevColor = 0;
                        break;
                }
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