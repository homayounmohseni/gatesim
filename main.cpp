#include <iostream>
#include <iterator>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <algorithm>
#include <random>
#include <cassert>

#include "primitives.hpp"

using namespace std;


string get_file_string(const string&);

vector<vector<char>> get_input_vectors(const string&);

void cleanup(const vector<Wire*>&, const vector<Gate*>&);

void randomize_gates(vector<Gate*>& gates); 

void simulate_default_order(const vector<Wire*>&, const vector<Wire*>&, const vector<Wire*>&,
		const vector<Gate*>&, const vector<vector<char>>&);

void simulate_ordered(const vector<Wire*>&, const vector<Wire*>&, const vector<Wire*>&,
		const vector<Gate*>&, const vector<vector<char>>&);

void print_wire_names(const vector<Wire*>&);

void print_wire_values(const vector<Wire*>&);


void print_gates(const vector<Gate*>&);

void set_inputs(const vector<Wire*>&, const vector<char>&);

void reset_wires(const vector<Wire*>&);

tuple<vector<Wire*>, vector<Wire*>, vector<Wire*>>
init_wires(const vector<vector<string>>& statements);

vector<Gate*> init_gates(const vector<vector<string>>&, const vector<Wire*>&);

void add_wires_to_vector(const vector<string>& statement, vector<Wire*>&, map<string, Wire*>&);

vector<string> extract_statement(const string &, string::iterator&, const vector<char>&, char);


int main(int argc, char **argv) {
	string vfilepath;
	string ifilepath;
	if (argc < 3) {
		vfilepath = "c432.v";
		ifilepath = "inputs.txt";
	}
	else {
		vfilepath = string(argv[1]);
		ifilepath = string(argv[2]);
	}

	string vfile_str = get_file_string(vfilepath);
	vector<vector<char>> input_vectors = get_input_vectors(ifilepath);

	vector<vector<string>> statements;
	for (auto it = vfile_str.begin(); it != vfile_str.end(); ) {
		auto words = extract_statement(vfile_str, it, {',', ' ', '\r', '\n', '\t', '(', ')'}, ';');
		if (!words.empty()) {
			statements.push_back(words);
		}
	}

	auto [wires, inputs, outputs] = init_wires(statements);
	vector<Gate*> gates = init_gates(statements, wires);

	cout << "input names: \n";
	print_wire_names(inputs);
	cout << "output names: \n";
	print_wire_names(outputs);
	cout << '\n';

	cout << "simulating with no randomization:\n";
	simulate_default_order(wires, inputs, outputs, gates, input_vectors);
	simulate_ordered(wires, inputs, outputs, gates, input_vectors);

	randomize_gates(gates);
	cout << "\nsimulating with randomization:\n";
	simulate_default_order(wires, inputs, outputs, gates, input_vectors);
	simulate_ordered(wires, inputs, outputs, gates, input_vectors);

	cleanup(wires, gates);
	return 0;
}


void cleanup(const vector<Wire*>& wires, const vector<Gate*>& gates) {
	for (const auto& wire : wires) {
		delete wire;
	}
	for (auto const& gate : gates) {
		delete gate;
	}
}

void randomize_gates(vector<Gate*>& gates) {
	unsigned seed = 3239;
	shuffle(gates.begin(), gates.end(), default_random_engine(seed));
}

void simulate_default_order(const vector<Wire*>& wires, const vector<Wire*>& inputs, const vector<Wire*>& outputs, 
		const vector<Gate*>& gates, const vector<vector<char>>& input_vectors) {
	cout << "simulating with default order\n";
	for (const auto& input_vector : input_vectors) {
		set_inputs(inputs, input_vector);
		for (auto it = gates.begin(); it != gates.end(); it++) {
			(*it)->evaluate();
		}

		for (auto gate : gates) {
			gate->evaluate();
		}
		cout << "inputs:  ";
		print_wire_values(inputs);
		cout << "outputs: ";
		print_wire_values(outputs);

		reset_wires(wires);
	}
}


void simulate_ordered (const vector<Wire*>& wires, const vector<Wire*>& inputs, const vector<Wire*>& outputs, 
		const vector<Gate*>& gates, const vector<vector<char>>& input_vectors) {
	cout << "simulating ordered\n";
	for (const auto& input_vector : input_vectors) {
		set_inputs(inputs, input_vector);

		vector<Gate*> gates_left(gates);
		while (!gates_left.empty()) {
			for (auto it = gates_left.begin(); it != gates_left.end(); ) {
				auto gate = *it;
				if (gate->are_inputs_valid()) {
					gate->evaluate();
					gate->make_output_chagned();
					gates_left.erase(it);
				}
				else {
					it++;
				}
			}
		}
		cout << "inputs:  ";
		print_wire_values(inputs);
		cout << "outputs: ";
		print_wire_values(outputs);

		reset_wires(wires);
	}
}

void print_wire_names(const vector<Wire*>& wires) {
	cout << "[";
	for (auto it = wires.begin(); it != wires.end(); it++) {
		auto wire = *it;
		cout << wire->get_name() << " ";
	}
	cout << "]\n";
}

void print_wire_values(const vector<Wire*>& wires) {
	for (auto it = wires.begin(); it != wires.end(); it++) {
		auto wire = *it;
		cout << wire->value;
	}
	cout << '\n';
}

void print_gates(const vector<Gate*>& gates) {
	cout << "Gates:\n";

	for (auto it = gates.begin(); it != gates.end(); it++) {
		auto gate = *it;
		cout << gate->get_name() << '\n';
	}
	cout << '\n';
}

void set_inputs(const vector<Wire*>& inputs, const vector<char>& input_vector) {
	assert(inputs.size() == input_vector.size());
	for (int i = 0; i < inputs.size(); i++) {
		inputs[i]->value = input_vector[i];
		inputs[i]->changed = true;
	}
}

void reset_wires(const vector<Wire*>& wires) {
	for (auto wire : wires) {
		wire->value = 'x';
		wire->changed = false;
	}
}

tuple<vector<Wire*>, vector<Wire*>, vector<Wire*>>
init_wires(const vector<vector<string>>& statements) {
	vector<Wire*> wires, inputs, outputs;
	map<string, Wire*> wires_map;
	for (const auto& statement : statements) {
		if (statement.size() == 0) {
			continue;
		}
		if (statement[0] == "input") {
			add_wires_to_vector(statement, inputs, wires_map);
		}
		if (statement[0] == "output") {
			add_wires_to_vector(statement, outputs, wires_map);
		}
		if (statement[0] == "input" || statement[0] == "output" || statement[0] == "wire") {
			add_wires_to_vector(statement, wires, wires_map);
		}
	}

	return {wires, inputs, outputs};
}

void add_wires_to_vector(const vector<string>& words, vector<Wire*>& v, map<string, Wire*>& m) {
	for (int i = 1; i < words.size(); i++) {
		const auto& word = words[i];
		if (word == "wire") {
			continue;
		}

		Wire *wire;
		if (m.find(word) == m.end()) {
			wire = new Wire(word);
			m[word] = wire;
		}
		else {
			wire = m.at(word);
		}
		v.push_back(wire);
	}
}


vector<Gate*> init_gates(const vector<vector<string>>& statements, const vector<Wire*>& wires) {
	vector<Gate*> gates;

	map<string, Wire*> wires_map;
	for (auto wire : wires) {
		wires_map[wire->get_name()] = wire;
	}
	for (const auto& statement : statements) {
		if (statement.size() < 4) {
			continue;
		}
		Gate *gate;
		if (statement[0] == "nand") {
			gate = new Nand;
		}
		else if (statement[0] == "and") {
			gate = new And;
		}
		else if (statement[0] == "nor") {
			gate = new Nor;
		}
		else if (statement[0] == "or") {
			gate = new Or;
		}
		else if (statement[0] == "xor") {
			gate = new Xor;
		}
		else if (statement[0] == "xnor") {
			gate = new Xnor;
		}
		else if (statement[0] == "not") {
			gate = new Not;
		}
		else {
			continue;
		}

		string name = statement[1];
		string output_str = statement[2];
		vector<string> inputs_str(statement.begin() + 3, statement.end());

		Wire *output;
		output = wires_map.at(output_str);
		vector<Wire*> inputs(inputs_str.size());
		transform(inputs_str.begin(), inputs_str.end(), inputs.begin(), [&](const string& str) {
				return wires_map.at(str);
				});

		gate->set_name(name);
		gate->set_io(inputs, output);
		gates.push_back(gate);
	}
	return gates;
}



string get_file_string(const string& filepath) {
	ifstream file(filepath);
	string file_str((istreambuf_iterator<char>(file)),
			(istreambuf_iterator<char>()));
	file.close();
	return file_str;
}

vector<vector<char>> get_input_vectors(const string& filepath) {
	ifstream file(filepath);
	vector<vector<char>> input_vectors;
	string line;
	while (getline(file, line)) {
		input_vectors.push_back(vector<char>(line.begin(), line.end()));
	}
	return input_vectors;
}


vector<string> extract_statement(const string &str, string::iterator &it, const vector<char>& delims, char end_c) { 
	vector<string> words;
	string word;
	char prev_c = 0;
	for (; it != str.end() && *it != end_c; it++) {
		bool matched_delim = false;
		for (auto delim : delims) {
			if (*it == delim) {
				if (!word.empty()) {
					words.push_back(word);
					word = "";
				}
				matched_delim = true;
				break;
			}
		}
		if (!matched_delim) {
			//check for comments
			if (prev_c == '/' && *it == '/') {
				for (; it != str.end() && *it != '\n'; it++);
				return vector<string>();
			}

			word += *it;
		}
		prev_c = *it;
	}
	if (it != str.end()) {
		it++;
	}
	if (!word.empty()) { 
		words.push_back(word);
	}
	return words;
}
