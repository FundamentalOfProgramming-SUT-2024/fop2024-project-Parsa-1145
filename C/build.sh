gcc -g GameObjects/Floor.c UiElements/TabWidget.c UiElements/UiBase.c GameObjects/Items/Stair.c GameObjects/Items/Potion.c GameObjects/Items/Key.c GameObjects/Items/Coin.c GameObjects/Items/ItemBase.c GameObjects/Items/Ammo.c GameObjects/Items/Weapon.c GameObjects/Items/Door.c Utilities/cJSON.c UiElements/PopUp.c ProgramStates/SignupMenu.c Utilities/MovingCell.c GameObjects/Renderer.c ProgramStates/MainMenu.c Main.c Globals.c UiElements/Button.c UiElements/Widget.c ProgramStates/NewGameMenu.c UiElements/TextBox.c ProgramStates/LogInMenu.c ProgramStates/MainGame.c Utilities/LinkedList.c GameObjects/Room.c Utilities/PointCloud.c GameObjects/Texture.c UiElements/CheckBox.c -o main -lncursesw -lm
./main 1024
pause -p "press any key to continue"
