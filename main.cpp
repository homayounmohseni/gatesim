#include <iostream>
#include <iterator>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <algorithm>

using namespace std;

string get_file_string(const string&);


class Wire {
public:
	Wire(const string&);
	string get_name();
	char value;
	bool changed;
private:
	string name;
	int activity_count;
};

class Gate {
public:
	void set_io(const vector<Wire*>&, Wire * const);
	void set_name(const string&);
	string get_name();
	virtual void evaluate() = 0;
	bool are_inputs_valid();
	void make_output_chagned();
	virtual ~Gate() = default;
	//TODO make inputs private
	vector<Wire*> inputs;
protected:
	string name;
	Wire *output;
};

class Not : public Gate {
public:
	Not();
	virtual void evaluate();
};

class Nand : public Gate {
public:
	Nand();
	virtual void evaluate();
};

class And : public Gate {
public:
	And();
	virtual void evaluate();
};

class Nor : public Gate {
public:
	Nor();
	virtual void evaluate();
};

class Or : public Gate {
public:
	Or();
	virtual void evaluate();
};

class Xor : public Gate {
public:
	Xor();
	virtual void evaluate();
};

class Xnor : public Gate {
public:
	Xnor();
	virtual void evaluate();
};


void cleanup(const vector<Wire*>&, const vector<Gate*>&);

void simulate_default_order(const vector<Wire*>&, const vector<Wire*>&, const vector<Wire*>&,
		const vector<Gate*>&, const vector<vector<char>>&);

void simulate_ordered(const vector<Wire*>&, const vector<Wire*>&, const vector<Wire*>&,
		const vector<Gate*>&, const vector<vector<char>>&);

void print_wires(const vector<Wire*>&);

void set_inputs(const vector<Wire*>&, const vector<char>&);

void reset_wires(const vector<Wire*>&);



tuple<vector<Wire*>, vector<Wire*>, vector<Wire*>>
init_wires(const vector<vector<string>>& statements);

vector<Gate*> init_gates(const vector<vector<string>>&, const vector<Wire*>&);

void add_wires_to_vector(const vector<string>& statement, vector<Wire*>&, map<string, Wire*>&);

vector<string> extract_statement(const string &, string::iterator&, const vector<char>&, char);

template<typename T> bool are_all(const vector<T>&, const T&);

template<typename T> bool is_any(const vector<T>&, const T&);

vector<char> wires_to_chars(vector<Wire*>&);


void print(const vector<string>& v) {
	cout << "{count: " << v.size() << "}\n";
	cout << "[\n";
	// for (int i = 0; i < v.size(); i++) {
	// 	cout << "{element " << i << "}:\n";
	// 	cout << v[i] << "\n";
	// 	for (char c : v[i]) {
	// 		cout << (int)c << " ";
	// 	}
	// 	cout << "\n";
	// }
	for (const auto& _v : v) {
		cout << _v << " ";
	}
	cout << "\n]";
	cout << endl;
}
void print(const vector<vector<string>>& v) {
	for (int i = 0; i < v.size(); i++) {
		print(v[i]);
	}
}


//TODO
int main(int argc, char **argv) {
	string filepath;
	if (argc < 2) {
		filepath = "c432.v";
	}
	else {
		filepath = string(argv[1]);
	}

	string file_str = get_file_string(filepath);

	vector<vector<string>> statements;
	for (auto it = file_str.begin(); it != file_str.end(); ) {
		auto words = extract_statement(file_str, it, {',', ' ', '\r', '\n', '\t', '(', ')'}, ';');
		if (!words.empty()) {
			statements.push_back(words);
		}
	}

	auto [wires, inputs, outputs] = init_wires(statements);
	vector<Gate*> gates = init_gates(statements, wires);



	vector<vector<char>> input_vectors;
	input_vectors.push_back(vector<char>(inputs.size(), '1'));
	input_vectors.push_back(vector<char>(inputs.size(), '0'));
	{
		vector<char> vc;
		for (int i = 0; i < inputs.size(); i++) {
			vc.push_back('0' + i % 2);
		}
		input_vectors.push_back(vc);
	}
	


	simulate_default_order(wires, inputs, outputs, gates, input_vectors);
	simulate_ordered(wires, inputs, outputs, gates, input_vectors);

	// print(statements);
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

void simulate_default_order(const vector<Wire*>& wires, const vector<Wire*>& inputs, const vector<Wire*>& outputs, 
		const vector<Gate*>& gates, const vector<vector<char>>& input_vectors) {
	for (const auto& input_vector : input_vectors) {
		set_inputs(inputs, input_vector);
		for (auto it = gates.begin(); it != gates.end(); it++) {
			(*it)->evaluate();
		}

		for (auto gate : gates) {
			gate->evaluate();
		}
		print_wires(inputs);
		print_wires(outputs);

		reset_wires(wires);
	}
}

void simulate_ordered (const vector<Wire*>& wires, const vector<Wire*>& inputs, const vector<Wire*>& outputs, 
		const vector<Gate*>& gates, const vector<vector<char>>& input_vectors) {
	for (const auto& input_vector : input_vectors) {
		set_inputs(inputs, input_vector);

		vector<Gate*> gates_left(gates);
		while (!gates_left.empty()) {
			// cout << gates_left.size() << '\n';
			for (auto it = gates_left.begin(); it != gates_left.end(); ) {
				auto gate = *it;
				// cout << gate->get_name() << '\n';
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
		print_wires(inputs);
		print_wires(outputs);

		reset_wires(wires);
	}
}

void print_wires(const vector<Wire*>& wires) {
	for (auto it = wires.begin(); it != wires.end(); it++) {
		auto wire = *it;
		cout << wire->value;
	}
	cout << '\n';
}

void set_inputs(const vector<Wire*>& inputs, const vector<char>& input_vector) {
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
		// string output_str = statement.back();
		string output_str = statement[2];
		// vector<string> inputs_str(statement.begin() + 2, statement.end() - 1);
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

char and_char(char a, char b) {
	if (a == '1' && b == '1') {
		return '1';
	}
	else if (a == '0' || b == '0') {
		return '0';
	}
	return 'x';
}

template<typename T>
bool are_all(const vector<T>& vec, const T& val) {
	return all_of(vec.begin(), vec.end(), [&](T element) { return element == val; });
}

template<typename T>
bool is_any(const vector<T>& vec, const T& val) {
	return any_of(vec.begin(), vec.end(), [&](T element) { return element == val; });
}

vector<char> wires_to_chars(vector<Wire*>& wires) {
	vector<char> chars(wires.size());
	transform(wires.begin(), wires.end(), chars.begin(), [](Wire *wire) { return wire->value; });
	return chars;
}

Wire::Wire(const string& _name) {
	value = 'x';
	name = _name;
	changed = false;
}

string Wire::get_name() {
	return name;
}


void Gate::set_name(const string& _name) {
	name = _name;
}

string Gate::get_name() {
	return name;
}

void Gate::set_io(const vector<Wire*>& _inputs, Wire * const _output) {
	inputs = _inputs;
	output = _output;
}

bool Gate::are_inputs_valid() {
	return all_of(inputs.begin(), inputs.end(), [](Wire *wire) { return wire->changed; });
}

void Gate::make_output_chagned() {
	output->changed = true;
}

Nand::Nand() {}

void Nand::evaluate() {
	auto inputs_char = wires_to_chars(inputs);
	if (are_all(inputs_char, '1')) {
		output->value = '0';
	}
	else if (is_any(inputs_char, '0')) {
		output->value = '1';
	}
	else {
		output->value = 'x';
	}
}

And::And() {}

void And::evaluate() {
	auto inputs_char = wires_to_chars(inputs);
	if (are_all(inputs_char, '1')) {
		output->value = '1';
	}
	else if (is_any(inputs_char, '0')) {
		output->value = '0';
	}
	else {
		output->value = 'x';
	}
}

Nor::Nor() {}

void Nor::evaluate() {
	auto inputs_char = wires_to_chars(inputs);
	if (is_any(inputs_char, '1')) {
		output->value = '0';
	}
	else if (are_all(inputs_char, '0')) {
		output->value = '1';
	}
	else {
		output->value = 'x';
	}
}

Or::Or() {}

void Or::evaluate() {
	auto inputs_char = wires_to_chars(inputs);
	if (is_any(inputs_char, '1')) {
		output->value = '1';
	}
	else if (are_all(inputs_char, '0')) {
		output->value = '0';
	}
	else {
		output->value = 'x';
	}
}

Xnor::Xnor() {}

void Xnor::evaluate() {
	int ones_cnt = 0;
	for (const auto& input : inputs) {
		if (input->value == '1') {
			ones_cnt++;
		}
		else if (input->value != '0') {
			output->value = 'x';
			return;
		}
	}
	if (ones_cnt % 2 == 0) {
		output->value = '1';
	}
	else {
		output->value = '0';
	}
}

Xor::Xor() {}

void Xor::evaluate() {
	int ones_cnt = 0;
	for (const auto& input : inputs) {
		if (input->value == '1') {
			ones_cnt++;
		}
		else if (input->value != '0') {
			output->value = 'x';
			return;
		}
	}
	if (ones_cnt % 2 == 0) {
		output->value = '0';
	}
	else {
		output->value = '1';
	}
}

Not::Not() {}

void Not::evaluate() {
	if (inputs[0]->value == '0') {
		output->value = '1';
	}
	else if (inputs[0]->value == '1') {
		output->value = '0';
	}
	else {
		output->value = 'x';
	}
}
