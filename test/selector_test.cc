#include "catch.hpp"
#include "util.h"
#include <iostream>
#include <selector_parser.h>
#include <selector.h>

using namespace shl;
using namespace shl::selector;

TEST_CASE("Selector Parser Test") {
    SECTION("parse a simple selector") {
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
}
