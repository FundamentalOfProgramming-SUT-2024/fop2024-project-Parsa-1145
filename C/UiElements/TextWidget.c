#include "TextWidget.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "../Globals.h"

void createTextWidget(TextWidget* t, Widget* parent, int alignmentX, int alignmentY, int x, int y, char* format, ...){
    t->widget = malloc(sizeof(Widget));

    t->format = copyString(format);

    t->wrap = 1;

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
                if((format[i+1] == 'o') || (format[i+1] == 'i')){
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

    if(n){
        t->args = calloc(n ,sizeof(Arg));
    }else{
        t->args = NULL;
    }

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
                    case 'D':
                        t->args[k].sd = va_arg(args, int);
                        k++;
                        break;
                    case 'F':
                        t->args[k].sf = va_arg(args, double);
                        k++;
                        break;
                    case 'u':
                        t->args[k].w = va_arg(args, wchar_t*);
                        k++;
                        break;
                    case 'U':
                        t->args[k].sw = va_arg(args, wchar_t);
                        k++;
                        break;
                    case 'S':
                        t->args[k].s = va_arg(args, char*);
                        t->args[k].s = copyString(t->args[k].s);
                        k++;
                        break;
                    case 'f':
                        t->args[k].f = va_arg(args, float*);
                        k++;
                        break;
                    case 'o':
                        t->args[k].sColor[0] = va_arg(args, int);
                        t->args[k+1].sColor[1] = va_arg(args, int);
                        t->args[k+2].sColor[2] = va_arg(args, int);
                        k+= 3;
                        break;
                    case 'i':
                        t->args[k].color[0] = va_arg(args, int*);
                        t->args[k+1].color[1] = va_arg(args, int*);
                        t->args[k+2].color[2] = va_arg(args, int*);
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

    if(parent){
        t->bgColor = getTopColor(parent);
    }else{
        t->bgColor = 0;
    }

    t->uiBase = malloc(sizeof(UiBase));
    t->uiBase->render = &renderTextWidget;
    t->uiBase->keyPress = &defaultKeyPressCb;
    t->uiBase->mouseClick = &defaultMouseClickCb;
    t->uiBase->mouseMove = &defaultMouseMoveCb;
    t->uiBase->update = &updateTextWidget;
    t->uiBase->delete = &deleteTextWidget;
    t->uiBase->isHovered = &defaultIsHovered;
    t->uiBase->z = &(t->widget->z);


    t->uiBase->object = t;
    t->uiBase->widget = t->widget;
    t->uiBase->type = UI_TYPE_TEXTWIDGET;

}
void changeTextWidget(TextWidget* t, char* format, ...){
    free(t->format);
    if(t->args){
        free(t->args);
    }

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
                if((format[i+1] == 'o') || (format[i+1] == 'i')){
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
                    case 'S':
                        t->args[k].s = va_arg(args, char*);
                        t->args[k].s = copyString(t->args[k].s);
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
                    case 'U':
                        t->args[k].sw = va_arg(args, wchar_t);
                        k++;
                        break;
                    case 'D':
                        t->args[k].sd = va_arg(args, int);
                        k++;
                        break;
                    case 'F':
                        t->args[k].sf = va_arg(args, double);
                        k++;
                        break;
                    case 'f':
                        t->args[k].f = va_arg(args, float*);
                        k++;
                        break;
                    case 'o':
                        t->args[k].sColor[0] = va_arg(args, int*);
                        t->args[k+1].sColor[1] = va_arg(args, int*);
                        t->args[k+2].sColor[2] = va_arg(args, int*);
                        k+=3;
                        break;
                    case 'i':
                        t->args[k].color[0] = va_arg(args, int*);
                        t->args[k+1].color[1] = va_arg(args, int*);
                        t->args[k+2].color[2] = va_arg(args, int*);
                        k+= 3;
                        break;
                }
                
                break;
            default:
                break;
        }
    }

    va_end(args);
}
int textWidgetGetStrLength(TextWidget* t){
    uint32_t out = 0;
    uint32_t argIter = 0;
    FOR(i, t->strLength){
        switch (t->format[i]){
        case '\\':
            if(t->format[i+1] == '%'){
                out += 1;
                i++;
            }
            break;
        case '%':
            switch (t->format[i+1]){
                case 'S':
                case 's':
                    out += snprintf(NULL, 0, "%s", t->args[argIter].s);
                    argIter++;
                    break;
                case 'd':
                    out += snprintf(NULL, 0, "%d", t->args[argIter].d[0]);
                    argIter++;
                    break;
                case 'D':
                    out += snprintf(NULL, 0, "%d", t->args[argIter].sd);
                    argIter++;
                    break;
                case 'F':
                    out += snprintf(NULL, 0,"%d", t->args[argIter].sf);
                    argIter++;
                    break;
                case 'u':
                    out += snprintf(NULL, 0,"%lc", t->args[argIter].w[0]);
                    argIter++;
                    break;
                case 'U':
                    out += snprintf(NULL, 0,"%lc", t->args[argIter].w);
                    argIter++;
                    break;
                case 'f':
                    out += snprintf(NULL, 0,"%f", t->args[argIter].f[0]);
                    argIter++;
                    break;
                case 'o':
                    argIter +=3;
                    break;
                case 'i':
                    argIter +=3;
                    break;
                case 'O':
                    break;
            }
            i++;
            break;
        default:
            out++;
            break;
        }
    }
    return out;
}
void renderTextWidget(TextWidget* t){
    if(isWidgetVisible(t->widget)){
        color_set(t->bgColor, NULL);
        moveInFrameBuffer(uiFrameBuffer, t->widget->topLeftY, t->widget->topLeftX);

        int argIter = 0;
        if(t->widget->parent && (!t->widget->overflow[0])){
            Widget* p = t->widget->parent;
            setRenderBoundingBox(p->topLeftX + p->bordered, p->topLeftY + p->bordered, p->topLeftX + p->wCopy - 1 - p->bordered - (p->scrollOn && (p->totalScrollArea > p->hCopy)),
                p->topLeftY + p->hCopy - 1 - p->bordered);
        }
        setWrapMode(t->wrap);
        FOR(i, t->strLength){
            switch (t->format[i]){
            case '\\':
                if(t->format[i+1] == '%'){
                    addWchToFrameBuffer(uiFrameBuffer, '%', t->widget->z, 0, 0);
                    i++;
                }
                break;
            case '%':
                switch (t->format[i+1]){
                    case 'S':
                    case 's':
                        framBufferPrintW(uiFrameBuffer,  t->widget->z, "%s" ,t->args[argIter].s);
                        argIter++;
                        break;
                    case 'd':
                        framBufferPrintW(uiFrameBuffer,  t->widget->z,"%d", t->args[argIter].d[0]);
                        argIter++;
                        break;
                    case 'D':
                        framBufferPrintW(uiFrameBuffer,  t->widget->z,"%d", t->args[argIter].sd);
                        argIter++;
                        break;
                    case 'F':
                        framBufferPrintW(uiFrameBuffer,  t->widget->z,"%d", t->args[argIter].sf);
                        argIter++;
                        break;
                    case 'u':
                        addWchToFrameBuffer(uiFrameBuffer, t->args[argIter].w[0], t->widget->z, 0, 0);
                        argIter++;
                        break;
                    case 'U':
                        addWchToFrameBuffer(uiFrameBuffer, t->args[argIter].w, t->widget->z, 0, 0);
                        argIter++;
                        break;
                    case 'f':
                        framBufferPrintW(uiFrameBuffer,  t->widget->z,"%f", t->args[argIter].f[0]);
                        argIter++;
                        break;
                    case 'o':
                        color_set(rgb[t->args[argIter].sColor[0]][t->args[argIter+1].sColor[1]][t->args[argIter+2].sColor[2]], NULL);
                        argIter +=3;
                        break;
                    case 'i':
                        color_set(rgb[t->args[argIter].color[0][0]][t->args[argIter+1].color[1][0]][t->args[argIter+2].color[2][0]], NULL);
                        argIter +=3;
                        break;
                    case 'O':
                        color_set(t->bgColor, NULL);
                        break;
                }
                i++;
                break;
            default:
                addWchToFrameBuffer(uiFrameBuffer, t->format[i], t->widget->z, 0, 0);
                break;
            }
        }
        setWrapMode(0);
        if(t->widget->parent){
            resetRenderBound();
        }
        color_set(0, NULL);
    }
}
void updateTextWidget(TextWidget* t){
    t->widget->w = textWidgetGetStrLength(t);
    updateWidgetTopLeft(t->widget);
    if(t->wrap){
        t->widget->hCopy = (t->widget->w / t->widget->parent->wCopy) + 1;
    }
}
void deleteTextWidget(TextWidget* t){
    int k = 0;
    FOR(i, t->strLength){
        switch (t->format[i]){
            case '\\':
                if(t->format[i+1] == '%'){
                    i++;
                }
                break;
            case '%':
                i++;
                switch (t->format[i]){
                    case 'S':
                        free(t->args[k].s);
                        k++;
                        break;
                    case 'i':
                    case 'o':
                        k += 3;
                        break;
                    case 'O':
                        break;
                    default:
                        k++;
                }
                
                break;
            default:
                break;
        }
    }
    deleteUiBase(t->uiBase);
    free(t->widget);
    free(t->format);
    free(t->args);
    free(t);
}