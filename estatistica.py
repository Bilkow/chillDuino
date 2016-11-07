import statistics
import sys

f = open(sys.argv[1])
texto = f.read()
dados = list(map(int, texto.split()))
print("qtdade de dados={quantity}\nstdev = {stdev}\nmean = {mean}\nSNR = {SNR}".format(quantity=len(dados), stdev=statistics.stdev(dados), mean=statistics.mean(dados), SNR=1/(statistics.stdev(dados) / statistics.mean(dados))))
