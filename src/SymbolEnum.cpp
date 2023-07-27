#include "SymbolEnum.hpp"
#include <algorithm>

#include "utils.hpp"

using namespace std;
using namespace csr;


// ----------------------------------- [ Functions ] ---------------------------------------- //


const Symbol* SymbolEnum::get(SymbolID id) const {
	if (VALID_INDEX((int)id, symbols.size()))
		return  symbols[(int)id].get();
	else
		return nullptr;
}


const Symbol* SymbolEnum::get(const std::string& name) const {
	auto p = map_name.find(name);
	if (p != map_name.end())
		return symbols[p->second].get();
	else
		return nullptr;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


std::shared_ptr<Symbol> SymbolEnum::share(SymbolID id) const {
	if (VALID_INDEX((int)id, symbols.size()))
		return  symbols[(int)id];
	else
		return nullptr;
}


std::shared_ptr<Symbol> SymbolEnum::share(const std::string& name) const {
	auto p = map_name.find(name);
	if (p != map_name.end())
		return symbols[p->second];
	else
		return nullptr;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


const shared_ptr<Symbol>& SymbolEnum::put(const string& name){
	auto p = map_name.try_emplace(name);
	
	if (std::get<1>(p)){
		shared_ptr<Symbol>& sym = symbols.emplace_back(new Symbol());
		p.first->second = nextId;
		sym->id = nextId++;
		sym->name = name;
	}
	
	return symbols[p.first->second];
}


// ------------------------------------------------------------------------------------------ //