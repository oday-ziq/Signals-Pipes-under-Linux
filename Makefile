G = gcc -g
O = -o
names = main team player graphics

files:
	$(G) main.c $(O) main
	$(G) team.c $(O) team
	$(G) player.c $(O) player
	$(G) graphics.c $(O) graphics -I/path/to/glfw/include -lGL -lglfw -lm -lGLU -lglut -I/usr/include/GL/ -lGLEW
	./main
	./graphics

clean:
	rm -f $(names)
	find  . -name 'PRIVATE*' -exec rm {} \;
	find  . -name '\tmp\PRIVATE*' -exec rm {} \;
	pkill -f player