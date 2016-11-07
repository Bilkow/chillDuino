import statistics
import sys

f = open(sys.argv[1])
texto = f.read()
dados = list(map(int, texto.split()))
print("stdev = {stdev}\nmean = {mean}\nSNR = {SNR}".format(stdev=statistics.stdev(dados), mean=statistics.mean(dados), SNR=1/(statistics.stdev(dados) / statistics.mean(dados))))
