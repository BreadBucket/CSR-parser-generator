#include "SymbolEnum.hpp"

using namespace std;
using namespace csr;


// ----------------------------------- [ Functions ] ---------------------------------------- //


const Symbol* SymbolEnum::get(SymbolID id) const {
	auto p = map_id.find(id);
	if (p != map_id.end())
		return p->second.get();
	else
		return nullSymbol.get();
}


const Symbol* SymbolEnum::get(const std::string& name) const {
	auto p = map_name.find(name);
	if (p != map_name.end())
		return p->second.get();
	else
		return nullSymbol.get();
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


std::shared_ptr<Symbol> SymbolEnum::share(SymbolID id) const {
	auto p = map_id.find(id);
	if (p != map_id.end())
		return p->second;
	else
		return nullSymbol;
}


std::shared_ptr<Symbol> SymbolEnum::share(const std::string& name) const {
	auto p = map_name.find(name);
	if (p != map_name.end())
		return p->second;
	else
		return nullSymbol;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


SymbolID SymbolEnum::put(const string& name){
	auto p = map_name.try_emplace(name);
	shared_ptr<Symbol>& sym = std::get<0>(p)->second;
	
	if (std::get<1>(p)){
		sym = make_unique<Symbol>();
		sym->name = name;
		sym->id = ++prevId;
		map_id[sym->id] = sym;
	}
	
	return sym->id;
}


// ------------------------------------------------------------------------------------------ //