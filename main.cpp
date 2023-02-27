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
	Wire();
	char value;
private:
	string name;
	int activity_count;
};

class Gate {
public:
	void set_io(const vector<Wire*>&, Wire * const);
	void set_name(const string&);
	virtual void evaluate() = 0;
	virtual ~Gate() = default;
protected:
	string name;
	vector<Wire*> inputs;
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


void cleanup(const map<string, Wire*>&, const vector<Gate*>&);

void simulate_default_order(const map<string, Wire*>&, const map<string, Wire*>&, const map<string, Wire*>&, 
		const vector<Gate*>&, const vector<vector<char>>&);

void simulate_ordered(const map<string, Wire*>&, const map<string, Wire*>&, const map<string, Wire*>&,
		const vector<Gate*>&, const vector<vector<char>>);


void add_wires_to_map(const vector<string>& statement, map<string, Wire*>&);

tuple<map<string, Wire*>, map<string, Wire*>, map<string, Wire*>>
init_wires(const vector<vector<string>>& statements);

vector<Gate*> init_gates(const vector<vector<string>>&, const map<string, Wire*>&);

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

	auto it = file_str.begin();
	vector<vector<string>> statements;
	while (it != file_str.end()) {
		auto words = extract_statement(file_str, it, {',', ' ', '\r', '\n', '\t', '(', ')'}, ';');
		if (!words.empty()) {
			statements.push_back(words);
		}
	}

	auto [wires_map, inputs_map, outputs_map] = init_wires(statements);
	vector<Gate*> gates = init_gates(statements, wires_map);

	//simulating gates with defaul random order in the file
	vector<vector<char>> input_vector;
	//TODO
	//you've got to initialize input_vector here
	simulate_default_order(wires_map, inputs_map, outputs_map, gates, input_vector);
	//simulating gaetes with the correct order
	simulate_ordered(wires_map, inputs_map, outputs_map, gates, input_vector);

	// print(statements);
	cleanup(wires_map, gates);
	return 0;
}

void cleanup(const map<string, Wire*>& wires, const vector<Gate*>& gates) {
	for (auto const& [key, wire] : wires) {
		delete wire;
	}
	for (auto const& gate : gates) {
		delete gate;
	}
}

void simulate_default_order(
		const map<string, Wire*>& wires, const map<string, Wire*>& inputs,
		const map<string, Wire*>& outputs, const vector<Gate*>& gates,
		const vector<vector<char>>& input_vectors) {
	for (const auto& input_vector : input_vectors) {
		// set_inputs(inputs, input_vectors);
		for (auto it = gates.begin(); it != gates.end(); it++) {
			(*it)->evaluate();
		}
	}
}

void simulate_ordered(
		const map<string, Wire*>& wires, const map<string, Wire*>& inputs,
		const map<string, Wire*>& outputs, const vector<Gate*>& gates,
		const vector<vector<char>>& input_vectors) {
}


void set_inputs(const map<string, Wire*>& inputs, const vector<char>& input_vector) {
	for (auto const& [ey, input] : inputs) {
		// input->value = 
	}
}



tuple<map<string, Wire*>, map<string, Wire*>, map<string, Wire*>>
init_wires(const vector<vector<string>>& statements) {
	map<string, Wire*> wires, inputs, outputs;
	for (const auto& statement : statements) {
		if (statement.size() == 0) {
			continue;
		}
		if (statement[0] == "input") {
			add_wires_to_map(statement, inputs);
		}
		if (statement[0] == "output") {
			add_wires_to_map(statement, outputs);
		}
		if (statement[0] == "input" || statement[0] == "output" || statement[0] == "wire") {
			add_wires_to_map(statement, wires);
		}
	}

	return {wires, inputs, outputs};
}

vector<Gate*> init_gates(const vector<vector<string>>& statements, const map<string, Wire*>& wires) {
	vector<Gate*> gates;
	for (const auto& statement : statements) {
		if (statement.size() < 4) {
			continue;
		}
		Gate *gate;
		// TODO
		// mabey gate constructor?
		if (statement[0] == "nand") {
			gate = new Nand;
		}
		if (statement[0] == "and") {
			gate = new And;
		}
		if (statement[0] == "nor") {
			gate = new Nor;
		}
		if (statement[0] == "or") {
			gate = new Or;
		}
		if (statement[0] == "xor") {
			gate = new Xor;
		}
		if (statement[0] == "xnor") {
			gate = new Xnor;
		}
		if (statement[0] == "not") {
			gate = new Not;
		}
		else {
			continue;
		}

		string name = statement[1];
		string output_str = statement.back();
		vector<string> inputs_str(statement.begin() + 2, statement.end() - 1);

		Wire *output;
		output = wires.at(output_str);
		vector<Wire*> inputs(inputs_str.size());
		//TODO check this
		// cout << "{map}\n";
		// for (auto [key, val] : wires) {
		// 	cout << key << " ";
		// }
		// cout << "\n";
		// cout << "{endMap}\n";
		transform(inputs_str.begin(), inputs_str.end(), inputs.begin(), [&](const string& str) {
				// cout << "{str}\n";
				// cout << str << "\n";
				// cout << "{endStr}";
				// cout << endl;
				return wires.at(str);
				});

		gate->set_name(name);
		gate->set_io(inputs, output);
		gates.push_back(gate);
	}
	return gates;
}


void add_wires_to_map(const vector<string>& words, map<string, Wire*>& m) {
	for (int i = 1; i < words.size(); i++) {
		const auto& word = words[i];
		if (word == "wire") {
			continue;
		}
		if (m.find(word) == m.end()) {
			//TODO Wire constructor
			m[word] = new Wire;
		}
	}
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

Wire::Wire() {}

void Gate::set_name(const string& _name) {
	name = _name;
}

void Gate::set_io(const vector<Wire*>& _inputs, Wire * const _output) {
	inputs = _inputs;
	output = _output;
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
