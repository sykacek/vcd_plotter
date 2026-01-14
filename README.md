# UTILITY FOR WORKING WITH VCD FILES

I needed some lightweigh utility for visualizing verilog outputs

## build

```
	sudo make install
```

## usage

List available signals
```
	vcd2txt -i input.vcd -l 
```

Convert whole module of index `idx`
```
	vcd2txt -i input.vcd -m 'idx'
```

In `.data` there will be generated files.
