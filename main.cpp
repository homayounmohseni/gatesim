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
private:
	char value;
	int activity_count;
};

class Gate {
public:
	Gate();
	void set_io(const vector<Wire*>&, Wire * const);
	void set_name(const string&);
	virtual void evaluate() = 0;
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


void add_wires_to_map(const vector<string>& statement, map<string, Wire*>&);

tuple<map<string, Wire*>, map<string, Wire*>, map<string, Wire*>>
init_wires(const vector<vector<string>>& statements);

vector<Gate*> init_gates(const vector<vector<string>>&, const map<string, Wire*>&);

vector<string> extract_statement(const string &, string::iterator&, const vector<char>&, char);

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

	// map<string, Wire*> wires_map, inputs_map, outputs_map;
	auto [wires_map, inputs_map, outputs_map] = init_wires(statements);
	vector<Gate*> gates = init_gates(statements, wires_map);

	// print(statements);

	return 0;
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


Wire::Wire() {}

Gate::Gate() {}

void Gate::set_name(const string& _name) {
	name = _name;
}

void Gate::set_io(const vector<Wire*>& _inputs, Wire * const _output) {
	inputs = _inputs;
	output = _output;
}

Nand::Nand() {}
void Nand::evaluate() {}
And::And() {}
void And::evaluate() {}
Nor::Nor() {}
void Nor::evaluate() {}
Or::Or() {}
void Or::evaluate() {}
Xnor::Xnor() {}
void Xnor::evaluate() {}
Xor::Xor() {}
void Xor::evaluate() {}
Not::Not() {}
void Not::evaluate() {}
