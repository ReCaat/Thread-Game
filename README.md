Jogo da cobrinha feito em C++, utilizando conceitos de threads e semáforos. Para renderização gráfica, a biblioteca raylib é utilizada.

### Clonar repositório

(Se você tiver recebido esse código através do moodle, ignore a etapa de clonagem).

Clone o repositório. Você deve importar os submódulos (do raylib) com `--recurse-submodules`:

```bash
git clone --recurse-submodules github.com/ReCaat/Thread-Game
cd Thread-Game
```

Caso você tenha clonado sem incluir o submódulo do raylib, execute o comando abaixo na pasta do repositório:

```bash
git submodule update --init --recursive
```

### Compilar com cmake

```bash
cd build
cmake ..
make
```

Agora basta executar o binário dentro da pasta de build:

```bash
./Thread-Game
```