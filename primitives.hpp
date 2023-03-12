#ifndef __PRIMITIVES_HPP__
#define __PRIMITIVES_HPP__

#include <string>
#include <vector>

class Gate;
class Wire;


// typedef std::pair<Wire*, char> ActivityEntry;

struct WireAssignment {
	Wire *wire;
	char value;
};

struct Event {
	WireAssignment wire_assignment;
	int time;
};

class Wire {
public:
	Wire(const std::string&);
	std::string get_name() const;
	char value;
	bool changed;
	std::vector<Gate*> output_gates;
private:
	std::string name;
};

class Gate {
public:
	void set_io(const std::vector<Wire*>&, Wire * const);
	void set_name(const std::string&);
	std::string get_name() const;
	virtual Event evaluate() = 0;
	// virtual std::pair<ActivityEntry, int> evaluate() = 0;
	// virtual void evaluate() = 0;
	bool are_inputs_valid();
	void make_output_chagned();
	virtual ~Gate() = default;
protected:
	std::string name;
	std::vector<Wire*> inputs;
	Wire *output;
	int delay;
};

class Not : public Gate {
public:
	Not();
	virtual Event evaluate();
	// virtual std::pair<ActivityEntry, int> evaluate();
};

class Nand : public Gate {
public:
	Nand();
	virtual Event evaluate();
};

class And : public Gate {
public:
	And();
	virtual Event evaluate();
};

class Nor : public Gate {
public:
	Nor();
	virtual Event evaluate();
};

class Or : public Gate {
public:
	Or();
	virtual Event evaluate();
};

class Xor : public Gate {
public:
	Xor();
	virtual Event evaluate();
};

class Xnor : public Gate {
public:
	Xnor();
	virtual Event evaluate();
};

template<typename T> bool are_all(const std::vector<T>&, const T&);

template<typename T> bool is_any(const std::vector<T>&, const T&);

std::vector<char> wires_to_chars(std::vector<Wire*>&);

#endif
