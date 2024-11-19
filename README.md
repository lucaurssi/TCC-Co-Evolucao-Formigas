## Coevolução entre duas Espécies de Formigas Artificiais Competindo por Recursos

Author:
	Luca Gomes Urssi

Orientador:
	Eduardo do Valle Simoes


Áreas relacionadas:
	Computação Bioinspirada, Sistemas Evolutivos

## Instalação

Para compilar a simulação visual deste trabalho, será necessario instalar a biblioteca de openGL:

```
sudo apt-get install mesa-common-dev
sudo apt-get install freeglut3-dev
```
utilize o comando abaixo para baixar este trabalho:
``` 
git clone https://github.com/lucaurssi/TTC-Co-Evolucao-Formigas.git
```

## Compilação

```
cd TCC-Co-Evolucao-Formigas
```
Para rodar a simulação visual:
```
make
make run
```

Para rodar a simulação que tenta evoluir os formigueiros sem graficos:
```
make evolve
```
Aviso: a simulação de evoluir pode levar multiplas horas para concluir.
