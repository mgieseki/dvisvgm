#ifndef PAPERSIZESPECIALHANDLER_H
#define PAPERSIZESPECIALHANDLER_H

#include <utility>
#include <vector>
#include "SpecialHandler.h"

class PapersizeSpecialHandler : public SpecialHandler, public DVIEndPageListener {
	typedef std::pair<double,double> DoublePair;
	typedef std::pair<unsigned,DoublePair> PageSize;

	public:
		void preprocess (const char *prefix, std::istream &is, SpecialActions &actions) override;
		bool process (const char *prefix, std::istream &is, SpecialActions &actions) override;
		const char* info () const override {return "special to set the page size";}
		const char* name () const override {return "papersize";}
		const char**  prefixes () const override;

	protected:
		void dviEndPage (unsigned pageno, SpecialActions &actions) override;
		static bool isLess (const PageSize &ps1, const PageSize &ps2);

	private:
		std::vector<PageSize> _pageSizes;
};

#endif

