pyfigures = $(wildcard figures/*.py)
drawiofigures = $(wildcard figures/*.drawio)
mdslides = $(wildcard site/static/slides/*.md)

drawiosvg = $(patsubst %.drawio,%.svg,$(drawiofigures))
drawiopdf = $(patsubst %.drawio,%.pdf,$(drawiofigures))
pysvg = $(patsubst %.py,%.svg,$(pyfigures))
pypdf = $(patsubst %.py,%.pdf,$(pyfigures))
htmlslides = $(patsubst %.md,%.html,$(mdslides))

.PHONY: html allsvg pysvg allcode alltgz killds_store

html: allcode alltgz pysvg htmlslides
	(cd site; hugo --minify --cleanDestinationDir)

allslides: allfigures

site/static/images:
	mkdir -p $@

htmlslides: $(htmlslides)

site/static/slides/%.html: site/static/slides/%.md
	(cd site/static/slides; python preprocess.py $(notdir $<))

allsvg: pysvg drawiosvg

pysvg: site/static/images $(pysvg)
	rsync --delete -rupm $(pysvg) site/static/images/auto/

drawiosvg: site/static/images $(drawiosvg)
	rsync --delete -rupm $(drawiosvg) site/static/images/manual/

allpdf: $(drawiopdf) $(pypdf)

figures/%.svg: figures/%.drawio
	drawio -s 2 -t -f svg -x --crop -o $@ $<

figures/%.pdf: figures/%.drawio
	drawio -f pdf -x --crop -o $@ $<

figures/%.svg: figures/%.py
	python $< $@

figures/%.pdf: figures/%.py
	python $< $@

killds_store:
	find . -name ".DS_Store" -print0 | xargs -0 rm -f

site/static/code/blur_image.tgz: code/blur_image/vec/Makefile $(wildcard code/blur_image/vec/*.[ch]) code/blur_image/openmp/Makefile $(wildcard code/blur_image/openmp/*.[ch]) $(wildcard code/blur_image/images/*.ppm)
	(cd code; tar -zcf $(abspath $@) $(patsubst code/%,%,$^))

site/static/code/add_numbers.tgz: code/add_numbers/serial/Makefile $(wildcard code/add_numbers/serial/*.[ch]) code/add_numbers/openmp/Makefile $(wildcard code/add_numbers/openmp/*.[ch])
	(cd code; tar -zcf $(abspath $@) $(patsubst code/%,%,$^))

site/static/code/coursework.tgz: code/coursework/Makefile code/coursework/valgrind.supp code/coursework/bench.c code/coursework/bench.h code/coursework/check.c code/coursework/check.h code/coursework/main.c code/coursework/mat.c code/coursework/mat.h code/coursework/solution.c code/coursework/utils.h code/coursework/vec.c code/coursework/vec.h
	(cd code; tar -zcf $(abspath $@) $(patsubst code/%,%,$^))

alltgz: killds_store site/static/code/blur_image.tgz site/static/code/add_numbers.tgz site/static/code/coursework.tgz

allcode:
	rsync --delete -rupm code/ site/static/code/ --filter '+ */' --filter '+ *.c' --filter '+ *.h' --filter '+ Makefile' --filter '+ *.slurm' --filter '- *'
