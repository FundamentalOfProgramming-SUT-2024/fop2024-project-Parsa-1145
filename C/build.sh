gcc -g Utilities/*.c UiElements/*.c ProgramStates/*.c GameObjects/*.c GameObjects/Items/*.c ./*.c -o main -lncursesw -lm -lpthread `sdl2-config --cflags --libs` -lSDL2_mixer
./main 1024
