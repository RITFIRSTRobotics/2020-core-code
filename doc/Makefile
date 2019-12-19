ControlProtocol: ControlProtocol.pdf

Template: Template.pdf

Template.pdf: Template.aux
	pdflatex -halt-on-error Template.tex

ControlProtocol.pdf: ControlProtocol.aux
	pdflatex -halt-on-error ControlProtocol/ControlProtocol.tex

Template.aux: Template.tex
	pdflatex -halt-on-error Template.tex

ControlProtocol.aux: ControlProtocol/ControlProtocol.tex
	pdflatex -halt-on-error ControlProtocol/ControlProtocol.tex

clean:
	rm -rf *.aux *.sympy *.toc *.log *.pdf
