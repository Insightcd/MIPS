#pragma once

#include "Header.h"
#include "storage.hpp"

map<string, int> text_label, data_label;


enum instr_type {
	ADD, ADDU, ADDIU,
	SUB, SUBU,
	MUL, MULU,
	MUL, MULU,
	DIV, DIVU,
	DIV, DIVU,
	XOR, XORU,
	NEG, NEGU,
	REM, REMU,
	LI, 
	SEG, SGE,
	SGT, SLE,
	SLT, SNE,
	B, BEQ, BNE,
	BGE, BLE,
	BGT, BLT,
	BEQZ, BNEZ,
	BLEZ, BGEZ,
	BGTZ, BLTZ,
	J, JR, JAL, JALR,
	LA, LB, LH, LW,
	SB, SH, SW,
	MOVE, MFHI, MFLO,
	NOP, SYSCALL
};

class instruction {//include nop instruction
public:
	int32_t status = 0;

	instruction(int32_t s = 0):status(s) {}//instruction_fetch
	virtual void data_preparation() {}
	virtual void execute() {}
	virtual void memory_access() {}
	virtual void write_back() {}

	virtual ~instruction() {}
	virtual instruction* copy() { return new instruction(*this); }
};

//write back
class load : public instruction {
public:
	int32_t rdest, rsrc = -1;
	string address;
	int32_t val = 0, pos = -1, offset = 0;

	load(const string &r, const string &a): instruction(), rdest(stoi(r)), address(a) {
		if (data_label.find(a) != data_label.end()) {
			pos = data_label[a];
			return;
		}

		int i = a.find('(');
		int j = a.find(')');
		offset = stoi(a.substr(0, i));
		rsrc = regist::storeg(a.substr(i + 1, j - i - 1));
	}
	virtual instruction* copy() { return new load(*this); }
	virtual void data_preparation(){//get register's pos from register number : rsrc
		if (pos != -1) return;
		pos = _regist.reg[rsrc];///??
	}
	virtual void execute() { pos += offset; }
	virtual void write_back(){ 
		_regist.reg[rsrc] = val;
	}
	virtual ~load(){}
};

class la : public load {
public:
	la(const string &r, const string &a) :load(r,a) {}
	virtual ~la() {}
	virtual instruction* copy() { return new la(*this); }
	virtual void write_back() { _regist.reg[rsrc] = val; }
};
class lb : public load {
public:
	lb(const string &r, const string &a) :load(r, a) {}
	virtual ~lb() {}
	virtual instruction* copy() { return new lb(*this); }
	virtual void memory_access() { memcpy(&val, _memory.mem + pos, 1); }
};
class lh : public load {
public:
	lh(const string &r, const string &a) :load(r, a) {}
	virtual ~lh() {}
	virtual instruction* copy() { return new lh(*this); }
	virtual void memory_access() { memcpy(&val, _memory.mem + pos, 2); }
};
class lw : public load {
public:
	lw(const string &r, const string &a) :load(r, a) {}
	virtual ~lw() {}
	virtual instruction* copy() { return new lw(*this); }
	virtual void memory_access() { memcpy(&val, _memory.mem + pos, 4); }
};


//??? write back??
class store : public instruction {
public:
	int rdest, rsrc = -1;
	string address;
	int val = 0, pos = -1, offset = 0;
	store(const string &r, const string &a):instruction(), rdest(regist::storeg(r)), address(a){
		if (data_label.find(a) != data_label.end()) {
			pos = data_label[a];
			return;
		}
		int i = a.find('(');
		int j = a.find(')');
		offset = stoi(a.substr(0, i));
		rsrc = regist::storeg(a.substr(i + 1, j - i - 1));
	}
	virtual instruction* copy() { return new store(*this); }
	virtual void data_prepare() {
		val = _regist.reg[rdest];
		if (pos != -1) return;
		pos = _regist.reg[rsrc];
	}
	virtual void execute() { pos += offset; }
};

class sb :public store {
public:
	sb(const string &r, const string &a):store(r,a){}
	virtual instruction* copy() { return new sb(*this); }
	virtual void memory_access() { memcpy(_memory.mem + pos, &val, 1); }
};
class sh : public store {
public:
	sh(const string &r, const string &a) :store(r, a) {}
	virtual instruction* copy() { return new sh(*this); }
	virtual void memory_access() { memcpy(_memory.mem + pos, &val, 2); }
};

class sw : public store {
public:
	sw(const string &r, const string &a) :store(r, a) {}
	virtual instruction* copy() { return new sw(*this); }
	virtual void memory_access() { memcpy(_memory.mem + pos, &val, 4); }
};

//class migration : public instruction {
//public:
//	int rdest, rsrc, imm;
//	migration(const string &r, const string &_r) :rdest(regist::storeg(r)), rsrc(regist::storeg(_r)) {
//		if (rsrc == -1) imm = stoi(_r);
//	}
//	virtual instruction* copy() { return new migration(*this); }
//	virtual void data_preparation() {
//		if (rsrc == -1) return;
//		imm = _regist.reg[rsrc];
//	}
//	virtual void write_back() {
//		_regist.reg[rdest] = imm;
//	}
//};
class move : public instruction {
public:
	int rdest, rsrc, imm;
	move(const string &r, const string &_r) :rdest(regist::storeg(r)), rsrc(regist::storeg(_r)) {
		if (rsrc == -1) imm = stoi(_r);
	}
	virtual instruction* copy() { return new move(*this); }
	virtual void data_preparation() {
		if (rsrc == -1) return;
		imm = _regist.reg[rsrc];
	}
	virtual void write_back() {
		_regist.reg[rdest] = imm;
	}
};
class mfhi :public move {
public:
	mfhi(const string &r, const string &_r = "hi"):move(r, _r){}
	virtual instruction* copy() { return new mfhi(*this); }
	virtual void data_preparation() {
		if (rsrc == -1) return;
		imm = _regist.reg[rsrc];
	}
	virtual void write_back() {
		_regist.reg[rdest] = imm;
	}
};
class mflo :public move {
public:
	mflo(const string &r, const string &_r = "lo") :move(r, _r) {}
	virtual instruction* copy() { return new mflo(*this); }
	virtual void data_preparation() {
		if (rsrc == -1) return;
		imm = _regist.reg[rsrc];
	}
	virtual void write_back() {
		_regist.reg[rdest] = imm;
	}
};

class calculation : public instruction {
public:
	int rdest, rsrc1, rsrc2;
	int imm1, imm2;
	int result;
	calculation(const string &r, const string &_r1, const string &_r2) : instruction(),
		rdest(regist::storeg(r)), rsrc1(regist::storeg(r)), rsrc2(regist::storeg(r)) {
		if (rsrc1 == -1) imm1 = stoi(_r1);// is a immediate number
		if (rsrc2 == -1) imm2 = stoi(_r2);
	}
	virtual void data_preparation() {
		if (rsrc1 != -1) imm1 = _regist.reg[rsrc1];
		if (rsrc2 != -1) imm2 = _regist.reg[rsrc2];
	}
	virtual void write_back() { _regist.reg[rdest] = result; }
};
class add : public calculation {
public:
	bool sign; // signed int --> sign = 1
	add(const string &r, const string &_r1, const string &_r2, bool s) :
		calculation(r, _r1, _r2), sign(s) {}
	virtual instruction* copy() { return new add(*this); }
	virtual void execute() { result = imm1 + imm2; }
};
class sub : public calculation {
public:
	bool sign; // signed int --> sign = 1
	sub(const string &r, const string &_r1, const string &_r2, bool s) :
		calculation(r, _r1, _r2), sign(s) {}
	virtual instruction* copy() { return new sub(*this); }
	virtual void execute() { result = imm1 - imm2; }
};

class mul : public calculation {
public:
	bool sign; // signed int --> sign = 1
	int32_t size;
	int64_t llresult;
	mul(const string &r, const string &_r1, const string &_r2, bool s) :
		calculation(r, _r1, _r2), sign(s) {
		size = (_r2 == "") ? 2 : 3;
	}
	virtual instruction* copy() { return new mul(*this); }
	virtual void data_prepare() {
		if (size == 2) {
			imm2 = imm1;
			imm1 = _regist.reg[rdest];
		}
		else {
			if (rsrc1 != -1) imm1 = _regist.reg[rsrc1];
			if (rsrc2 != -1) imm2 = _regist.reg[rsrc2];
		}
	}
	virtual void execute() {
		if (sign) llresult =  (int64_t)(imm1) * (int64_t)(imm2);
		else llresult = (uint64_t)(imm1)* (uint64_t)(imm2); //1ll * 
	}
	virtual void write_back() {
		if (size == 2) {
			_regist.reg[32] = llresult;// low
			_regist.reg[33] = llresult >> 32;//high
		}
		else _regist.reg[rdest] = llresult;
	}
};

class div : public calculation {
public:
	bool sign; // signed int --> sign = 1
	int32_t size;
	int32_t quotient, remainder;
	div(const string &r, const string &_r1, const string &_r2, bool s) :
		calculation(r, _r1, _r2), sign(s) {
		size = (_r2 == "") ? 2 : 3;
	}
	virtual instruction* copy() { return new div(*this); }
	virtual void data_prepare() {
		if (size == 2) {
			imm2 = imm1;
			imm1 = _regist.reg[rdest];
			if (rsrc1 != -1) imm2 = _regist.reg[rsrc1];
		}
		else {
			if (rsrc1 != -1) imm1 = _regist.reg[rsrc1];
			if (rsrc2 != -1) imm2 = _regist.reg[rsrc2];
		}
	}
	virtual void execute() {
		if (sign) {
			quotient = imm1 / imm2;
			remainder = imm1 % imm2;
		}
		else {
			quotient = (uint32_t)(imm1) / (uint32_t) (imm2);
			remainder = (uint32_t)(imm1) % (uint32_t)(imm2);
		}
	}
	virtual void write_back() {
		if (size == 2) {
			_regist.reg[32] = quotient;// lo-- q
			_regist.reg[33] = remainder;// hi
		}
		else _regist.reg[rdest] = quotient;
	}
};

class rem : public calculation {
public:
	bool sign; // signed int --> sign = 1
	int32_t remainder;
	rem(const string &r, const string &_r1, const string &_r2, bool s) :
		calculation(r, _r1, _r2), sign(s) {}
	virtual instruction* copy() { return new rem(*this); }
	virtual void data_prepare() {
			if (rsrc1 != -1) imm1 = _regist.reg[rsrc1];
			if (rsrc2 != -1) imm2 = _regist.reg[rsrc2];
	}
	virtual void execute() {
		if (sign) 	remainder = imm1 % imm2;
		else 	remainder = (uint32_t)(imm1) % (uint32_t)(imm2);
	}
	///???
	/*
	virtual void write_back() {
		_regist.reg[rdest] = remainder;
	}*/
};
class Xor : public calculation {
public:
	bool sign; // signed int --> sign = 1

	Xor(const string &r, const string &_r1, const string &_r2, bool s) :
		calculation(r, _r1, _r2), sign(s) {}
	virtual instruction* copy() { return new Xor(*this); }
	virtual void execute() { 
		if(sign) result = imm1 ^ imm2;
		else result = (uint32_t)(imm1) ^ (uint32_t)(imm2); 
	}
};
class neg : public calculation {
public:
	bool sign; // signed int --> sign = 1

	neg(const string &r, const string &_r1, const string &_r2, bool s) :
		calculation(r, _r1, _r2), sign(s) {}
	virtual instruction* copy() { return new neg(*this); }
	virtual void execute() {
		if (sign) result = ~imm1;
		else result = ~(uint32_t)(imm1);
	}
};

class comparation : public instruction {
public:
	int rdest, rsrc1, rsrc2;
	int imm1, imm2;
	bool cmp;
	int type;
	comparation(const string &r, const string &_r1, const string &_r2, const int t) :instruction(),
		rdest(regist::storeg(r)), rsrc1(regist::storeg(_r1)), rsrc2(regist::storeg(_r2)) , type(t){
		if (rsrc1 == -1) imm1 = stoi(_r1);
		if (rsrc2 == -1) imm2 = stoi(_r2);
	}
	//virtual instruction* copy() { return new comparation(*this); }
	virtual void data_preparation(){
		if (rsrc1 != -1) imm1 = _regist.reg[rsrc1];
		if (rsrc2 != -1) imm2 = _regist.reg[rsrc2];
	}
	virtual void write_back() {
		_regist.reg[rdest] = cmp;
	}
	virtual void execute() {
		switch (type) {
		case 1://seq
			cmp = (imm1 == imm2);
			break;	
		case 2://sge
			cmp = (imm1 >= imm2);
			break;
		case 3://sgt
			cmp = (imm1 > imm2);
			break;
		case 4://sle
			cmp = (imm1 <= imm2);
			break;
		case 5://slt
			cmp = (imm1 <imm2);
			break;
		case 6://sne
			cmp = (imm1 != imm2);
			break;
		}
	}
};


int ins_top, heap_top;
class branch : public instruction {
public:
	int rsrc1, rsrc2;
	int imm1, imm2;
	int pos;
	int flag;

	branch(const string &_r1, const string &_r2, const string &label):instruction(),
		rsrc1(regist::storeg(_r1)), rsrc2(regist::storeg(_r2)), pos(text_label[label])
	{}
	virtual instruction* copy() { return new branch(*this); }
	virtual void data_preparation() {
		if (rsrc1 != -1) imm1 = _regist.reg[rsrc1];
		if (rsrc2 != -1) imm2 = _regist.reg[rsrc2];
		else imm2 = 0;//? does it work?
	}
	virtual void write_back() {
		if (flag) ins_top = pos;
	}
};

class b_jump : public instruction {
public:
	int rsrc = -1;
	int pos = -1;
	//b, j, jr ==> type == 0
	//jal, jalr ==> type == 1 ==> writeback next order in $31
	bool type;
	bool isLabel = 0;
	b_jump(const string &r, const bool t) : instruction(), type(t){
		if (text_label.find(r) == text_label.end()) {
			isLabel = 0;
			rsrc = regist::storeg(r);
		}
		else {
			isLabel = 1;
			pos = text_label[r];
		}
	}
	virtual instruction* copy() { return new b_jump(*this); }
	virtual void data_preparation() {
		if (rsrc != -1) pos = _regist.reg[rsrc];
	}
	virtual void write_back() {
		if (type) _regist.reg[31] = ins_top;
		ins_top = pos;

	}
};

class beq : public branch {
public:
	beq(const string &_r1, const string &_r2, const string &label) :
		branch(_r1, _r2, label) {}
	virtual instruction* copy() { return new beq(*this); }
	virtual void execute() { flag = (imm1 == imm2); }
};
class bne : public branch {
public:
	bne(const string &_r1, const string &_r2, const string &label) :
		branch(_r1, _r2, label) {}
	virtual instruction* copy() { return new bne(*this); }
	virtual void execute() { flag = (imm1 != imm2); }
};
class bge : public branch {
public:
	bge(const string &_r1, const string &_r2, const string &label) :
		branch(_r1, _r2, label) {}
	virtual instruction* copy() { return new bge(*this); }
	virtual void execute() { flag = (imm1 >= imm2); }
};
class ble : public branch {
public:
	ble(const string &_r1, const string &_r2, const string &label) :
		branch(_r1, _r2, label) {}
	virtual instruction* copy() { return new ble(*this); }
	virtual void execute() { flag = (imm1 == imm2); }
};
class bgt : public branch {
public:
	bgt(const string &_r1, const string &_r2, const string &label) :
		branch(_r1, _r2, label) {}
	virtual instruction* copy() { return new bgt(*this); }
	virtual void execute() { flag = (imm1 > imm2); }
};
class blt : public branch {
public:
	blt(const string &_r1, const string &_r2, const string &label) :
		branch(_r1, _r2, label) {}
	virtual instruction* copy() { return new blt(*this); }
	virtual void execute() { flag = (imm1 < imm2); }
};


class syscall : public instruction {
public:
	istream &in;
	ostream &out;
	int type, v0, v1, result;
	string str;

	syscall(istream &_in, ostream &_out):in(_in), out(_out){}
	virtual instruction *copy() { return new syscall(*this); }
	virtual void data_prepare() {
		type = _regist.reg[2];// $v0
		switch (type) {
		case 1: case 4: case 9: case 17:
			v0 = _regist.reg[4];//$a0
			break;
		case 8:
			v1 = _regist.reg[5];//$a1
			break;
		}
	}
	virtual void execute() {
		switch (type) {
		case 1:
			out << v0;
			break;
		case 5:
			in >> result;
			break;
		case 8:
			getline(in, str);
			break;
		case 10:
			exit(0);
			break;
		case 17:
			exit(v0);
			break;
		}
	}
	virtual void memory_access() {
		switch (type) {
		case 4:
			int i = v0;
			while (_memory.mem[i]) out << _memory.mem[i++];
			break;
		case 8:
			int l = str.length();
			int i = 0;
			while (i < l) mem[v0 + i] = str[i++];
			break;
		}
	}
	virtual void write_back() {
		switch (type) {
		case 5:
			_regist.reg[2] = result;
			break;
		case 9:
			_regist.reg[2] = heap_top;
			heap_top += v0;
			break;
		}
	}
};

vector<instruction*> _instruction;




//interprete
void fetch(fstream &fin) {
	string str;// instruction sentences
	int ins_cnt = 0;
	vector<string> _name, _r1, _r2, _r3;
	bool text_block = 0;


	while (getline(fin, str)) {
		string tmp;
		int i = 0, l = str.length();
	}

}

void execute_text() {

}




