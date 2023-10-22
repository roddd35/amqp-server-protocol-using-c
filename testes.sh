#!/bin/bash

export cpuUS
export cpuSYS
export memUsage
export cont

consumers=0
publishers=0
qtdDados=0

#  compilar e executar
make
gnome-terminal -- bash -c "./server 5672"   # abre uma nova janela de terminal e executa um comando

# declarar a fila de nome fila
amqp-declare-queue -q fila

# cadastrar 50 consumers na fila
while [ $consumers -lt 30 ]; do
    let consumers=consumers+1
    gnome-terminal -- bash -c "amqp-consume -q fila cat"
done

# a cada 7 publishers, vemos a quantidade de usos da CPU e afins
# vamos ter 70 publishers no final
# queremos fazer o teste com 70 publishers e 30 consumers
while [ $qtdDados -lt 10 ]; do
    publishers=0
    while [ $publishers -lt 7 ]; do
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