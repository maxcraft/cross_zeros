This is a simple implementation of a simple game. However I tried to define four diffuculties: _noob_, _easy_, _normal_, _hard_. In all difficulties except _hard_ the player has a first move.

The build process looks simle 

```gcc -O2 -o cross_zeros main.c game.c field.c```

So there is no Makefile so far. But it's in a plan.

###TODO:
* seeding random
* Makefile
* ncurses UI
* change difficulties without leaving the program

