all: build


build:
	@gcc -Wall -ggdb -o vcd2svg -I src/ src/main.c src/svg.c src/vcd.c
	#@./vcd2svg -l -i test.vcd
	@./vcd2svg -s 0 0 -i test.vcd

test: build
	@valgrind ./vcd2svg -i test.vcd


install: build
	@cp vcd2svg /usr/bin/
	@echo "Installed vcd2svg"

uninstall: 
	@rm vcd2svg /usr/bin/vcd2svg
	@echo "Removed vcd2svg"

clean:
	@rm *.o
