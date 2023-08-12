#include "GraphSerializer.hpp"

#include "utils.hpp"
#include "Symbol.hpp"
#include "SymbolEnum.hpp"
#include "Reduction.hpp"
#include "Graph.hpp"
#include "Document.hpp"

using namespace std;
using namespace csr;


// ---------------------------------- [ Structures ] ---------------------------------------- //


struct Serializer {
// -------------------------------- //
public:
	const GraphSerializer& settings;
	ostream& out;
	
// -------------------------------- //
public:
	void writeSymbol(const Symbol*);
	void writeSymbols(const SymbolEnum&);
	
public:
	void writeRightSymbol(const Reduction&, const Reduction::RightSymbol&, bool includeConstructionInfo = false);
	void writeReduction(const Reduction&, bool includeConstructionInfo = false);
	void writeReductions(const vector<shared_ptr<Reduction>>&);
	
public:
	void writeConnection(const Connection&);
	void writeConnections(const vector<Connection*>&);
	
public:
	void writeItem(const Item&);
	void writeState(const State&);
	void writeGraph(const Graph&);
	
// -------------------------------- //
};


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Serializer::writeSymbol(const Symbol* symbol){
	if (symbol != nullptr){
		if (!symbol->name.empty())
			out << symbol->name;
		else
			out << '[' << symbol->id << ']';
	} else {
		out << "?";
	}
}


void Serializer::writeSymbols(const SymbolEnum& symbols){
	if (symbols.size() > 0){
		out << "symbols:\n";
		
		for (const shared_ptr<Symbol> sym : symbols.getSymbols()){
			if (sym != nullptr){
				out << "  ";
				writeSymbol(sym.get());
				out << '\n';
			}
		}
		
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Serializer::writeRightSymbol(const Reduction& r, const Reduction::RightSymbol& rsym, bool includeConstructionInfo){
	// Helper function for locating left-symbol by index
	auto getLeftSymbol = [&](int i) -> const Symbol* {
		if (VALID_INDEX(i, r.left.size()))
			return r.left[i].get();
		else
			return nullptr;
	};
	
	
	if (includeConstructionInfo){
		
		// Is copy
		if (holds_alternative<Reduction::SymbolCopy>(rsym)){
			int i = (int)get<Reduction::SymbolCopy>(rsym);
			const Symbol* lsym = getLeftSymbol(i);
			
			writeSymbol(lsym);
			out << '[' << to_string(i) << ']';
		}
		
		// Is constructor
		else {
			const Reduction::SymbolConstructor& ctr = get<Reduction::SymbolConstructor>(rsym);
			
			writeSymbol(ctr.symbol.get());
			
			// Write constructor
			if (ctr.args.size() > 0){
				out << '(';
				
				for (int i = 0 ; i < ctr.args.size() ; i++){
					if (i > 0)
						out << ", ";
					writeRightSymbol(r, ctr.args[i], includeConstructionInfo);
				}
				
				out << ')';
			}
			
		}
		
	}
	
	else {
		if (holds_alternative<Reduction::SymbolCopy>(rsym)){
			int i = (int)get<Reduction::SymbolCopy>(rsym);
			writeSymbol(getLeftSymbol(i));
		} else {
			writeSymbol(get<Reduction::SymbolConstructor>(rsym).symbol.get());
		}
	}
	
}


void Serializer::writeReduction(const Reduction& r, bool includeConstructitonInfo){
	// Left
	if (r.left.size() > 0){
		for (int i = 0 ; i < r.left.size() ; i++){
			writeSymbol(r.left[i].get());
			out << ' ';
		}
	} else {
		if (settings.unicode)
			out << "ϵ ";
		else
			out << "$ ";
	}
	
	// Arrow
	out << "->";
	
	// Right
	if (r.right.size() > 0){
		for (const Reduction::RightSymbol& rsym : r.right){
			out << ' ';
			writeRightSymbol(r, rsym, includeConstructitonInfo);
		}
	} else {
		if (settings.unicode)
			out << "ϵ ";
		else
			out << "$ ";
	}
	
};


void Serializer::writeReductions(const vector<shared_ptr<Reduction>>& reductions){
	if (reductions.size() > 0){
		out << "reductions:\n";
		
		for (const shared_ptr<Reduction>& red : reductions){
			if (red != nullptr){
				out << "  ";
				writeReduction(*red, true);
				out << '\n';
			}
		}
		
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Serializer::writeConnection(const Connection& connection){
	if (connection.from != nullptr){
		out << "S" << connection.from->id;
	} else {
		if (settings.unicode)
			out << "ϵ";
		else
			out << "$";
	}
	
	out << " --[";
	writeSymbol(connection.symbol);
	out << "]--> ";
	
	// Another state
	if (connection.to != nullptr){
		out << "S" << connection.to->id;
	}
	
	// Reduction item
	else if (connection.reductionItem != nullptr && connection.reductionItem->reduction != nullptr){
		out << '(';
		writeReduction(*connection.reductionItem->reduction);
		out << ")+" << connection.reductionItem->extra;
	}
	
	// Nothing
	else {
		if (settings.unicode)
			out << "ϵ";
		else
			out << "$";
	}
	
};


void Serializer::writeConnections(const vector<Connection*>& connections){
	if (connections.size() > 0){
		out << "connections:\n";
		
		for (const Connection* conn : connections){
			if (conn != nullptr){
				out << "  ";
				writeConnection(*conn);
				out << '\n';
			}
		}
		
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Serializer::writeItem(const Item& item){
	
	auto writeSection = [&](int i, int n){
		int _i = i + n;
		if (n <= 0){
			if (settings.unicode)
				out << "ϵ";
			else
				out << "$";
		} else {
			while (i < _i){
				writeSymbol(item[i]);
				i++;
				if (i < _i)
					out << ' ';
			}
		}
	};
	
	writeSection(item.iobserved(), item.observed);
	
	if (settings.unicode)
		out << " • ";
	else
		out << " . ";
	
	writeSection(item.imissing(), item.missing);
	out << " + ";
	writeSection(item.iextra(), item.extra);
	
};


void Serializer::writeState(const State& state){
	// Header
	out << "  S" << (int)state.id << ":\n";
	
	// Write relevant items
	if (state.items.size() > 0){
		out << "    items:\n";
		for (const Item& item : state.items){
			out << "      ";
			writeItem(item);
			out << "\n";
		}
	}
	
	// Write empty items
	else if (state.emptyItems != nullptr && state.emptyItems->size() > 0){
		out << "    items:\n";
		for (const Item& item : *state.emptyItems){
			out << "      ";
			writeItem(item);
			out << "\n";
		}
	}
	
	// Write connections
	if (state.connections.size() > 0){
		out << "    connections:\n";
		for (const Connection* c : state.connections){
			if (c != nullptr){
				out << "      ";
				writeConnection(*c);
				out << "\n";
			}
		}
	}
	
}


void Serializer::writeGraph(const Graph& graph){
	if (graph.states.size() > 0){
		out << "graph:\n";
		
		for (const State* state : graph.states){
			if (state != nullptr)
				writeState(*state);
		}
		
	}
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void GraphSerializer::serialize_txt(ostream& out, const Document& doc){
	Serializer serializer = {*this, out};
	
	if (doc.symEnum != nullptr){
		serializer.writeSymbols(*doc.symEnum);
		out << "\n";
	}
	
	serializer.writeReductions(doc.reductions);
	out << "\n";
	
	if (doc.graph != nullptr){
		serializer.writeConnections(doc.graph->connections);
		out << "\n";
		serializer.writeGraph(*doc.graph);
		out << '\n';
	}
	
}


// ------------------------------------------------------------------------------------------ //