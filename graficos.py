import pandas as pd
from matplotlib import pyplot as plt

data = pd.read_csv('data.csv', sep=';')

x = data['pub']
y = data['cpu']

data = data.sort_values(by='pub')

plt.plot(x,y, marker='o')
plt.xlabel("Quantidade de Publishers")
plt.ylabel("Uso da CPU (%)")
plt.title("Desempenho com 10 clientes")
plt.show()