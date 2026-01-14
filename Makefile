all: build


build:
	@gcc -Wall -ggdb -o vcd2txt -I src/ src/main.c src/svg.c src/vcd.c

test: build
	@valgrind ./vcd2txt -i test.vcd


install: build
	@cp vcd2txt /usr/bin/
	@echo "Installed vcd2txt"

uninstall: 
	@rm vcd2txt /usr/bin/vcd2txt
	@echo "Removed vcd2txt"

clean:
	@rm *.o
