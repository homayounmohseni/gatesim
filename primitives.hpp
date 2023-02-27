#ifndef __PRIMITIVES_HPP__
#define __PRIMITIVES_HPP__

#include <string>
#include <vector>

class Wire {
public:
	Wire(const std::string&);
	std::string get_name();
	char value;
	bool changed;
private:
	std::string name;
};

class Gate {
public:
	void set_io(const std::vector<Wire*>&, Wire * const);
	void set_name(const std::string&);
	std::string get_name();
	virtual void evaluate() = 0;
	bool are_inputs_valid();
	void make_output_chagned();
	virtual ~Gate() = default;
protected:
	std::string name;
	std::vector<Wire*> inputs;
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

template<typename T> bool are_all(const std::vector<T>&, const T&);

template<typename T> bool is_any(const std::vector<T>&, const T&);

std::vector<char> wires_to_chars(std::vector<Wire*>&);

#endif
