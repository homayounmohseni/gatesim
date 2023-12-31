#ifndef __MAIN_HPP__
#define __MAIN_HPP__

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "primitives.hpp"


const int NOT_DELAY = 2;
const int NAND_DELAY = 5;
const int AND_DELAY = 3;
const int NOR_DELAY = 4;
const int OR_DELAY = 4;
const int XOR_DELAY = 6;
const int XNOR_DELAY = 6;


const int ENGINE_BUFF_SIZE = 150;
const char VCD_CHAR_ID_FIRST = 33;
const char VCD_CHAR_ID_LAST = 126;

class VCDTracer {
public:
	VCDTracer(const std::string&, const std::string&);
	void add_change(const Event&);
	void dump(const std::vector<Wire*>&, int) const;
private:
	std::vector<Event> change_vector;
	std::string dumpfilepath;
	std::string module_name;
};

class EventEngine {
public:
	EventEngine(const std::list<Event>&, VCDTracer*);
	void run(const int);
	void schedule_activity(const Event&);
	void schedule_activity(const Event&, int);
	void schedule_activity(const std::list<Event>&);
private:
	std::array<std::list<WireAssignment>, ENGINE_BUFF_SIZE> time_array;
	std::list<Event> input_feed;
	VCDTracer *tracer;
};


std::string get_file_string(const std::string&);

std::list<Event> get_input_feed(const std::string&, const std::vector<Wire*>&);

void cleanup(const std::vector<Wire*>&, const std::vector<Gate*>&);

void randomize_gates(std::vector<Gate*>&); 

void dump_wire(std::ofstream&, const std::vector<Wire*>&);

void dump_gates(std::ofstream&, const std::vector<Gate*>&);

void print_wire_names(const std::vector<Wire*>&);

void print_wire_values(const std::vector<Wire*>&);

void print_gates(const std::vector<Gate*>&);

std::string init_module_name(const std::vector<std::vector<std::string>>&);

std::tuple<std::vector<Wire*>, std::vector<Wire*>, std::vector<Wire*>, std::vector<Wire*>>
init_wires(const std::vector<std::vector<std::string>>&);

std::vector<Gate*> init_gates(const std::vector<std::vector<std::string>>&,
		const std::vector<Wire*>&);

void add_wires_to_vector(const std::vector<std::string>&,
		std::vector<Wire*>&, std::unordered_map<std::string, Wire*>&);

std::vector<std::string> extract_statement(const std::string &,
		std::string::iterator&, const std::vector<char>&, char);

std::string next_vcd_id(std::string);

void dump_netlist(const std::string&, const std::vector<Wire*>&, const std::vector<Wire*>&,
		const std::vector<Wire*>&, const std::vector<Gate*>&);

#endif
