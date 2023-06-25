# gatesim: gate-level event-driven simulator
gatesim is a parser and gate-level simulator that extracts gates and wires from a subset of the Verilog language to build a graph-like data structure. It enables simulation while considering specified delays. Additionally, gatesim includes a VCD generator, which displays the waveform results of the simulation

## Build and Run
```bash
make
./main.out c432.v inputfeed.txt netlist.txt wave.vcd
```
The input verilog file can be replaced with `c432.v`, input vector value and timing are read from `inputfeed.txt`, a text representation of netlist will be generated as `netlist.txt`, 
and the simulation result will be generated as a VCD file at `wave.vcd`.