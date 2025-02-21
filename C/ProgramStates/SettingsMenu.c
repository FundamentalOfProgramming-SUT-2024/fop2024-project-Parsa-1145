#include <stdlib.h>
#include <string.h>
#include "SettingsMenu.h"

#include "../UiElements/Button.h"
#include "../UiElements/ComboBox.h"
#include "../UiElements/TabWidget.h"
#include "../UiElements/TextBox.h"
#include "../UiElements/TextWidget.h"
#include "../UiElements/PopUp.h"

#include "../Globals.h"

#include "../GameObjects/AudioManager.h"

LinkedList smUiList;

EngineState settingsMenu = {NULL, &updateSettingsMenu, &renderSettingsMenu, NULL, &smUiList};


Widget smMainWidget;
TabWidget smTabWidget;

Widget smAudioWidget;
Widget smGamePlayWidget;
Widget smGraphicsWidget;



Widget smAudioComboWrapper;
TextWidget smAudioComboLabel;
ComboBox smAudioCombo;

Widget smMusicVolumeWrapper;
TextWidget smMusicVolumeLabel;
TextBox smMusicVolumeTextBox;

Widget smEffectsVolumeWrapper;
TextWidget smEffectsVolumeLabel;
TextBox smEffectsVolumeTextBox;

Button smApplyBtn;
Button smBackBtn;

EngineState* prevEngineState;

Settings tmpSettings;

void forceSmBack(){
    engineState = prevEngineState;
    prevEngineState->enter();
}
void smBack(){
    if(memcmp(&tmpSettings, &settings, sizeof(Settings))){
        createPopUp("Unsaved changes", NULL, 15, 20, NULL, &smUiList);
    }else{
        engineState = prevEngineState;
        prevEngineState->enter();
    }
}
void applySettings(){
    memcpy(&settings, &tmpSettings, sizeof(Settings));
    setMusicVolume();
}

void initSettingsMenu(){
    createLinkedList(&smUiList, sizeof(UiBase*));

    createWidget(&smMainWidget, NULL, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 50, 50, C_BG_GRAY0);
    smMainWidget.bordered = 1;
    {
        createTabWidget(&smTabWidget, &smMainWidget, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_CENTER, 2, 2, 100, 100, NULL);
        smTabWidget.widget->bordered = 1;
        {
            createWidget(&smAudioWidget, smTabWidget.tabArea, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 100, 100, NULL);
            smAudioWidget.layoutType = VERTICAL_LAYOUT;
            smAudioWidget.layoutPadding = 1;
            {
                createWidget(&smAudioComboWrapper, &smAudioWidget, RELATIVE, ABSOLUTE, ALIGN_CENTER, WITH_PARENT, 0, 0, 100, 1, NULL);
                {
                    createTextWidget(&smAudioComboLabel, &smAudioWidget, ALIGN_LEFT, ABSOLUTE, 0, 0, "Music:");
                    createComboBox(&smAudioCombo, &smAudioWidget, ABSOLUTE, ABSOLUTE, ALIGN_RIGHT, ABSOLUTE, 1, 0, 25, 1, rgb[3][3][3]);
                    {
                        comboBoxAddOption(&smAudioCombo, "Terraria's ost");
                        comboBoxAddOption(&smAudioCombo, "Hollow knight's ost");
                        comboBoxAddOption(&smAudioCombo, "Dummy");
                        comboBoxAddOption(&smAudioCombo, "Dummy");
                        comboBoxAddOption(&smAudioCombo, "Dummy");

                    }

                    linkedListPushBack(smAudioComboWrapper.children, smAudioCombo.uiBase);
                    linkedListPushBack(smAudioComboWrapper.children, smAudioComboLabel.uiBase);
                }
                createWidget(&smMusicVolumeWrapper, &smAudioWidget, RELATIVE, ABSOLUTE, ALIGN_CENTER, WITH_PARENT, 0, 0, 100, 3, NULL);
                {
                    createTextWidget(&smMusicVolumeLabel, &smMusicVolumeWrapper, ALIGN_LEFT, ALIGN_CENTER, 0, 0, "Music volume:");
                    createNumberInput(&smMusicVolumeTextBox, &smMusicVolumeWrapper, "", &(tmpSettings.musicVolume), 100, 0, RELATIVE, ABSOLUTE, ALIGN_RIGHT, ABSOLUTE, 0, 0, 30);
                    linkedListPushBack(smMusicVolumeWrapper.children, smMusicVolumeLabel.uiBase);
                    linkedListPushBack(smMusicVolumeWrapper.children, smMusicVolumeTextBox.uiBase);
                }

                createWidget(&smEffectsVolumeWrapper, &smAudioWidget, RELATIVE, ABSOLUTE, ALIGN_CENTER, WITH_PARENT, 0, 0, 100, 3, NULL);
                {
                    createTextWidget(&smEffectsVolumeLabel, &smEffectsVolumeWrapper, ALIGN_LEFT, ALIGN_CENTER, 0, 0, "Effects volume:");
                    createNumberInput(&smEffectsVolumeTextBox, &smEffectsVolumeWrapper, "", &(tmpSettings.effectsVolume), 100, 0, RELATIVE, ABSOLUTE, ALIGN_RIGHT, ABSOLUTE, 0, 0, 30);
                    linkedListPushBack(smEffectsVolumeWrapper.children, smEffectsVolumeLabel.uiBase);
                    linkedListPushBack(smEffectsVolumeWrapper.children, smEffectsVolumeTextBox.uiBase);
                }

                linkedListPushBack(smAudioWidget.children, smAudioComboWrapper.uiBase);
                linkedListPushBack(smAudioWidget.children, smMusicVolumeWrapper.uiBase);
                linkedListPushBack(smAudioWidget.children, smEffectsVolumeWrapper.uiBase);
            }
            createWidget(&smGraphicsWidget, smTabWidget.tabArea, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 100, 100, NULL);
            smGraphicsWidget.layoutType = VERTICAL_LAYOUT;
            smGraphicsWidget.layoutPadding = 1;

            createWidget(&smGamePlayWidget, smTabWidget.tabArea, RELATIVE, RELATIVE, ALIGN_CENTER, ALIGN_CENTER, 0, 0, 100, 100, NULL);
            smGamePlayWidget.layoutType = VERTICAL_LAYOUT;
            smGamePlayWidget.layoutPadding = 1;

            tabWidgetAddTab(&smTabWidget, "Audio", &smAudioWidget, NULL);
            tabWidgetAddTab(&smTabWidget, "Graphics", &smGraphicsWidget, NULL);
            tabWidgetAddTab(&smTabWidget, "Gameplay", &smGamePlayWidget, NULL);

        }
        createButton(&smApplyBtn, &smMainWidget, "Apply", ABSOLUTE, ALIGN_RIGHT, ALIGN_BOTTOM, 2, 2, 7);
        createButton(&smBackBtn, &smMainWidget, "Back", ABSOLUTE, ALIGN_LEFT, ALIGN_BOTTOM, 2, 2, 6);

        smBackBtn.callBack = &smBack;
        smApplyBtn.callBack = &applySettings;

        linkedListPushBack(smMainWidget.children, smTabWidget.uiBase);
        linkedListPushBack(smMainWidget.children, smApplyBtn.uiBase);
        linkedListPushBack(smMainWidget.children, smBackBtn.uiBase);

    }

    linkedListPushBack(&smUiList, smMainWidget.uiBase);
}
void enterSettingsMenu(){
    renderFrameBuffer(uiFrameBuffer);
    engineState->exit();
    memcpy(&tmpSettings, &settings, sizeof(Settings));

    prevEngineState = engineState;
    engineState = &settingsMenu;
}
void updateSettingsMenu(int key){

}
void renderSettingsMenu(){
    erase();

    void** tmp = smUiList.data;
    UiBase* ptr;
    while(tmp){
        ptr = tmp[1];
        tmp = tmp[0];

        ptr->render(ptr->object);
    }

    renderFrameBuffer(uiFrameBuffer);

    refresh();
    emptyFrameBuffer(uiFrameBuffer);
}
void exitSettingsMenu(){

}
