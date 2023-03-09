#ifndef __MAIN_HPP__
#define __MAIN_HPP__

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <random>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "primitives.hpp"


typedef std::pair<int, std::vector<ActivityEntry>> TraceEntry;

class VCDTracer {
public:
	//TODO define member functions
	VCDTracer(const std::string&);
	void add_change(int, Wire*, char);
	void dump() const;
private:
	std::set<std::string> nodes;
	std::vector<TraceEntry> change_vector;
	std::string dumpfilepath;
};

class EventEngine {
public:
	EventEngine(int, VCDTracer*);
	void run(int);

	void schedule_activity(Wire*, char, int);
	void schedule_activity(const ActivityEntry, int);
	void schedule_activity(const std::vector<std::pair<ActivityEntry, int>>&);
private:
	int cur_time;
	std::vector<std::list<ActivityEntry>> time_vector;
	VCDTracer *tracer;
};


std::string get_file_string(const std::string&);

std::vector<std::vector<char>> get_input_vectors(const std::string&);

void cleanup(const std::vector<Wire*>&, const std::vector<Gate*>&);

void randomize_gates(std::vector<Gate*>& gates); 

void simulate_default_order(const std::vector<Wire*>&, const std::vector<Wire*>&, 
		const std::vector<Wire*>&, const std::vector<Gate*>&,
		const std::vector<std::vector<char>>&);

void simulate_ordered(const std::vector<Wire*>&, const std::vector<Wire*>&,
		const std::vector<Wire*>&, const std::vector<Gate*>&,
		const std::vector<std::vector<char>>&);

void print_wire_names(const std::vector<Wire*>&);

void print_wire_values(const std::vector<Wire*>&);


void print_gates(const std::vector<Gate*>&);

void set_inputs(const std::vector<Wire*>&, const std::vector<char>&);

void reset_wires(const std::vector<Wire*>&);

std::tuple<std::vector<Wire*>, std::vector<Wire*>, std::vector<Wire*>>
init_wires(const std::vector<std::vector<std::string>>& statements);

std::vector<Gate*> init_gates(const std::vector<std::vector<std::string>>&,
		const std::vector<Wire*>&);

void add_wires_to_vector(const std::vector<std::string>& statement,
		std::vector<Wire*>&, std::map<std::string, Wire*>&);

std::vector<std::string> extract_statement(const std::string &,
		std::string::iterator&, const std::vector<char>&, char);

#endif
