#include "Graph.hpp"

#include <unordered_set>
#include <algorithm>

#include "util/utils.hpp"
#include "util/ANSI.h"	// DEBUG


using namespace std;
using namespace csr;


// ----------------------------------- [ Functions ] ---------------------------------------- //


// DEBUG
#define STR(obj)		str(obj, *DEBUG_map)
#define CSTR(obj)		str(obj, *DEBUG_map).c_str()
const Map_IdToSymbol* DEBUG_map;

// DEBUG
string str(int id, const Map_IdToSymbol& id_to_symbol){
	auto p = id_to_symbol.find(id);
	if (p != id_to_symbol.end())
		return p->second;
	else
		return to_string(id);
}


// DEBUG
string str(const vector<SymbolID> v, const Map_IdToSymbol& id_to_symbol){
	if (v.size() <= 0)
		return "ϵ";
	
	string s = "(";
	
	for (SymbolID id : v){
		auto p = id_to_symbol.find(id);
		
		if (p != id_to_symbol.end())
			s += p->second;
		else
			s += to_string(id);
		
		s += " ";
	}
	
	s.pop_back();
	s += ')';
	return s;
}


// DEBUG
string str(const Item& item, const Map_IdToSymbol& id_to_symbol){
	string s = {};
	
	auto add = [&](int i, int n){
		if (n <= 0){
			s += "ϵ";
		} else {
			while (n > 0){
				int symbol = item.symbols[i];
				auto p = id_to_symbol.find(symbol);
				
				if (p != id_to_symbol.end())
					s += p->second;
				else
					s += to_string(symbol);
				
				s += " ";
				i++;
				n--;
			}
			s.pop_back();
		}
	};
	
	add(item.iobserved(), item.observed);
	s += " • ";
	add(item.imissing(), item.missing);
	s += " + ";
	add(item.iextra(), item.extra);
	
	return s;
}


// DEBUG
string str(const State& state, const Map_IdToSymbol& id_to_symbol){
	string s = "S" + to_string(state.id) + ":\n";
	
	for (const Item& item : state.items){
		s += "  ";
		s += str(item, id_to_symbol);
		s += "\n";
	}
	
	if (state.emptyItems != nullptr){
		for (const Item& item : *state.emptyItems){
			s += "  [";
			s += str(item, id_to_symbol);
			s += "]\n";
		}
	}
	
	if (s.size() > 0 && s.back() == '\n')
		s.pop_back();
	return s;
}


// DEBUG
string str(const Connection& v, const Map_IdToSymbol& id_to_symbol){
	string s = {};
	
	if (v.from != nullptr)
		s += "S" + to_string(v.from->id);
	else
		s += "(null)";
	
	s += " --[";
	s += str(v.symbol, id_to_symbol);
	s += "]--> ";
	
	if (v.to != nullptr)
		s += "S" + to_string(v.to->id);
	else if (v.reductionItem != nullptr)
		s += str(v.reductionItem->right, id_to_symbol);
	else
		s += "(null)";
	
	return s;
}


// DEBUG
string str(const vector<Connection*>& v, const Map_IdToSymbol& id_to_symbol){
	string s = {};
	
	for (const Connection* c : v){
		s += str(*c, id_to_symbol);
		s += '\n';
	}
	
	if (s.size() > 0 && s.back() == '\n')
		s.pop_back();
	return s;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Graph::build(const vector<ParsedReduction>& v){
	// DEBUG
	DEBUG_map = &id_to_symbol;
	
	// Setup
	clear();
	createEnum(v);
	createReductionItems(v);
	createEmptySet();
	getEvolutionSymbols(*emptySet, emptySetEvolutionSymbols);

	
	State* s0 = states.emplace_back(new State(0));
	s0->emptyItems = emptySet;
	evolutionQueue.push_back(s0);
	
	
	// for (int i = 0 ; i <= 1 && evolutionQueue.size() > 0 ; i++){
	while (evolutionQueue.size() > 0){
		printf("\n\n");	// DEBUG
		const State* s = evolutionQueue.front();
		evolutionQueue.pop_front();
		evolve(*s);
	}
	
	
	// DEBUG
	printf("\n\n\n\n\n\n");
	printf("%s\n\n", str(connections, id_to_symbol).c_str());
	for (const State* s : states){
		bool unresolved = false;
		
		for (const State* e : evolutionQueue){
			if (s->id == e->id){
				unresolved = true;
				break;
			}
		}
		
		printf(ANSI_GREEN);
		if (unresolved)
			printf(ANSI_YELLOW);
		printf("%s\n", str(*s, id_to_symbol).c_str());
		printf(ANSI_RESET);
	}
	
	printf("\nQUEUE: %d\n", evolutionQueue.size());
	
	
	// Release unused memory
	emptySetEvolutionSymbols.rehash(0);
	evolutionQueue.shrink_to_fit();
}


void Graph::createEnum(const std::vector<ParsedReduction>& v){
	symbol_to_id.clear();
	id_to_symbol.clear();
	
	int id = 1;
	auto add = [&](const vector<ParsedSymbol>& v){
		for (const ParsedSymbol& s : v){
			auto pair = symbol_to_id.try_emplace(s.name, id);
			
			// If inserted
			if (get<1>(pair)){
				id_to_symbol[id] = s.name;
				id++;
			}
			
		}
	};
	
	for (const ParsedReduction& r : v){
		add(r.left);
		add(r.right);
	}
	
}


void Graph::createReductionItems(const vector<ParsedReduction>& v){
	reductionItems.reserve(reductionItems.size() + v.size());
	
	for (int i = 0 ; i < v.size() ; i++){
		const ParsedReduction& r = v[i];
		ReductionItem& item = *reductionItems.emplace_back(new ReductionItem());
		
		item.id = reductionItems.size() - 1;
		item.left.reserve(r.left.size());
		item.right.reserve(r.right.size());
		
		// Left side
		for (const ParsedSymbol& s : r.left){
			auto p = symbol_to_id.find(s.name);
			if (p != symbol_to_id.end())
				item.left.push_back(p->second);
			else
				item.left.push_back(SymbolID(-1));
		}
		
		// Right side
		for (const ParsedSymbol& s : r.right){
			auto p = symbol_to_id.find(s.name);
			if (p != symbol_to_id.end())
				item.right.push_back(p->second);
			else
				item.right.push_back(SymbolID(-1));
		}
		
	}
	
}


void Graph::createEmptySet(){
	emptySet->clear();
	for (ReductionItem* r : reductionItems){
		
		// Distinct
		bool exists = false;
		for (const Item& item : *emptySet){
			if (r->left == item.symbols){
				exists = true;
				break;
			}
		}
		
		if (!exists){
			Item& item = emptySet->emplace_back();
			item.reduction = r;
			item.symbols = r->left;
			item.missing = r->left.size();
		}
		
	}
}


void Graph::getEvolutionSymbols(const vector<Item>& v, unordered_set<SymbolID>& out){
	for (const Item& item : v){
		if (item.missing > 0)
			out.emplace(item.pmissing(0));
	}
}


void Graph::clear(){
	for (auto p : states)
		delete p;
	for (auto p : connections)
		delete p;
	for (auto p : reductionItems)
		delete p;
	
	states.clear();
	connections.clear();
	reductionItems.clear();
	emptySet->clear();
	
	emptySetEvolutionSymbols.clear();
	evolutionQueue.clear();
	
	symbol_to_id.clear();
	id_to_symbol.clear();
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Graph::evolve(const State& base){
	printf("STEP " ANSI_CYAN "%s\n" ANSI_RESET, str(base, id_to_symbol).c_str());
	
	auto evolve = [&](State* temp, SymbolID symbol) -> State* {
		if (temp == nullptr)
			temp = new State();
		
		temp->evolve(base, symbol);
		
		// Find existing state
		const State* existingState = nullptr;
		for (int ii = 0 ; ii < states.size() ; ii++){
			if (temp->equivalent(*states[ii])){
				existingState = states[ii];
				break;
			}
		}
		
		// DEBUG
		printf(ANSI_YELLOW "%s ", str(symbol, id_to_symbol).c_str());
		temp->id = states.size();
		if (existingState != nullptr){
			temp->id = existingState->id;
			printf(ANSI_RED "%s\n" ANSI_RESET, str(*temp, id_to_symbol).c_str());
		}
		
		// Connect already existing state
		if (existingState != nullptr){
			connections.emplace_back(new Connection(&base, symbol, existingState));
			return temp;
		}
		
		// Check for reduction item
		const Item* ri = temp->getReductionItem();
		if (ri != nullptr){
			printf(ANSI_PURPLE "%s\n" ANSI_RESET, str(*temp, id_to_symbol).c_str());
			connections.emplace_back(new Connection(&base, symbol, ri->reduction));
		}
		
		// State is completely new
		else {
			printf(ANSI_GREEN "%s\n" ANSI_RESET, str(*temp, id_to_symbol).c_str());
			
			// Register new state
			temp->id = states.size();
			states.push_back(temp);
			evolutionQueue.push_back(temp);
			
			// Form connection
			Connection* c = new Connection(&base, symbol, temp);
			connections.emplace_back(c);
			
			return nullptr;
		}
		
		return temp;
	};
	
	
	State* tempState = nullptr;
	unordered_set<SymbolID> evolvedSymbols = {};
	
	// Evolve relevant items
	for (const Item& item : base.items){
		if (item.missing > 0){
			SymbolID symbol = item.pmissing(0);
			
			auto p = evolvedSymbols.emplace(symbol);
			if (get<1>(p))
				tempState = evolve(tempState, symbol);
			
		}
	}
	
	// Evolve empty set
	for (SymbolID symbol : emptySetEvolutionSymbols){
		if (!evolvedSymbols.contains(symbol))
			tempState = evolve(tempState, symbol);
	}
	
	delete tempState;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void State::evolve(const State& base, SymbolID symbol){
	// Reserve space
	items.clear();
	if (base.emptyItems != nullptr)
		items.reserve(base.items.size() + base.emptyItems->size());
	else
		items.reserve(base.items.size());
	
	
	// Temporary item object
	Item* item = &items.emplace_back();
	
	/**
	 * @brief When symbol is accepted, temporary item object is replaced.
	 */
	auto expandItem = [&](const Item& base, SymbolID symbol){
		
		// Accept missing symbol
		if (base.missing != 0){
			if (base.pmissing(0) == symbol){
				*item = base;
				item->observed++;
				item->missing--;
				item = &items.emplace_back();
			}
		}
		
		// Expand extra symbols
		else {
			item->symbols.reserve(base.symbols.size() + 1);
			*item = base;
			item->symbols.push_back(symbol);
			item->extra++;
			item = &items.emplace_back();
		}
		
	};
	
	// Handle relevant items
	for (const Item& baseItem : base.items){
		expandItem(baseItem, symbol);
	}
	
	// Handle empty set
	emptyItems = base.emptyItems;
	if (base.emptyItems != nullptr){
		for (const Item& baseItem : *base.emptyItems)
			expandItem(baseItem, symbol);
	}
	
	
	// Delete temporary item object
	items.pop_back();
}


const Item* State::getReductionItem() const {
	if (items.size() <= 0)
		return nullptr;
	
	// Find largest leftmost item
	const Item* ll = &items[0];
	int ll_pos = ll->position();
	int ll_size = ll->reductionSize();
	
	for (int i = 1 ; i < items.size() ; i++){
		int pos = items[i].position();
		int size = items[i].reductionSize();
		
		if (pos < ll_pos || (pos == ll_pos && size > ll_size)){
			ll = &items[i];
			ll_pos = pos;
			ll_size = size;
		}
		
	}
	
	// Item must be completely observed
	if (ll->missing == 0)
		return ll;
	else
		return nullptr;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


// void State::sort(){
// 	std::sort(items.begin(), items.end(), [](const Item& a, const Item& b) -> bool {
// 		if (a.reduction < b.reductionId)
// 			return true;
// 		else if (a.reductionId == b.reductionId){
// 			if (a.observed > b.observed)
// 				return true;
// 		}
// 		return false;
// 	});
// }


bool State::equivalent(const State& other) const {
	if (items.size() != other.items.size())
		return false;
	
	for (int i = 0 ; i < other.items.size() ; i++){
		if (!containsEquivalent(other.items[i]))
			return false;
	}
	
	return true;
}


bool State::containsEquivalent(const Item& item) const {
	for (int i = 0 ; i < items.size() ; i++){
		if (item.equivalent(items[i]))
			return true;
	}
	return false;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Item::set(const ParsedReduction& r, const Map_SymbolToId& map){
	observed = 0;
	missing = 0;
	extra = 0;
	
	symbols.clear();
	symbols.reserve(r.left.size());
	
	for (const ParsedSymbol& s : r.left){
		auto p = map.find(s.name);
		if (p != map.end()){
			symbols.push_back(p->second);
			missing++;
		}
	}
	
}


bool Item::equivalent(const Item& other) const {
	if (reduction != other.reduction)
		return false;
	else if (observed != other.observed || missing != other.missing || extra != other.extra)
		return false;
	else
		return symbols == other.symbols;
}


// ------------------------------------------------------------------------------------------ //