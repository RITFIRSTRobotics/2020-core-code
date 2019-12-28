LATEX_TMP_EXT=*.acn *.aux *.sympy *.toc *.glg *.gls *.ist *.glo *.acr *.alg
LATEX_OUT_EXT=*.log *.pdf

ControlProtocol: ControlProtocol.pdf

Template: Template.pdf

Template.pdf: Template.aux
	pdflatex -halt-on-error Template.tex

ControlProtocol.pdf: ControlProtocol.aux ControlProtocol.glo
	pdflatex -halt-on-error ControlProtocol/ControlProtocol.tex

ControlProtocol.glo: ControlProtocol.aux
	makeglossaries ControlProtocol

Template.aux: Template.tex
	pdflatex -halt-on-error Template.tex

ControlProtocol.aux: ControlProtocol/ControlProtocol.tex
	pdflatex -halt-on-error ControlProtocol/ControlProtocol.tex

clean: cleantmp
	rm -f $(LATEX_OUT_EXT)

cleantmp:
	rm -f $(LATEX_TMP_EXT)
