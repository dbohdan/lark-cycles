
CFLAGS= +L +Iram:symbols.m

all: ram:symbols.m cycles\
         player1 player2 player3 player4 player5 player6 player7 player8

OBJS= cycles.o time.o graphic.o dobikes.o

cycles:  $(OBJS)
         ln $(OBJS) -lc32

ram:symbols.m: symbols.c
         cc +L +Hram:symbols.m symbols.c

player1: player1.o play.o manager.o
         ln player1.o manager.o play.o -lc32

player2: player2.o play.o manager.o
         ln player2.o manager.o play.o -lc32

player3: player3.o play.o manager.o
         ln player3.o manager.o play.o -lc32

player4: player4.o play.o manager.o
         ln player4.o manager.o play.o -lc32

player5: player5.o play.o manager.o
         ln player5.o manager.o play.o -lc32

player6: player6.o play.o manager.o
         ln player6.o manager.o play.o -lc32

player7: player7.o play.o manager.o
         ln player7.o manager.o play.o -lc32

player8: player8.o play.o manager.o
         ln player8.o manager.o play.o -lc32
