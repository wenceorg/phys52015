pyfigures = $(wildcard figures/*.py)
drawiofigures = $(wildcard figures/*.drawio)
mdslides = $(wildcard site/static/slides/*.md)

drawiosvg = $(patsubst %.drawio,%.svg,$(drawiofigures))
drawiopdf = $(patsubst %.drawio,%.pdf,$(drawiofigures))
pysvg = $(patsubst %.py,%.svg,$(pyfigures))
pypdf = $(patsubst %.py,%.pdf,$(pyfigures))
htmlslides = $(patsubst %.md,%.html,$(mdslides))

.PHONY: html allsvg pysvg allcode killds_store

html: allcode pysvg htmlslides
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

allcode:
	rsync --delete -rupm code/ site/static/code/ --filter '+ */' --filter '+ *.c' --filter '+ *.h' --filter '+ Makefile' --filter '+ *.slurm' --filter '- *'
