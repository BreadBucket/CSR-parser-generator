#include <string>
#include <vector>
#include <regex>
#include <iostream>

#include "../ANSI.h"

using namespace std;


// ----------------------------------- [ Structures ] --------------------------------------- //


typedef vector<vector<string>> LeftSymbol;


struct RightSymbol {
	string name;
	vector<int> args;
};


// ----------------------------------- [ Functions ] ---------------------------------------- //


void genAlternatives(vector<LeftSymbol>& left, vector<RightSymbol>& right){
	// Initialize alt counter
	vector<int> alt = {};
	alt.reserve(left.size());
	
	for (int i = 0 ; i < left.size() ; i++){
		alt.push_back(0);
	}
	
	
	// Increment alt counter
	auto inc = [&]() -> bool {
		for (int i = 0 ; i < alt.size() ; i++){
			alt[i]++;
			
			if (alt[i] >= left[i].size()){
				alt[i] = 0;
				continue;
			} else {
				return true;
			}
			
		}
		return false;
	};
	
	
	// Instantiate reference to an alternative
	vector<vector<string>*> left_altBuff = {};
	
	auto build = [&](){
		left_altBuff.clear();
		for (int i = 0 ; i < left.size() ; i++){
			vector<string>& ver = left[i][alt[i]];
			left_altBuff.push_back(&ver);
		}
	};
	
	
	// Ref translation
	vector<int> ref_corrections = {};
	
	auto refCorrect = [&](vector<int>& src){
		ref_corrections.clear();
		
		for (int i : src){
			i--;	// Start at [0]
			int x = 1;
			
			// Find offset
			for (int ii = 0 ; ii < i ; ii++){
				for (string& _ : *left_altBuff[ii])
					x++;
			}
			
			// Inflate ref
			for (string& _ : *left_altBuff[i]){
				ref_corrections.push_back(x++);
			}
			
		}
		
	};
	
	
	string s;
	while (true){
		s.clear();
		build();
		
		for (vector<string>* p : left_altBuff){
			for (string& _s : *p){
				s += _s;
				s += ' ';
			}
		}
		
		s += "-> ";
		
		for (RightSymbol& rsym : right){
			if (rsym.args.size() > 0){
				refCorrect(rsym.args);
				
				// Build constructor
				if (!rsym.name.empty()){
					s.append(rsym.name).push_back('(');
					
					for (int i = 0 ; i < ref_corrections.size() ; i++){
						if (i > 0)
							s += ',';
						s += to_string(ref_corrections[i]);
					}
					
					s += ") ";
				}
				
				else {
					for (int i = 0 ; i < ref_corrections.size() ; i++)
						s.append(to_string(ref_corrections[i])).push_back(' ');
				}
					
				
			} else if (!rsym.name.empty()){
				s += rsym.name;
				s += ' ';
			}
		}
		
		// Erase trailing whitespace
		while (s.size() > 0 && s.back() == ' '){
			s.pop_back();
		}
		
		cout << s << endl;
		
		if (!inc())
			break;
	}
	
	
	return;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


LeftSymbol parseGroup(const string& group){
	static regex r  = regex(
		"([a-zA-Z0-9_]+)"
		"|"
		"([|])"
	);
	
	sregex_iterator begin = sregex_iterator(group.begin(), group.end(), r);
	sregex_iterator end = sregex_iterator();
	
	LeftSymbol v = {};
	v.emplace_back();
	
	for (sregex_iterator p = begin ; p != end ; p++){
		const smatch& m = *p;
		
		if (m[1].matched)
			v.back().emplace_back(m[1].str());
		else if (m[2].matched)
			v.emplace_back();
		
	}
	
	while (v.size() > 0 && v.back().size() == 0){
		v.pop_back();
	}
	
	
	return v;
}


vector<int> parseConstructor(const string& s){
	static regex r  = regex("\\d+");
	
	sregex_iterator begin = sregex_iterator(s.begin(), s.end(), r);
	sregex_iterator end = sregex_iterator();
	vector<int> v = {};
	
	for (sregex_iterator p = begin ; p != end ; p++){
		const smatch& m = *p;
		string _s = m.str();
		v.emplace_back(atoi(_s.c_str()));
	}
	
	return v;
}


void processReduction(const string& line){
	static regex r_left  = regex(
		"\\s*"
		"(?:"
			"(-->)"
			"|"
			"([a-zA-Z0-9_]+)"
			"|"
			"[(]([^-]*?)[)]"
		")"
	);
	
	static regex r_right  = regex(
		"(?:"
			"(\\d+)"
			"|"
			"([a-zA-Z_]+)"
			"\\s*(?:[(](.*?)[)])?"
		")"
	);
	
	const sregex_iterator end = sregex_iterator();
	sregex_iterator begin = sregex_iterator(line.begin(), line.end(), r_left);
	
	
	vector<LeftSymbol> left;
	vector<RightSymbol> right;
	
	
	for (sregex_iterator p = begin ; p != end ; p++){
		const smatch& m = *p;
		
		if (m[1].matched){
			begin = sregex_iterator(m[1].second, line.end(), r_right);
			break;
		} else if (m[2].matched){
			LeftSymbol& sym = left.emplace_back();
			sym.emplace_back().emplace_back(m[2].str());
		} else if (m[3].matched){
			LeftSymbol& sym = left.emplace_back();
			sym = parseGroup(m[3].str());
		}
		
	}
	
	
	for (sregex_iterator p = begin ; p != end ; p++){
		const smatch& m = *p;
		RightSymbol& sym = right.emplace_back();
		
		if (m[1].matched){
			string _s = m[1].str();
			sym.args.push_back(atoi(_s.c_str()));
		} else if (m[2].matched){
			sym.name = m[2];
			if (m[3].matched)
				sym.args = parseConstructor(m[3].str());
		}
		
	}
	
	
	genAlternatives(left, right);
}


void processFile(const string& data){
	static regex r_line = regex(
		"(?:"
			"(\\s*(?:\n|$))"
			"|"
			"[%#]+(.*?)(?:\n|$)"
			"|"
			"\\s*(\\S[\\S\\s]*?)\\s*;[^\\S\n]*(?:\n|$)"
		")"
	);
	
	sregex_iterator begin = sregex_iterator(data.begin(), data.end(), r_line);
	sregex_iterator end = sregex_iterator();
	
	for (sregex_iterator p = begin ; p != end ; p++){
		const smatch& m = *p;
		
		if (m[1].matched)
			cout << m[1];
		else if (m[2].matched)
			cout << "//" << m[2] << endl;
		else if (m[3].matched)
			processReduction(m[3].str());
		
	}
	
}


// --------------------------------- [ Main Function ] -------------------------------------- //


int main(int argc, char const* const* argv){
	string data;
	string line;
	
	while (getline(cin, line)){
		data.append(line).push_back('\n');
	}
	
	processFile(data);
	return 0;
}


// ------------------------------------------------------------------------------------------ //