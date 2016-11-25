gcc steiner.c -o steiner;
./steiner;
sfdp steiner1.dot -Tpdf -o graph1.pdf;
sfdp steiner2.dot -Tpdf -o graph2.pdf;
sfdp steiner3.dot -Tpdf -o graph3.pdf;
open graph1.pdf;
open graph2.pdf;
open graph3.pdf
