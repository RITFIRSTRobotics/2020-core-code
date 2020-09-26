# core/utils/Makefile
#
# @author Connor Henley, @thatgingerkid

define COMMIT_GEN_HEADER
/**
 * dbgcommit.gen.h
 *
 * Automatically generated header with commit information
 *
 * @generated on $(shell hostname) at $(shell date)
 */
#ifndef __CORE_UTILS_COMMIT_GEN
#define __CORE_UTILS_COMMIT_GEN

// allow C++ to parse this
#ifdef __cplusplus
extern "C" {
#endif
endef
export COMMIT_GEN_HEADER

define COMMIT_GEN_FOOTER
#ifdef __cplusplus
}
#endif

#endif
endef
export COMMIT_GEN_FOOTER

dbgcommit.gen.h:
	@# Put the header into the file
	@printf "" > dbgcommit.gen.h
	@printf "$$COMMIT_GEN_HEADER\n\n" >> dbgcommit.gen.h
	@printf "#define GIT_COMMIT_HASH (0x%s)\n" $(GIT_HASH) >> dbgcommit.gen.h
	@printf "#define DBG_BUILD_TYPE (DBG_BT_%s)\n\n" $(MAKE_CONFIG) >> dbgcommit.gen.h
	@printf "$$COMMIT_GEN_FOOTER\n" >> dbgcommit.gen.h

### CI Testing Recipes

ci-build: dbgcommit.gen.h
ci-test:  ; # do nothing

### Cleaning Recipe

clean:
	-@/bin/rm -f dbgcommit.gen.h
