#include "catch.hpp"
#include "util.h"
#include <iostream>
#include <limits>
#include <selector_parser.h>
#include <selector.h>
#include <scope_selector.h>
#include <shl_exception.h>

using namespace shl;
using namespace shl::selector;

TEST_CASE("Selector Parser Test") {
    SECTION("parse a simple selector") {
        using shl::selector::Selector;
        selector::Parser parser("R:source.ruby ruby.* | - L:pic.a pic b, source.c storage.modifier.c keyword.core.control.if highlight.*, L:(man.kind man.hello man.last $, B:( ^ test.n test.g, test.a > test.b > test.c $))");

        Selector s = parser.parse();

        REQUIRE( s.selectors.size() == 3);

        CompositeSelctor& first = s.selectors[0];
        REQUIRE( first.operators.size() == 2);
        REQUIRE( first.operators[0] == selector::CompositeSelctor::NONE );
        REQUIRE( first.operators[1] == selector::CompositeSelctor::OR );
        REQUIRE( first.selectors.size() == 2 );
        REQUIRE( first.selectors[0].is_negative == false );
        FilterSelector& first_filter = *(FilterSelector*)first.selectors[0].selector.get();
        REQUIRE( first_filter.side == FilterSelector::Right );
        ScopeSelector& first_filter_scope = *(ScopeSelector*)first_filter.selector.get();
        REQUIRE( first_filter_scope.atoms.size() == 2 );
        REQUIRE( first_filter_scope.atoms[0].components.size() == 2 );
        REQUIRE( first_filter_scope.atoms[0].components[0] == "source" );
        REQUIRE( first_filter_scope.atoms[0].components[1] == "ruby" );
        REQUIRE( first_filter_scope.atoms[1].components.size() == 2 );
        REQUIRE( first_filter_scope.atoms[1].components[0] == "ruby" );
        REQUIRE( first_filter_scope.atoms[1].components[1] == "*" );


        REQUIRE( first.selectors[1].is_negative == true );
        FilterSelector& first_filter2 = *(FilterSelector*)first.selectors[1].selector.get();
        REQUIRE( first_filter2.side == FilterSelector::Left );
        ScopeSelector& first_filter2_scope = *(ScopeSelector*)first_filter2.selector.get();
        REQUIRE( first_filter2_scope.atoms.size() == 3 );
        REQUIRE( first_filter2_scope.atoms[0].components.size() == 2 );
        REQUIRE( first_filter2_scope.atoms[0].components[0] == "pic" );
        REQUIRE( first_filter2_scope.atoms[0].components[1] == "a" );
        REQUIRE( first_filter2_scope.atoms[1].components.size() == 1 );
        REQUIRE( first_filter2_scope.atoms[1].components[0] == "pic" );
        REQUIRE( first_filter2_scope.atoms[2].components.size() == 1 );
        REQUIRE( first_filter2_scope.atoms[2].components[0] == "b" );

        CompositeSelctor& second = s.selectors[1];
        REQUIRE( second.operators.size() == 1 );
        REQUIRE( second.operators[0] == CompositeSelctor::NONE );
        REQUIRE( second.selectors.size() == 1 );
        ScopeSelector& second_scope = *(ScopeSelector*)second.selectors[0].selector.get();
        REQUIRE( second_scope.atoms.size() == 4 );

        CompositeSelctor& third = s.selectors[2];
        REQUIRE( third.operators.size() == 1 );
        FilterSelector& third_filter = *(FilterSelector*)third.selectors[0].selector.get();
        REQUIRE( third_filter.side == FilterSelector::Left );
        Selector& nest = ((GroupSelector*)third_filter.selector.get())->selector;
        REQUIRE( nest.selectors.size() == 2 );
        CompositeSelctor& nest_first = nest.selectors[0];
        REQUIRE( nest_first.selectors.size() == 1 );
        ScopeSelector& nest_first_scope = *(ScopeSelector*)nest_first.selectors[0].selector.get();
        REQUIRE( nest_first_scope.anchor_begin == false );
        REQUIRE( nest_first_scope.anchor_end == true );
        REQUIRE( nest_first_scope.atoms.size() == 3 );

        FilterSelector& nest_second = *(FilterSelector*)nest.selectors[1].selectors[0].selector.get();
        REQUIRE( nest_second.side == FilterSelector::Both );
        GroupSelector& nest_second_group = *(GroupSelector*)nest_second.selector.get();
        Selector& nest_nest = nest_second_group.selector;
        REQUIRE( nest_nest.selectors.size() == 2 );
        ScopeSelector& nest_nest_first = *(ScopeSelector*)nest_nest.selectors[0].selectors[0].selector.get();
        REQUIRE( nest_nest_first.atoms.size() == 2 );
        REQUIRE( nest_nest_first.anchor_begin == true );
        REQUIRE( nest_nest_first.anchor_end == false );

        ScopeSelector& nest_nest_second = *(ScopeSelector*)nest_nest.selectors[1].selectors[0].selector.get();
        REQUIRE( nest_nest_second.atoms.size() == 3 );
        REQUIRE( nest_nest_second.anchor_begin == false );
        REQUIRE( nest_nest_second.anchor_end == true );
        REQUIRE( nest_nest_second.atoms[0].anchor_prev == false );
        REQUIRE( nest_nest_second.atoms[1].anchor_prev == true );
    }

    SECTION("simpile selector can select scopes") {
        using shl::selector::Selector;
        selector::Parser p1("source.ruby.structure.*");
        Selector s1 = p1.parse();
        Scope scope1("source.ruby.structure.if");
        Scope scope2("source.ruby.structure");
        Scope scope3("source.ruby.structure.for.additional");

        REQUIRE( s1.match(scope1, ' ', nullptr) );
        REQUIRE( s1.match(scope2, ' ', nullptr) == false );
        REQUIRE( s1.match(scope3, ' ', nullptr) );
        


        selector::Parser p2("source.ruby.structure");
        Selector s2 = p2.parse();
        Scope scope4("source.ruby.structure.if");

        REQUIRE( s2.match(scope4, ' ', nullptr) );



        selector::Parser p3("source.*.structure");
        Selector s3 = p3.parse();
        Scope scope5("source.cpp.structure.if");

        REQUIRE( s3.match(scope5, ' ', nullptr) );
    }

    SECTION("path selector can selector scopes") {
        using shl::selector::Selector;
        selector::Parser p1("source.ruby control.flow if");
        Selector s1 = p1.parse();
        Scope scope1("source.ruby control.flow if");
        Scope scope2("source.ruby block control.flow if embeded");
        Scope scope3("source.ruby block if embeded");

        REQUIRE( s1.match(scope1, ' ', nullptr ) );
        REQUIRE( s1.match(scope2, ' ', nullptr ) );
        REQUIRE( s1.match(scope3, ' ', nullptr ) == false );
        
    }

    SECTION("anchors will work") {
        using shl::selector::Selector;
        selector::Parser p1("A > B");
        Selector s1 = p1.parse();
        Scope scope1("A S B C A B");
        Scope scope2("A S B C A");

        REQUIRE( s1.match(scope1, ' ', nullptr) );
        REQUIRE( s1.match(scope2, ' ', nullptr) == false );

        selector::Parser p2("^ A > B");
        Selector s2 = p2.parse();
        Scope scope3("A S B C A B");
        Scope scope4("A B C A B");
        Scope scope5("B C A B");

        REQUIRE( s2.match(scope3, ' ', nullptr) == false );
        REQUIRE( s2.match(scope4, ' ', nullptr));
        REQUIRE( s2.match(scope5, ' ', nullptr) == false );

        selector::Parser p3(" A > B $");
        Selector s3 = p3.parse();
        Scope scope6("A C A B B");

        REQUIRE( s3.match(scope3, ' ', nullptr) );
        REQUIRE( s3.match(scope4, ' ', nullptr));
        REQUIRE( s3.match(scope6, ' ', nullptr) == false );

        selector::Parser p4(" ^ A $");
        Selector s4 = p4.parse();
        Scope scope7("A");
        Scope scope8("A B");
        Scope scope9("A A");

        REQUIRE( s4.match(scope7, ' ', nullptr) );
        REQUIRE( s4.match(scope8, ' ', nullptr) == false );
        REQUIRE( s4.match(scope9, ' ', nullptr) == false );

    }

    SECTION("test selector") {
        using shl::Selector;

        REQUIRE( Selector("foo fud").match("foo bar fud") );
        REQUIRE( Selector("foo > fud").match("foo bar fud") == false );
        REQUIRE( Selector("foo > foo > fud").match("foo foo fud") );
        REQUIRE( Selector("foo > foo > fud").match("foo foo fud baz") );
        REQUIRE( Selector("foo > foo fud > fud").match("foo foo bar fud fud") );

        REQUIRE( Selector("^ foo > bar").match("foo bar foo") );
        REQUIRE( Selector("foo > bar $").match("foo bar foo") == false );
        REQUIRE( Selector("bar > foo $").match("foo bar foo") );
        REQUIRE( Selector("foo > bar > foo $").match("foo bar foo") );
        REQUIRE( Selector("^ foo > bar > foo $").match("foo bar foo") );
        REQUIRE( Selector("bar > foo $").match("foo bar foo") );
        REQUIRE( Selector("^ foo > bar > baz").match("foo bar baz foo bar baz") );
        REQUIRE( Selector("^ foo > bar > baz").match("foo foo bar baz foo bar baz") == false );

        REQUIRE( Selector("foo bar").match("foo bar dyn.selection") );
        REQUIRE( Selector("foo bar dyn").match("foo bar dyn.selection") );
        REQUIRE( Selector("foo bar dyn").match("foo bar dyn.selection") );

        REQUIRE( Selector("foo").match("foo.qux bar.quux.grault baz.corge.garply") );
        REQUIRE( Selector("foo bar").match("foo.qux bar.quux.grault baz.corge.garply") );
        REQUIRE( Selector("foo bar baz").match("foo.qux bar.quux.grault baz.corge.garply") );
        REQUIRE( Selector("foo baz").match("foo.qux bar.quux.grault baz.corge.garply") );
        REQUIRE( Selector("foo.*").match("foo.qux bar.quux.grault baz.corge.garply") );
        REQUIRE( Selector("foo.qux").match("foo.qux bar.quux.grault baz.corge.garply") );
        REQUIRE( Selector("foo.qux baz.*.garply").match("foo.qux bar.quux.grault baz.corge.garply") );
        REQUIRE( Selector("bar").match("foo.qux bar.quux.grault baz.corge.garply") );
        REQUIRE( Selector("foo qux").match("foo.qux bar.quux.grault baz.corge.garply") == false );
        REQUIRE( Selector("foo.bar").match("foo.qux bar.quux.grault baz.corge.garply") == false );
        REQUIRE( Selector("foo.qux baz.garply").match("foo.qux bar.quux.grault baz.corge.garply") == false );
        REQUIRE( Selector("bar.*.baz").match("foo.qux bar.quux.grault baz.corge.garply") == false );
        REQUIRE( Selector("foo > bar").match("foo bar baz bar baz") );
        REQUIRE( Selector("bar > baz").match("foo bar baz bar baz") );
        REQUIRE( Selector("foo > bar baz").match("foo bar baz bar baz") );
        REQUIRE( Selector("foo bar > baz").match("foo bar baz bar baz") );
        REQUIRE( Selector("foo > bar > baz").match("foo bar baz bar baz") );
        REQUIRE( Selector("foo > bar bar > baz").match("foo bar baz bar baz") );
        REQUIRE( Selector("foo > bar > bar > baz").match("foo bar baz bar baz") == false );
        REQUIRE( Selector("baz $").match("foo bar baz bar baz") );
        REQUIRE( Selector("bar > baz $").match("foo bar baz bar baz") );
        REQUIRE( Selector("bar > baz $").match("foo bar baz bar baz") );
        REQUIRE( Selector("foo bar > baz $").match("foo bar baz bar baz") );
        REQUIRE( Selector("foo > bar > baz").match("foo bar baz bar baz") );
        REQUIRE( Selector("foo > bar > baz $").match("foo bar baz bar baz") == false );
        REQUIRE( Selector("bar $").match("foo bar baz bar baz") == false );
        REQUIRE( Selector("baz $").match("foo bar baz bar baz ") );
        REQUIRE( Selector("bar > baz $").match("foo bar baz bar baz") );
        REQUIRE( Selector("bar > baz $").match("foo bar baz bar baz") );
        REQUIRE( Selector("foo bar > baz $").match("foo bar baz bar baz") );
        REQUIRE( Selector("foo > bar > baz $").match("foo bar baz bar baz") ==false );
        REQUIRE( Selector("bar $").match("foo bar baz bar baz") == false );
        REQUIRE( Selector("^ foo").match("foo bar foo bar baz") );
        REQUIRE( Selector("^ foo > bar").match("foo bar foo bar baz") );
        REQUIRE( Selector("^ foo bar > baz").match("foo bar foo bar baz") );
        REQUIRE( Selector("^ foo > bar baz").match("foo bar foo bar baz") );
        REQUIRE( Selector("^ foo > bar > baz").match("foo bar foo bar baz") == false );
        REQUIRE( Selector("^ bar").match("foo bar foo bar baz") == false );
        REQUIRE( Selector("foo > bar > baz").match("foo bar baz foo bar baz") );
        REQUIRE( Selector("^ foo > bar > baz").match("foo bar baz foo bar baz") );
        REQUIRE( Selector("foo > bar > baz $").match("foo bar baz foo bar baz") );
        REQUIRE( Selector("^ foo > bar > baz $").match("foo bar baz foo bar baz") ==false );
    }

    SECTION("throw when invalid selector") {
        using shl::Selector;

        REQUIRE_THROWS_AS( Selector(""), InvalidScopeSelector );
        REQUIRE_THROWS_AS( Selector("> A > B"), InvalidScopeSelector );
    }

    SECTION("selectors rank will work") {
        using shl::Selector;
        string scope("text.html.markdown meta.paragraph.markdown markup.bold.markdown");
        string selectors[] = {
            "text.* markup.bold",
            "text markup.bold",
            "markup.bold",
            "text.html meta.*.markdown markup",
            "text.html meta.* markup",
            "text.html * markup",
            "text.html markup",
            "text markup",
            "markup",
            "text.html",
            "text"
        };

        double lastRank = std::numeric_limits<double>::max(), rank = 0;
        for(size_t i = 0; i < sizeof(selectors)/sizeof(selectors[0]); i++) {
            REQUIRE( Selector(selectors[i]).match(scope, Selector::BOTH, &rank) );
            if (rank >= lastRank ) {
                std::cout << selectors[i] << std::endl;
            }
            REQUIRE( rank < lastRank );
            lastRank =  rank;
        }


    }
}
