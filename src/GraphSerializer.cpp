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


void GraphSerializer::serialize(const Graph& graph, ostream& stream){
	switch (format){
		case Format::TXT:
			serialize_txt(graph, stream);
			break;
	}
}


// ------------------------------------------------------------------------------------------ //