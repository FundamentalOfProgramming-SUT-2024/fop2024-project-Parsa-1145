gcc -g Utilities/MovingCell.c GameObjects/Renderer.c ProgramStates/MainMenu.c Main.c Globals.c UiElements/Button.c UiElements/Widget.c ProgramStates/NewGameMenu.c UiElements/TextBox.c ProgramStates/LogInMenu.c ProgramStates/MainGame.c Utilities/LinkedList.c GameObjects/Room.c Utilities/PointCloud.c GameObjects/Texture.c -o main -lncurses -lm
./main 1024
pause -p "press any key to continue"
