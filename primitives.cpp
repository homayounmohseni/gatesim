#include "primitives.hpp"
#include <algorithm>


using namespace std;

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

string Wire::get_name() const {
	return name;
}


void Gate::set_name(const string& _name) {
	name = _name;
}

string Gate::get_name() const {
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

pair<ActivityEntry, int> Nand::evaluate() {
	auto inputs_char = wires_to_chars(inputs);
	char value;
	if (are_all(inputs_char, '1')) {
		value = '0';
	}
	else if (is_any(inputs_char, '0')) {
		value = '1';
	}
	else {
		value = 'x';
	}
	return {{output, value}, delay};
}

And::And() {}

pair<ActivityEntry, int> And::evaluate() {
	auto inputs_char = wires_to_chars(inputs);
	char value;
	if (are_all(inputs_char, '1')) {
		value = '1';
	}
	else if (is_any(inputs_char, '0')) {
		value = '0';
	}
	else {
		value = 'x';
	}
	return {{output, value}, delay};
}

Nor::Nor() {}

pair<ActivityEntry, int> Nor::evaluate() {
	auto inputs_char = wires_to_chars(inputs);
	char value;
	if (is_any(inputs_char, '1')) {
		value = '0';
	}
	else if (are_all(inputs_char, '0')) {
		value = '1';
	}
	else {
		value = 'x';
	}
	return {{output, value}, delay};
}

Or::Or() {}

pair<ActivityEntry, int> Or::evaluate() {
	auto inputs_char = wires_to_chars(inputs);
	char value;
	if (is_any(inputs_char, '1')) {
		value = '1';
	}
	else if (are_all(inputs_char, '0')) {
		value = '0';
	}
	else {
		value = 'x';
	}
	return {{output, value}, delay};
}

Xnor::Xnor() {}

pair<ActivityEntry, int> Xnor::evaluate() {
	int ones_cnt = 0;
	for (const auto& input : inputs) {
		if (input->value == '1') {
			ones_cnt++;
		}
		else if (input->value != '0') {
			output->value = 'x';
			return {{output, 'x'}, delay};
		}
	}
	char value;
	if (ones_cnt % 2 == 0) {
		value = '1';
	}
	else {
		value = '0';
	}
	return {{output, value}, delay};
}

Xor::Xor() {}

pair<ActivityEntry, int> Xor::evaluate() {
	int ones_cnt = 0;
	for (const auto& input : inputs) {
		if (input->value == '1') {
			ones_cnt++;
		}
		else if (input->value != '0') {
			output->value = 'x';
			return {{output, 'x'}, delay};
		}
	}
	char value;
	if (ones_cnt % 2 == 0) {
		value = '0';
	}
	else {
		value = '1';
	}
	return {{output, value}, delay};
}

Not::Not() {}

pair<ActivityEntry, int> Not::evaluate() {
	char value;
	if (inputs[0]->value == '0') {
		value = '1';
	}
	else if (inputs[0]->value == '1') {
		value = '0';
	}
	else {
		value = 'x';
	}
	return {{output, value}, delay};
}
