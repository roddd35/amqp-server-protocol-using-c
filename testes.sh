#!/bin/bash

export cpuUS
export cpuSYS
export memUsage
export cont

clientes=0
qtdDados=0


# a cada 5 publishers, vemos a quantidade de usos da CPU e afins
# vamos ter 50 publishers no final
# queremos fazer o teste com 50 publishers e 50 consumers
while [ $qtdDados -lt 10 ]; do
    while [ $clientes -lt $5 ]; do
        amqp-publish -r fila -b "mensagem "
        let cont=cont+1
    done
    # obter dados do uso da CPU
    cpuUS=$(top -b -n 1 | grep "Cpu" | awk -F '[, ]+' '{printf "%s,%s ", $2, $3}')
    echo "Uso da CPU por processos de usuário: $cpuUS \n"
    cpuSYS=$(top -b -n 1 | grep "Cpu" | awk -F '[, ]+' '{printf "%s,%s" $5, $6}')
    echo "Uso da CPU por processos de sistema: $cpuSYS \n"
    memUsage=$(free -h | grep "Mem:" | awk '{printf "%s/%s", $3, $2}')
    echo "Uso da memória: $memUsage \n"
done