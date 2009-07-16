
atlast.dvi:
	latex atlast
	latex atlast
	
view:	atlast.dvi
	xdvi atlast
	
postscript:	atlast.dvi
	dvips -f atlast >atlast.ps
	
pdf:
	pdflatex atlast
	pdflatex atlast

clean:
	rm -f *.aux *.dvi *.log *.ps *.pdf
