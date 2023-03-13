#ifndef __MAIN_HPP__
#define __MAIN_HPP__

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <list>
// #include <map>
#include <unordered_map>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "primitives.hpp"


const int ENGINE_BUFF_SIZE = 250;

class VCDTracer {
public:
	//TODO define member functions
	VCDTracer(const std::string&);
	void add_change(const Event&);
	void dump() const;
private:
	std::set<std::string> nodes;
	std::vector<Event> change_vector;
	std::string dumpfilepath;
};

class EventEngine {
public:
	EventEngine(const std::list<Event>&, VCDTracer*);
	void run(int);
	void schedule_activity(const Event&);
	void schedule_activity(const Event&, int);
	void schedule_activity(const std::vector<Event>&);
private:
	std::array<std::list<WireAssignment>, ENGINE_BUFF_SIZE> time_array;
	std::list<Event> input_feed;
	VCDTracer *tracer;
};


std::string get_file_string(const std::string&);

std::vector<std::vector<char>> get_input_vectors(const std::string&);

std::list<Event> get_input_feed(const std::string&, const std::vector<Wire*>&);

void cleanup(const std::vector<Wire*>&, const std::vector<Gate*>&);

void randomize_gates(std::vector<Gate*>& gates); 

void print_wire_names(const std::vector<Wire*>&);

void print_wire_values(const std::vector<Wire*>&);


void print_gates(const std::vector<Gate*>&);

void set_inputs(const std::vector<Wire*>&, const std::vector<char>&);

std::tuple<std::vector<Wire*>, std::vector<Wire*>, std::vector<Wire*>>
init_wires(const std::vector<std::vector<std::string>>& statements);

std::vector<Gate*> init_gates(const std::vector<std::vector<std::string>>&,
		const std::vector<Wire*>&);

void add_wires_to_vector(const std::vector<std::string>& statement,
		std::vector<Wire*>&, std::unordered_map<std::string, Wire*>&);

std::vector<std::string> extract_statement(const std::string &,
		std::string::iterator&, const std::vector<char>&, char);

#endif
