#include "GraphSerializer.hpp"

using namespace std;
using namespace csr;


// ----------------------------------- [ Functions ] ---------------------------------------- //


GraphSerializer::Format GraphSerializer::getFormat(const string& path){
	if (path.ends_with(".txt"))
		return Format::TXT;
	else
		return Format::UNKNOWN;
}


// ----------------------------------- [ Functions ] ---------------------------------------- //


void GraphSerializer::serialize(ostream& stream, const Graph& graph, const SymbolEnum& symEnum){
	switch (format){
		case Format::TXT:
			serialize_txt(stream, graph, symEnum);
			break;
	}
}


// ------------------------------------------------------------------------------------------ //