#!/bin/bash

export cpuUS
export cpuSYS
export memUsage
export cont

consumers=0
publishers=0
qtdDados=0

# declarar a fila de nome fila
amqp-declare-queue -q fila

# cadastrar 50 consumers na fila
while [ $consumers -lt 50 ]; do
    gnome-terminal -- bash -c "amqp-consume -q fila cat"
done

# a cada 10 publishers, vemos a quantidade de usos da CPU e afins
# vamos ter 50 publishers no final
# queremos fazer o teste com 50 publishers e 50 consumers
while [ $qtdDados -lt 5 ]; do
    publishers=0
    while [ $publishers -lt 10 ]; do
        let publishers=publishers+1
        amqp-publish -r fila -b "mensagem "
    done
    echo "Quantidade de Publishers: $qtdDados"

    # obter dados do uso da CPU
    cpuUS=$(top -b -n 1 | grep "Cpu" | awk -F '[, ]+' '{printf "%s,%s ", $2, $3}')
    echo "Uso da CPU por processos de usuário: $cpuUS"
    cpuSYS=$(top -b -n 1 | grep "Cpu" | awk -F '[, ]+' '{printf "%s,%s", $5, $6}')
    echo "Uso da CPU por processos de sistema: $cpuSYS"
    memUsage=$(free -h | grep "Mem:" | awk '{printf "%s/%s", $3, $2}')
    echo "Uso da memória: $memUsage"

    let qtdDados=qtdDados+1
done