/*
 * Mihalcea.cc
 *
 * Implements the Rada Mihalcea word-sense disambiguation algorithm.
 *
 * Copyright (c) 2008 Linas Vepstas <linas@linas.org>
 */

#include "Mihalcea.h"

#include <stdio.h>

#include <opencog/atomspace/ForeachChaseLink.h>
#include <opencog/nlp/wsd/atom_types.h>
#include <opencog/nlp/wsd/MihalceaEdge.h>
#include <opencog/nlp/wsd/MihalceaLabel.h>
#include <opencog/nlp/wsd/ParseRank.h>
#include <opencog/nlp/wsd/SenseRank.h>
#include <opencog/nlp/wsd/ReportRank.h>
#include <opencog/util/platform.h>

using namespace opencog;

#define DEBUG

Mihalcea::Mihalcea(void)
{
	atom_space = NULL;
	labeller = new MihalceaLabel();
	edger = new MihalceaEdge();
	nn_adjuster = new NNAdjust();
	parse_ranker = new ParseRank();
	sense_ranker = new SenseRank();
	reporter = new ReportRank();

	previous_parse = Handle::UNDEFINED;
}

Mihalcea::~Mihalcea()
{
	atom_space = NULL;
	delete labeller;
	delete edger;
	delete nn_adjuster;
	delete parse_ranker;
	delete sense_ranker;
	delete reporter;
}

void Mihalcea::set_atom_space(AtomSpace *as)
{
	atom_space = as;
	labeller->set_atom_space(as);
	edger->set_atom_space(as);
}

bool Mihalcea::process_sentence(Handle h)
{
	Handle top_parse = parse_ranker->get_top_ranked_parse(h);
	parse_list.push_back(top_parse);

#ifdef DEBUG
	printf("; Mihalcea::process_sentence parse %lx for sentence %lx\n", top_parse.value(), h.value()); 
#endif

	// Attach senses to word instances
	labeller->annotate_parse(top_parse);

	// Create edges between sense pairs
	edger->annotate_parse(top_parse);

	// Tweak, based on parser markup
	// nn_adjuster->adjust_parse(top_parse);

	// Link sentences together, since presumably the next 
	// sentence deals with topics similar to the previous one.
	if (Handle::UNDEFINED != previous_parse)
	{
		edger->annotate_parse_pair(previous_parse, top_parse);
	}
	previous_parse = top_parse;

	return false;
}

bool Mihalcea::process_sentence_list(Handle h)
{
	foreach_outgoing_handle(h, &Mihalcea::process_sentence, this);

	// Solve the page-rank equations for the whole set of sentences.
	sense_ranker->rank_document(parse_list);

	// Report the results.
	reporter->report_document(parse_list);

	return false;
}

void Mihalcea::process_document(Handle h)
{
	foreach_binary_link(h, REFERENCE_LINK, &Mihalcea::process_sentence_list, this);
}
