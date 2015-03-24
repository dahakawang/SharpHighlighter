#include <iostream>
#include <parse/parse.h>
#include <parse/grammar.h>
#include <text/parse.h>
#include <plist/ascii.h>
#include <bundles/bundles.h>

using namespace std;
	struct bundle_index_t
	{
		bundle_index_t ()
		{
			_bundle = add(bundles::kItemTypeBundle, "{ name = 'Fixtures Bundle'; }");
		}

		bundles::item_ptr add (bundles::kind_t itemKind, plist::dictionary_t const& plist)
		{
			oak::uuid_t uuid;
			if(!plist::get_key_path(plist, bundles::kFieldUUID, uuid))
				uuid.generate();

			auto item = std::make_shared<bundles::item_t>(uuid, itemKind == bundles::kItemTypeBundle ? bundles::item_ptr() : _bundle, itemKind);
			item->set_plist(plist);
			_items.push_back(item);

			return item;
		}

		bundles::item_ptr add (bundles::kind_t itemKind, std::string const& plistString)
		{
			return add(itemKind, boost::get<plist::dictionary_t>(plist::parse_ascii(plistString)));
		}

		bool commit () const
		{
			return bundles::set_index(_items);
		}

	private:
		bundles::item_ptr _bundle;
		std::vector<bundles::item_ptr> _items;
	};

	static std::string TestLanguageGrammar =
		"{ scopeName = 'mdown';"
		"  patterns = ("
		"    { include = '#block'; },"
		"  );"
		"  repository = {"
		"    block = {"
		"      patterns = ("
		"        { include = '#heading'; },"
		"        { include = '#quote';   },"
		"        { include = '#list';    },"
		"        { include = '#raw';     },"
		"        { include = '#par';     },"
		"      );"
		"      repository = {"
		"        heading = {"
		"          name = 'hn';"
		"          begin = '(^|\\G)#+ ';"
		"          end = '\n';"
		"          patterns = ("
		"            { include = '#inline'; }"
		"          );"
		"        };"
		"        quote = {"
		"          name = 'q';"
		"          begin = '(^|\\G)> ';"
		"          while = '\\G> ';"
		"          patterns = ("
		"            { include = '#block'; }"
		"          );"
		"        };"
		"        list = {"
		"          name = 'li';"
		"          begin = '(^|\\G) [*] ';"
		"          while = '\\G   ';"
		"          patterns = ("
		"            { include = '#block'; }"
		"          );"
		"        };"
		"        raw = {"
		"          name = 'pre';"
		"          begin = '(^|\\G)    ';"
		"          while = '\\G    ';"
		"          patterns = ("
		"          );"
		"        };"
		"        par = {"
		"          name = 'p';"
		"          begin = '(?=\\S)';"
		"          end = '$';"
		"          patterns = ("
		"            { include = '#inline'; }"
		"          );"
		"        };"
		"      };"
		"    };"
		"    inline = {"
		"      patterns = ("
		"        { include = '#emph'; },"
		"      );"
		"      repository = {"
		"        emph = {"
		"          name = 'em';"
		"          begin = '_';"
		"          end = '_';"
		"          patterns = ("
		"          );"
		"        };"
		"      };"
		"    };"
		"  };"
		"  uuid = 'CF36D2F4-449E-481D-B6D3-FDE0F0CFD76D';"
		"}";

	bundle_index_t bundleIndex;

static std::string to_s (std::string const& buf, std::map<size_t, scope::scope_t> const& scopes)
{
	if(scopes.empty())
		return buf;

	std::string res;
	res.append(xml_difference(scope::scope_t(), scopes.begin()->second, "«", "»"));
	for(auto it = scopes.begin(); it != scopes.end(); )
	{
		auto from = *it;
		auto to = ++it != scopes.end() ? *it : std::map<size_t, scope::scope_t>::value_type(buf.size(), scope::scope_t());
		res.append(buf.substr(from.first, to.first - from.first));
		res.append(xml_difference(from.second, to.second, "«", "»"));
	}
	return res;
}

static std::map<size_t, scope::scope_t> scopes_for (std::string const& buf, parse::grammar_ptr grammar)
{
	std::map<size_t, scope::scope_t> res;
	if(!grammar)
		return res;

	parse::stack_ptr parserState = grammar->seed();
	for(std::string::size_type i = 0; i != buf.size(); )
	{
		auto eol = buf.find('\n', i);
		eol = eol != std::string::npos ? ++eol : buf.size();

		std::string const line = buf.substr(i, eol - i);
		std::map<size_t, scope::scope_t> scopes;
		parserState = parse::parse(line.data(), line.data() + line.size(), parserState, scopes, i == 0);

		for(auto const& pair : scopes)
			res[i + pair.first] = pair.second;

		i = eol;
	}
	return res;
}

std::string markup (parse::grammar_ptr grammar, std::string const& buf)
{
	return to_s(buf, scopes_for(buf, grammar));
}

int main() {
	bundles::item_ptr BeginWhileTestGrammarItem;
	BeginWhileTestGrammarItem = bundleIndex.add(bundles::kItemTypeGrammar, TestLanguageGrammar);
	

	auto grammar = parse::parse_grammar(BeginWhileTestGrammarItem);
	
	std::string const buf =
		"# Heading\n"
		"\n"
		"> Quoted\n"
		"> \n"
		"> > Double Quoted\n"
		"> >  * First item\n"
		"> >    still first\n"
		"> >  * Second item\n"
		"> >  * Third item\n"
		"> >  * Fourth item\n"
		"> >    \n"
		"> >        Raw _in_ item\n"
		"> >        More raw\n"
		"> >    \n"
		"> >    same _item_.\n"
		"> >    \n"
		"> >    # Heading in _that_ item\n"
		"> > # Heading in quote\n"
		"> Back to _quote_.\n"
		"And normal text.\n"
	;

	cout << markup(grammar, buf) << endl;
	cout << "hello" << endl;
}
