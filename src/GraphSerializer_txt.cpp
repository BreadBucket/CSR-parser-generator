#include "GraphSerializer.hpp"

using namespace std;
using namespace csr;


// ---------------------------------- [ Structures ] ---------------------------------------- //


struct Serializer {
// -------------------------------- //
public:
	const GraphSerializer& writer;
	ostream& out;
	
// -------------------------------- //
public:
	void writeSymbol(const Symbol*);
	void writeReduction(const Reduction&);
	void writeItem(const Item&);
	void writeConnection(const Connection&);
	
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


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Serializer::writeReduction(const Reduction& r){
	// Left
	if (r.left.size() > 0){
		for (int i = 0 ; i < r.left.size() ; i++){
			writeSymbol(r.left[i].get());
			out << ' ';
		}
	} else {
		if (writer.unicode)
			out << "ϵ ";
		else
			out << "$ ";
	}
	
	// Arrow
	out << "->";
	
	// Right
	if (r.right.size() > 0){
		for (int i = 0 ; i < r.right.size() ; i++){
			out << ' ';
			writeSymbol(r.right[i].get());
		}
	} else {
		if (writer.unicode)
			out << "ϵ ";
		else
			out << "$ ";
	}
	
};


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Serializer::writeItem(const Item& item){
	
	auto writeSection = [&](int i, int n){
		int _i = i + n;
		if (n <= 0){
			if (writer.unicode)
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
	
	if (writer.unicode)
		out << " • ";
	else
		out << " . ";
	
	writeSection(item.imissing(), item.missing);
	out << " + ";
	writeSection(item.iextra(), item.extra);
	
};


// ----------------------------------- [ Functions ] ---------------------------------------- //


void Serializer::writeConnection(const Connection& connection){
	if (connection.from != nullptr){
		out << "S" << connection.from->id;
	} else {
		if (writer.unicode)
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
	else if (connection.reductionItem != nullptr){
		out << "(";
		writeReduction(*connection.reductionItem);
		out << ")";
	}
	
	// Nothing
	else {
		if (writer.unicode)
			out << "ϵ";
		else
			out << "$";
	}
	
};


// ----------------------------------- [ Functions ] ---------------------------------------- //


void GraphSerializer::serialize_txt(const Graph& graph, ostream& out){
	Serializer w = {*this, out};
	
	// Write all reductions
	if (graph.reductionCount() > 0){
		out << "reductions:\n";
		
		for (int i = 0 ; i < graph.reductionCount() ; i++){
			if (graph.getReduction(i) != nullptr){
				out << "  ";
				w.writeReduction(*graph.getReduction(i));
				out << "\n";
			}
		}
		
		out << "\n\n";
	}
	
	// Write all graph states and their components
	for (const State* state : graph.states){
		if (state == nullptr)
			continue;
		
		// Header
		out << "S" << (int)state->id << ":\n";
		
		// Write relevant items
		if (state->items.size() > 0){
			out << "  items:\n";
			for (const Item& item : state->items){
				out << "    ";
				w.writeItem(item);
				out << "\n";
			}
		}
		
		// Write empty items
		else if (state->emptyItems != nullptr && state->emptyItems->size() > 0){
			out << "  items:\n";
			for (const Item& item : *state->emptyItems){
				out << "    ";
				w.writeItem(item);
				out << "\n";
			}
		}
		
		// Write connections
		if (state->connections.size() > 0){
			out << "  connections:\n";
			for (const Connection* c : state->connections){
				if (c != nullptr){
					out << "    ";
					w.writeConnection(*c);
					out << "\n";
				}
			}
		}
		
		out << "\n";
	}
	
}


// ------------------------------------------------------------------------------------------ //