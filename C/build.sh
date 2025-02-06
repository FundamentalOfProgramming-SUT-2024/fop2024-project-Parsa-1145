gcc -g -w Utilities/*.c UiElements/*.c ProgramStates/*.c GameObjects/*.c GameObjects/Items/*.c ./*.c -o main $(pkg-config --cflags --libs ncursesw) -lm -lpthread `sdl2-config --cflags --libs` -lSDL2_mixer
./main 1024
