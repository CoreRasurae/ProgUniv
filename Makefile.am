AUTOMAKE_OPTIONS = dist-bzip2 no-dist-gzip
ACLOCAL_AMFLAGS = -I m4
#No libraries yet, following not needed
#DISTCLEANFILES = proguniv-1.0.pc
MAINTAINERCLEANFILES = ChangeLog
EXTRA_DIST = TODO
SUBDIRS = proguniv
#SUBDIRS = proguniv doc

#if BUILD_EXAMPLES
#SUBDIRS += examples
#endif

#No libraries yet, following not needed
#pkgconfigdir=$(libdir)/pkgconfig
#pkgconfig_DATA=proguniv-1.0.pc

.PHONY: dist-up
#.PHONY: ChangeLog dist-up
#ChangeLog:
#	git --git-dir $(top_srcdir)/.git log > ChangeLog || touch ChangeLog

#dist-hook: ChangeLog

reldir = .release/$(distdir)
dist-up: dist
	rm -rf $(reldir)
	mkdir -p $(reldir)
	cp $(distdir).tar.bz2 $(reldir)
	rm -rf $(reldir)

