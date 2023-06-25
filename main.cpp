#include "main.hpp"
#include "primitives.hpp"

using namespace std;

int main(int argc, char **argv) {
	string verilog_filepath, input_feed_filepath, output_netlist_filepath, output_wave_filepath;
	if (argc < 5) {
		verilog_filepath = "c432.v";
		input_feed_filepath = "inputfeed.txt";
		output_netlist_filepath = "netlist.txt";
		output_wave_filepath = "wave.vcd";
	}
	else {
		verilog_filepath = string(argv[1]);
		input_feed_filepath = string(argv[2]);
		output_netlist_filepath = string(argv[3]);
		output_wave_filepath = string(argv[4]);
	}

	string vfile_str = get_file_string(verilog_filepath);

	vector<vector<string>> statements;
	for (auto it = vfile_str.begin(); it != vfile_str.end(); ) {
		auto words = extract_statement(vfile_str, it, {',', ' ', '\r', '\n', '\t', '(', ')'}, ';');
		if (!words.empty()) {
			statements.push_back(words);
		}
	}

	string module_name = init_module_name(statements);
	auto [wires, inputs, outputs, ports] = init_wires(statements);
	vector<Gate*> gates = init_gates(statements, wires);
	auto input_feed = get_input_feed(input_feed_filepath, inputs);

	dump_netlist(output_netlist_filepath, wires, inputs, outputs, gates);

	VCDTracer tracer(output_wave_filepath, module_name);
	EventEngine engine(input_feed, &tracer);

	const int runtime_duration = 1000;
	engine.run(runtime_duration);

	tracer.dump(ports, runtime_duration);

	cleanup(wires, gates);
	return 0;
}

VCDTracer::VCDTracer(const string& _dumpfilepath, const string& _module_name) : 
	dumpfilepath(_dumpfilepath), module_name(_module_name) {}

void VCDTracer::add_change(const Event& event) {
	change_vector.push_back(event);
}

void VCDTracer::dump(const vector<Wire*>& wires, const int runtime_duration) const {
	unordered_map<Wire*, string> wire_identifiers;
	string vcd_id {VCD_CHAR_ID_FIRST};
	for (auto wire : wires) {
		wire_identifiers[wire] = vcd_id;
		vcd_id = next_vcd_id(vcd_id);
	}

	ofstream f(dumpfilepath);
	f << "$timescale 1 ns $end" << '\n';
	f << "$scope module " << module_name << " $end\n";
	for (auto wire : wires) {
		f << "$var wire 1 " << wire_identifiers[wire] << ' ' << wire->get_name() << " $end\n";
	}
	f << "$uscope $end\n";
	f << "$enddefinitions $end\n";
	f << "$dumpvars\n";
	for (auto wire : wires) {
		f << 'x' << wire_identifiers[wire] << '\n';
	}
	f << "$end\n";

	int prev_time = -1;
	for (const auto& change : change_vector) {
		auto wire = change.wire_assignment.wire;
		if (wire_identifiers.find(wire) != wire_identifiers.end()) {
			const int time = change.time;
			if (time > runtime_duration) {
				break;
			}
			if (time != prev_time) {
				f << '#' << change.time << '\n';
				prev_time = time;
			}
			f << change.wire_assignment.value;
			f << wire_identifiers[wire] << '\n';
		}
	}
	f << '#' << runtime_duration << '\n';
	f << "$dumpoff\n";
	f.close();
}


EventEngine::EventEngine(const list<Event>& _input_feed, VCDTracer *_tracer) :
	input_feed(_input_feed) {
	tracer = _tracer;
}


void EventEngine::schedule_activity(const Event& event) {
	int ind = event.time % time_array.size();
	time_array[ind].push_back(event.wire_assignment);
}

void EventEngine::schedule_activity(const Event& event, int added_time) {
	int ind = (event.time + added_time) % time_array.size();
	time_array[ind].push_back(event.wire_assignment);
}

void EventEngine::schedule_activity(const list<Event>& events) {
	for (const auto& event : events) {
		schedule_activity(event);
	}
}


void EventEngine::run(const int runtime_duration) {
	for (int cur_time = 0; cur_time < runtime_duration; cur_time++) {
		const int cur_index = cur_time % time_array.size();
		while (!input_feed.empty() && input_feed.front().time == cur_time) {
			time_array[cur_index].push_front(input_feed.front().wire_assignment);
			input_feed.pop_front();
		}
		set<Gate*> gates_to_evaluate;
		for (const auto& wire_assignment : time_array[cur_index]) {
			const auto [wire, value] = wire_assignment;
			wire->value = value;
			tracer->add_change({wire_assignment, cur_time});
			for (auto gate : wire->output_gates) {
				gates_to_evaluate.insert(gate);
			}
		}
		for (auto gate : gates_to_evaluate) {
			auto new_event = gate->evaluate();
			schedule_activity(new_event, cur_time);
		}
		time_array[cur_index].clear();
	}
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


void dump_wires(ofstream& f, const vector<Wire*>& wires) {
	for (auto wire : wires) {
		f << "Name: ";
		f << wire->get_name();
		f << '\n';
		f << "Output Gates:\n";
		for (auto gate : wire->output_gates) {
			f << gate->get_name() << " ";
		}
		f << "\n\n";
	}
}

void dump_gates(ofstream& f, const vector<Gate*>& gates) {
	for (auto gate : gates) {
		f << "Gate Name: ";
		f << gate->get_name();
		f << '\n';
		auto inputs = gate->get_inputs();
		f << "Inputs:\n";
		for (auto input : inputs) {
			f << input->get_name() << " ";
		}
		f << '\n';
		f << "Output: ";
		f << gate->get_output()->get_name();
		f << "\n\n";
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

string init_module_name(const vector<vector<string>>& statements) {
	for (const auto& statement : statements) {
		if (!statement.empty() && statement[0] == "module") {
			assert(statement.size() >= 2);
			return statement[1];
		}
	}
	return "";
}

tuple<vector<Wire*>, vector<Wire*>, vector<Wire*>, vector<Wire*>>
init_wires(const vector<vector<string>>& statements) {
	vector<Wire*> wires, inputs, outputs, ports;
	unordered_map<string, Wire*> wires_map;
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
		if (statement[0] == "input" || statement[0] == "output") {
			add_wires_to_vector(statement, ports, wires_map);
		}
		if (statement[0] == "input" || statement[0] == "output" || statement[0] == "wire") {
			add_wires_to_vector(statement, wires, wires_map);
		}
	}
	return {wires, inputs, outputs, ports};
}

void add_wires_to_vector(const vector<string>& words, vector<Wire*>& v, unordered_map<string, Wire*>& m) {
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

	unordered_map<string, Wire*> wires_map;
	for (auto wire : wires) {
		wires_map[wire->get_name()] = wire;
	}
	for (const auto& statement : statements) {
		if (statement.size() < 4) {
			continue;
		}
		Gate *gate;
		if (statement[0] == "nand") {
			gate = new Nand(NAND_DELAY);
		}
		else if (statement[0] == "and") {
			gate = new And(AND_DELAY);
		}
		else if (statement[0] == "nor") {
			gate = new Nor(NOR_DELAY);
		}
		else if (statement[0] == "or") {
			gate = new Or(OR_DELAY);
		}
		else if (statement[0] == "xor") {
			gate = new Xor(XOR_DELAY);
		}
		else if (statement[0] == "xnor") {
			gate = new Xnor(XNOR_DELAY);
		}
		else if (statement[0] == "not") {
			gate = new Not(NOT_DELAY);
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

		for (auto input : inputs) {
			input->output_gates.push_back(gate);
		}

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

list<Event> get_input_feed(const string& filepath, const vector<Wire*>& inputs) {
	ifstream file(filepath);
	list<Event> input_feed;
	string word;
	while (file >> word) {
		int time = stoi(word);
		file >> word;
		assert(word.size() == inputs.size());
		for (int i = 0; i < word.size(); i++) {
			Event e;
			e.time = time;
			e.wire_assignment.wire = inputs[i];
			e.wire_assignment.value = word[i];
			input_feed.push_back(e);
		}
	}
	file.close();
	return input_feed;
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

string next_vcd_id(string id) {
	assert(!id.empty());
	if (id.back() == VCD_CHAR_ID_LAST) {
		id.back() = VCD_CHAR_ID_FIRST;
		int i;
		for (i = id.size() - 2; i >= 0; i--) {
			if (id[i] != VCD_CHAR_ID_LAST) {
				id[i]++;
				break;
			}
			else {
				id[i] = VCD_CHAR_ID_FIRST;
			}
		}
		if (i == -1) {
			id = VCD_CHAR_ID_FIRST + id;
		}
	}
	else {
		id.back()++;
	}
	return id;
}

void dump_netlist(const string& filepath, const vector<Wire*>& wires, const vector<Wire*>& inputs, 
		const vector<Wire*>& outputs, const vector<Gate*>& gates) {
	ofstream f(filepath);
	f << "===================================";
	f << "ALL WIRES";
	f << "===================================";
	f << '\n';
	dump_wires(f, wires);
	f << "===================================";
	f << "INPUTS";
	f << "===================================";
	f << '\n';
	dump_wires(f, inputs);
	f << "===================================";
	f << "Outputs";
	f << "===================================";
	f << '\n';
	dump_wires(f, outputs);
	f << "===================================";
	f << "Gates";
	f << "===================================";
	f << '\n';
	dump_gates(f, gates);

	f.close();
}
